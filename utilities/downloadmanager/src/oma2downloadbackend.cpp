/**
   This file is part of CWRT package **

   Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies). **

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU (Lesser) General Public License as 
   published by the Free Software Foundation, version 2.1 of the License. 
   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of 
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
   (Lesser) General Public License for more details. You should have 
   received a copy of the GNU (Lesser) General Public License along 
   with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "oma2downloadbackend.h"
#include "omaddparser.h"
#include "downloadmanager.h"
#include "download.h"
#include "clientdownload.h"
#include "downloadcore.h"
#include "downloadevent.h"
#include <QCoreApplication>
#include <QFileInfo>

#ifdef Q_OS_LINUX
  #include <sys/statvfs.h>
#endif

#ifdef Q_OS_LINUX 
static double freeSpace(const char *path)
#else
static double freeSpace(const char* /*path*/)
#endif
{
#ifdef Q_OS_LINUX   
    struct statvfs fiData;
    if ((statvfs(path,&fiData)) < 0 ) 
        return 0;
    else {
        qint64 blockSize = fiData.f_bsize/1024;
        qint64 freeSize = (fiData.f_bavail)*blockSize;
        freeSize = freeSize/1024;
        return freeSize;
    }
#endif
    return 0;
}

// Constants
const char* const SuppressUserConfirmation_Never = "Never";

// private implementation
class OMA2DownloadBackendPrivate {
    DM_DECLARE_PUBLIC(OMA2DownloadBackend);
public:
    OMA2DownloadBackendPrivate();
    ~OMA2DownloadBackendPrivate();
    DownloadCore *m_downloadCore;
    ClientDownload *m_download;
    OMADownloadDescParser *m_parser; // for parsing oma descriptor
    OMADownloadDescriptor *m_downloadDesc;
    bool m_isMediaDownload; // flag to indicate if media download is happening  
    MediaDownloadList m_mediaDownloadList; // media downloads
    int m_currDownloadIndex;
    int m_currProductIndex;
    qint64 m_dlCompletedSize;
    QMap<int, int> m_mediaObjIdStateMap; // map to have the list of Media Objects' id and it's state.
    QList<QVariant> m_childIds;
    OMA2DownloadProduct *m_currProduct;
    OMA2DownloadMediaObj *m_currMediaObj;
};

OMA2DownloadBackendPrivate::OMA2DownloadBackendPrivate():m_downloadCore(0)
                                                        ,m_download(0)
                                                        ,m_parser(0)
                                                        ,m_downloadDesc(0)
                                                        ,m_isMediaDownload(false)
                                                        ,m_currDownloadIndex(-1)
                                                        ,m_currProductIndex(0)
                                                        ,m_dlCompletedSize(0)
{ }

OMA2DownloadBackendPrivate::~OMA2DownloadBackendPrivate()
{
    if (m_parser) {
        delete m_parser;
        m_parser = 0;
        m_downloadDesc = 0;
    }
    // In case of persistent storage
    if (m_downloadDesc) {
        delete m_downloadDesc;
        m_downloadDesc = 0;
    }
    
    int count = m_mediaDownloadList.count();
    for (int i = 0; i < count; i++)
        delete m_mediaDownloadList[i];
}

OMA2DownloadBackend::OMA2DownloadBackend(DownloadCore *dlCore, ClientDownload *dl)
    :DownloadBackend(dlCore, dl)
{
    DM_INITIALIZE(OMA2DownloadBackend);
    priv->m_downloadCore = dlCore;
    priv->m_download = dl;
}

OMA2DownloadBackend::~OMA2DownloadBackend()
{
    DM_UNINITIALIZE(OMA2DownloadBackend);
}

int OMA2DownloadBackend::pause()
{
    DM_PRIVATE(OMA2DownloadBackend);
    if (priv->m_isMediaDownload) {
        priv->m_mediaDownloadList[priv->m_currDownloadIndex]->pause();
        setDownloadState(DlPaused);
    }
    return 0;
}

int OMA2DownloadBackend::resume()
{
    DM_PRIVATE(OMA2DownloadBackend);
    if (priv->m_isMediaDownload)
        return (priv->m_mediaDownloadList[priv->m_currDownloadIndex]->resume());

    // Ready to download after the descriptor is parsed
    // capability check on the descriptor information
    if (checkDownloadDescriptor()) {
        ProductList product = priv->m_downloadDesc->productList();
        int count =  product[priv->m_currProductIndex]->mediaCount();
        
        for (int i=0; i < count; i++) {
            QUrl baseUrl = priv->m_downloadCore->url();
            QUrl relativeUrl = product[priv->m_currProductIndex]->mediaObjList()[i]->getAttribute(OMA2DownloadDescMediaObjServer).toString();
            QString url = baseUrl.resolved(relativeUrl).toString();
            // create download for media object
            // id's of media downloads are unique under its parent oma download
            ClientDownload *dl = new ClientDownload(priv->m_download->downloadManager(), url, (i+1), Parallel
                                                                                            , priv->m_download->id() );
            if (NULL == dl)
                return -1;
            // functions for persistent storage
            serializeData(dl, i);
            setValueForChild(DownloadInfo::EUrl, url, dl->id());
            setValueForChild(DownloadInfo::EDlState, DlCreated, dl->id());
            long state;
            getValue(DownloadInfo::EDlState, state);
            priv->m_mediaObjIdStateMap[dl->id()] = state;
            addtoDownloadList(dl);
        }
        setValue(DownloadInfo::EChildIdList, priv->m_childIds);
        qint64 tSize = product[priv->m_currProductIndex]->albumSize();
        setTotalSize(tSize);
        priv->m_currDownloadIndex = 0;
        priv->m_mediaDownloadList[priv->m_currDownloadIndex]->start();
        priv->m_isMediaDownload = true; 
        return 0;         
    }
    return -1;
}

int OMA2DownloadBackend::cancel()
{
    DM_PRIVATE(OMA2DownloadBackend);
    if (priv->m_isMediaDownload)
        priv->m_mediaDownloadList[priv->m_currDownloadIndex]->cancel();
    else
       DownloadBackend::cancel();
    return 0;
}

QVariant OMA2DownloadBackend::getAttribute(DownloadAttribute attr)
{
    DM_PRIVATE(OMA2DownloadBackend);
    // moIndex is zero because when product name is empty the name of 0th Mediaobject should be shown in dd and also the type is of the 0th Mediaobject
    const int moIndex = 0;
    QString text = "";
    ProductList product;
    if (priv->m_downloadDesc)
        product = priv->m_downloadDesc->productList();
    switch(attr) {
    case OMADownloadDescriptorName:
    {
        if (product[priv->m_currProductIndex]) {
            QString name = product[priv->m_currProductIndex]->getAttribute(OMA2DownloadDescProductName).toString();
            if (!name.isEmpty())
                return name;
            if (product[priv->m_currProductIndex]->mediaCount() > 0)
                return product[priv->m_currProductIndex]->mediaObjList()[moIndex]->getAttribute(OMA2DownloadDescMediaObjName);
            return text;
        }  
        return text;
    }

    case OMADownloadDescriptorType:
    {
        if (product[priv->m_currProductIndex]) {
            if (product[priv->m_currProductIndex]->mediaCount() > 0)
                return product[priv->m_currProductIndex]->mediaObjList()[moIndex]->getAttribute(OMA2DownloadDescMediaObjType);
            return text;
        }
        return text;
    }

    case OMADownloadDescriptorSize:
    {
        qint64 size = 0;
        if (product[priv->m_currProductIndex])
            return product[priv->m_currProductIndex]->albumSize();
        return size;
    }

    case OMADownloadDescriptorNextURL:
    {
        return priv->m_downloadDesc->getAttribute(OMADownloadDescNextURL);
    }

    case DlFileName:
    {
        if (!priv->m_isMediaDownload) {
            QString url = priv->m_downloadCore->url();
            QFileInfo fileUrl(url);
            return QVariant(fileUrl.fileName());
        }
        // If the product tag doesn't have a name then display the media object's name in the download list.
        // This is helpful for single OMA2 download and is done for better user readability
        if (product[priv->m_currProductIndex]) {
            QString name = product[priv->m_currProductIndex]->getAttribute(OMA2DownloadDescProductName).toString();
            if (!name.isEmpty())
                return name;
            if (product[priv->m_currProductIndex]->mediaCount() > 0)
                return product[priv->m_currProductIndex]->mediaObjList()[priv->m_currDownloadIndex]->getAttribute(OMA2DownloadDescMediaObjName);
            return text;
        }  
        return text;
    }

    case DlContentType:
    {
        if (!priv->m_isMediaDownload)
            return DownloadBackend::getAttribute(DlContentType);
        QString contentType = priv->m_mediaDownloadList[priv->m_currDownloadIndex]->getAttribute(DlContentType).toString();
        if (contentType != "")
            return contentType;
        QString mediaObjContentType;
        getValueForChild(DownloadInfo::EContentType, mediaObjContentType, priv->m_childIds[priv->m_currDownloadIndex].toInt());
        return mediaObjContentType;
    }

    default:
        return DownloadBackend::getAttribute(attr);
    }
    return QVariant();
}

int OMA2DownloadBackend::setAttribute(DownloadAttribute attr, const QVariant& value)
{
    return DownloadBackend::setAttribute(attr, value);
}

// stores the data in storage
void OMA2DownloadBackend::store(QByteArray /*data*/, bool /*lastChunk=false*/)
{
    return;
}

// deletes the storage
void OMA2DownloadBackend::deleteStore()
{
    return;
}

// returns the size of stored data
qint64 OMA2DownloadBackend::storedDataSize()
{
    return 0;
}

void OMA2DownloadBackend::bytesRecieved(qint64 /*bytesRecieved*/, qint64 /*bytesTotal*/)
{
     //Do nothing. This is here to avoid this signal to reach to base class' slot.
     return;
}

void OMA2DownloadBackend::bytesUploaded(qint64 bytesUploaded, qint64 bytesTotal)
{
    DM_PRIVATE(OMA2DownloadBackend);
    // once data is uploaded, cancel the transaction
    if (bytesUploaded == bytesTotal)
        priv->m_downloadCore->abort() ;

}

void OMA2DownloadBackend::handleFinished()
{
     DM_PRIVATE(OMA2DownloadBackend);
     QString contentType = priv->m_downloadCore->contentType();
     if (contentType == OMA2_CONTENT_TYPE) {
         bool bSucceeded = parseDownloadDescriptor();
         priv->m_downloadDesc = priv->m_parser->downloadDescriptor();
         if (bSucceeded) {
             ProductList product = priv->m_downloadDesc->productList();
             int count =  product[priv->m_currProductIndex]->mediaCount();
             if (count == 0) {
                 priv->m_downloadCore->post(priv->m_downloadDesc->getAttribute(OMADownloadDescInstallNotifyURI).toString(), QByteArray("906 Invalid descriptor"));
                 priv->m_downloadCore->setLastError(QNetworkReply::UnknownContentError);
                 priv->m_downloadCore->setLastErrorString(tr("Invalid Descriptor"));
                 setDownloadState(DlFailed);
                 postEvent(Error, NULL);
                 return;
             }
             for (int i=0; i < count; i++) {
                 QString server = product[priv->m_currProductIndex]->mediaObjList()[i]->getAttribute(OMA2DownloadDescMediaObjServer).toString();
                 QString size = product[priv->m_currProductIndex]->mediaObjList()[i]->getAttribute(OMA2DownloadDescMediaObjSize).toString();
                 QString type = product[priv->m_currProductIndex]->mediaObjList()[i]->getAttribute(OMA2DownloadDescMediaObjType).toString();
                 if (server.isEmpty() || size.isEmpty() || type.isEmpty()) {
                     QString string;
                     string = product[priv->m_currProductIndex]->mediaObjList()[i]->getAttribute(OMA2DownloadDescMediaObjInstallNotifyURI).toString();
                     priv->m_downloadCore->post(string, QByteArray("905 Attribute Mismatch"));
                     priv->m_downloadCore->setLastError(QNetworkReply::UnknownContentError);
                     priv->m_downloadCore->setLastErrorString(tr("Invalid Descriptor"));
                     setDownloadState(DlFailed);
                     postEvent(Error, NULL);
                     return;
                 }
             }
             // if download is restarted OR suppressUserConfirmation is true, do not show descriptor
             if (suppressUserConfirmation() || (DlDescriptorUpdated == downloadState())) {
                 resume();
             } else {
                 setDownloadState(DlPaused);
                 postEvent(OMADownloadDescriptorReady, NULL);
             }
         } else {
             priv->m_downloadCore->post(priv->m_downloadDesc->getAttribute(OMADownloadDescInstallNotifyURI).toString(), QByteArray("906 Invalid descriptor"));
             priv->m_downloadCore->setLastError(QNetworkReply::UnknownContentError);
             priv->m_downloadCore->setLastErrorString(tr("Invalid Descriptor"));
             setDownloadState(DlFailed);
             postEvent(Error, NULL);
        }
    }
}
          
bool OMA2DownloadBackend::parseDownloadDescriptor()
{
    DM_PRIVATE(OMA2DownloadBackend);
    priv->m_parser = new OMADownloadDescParser();
    QXmlInputSource source(priv->m_downloadCore->reply());
    QXmlSimpleReader reader;
    reader.setContentHandler(priv->m_parser);
    reader.setErrorHandler(priv->m_parser);
    return reader.parse(source);
}

// capability check on the descriptor
bool OMA2DownloadBackend::checkDownloadDescriptor()
{
#ifdef Q_OS_LINUX
    DM_PRIVATE(OMA2DownloadBackend); 
    ProductList product = priv->m_downloadDesc->productList();
    double fileSize = product[priv->m_currProductIndex]->albumSize();
    double mbFactor = 1024*1024;
    fileSize = fileSize/mbFactor; //fileSize in MB
    double spaceLeft = freeSpace(ROOT_PATH); //spaze left in MB
    
    if (fileSize > spaceLeft) {
        priv->m_downloadCore->post(priv->m_downloadDesc->getAttribute(OMADownloadDescInstallNotifyURI).toString(), QByteArray("901 Insufficient memory"));
        priv->m_downloadCore->setLastError(QNetworkReply::UnknownContentError);
        priv->m_downloadCore->setLastErrorString(tr("901 Insufficient Memory"));
        setDownloadState(DlFailed);
        postEvent(Error, NULL);
        return false;       
    } 
#endif
    return true;
}

bool OMA2DownloadBackend::event(QEvent *event)
{
    DM_PRIVATE(OMA2DownloadBackend);
    ProductList product = priv->m_downloadDesc->productList();
    DownloadEvent* downloadEvent =  dynamic_cast<DownloadEvent*>(event);
    DEventType type = (DEventType)event->type();
    switch(type) {
    case Started:
        break;
    case HeaderReceived:
    {
        int dlId = ((DownloadEvent*)event)->getId();
        if (downloadEvent) {
            int statusCode = downloadEvent->getAttribute(HeaderReceivedStatusCode).toInt();
            handleStatusCode(statusCode);
        }
        ClientDownload *dl = findDownload(dlId);
        // Check the mismatch in total size returned by server with the size given in the descriptor.
        //This piece of code is commented as-of-now. when needed in future , will be uncommented
        /*qint64 totalSize = dl->getAttribute(DlTotalSize).toInt();
        if(product[priv->m_currProductIndex]->mediaObjList()[priv->m_currDownloadIndex]->getAttribute(OMA2DownloadDescMediaObjSize).toInt() != totalSize) {
            postInstallNotifyEvent("905 Attribut Mismatch");
            priv->m_mediaObjIdStateMap[downloadEvent->getId()] = DlFailed;
        } */
        // Check the mismatch in content type returned by server with the content type given in the descriptor.
        QString contentType = dl->getAttribute(DlContentType).toString();
        if(product[priv->m_currProductIndex]->mediaObjList()[priv->m_currDownloadIndex]->getAttribute(OMA2DownloadDescMediaObjType).toString() != contentType) {
            postInstallNotifyEvent("905 Attribute Mismatch");
            priv->m_mediaObjIdStateMap[downloadEvent->getId()] = DlFailed;
            if (priv->m_currDownloadIndex < product[priv->m_currProductIndex]->mediaCount()-1) {
                priv->m_currDownloadIndex++;
                priv->m_mediaDownloadList[priv->m_currDownloadIndex]->start();
            }
       }        
        break;
    }

    case Progress:
    {        
        QVariant curDlsize = priv->m_mediaDownloadList[priv->m_currDownloadIndex]->getAttribute(DlDownloadedSize);
        qint64 currentDlSize =  priv->m_dlCompletedSize + (curDlsize.toInt());
        setDownloadedDataSize(currentDlSize);
        setDownloadState(DlInprogress);
        priv->m_mediaObjIdStateMap[downloadEvent->getId()] = DlInprogress;
        setValueForChild(DownloadInfo::EDlState, DlInprogress, downloadEvent->getId());
        postEvent(Progress, NULL);
        break;
    }
    case Paused:
    {
        setDownloadState(DlPaused);
        priv->m_mediaObjIdStateMap[downloadEvent->getId()] = DlPaused;
        setValueForChild(DownloadInfo::EDlState, DlPaused, downloadEvent->getId());
        postEvent(Paused, NULL);
        break;
    }
    case NetworkLoss:
    {
        postEvent(NetworkLoss, NULL);
        break;
    }
    case Cancelled:
    {
        postInstallNotifyEvent("902 User Cancelled");
        priv->m_mediaObjIdStateMap[downloadEvent->getId()] = DlCancelled;
        setValueForChild(DownloadInfo::EDlState, DlCancelled, downloadEvent->getId());
        setDownloadState(DlCancelled);
        postEvent(Cancelled, NULL);
        break;
    }
    case Completed:
    {
        postInstallNotifyEvent("900 Success");
        priv->m_mediaObjIdStateMap[downloadEvent->getId()] = DlCompleted;
        setValueForChild(DownloadInfo::EDlState, DlCompleted, downloadEvent->getId());
        QVariant curDlsize = priv->m_mediaDownloadList[priv->m_currDownloadIndex]->getAttribute(DlDownloadedSize);
        priv->m_dlCompletedSize = priv->m_dlCompletedSize + (curDlsize.toInt());
        if (priv->m_currDownloadIndex < product[priv->m_currProductIndex]->mediaCount()-1) {
            priv->m_currDownloadIndex++;
            priv->m_mediaDownloadList[priv->m_currDownloadIndex]->start();
        } else if (priv->m_currDownloadIndex == product[priv->m_currProductIndex]->mediaCount()-1) 
            verifyDownloads();
        break;
    }        
    case Error:
    {
        priv->m_downloadCore->setLastError(QNetworkReply::UnknownContentError);
        priv->m_downloadCore->setLastErrorString(tr("905 Attribute Mismatch"));
        setDownloadState(DlFailed);
        postEvent(Error, NULL);
        priv->m_mediaObjIdStateMap[downloadEvent->getId()] = DlFailed;
        setValueForChild(DownloadInfo::EDlState, DlFailed, downloadEvent->getId());
        break;
    } 
    default:
        break;
    }
    return true;
}

void OMA2DownloadBackend::getChildren(QList<Download*>& list)
{
    DM_PRIVATE(OMA2DownloadBackend);
    for (int i = 0; i < priv->m_mediaDownloadList.count(); i++) 
        list.append(priv->m_mediaDownloadList[i]);

}

bool OMA2DownloadBackend::suppressUserConfirmation()
{
    DM_PRIVATE(OMA2DownloadBackend);

    ProductList productList = priv->m_downloadDesc->productList();
    QString suppressValue = productList[priv->m_currProductIndex]->getAttribute(OMA2DownloadDescProductSuppressConfirmation).toString();
    QString suppressUserConfirmNever(SuppressUserConfirmation_Never);

    // If suppressUserConfirmation attribute is present for Product tag, use it and don't check media objects
    // for this attribute
    if (!suppressValue.isEmpty()) {
        if (suppressValue == suppressUserConfirmNever)
            return false;
        return true;
    }
    
    int mediaCount = productList[priv->m_currProductIndex]->mediaCount();
    // If suppressUserConfirmation is present and it is NOT "Never" for even a single media object,
    // then treat this as true and do not display user confirmation.
    QString suppressConfirmMediaObj = "";

    for (int i=0; i<mediaCount; ++i) {
        // If the mediaObject element in DD file has this attribute
        suppressValue = productList[priv->m_currProductIndex]->mediaObjList()[i]->getAttribute(OMA2DownloadDescMediaObjSuppressConfirmation).toString();
        if (!suppressValue.isEmpty()) {
            // Check if two or more mediObjects have different values for this attribute.
            // If so, then this is an invalid case. Hence we ignore this attribute.
            if (suppressConfirmMediaObj.isEmpty())
                suppressConfirmMediaObj = suppressValue;
            else if (suppressConfirmMediaObj != suppressValue) {
                // Two mediaObjects have different values of suppressUserConfirmation attribute.
                // Ignore this attribute.
                return false;
            }
        }
    }
    
    // If string is empty OR its value is "Never", return false.
    if (suppressConfirmMediaObj.isEmpty() || (suppressConfirmMediaObj == suppressUserConfirmNever))
        return false;
    
    return true;
}

void OMA2DownloadBackend::handleStatusCode(const int& statusCode)
{
    HttpStatusCode status = (HttpStatusCode)statusCode;
    switch(status) {
    case HttpPreconditionFailed:    // Precondition Failed
    {
        handlePreconditionFailed();
        postEvent(DescriptorUpdated, NULL);
        return;
    }
    
    default:
        return;
    }
}

void OMA2DownloadBackend::handlePreconditionFailed()
{
    DM_PRIVATE(OMA2DownloadBackend);
    QString updatedDDUri = priv->m_downloadDesc->getAttribute(OMA2DownloadDescUpdatedDDURI).toString();
    if (updatedDDUri.isEmpty())
        return;

    DownloadManager* dlMgr = priv->m_download->downloadManager();
    if (!dlMgr)
        return;

    // Delete all media objects
    priv->m_isMediaDownload = false;
    ProductList productList = priv->m_downloadDesc->productList();
    int count =  productList[priv->m_currProductIndex]->mediaCount();

    // Remove all media objects
    for(int i=0; i<count; ++i) {
        ClientDownload* dl = priv->m_mediaDownloadList[i];
        QCoreApplication::removePostedEvents(this);
        dl->unregisterEventReceiver(this);
        priv->m_mediaDownloadList.removeOne(dl);
        dlMgr->removeOne(dl);
    }
    
    // start download of updated DD
    priv->m_downloadCore->changeUrl(updatedDDUri);
    priv->m_downloadCore->doDownload();
    setDownloadState(DlDescriptorUpdated);
}

void OMA2DownloadBackend::addtoDownloadList(ClientDownload* dl)
{
    DM_PRIVATE(OMA2DownloadBackend); 
    dl->registerEventReceiver(this);
    priv->m_childIds.append(dl->id());
    priv->m_mediaDownloadList.append(dl);
}

ClientDownload* OMA2DownloadBackend::findDownload(int id)
{
    DM_PRIVATE(OMA2DownloadBackend);
    for (int i = 0; i < priv->m_mediaDownloadList.size(); ++i) {
        if (priv->m_mediaDownloadList[i]->id() == id)
            return priv->m_mediaDownloadList[i];
    }
    return 0; 
}

void OMA2DownloadBackend::postInstallNotifyEvent(const char* statusMessage)
{
    DM_PRIVATE(OMA2DownloadBackend);
    ProductList product = priv->m_downloadDesc->productList();
    QString string;
    string = product[priv->m_currProductIndex]->mediaObjList()[priv->m_currDownloadIndex]->getAttribute(OMA2DownloadDescMediaObjInstallNotifyURI).toString();
    priv->m_downloadCore->post(string, QByteArray(statusMessage));   
}

void OMA2DownloadBackend::verifyDownloads()
{
    DM_PRIVATE(OMA2DownloadBackend);
    // go throught the mediaObjmap of id, state value pair and if any dl is failed then set the state of parent as paused.
    ProductList product = priv->m_downloadDesc->productList();
    int counter = 0;
    for (int i=0; i < priv->m_childIds.count(); i++) {
        if (priv->m_mediaObjIdStateMap[priv->m_childIds[i].toInt()] == DlCompleted)
            counter++;
        else if (priv->m_mediaObjIdStateMap[priv->m_childIds[i].toInt()] == DlFailed) {
            setDownloadState(DlFailed);
            postEvent(Error, NULL);
            break;
        }
    }
    if (counter == priv->m_childIds.count()) {
        setDownloadState(DlCompleted);
        postEvent(Completed, NULL);
    }
}

int OMA2DownloadBackend::currentIndex()
{
    // If a download is in progress and closed,  a track in an album is either paused or created.
    // This function as part of persistant storage,returns the index of the download which is either in paused or created state
    DM_PRIVATE(OMA2DownloadBackend);
    ProductList product = priv->m_downloadDesc->productList();
    for (int i=0; i < priv->m_childIds.count(); i++) {
        if (priv->m_mediaObjIdStateMap[priv->m_childIds[i].toInt()] == DlPaused
            || priv->m_mediaObjIdStateMap[priv->m_childIds[i].toInt()] == DlCreated) {
            priv->m_currDownloadIndex = i;
            break;
        }
    }
    return priv->m_currDownloadIndex;
}

void OMA2DownloadBackend::init()
{
    DM_PRIVATE(OMA2DownloadBackend);
    priv->m_isMediaDownload = true; // since init() is called during restoring the persistent info, the flag should be set to true.

    priv->m_downloadDesc = new OMADownloadDescriptor();
    priv->m_currProduct = new OMA2DownloadProduct();
    priv->m_downloadDesc->addProduct(priv->m_currProduct);
    // populating the product map
    QString productName;
    getValue(DownloadInfo::EFileName, productName);
    priv->m_currProduct->setAttribute("name", QVariant(productName));

    QList<QVariant> id;
    getValue(DownloadInfo::EChildIdList, id);
    QString oma2ContentType,mediaObjName,mediaObjUrl,mediaObjContentType;
    long mediaObjSize;
    long state;
    priv->m_currProductIndex = 0;
    for(int i=0; i<id.size(); i++) {
        //populating the mediaobj map
        priv->m_currMediaObj = new OMA2DownloadMediaObj();
        priv->m_currProduct->addMediaObject(priv->m_currMediaObj);
        
        // getting all the values related to media object for populating descriptor's mediaobj map        
        getValueForChild(DownloadInfo::EFileName, mediaObjName, id[i].toInt());
        getValueForChild(DownloadInfo::ETotalSize, mediaObjSize, id[i].toInt());
        getValueForChild(DownloadInfo::EUrl, mediaObjUrl, id[i].toInt());
        getValueForChild(DownloadInfo::EContentType, mediaObjContentType, id[i].toInt());
        getValueForChild(DownloadInfo::EDlState, state, id[i].toInt());
        priv->m_mediaObjIdStateMap[id[i].toInt()] = state;

        // creaating downloads for the media objects
        ClientDownload *dl = NULL;
        if (state == DlCreated) {
            dl = new ClientDownload(priv->m_download->downloadManager(), mediaObjUrl, id[i].toInt(), Parallel
                                                                                                   , priv->m_download->id() );
        }
        else
            dl = new ClientDownload(priv->m_download->downloadManager(), id[i].toInt(), priv->m_download->id());

        setValueForChild(DownloadInfo::EDlState, priv->m_mediaObjIdStateMap[id[i].toInt()], id[i].toInt());
        addtoDownloadList(dl);
        // re-constructing descriptor's mediaobj map
        priv->m_currMediaObj->setAttribute("name", QVariant(mediaObjName));
        int size = mediaObjSize;
        priv->m_currMediaObj->setAttribute("size", QVariant(size));
        priv->m_currMediaObj->setAttribute("type", QVariant(mediaObjContentType));
        priv->m_currMediaObj->setAttribute("server",QVariant(mediaObjUrl));
    }
    ProductList product = priv->m_downloadDesc->productList();
    getValue(DownloadInfo::EDlState,state);
    if(state == DlCompleted)
        priv->m_currDownloadIndex = product[priv->m_currProductIndex]->mediaCount()-1;
    else
        priv->m_currDownloadIndex = currentIndex();
}

void OMA2DownloadBackend::serializeData(ClientDownload* dl, int index)
{
    DM_PRIVATE(OMA2DownloadBackend);
    ProductList product = priv->m_downloadDesc->productList();
    // set OMA2 parent values in QSettings.
    QString productName = product[priv->m_currProductIndex]->getAttribute(OMA2DownloadDescProductName).toString(); // contenttype,url being written in dlbackend's constructor
    setValue(DownloadInfo::EFileName, productName);

    // set minimal MediaObject attributes in QSettings like name, size, url and contentType as these are the ones required for persistent storage.
    QString mediaObjName = product[priv->m_currProductIndex]->mediaObjList()[index]->getAttribute(OMA2DownloadDescMediaObjName).toString();
    setValueForChild(DownloadInfo::EFileName, mediaObjName, dl->id());
    long mediaObjSize = product[priv->m_currProductIndex]->mediaObjList()[index]->getAttribute(OMA2DownloadDescMediaObjSize).toInt();
    setValueForChild(DownloadInfo::ETotalSize, mediaObjSize, dl->id());
    QString mediaObjContentType = product[priv->m_currProductIndex]->mediaObjList()[index]->getAttribute(OMA2DownloadDescMediaObjType).toString();
    setValueForChild(DownloadInfo::EContentType, mediaObjContentType, dl->id());
    QString destPath = (download()->attributes().value(DlDestPath)).toString();
    setValueForChild(DownloadInfo::EFinalPath, destPath, dl->id());
    setValueForChild(DownloadInfo::EETag, priv->m_downloadCore->entityTag(), dl->id()); //not getting set check later
}
