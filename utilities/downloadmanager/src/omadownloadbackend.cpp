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

#include "omadownloadbackend.h"
#include "omaddparser.h"
#include "downloadmanager.h"
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
        int blockSize = fiData.f_bsize/1024;
        double freeSize = (fiData.f_bavail)*blockSize;
        freeSize = freeSize/1024;
        return freeSize;
    }
#endif
    return 0;
}

// private implementation
class OMADownloadBackendPrivate {
    DM_DECLARE_PUBLIC(OMADownloadBackend);
public:
    OMADownloadBackendPrivate();
    ~OMADownloadBackendPrivate();

    DownloadCore *m_downloadCore;
    ClientDownload *m_download;
    OMADownloadDescParser *m_parser; // for parsing oma descriptor
    OMADownloadDescriptor *m_downloadDesc;
    bool m_isMediaDownload; // flag to indicate if media download is happening
    ClientDownload *m_mediaDownload; // not owned
    bool m_isUserCancelled;
};

OMADownloadBackendPrivate::OMADownloadBackendPrivate()
{
    m_downloadCore = 0;
    m_download = 0;
    m_parser = 0;
    m_downloadDesc = 0;
    m_isMediaDownload = false;
    m_mediaDownload = 0;
    m_isUserCancelled = false;
}

OMADownloadBackendPrivate::~OMADownloadBackendPrivate()
{
    if (m_parser) {
        delete m_parser;
        m_parser = 0;
        m_downloadDesc = 0;
    }
    if (m_mediaDownload) {
        delete m_mediaDownload;
        m_mediaDownload = NULL;
    }
}

OMADownloadBackend::OMADownloadBackend(DownloadCore *dlCore, ClientDownload *dl)
    :DownloadBackend(dlCore, dl)
{
    DM_INITIALIZE(OMADownloadBackend);
    priv->m_downloadCore = dlCore;
    priv->m_download = dl;
}

OMADownloadBackend::~OMADownloadBackend()
{
     DM_UNINITIALIZE(OMADownloadBackend);
}

int OMADownloadBackend::pause()
{
    DM_PRIVATE(OMADownloadBackend);
    if (priv->m_isMediaDownload) {
        priv->m_mediaDownload->pause();
        setDownloadState(DlPaused);
    }
    return 0;
}

int OMADownloadBackend::resume()
{
    DM_PRIVATE(OMADownloadBackend);
    priv->m_isUserCancelled = false;
    if (priv->m_isMediaDownload)
        return (priv->m_mediaDownload->resume());

    // Ready to download after the descriptor is parsed
    // capability check on the descriptor information
    if (checkDownloadDescriptor()) {
         // create download for media object
        priv->m_mediaDownload = new ClientDownload(priv->m_download->downloadManager(), 
                priv->m_downloadDesc->getAttribute(OMADownloadDescObjectURI).toString(), priv->m_download->id()); 

        if (!priv->m_mediaDownload) {
            deleteInfo(); 
            return -1;               
        }
        priv->m_mediaDownload->setAttribute(DlFileName, priv->m_downloadDesc->getAttribute(OMADownloadDescName));
        priv->m_mediaDownload->registerEventReceiver(this);
        priv->m_mediaDownload->start();
        priv->m_isMediaDownload = true; 
        deleteInfo(); 
        return 0;         
    }
    return -1;
}

int OMADownloadBackend::cancel()
{
    DM_PRIVATE(OMADownloadBackend);
    priv->m_isUserCancelled = true;
    if (priv->m_isMediaDownload)
        priv->m_mediaDownload->cancel();
    else
        DownloadBackend::cancel();
    return 0;
}

QVariant OMADownloadBackend::getAttribute(DownloadAttribute attr)
{
    DM_PRIVATE(OMADownloadBackend);
    switch(attr) {
    case OMADownloadDescriptorName:
        return priv->m_downloadDesc->getAttribute(OMADownloadDescName);
    case OMADownloadDescriptorVersion:
        return priv->m_downloadDesc->getAttribute(OMADownloadDescVersion);
    case OMADownloadDescriptorType:
        return priv->m_downloadDesc->getAttribute(OMADownloadDescType);
    case OMADownloadDescriptorSize:
        return priv->m_downloadDesc->getAttribute(OMADownloadDescSize);
    case OMADownloadDescriptorVendor:
        return priv->m_downloadDesc->getAttribute(OMADownloadDescVendor);
    case OMADownloadDescriptorDescription:
        return priv->m_downloadDesc->getAttribute(OMADownloadDescDescription);

    case DlFileName:
    {
        if (!priv->m_isMediaDownload) {
            QString url = priv->m_downloadCore->url();
            QFileInfo fileUrl(url);
            return QVariant(fileUrl.fileName());
        } else
            return priv->m_downloadDesc->getAttribute(OMADownloadDescName);
    }
    case DlContentType:
    {
        if (!priv->m_isMediaDownload)
            return DownloadBackend::getAttribute(DlContentType);
            
        else
            return priv->m_mediaDownload->getAttribute(DlContentType);
    }
    default:
        return DownloadBackend::getAttribute(attr);
    }    
    return QVariant();
}

int OMADownloadBackend::setAttribute(DownloadAttribute attr, const QVariant& value)
{
    return DownloadBackend::setAttribute(attr, value);
}

// stores the data in storage
void OMADownloadBackend::store(QByteArray /*data*/, bool /*lastChunk=false*/)
{
    return;
}

// deletes the storage
void OMADownloadBackend::deleteStore()
{
    return;
}

// returns the size of stored data
qint64 OMADownloadBackend::storedDataSize()
{
    return 0;
}

void OMADownloadBackend::bytesRecieved(qint64 /*bytesRecieved*/, qint64 /*bytesTotal*/)
{
     //Do nothing. This is here to avoid this signal to reach to base class' slot.
     return;
}

void OMADownloadBackend::bytesUploaded(qint64 bytesUploaded, qint64 bytesTotal)
{
    DM_PRIVATE(OMADownloadBackend); 
    // once data is uploaded, cancel the transaction
    if (bytesUploaded == bytesTotal)
        priv->m_downloadCore->abort() ;
}

void OMADownloadBackend::handleFinished()
{
     DM_PRIVATE(OMADownloadBackend);
     QString contentType = priv->m_downloadCore->contentType();
     if (contentType == OMA_CONTENT_TYPE) {
         bool bSucceeded = parseDownloadDescriptor();
         if (bSucceeded) {
             priv->m_downloadDesc = priv->m_parser->downloadDescriptor();
             
             QString objectURI = priv->m_downloadDesc->getAttribute(OMADownloadDescObjectURI).toString();
             if (objectURI.isEmpty()) {
                 priv->m_downloadCore->post(priv->m_downloadDesc->getAttribute(OMADownloadDescInstallNotifyURI).toString(), QByteArray("905 Attribute Mismatch"));
                 priv->m_downloadCore->setLastError(QNetworkReply::UnknownContentError);
                 priv->m_downloadCore->setLastErrorString(tr("No ObjectURI"));
                 setDownloadState(DlFailed);
                 postEvent(Error, NULL);
                 return;
             }
             setDownloadState(DlPaused);
             postEvent(OMADownloadDescriptorReady, NULL);
         } else {
             priv->m_downloadDesc = priv->m_parser->downloadDescriptor();
             priv->m_downloadCore->post(priv->m_downloadDesc->getAttribute(OMADownloadDescInstallNotifyURI).toString(), QByteArray("906 Invalid descriptor"));
             priv->m_downloadCore->setLastError(QNetworkReply::UnknownContentError);
             priv->m_downloadCore->setLastErrorString(tr("Invalid Descriptor"));
             setDownloadState(DlFailed);
             postEvent(Error, NULL);
         }
     }
}
          
bool OMADownloadBackend::parseDownloadDescriptor()
{
    DM_PRIVATE(OMADownloadBackend);
    priv->m_parser = new OMADownloadDescParser();
    QXmlInputSource source(priv->m_downloadCore->reply());
    QXmlSimpleReader reader;
    reader.setContentHandler(priv->m_parser);
    reader.setErrorHandler(priv->m_parser);
    return reader.parse(source);
}

// capability check on the descriptor
bool OMADownloadBackend::checkDownloadDescriptor()
{
    DM_PRIVATE(OMADownloadBackend);

    QString version = priv->m_downloadDesc->getAttribute(OMADownloadDescVersion).toString();
    if (! version.isEmpty() && (version != OMA_VERSION_1)) {
        priv->m_downloadCore->post(priv->m_downloadDesc->getAttribute(OMADownloadDescInstallNotifyURI).toString(), QByteArray("951 Invalid DDVersion"));
        priv->m_downloadCore->setLastError(QNetworkReply::UnknownContentError);
        priv->m_downloadCore->setLastErrorString(tr("951 Invalid DDVersion"));
        setDownloadState(DlFailed); 
        postEvent(Error, NULL);
        return false; 
    } 

#ifdef Q_OS_LINUX
    // "Size" check needs to be done
    double fileSize =  priv->m_downloadDesc->getAttribute(OMADownloadDescSize).toDouble();
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

bool OMADownloadBackend::event(QEvent *event)
{
    DM_PRIVATE(OMADownloadBackend);
    DEventType type = (DEventType)event->type();
    switch(type) {
    case Started:
        break;
    case HeaderReceived:
    {
        // handling the events from media object downloads
        // Check the mismatch in total size returned by server with the size given in the descriptor.
        //This piece of code is commented as-of-now. when needed in future , will be uncommented
        /*int totalSize = dl->getAttribute(DlTotalSize).toInt();
        if (priv->m_downloadDesc->getAttribute(OMADownloadDescSize).toInt() != totalSize)
        {
            priv->m_downloadCore->post(priv->m_downloadDesc->getAttribute(OMADownloadDescInstallNotifyURI).toString(), QByteArray("905 Attribute Mismatch"));
             
            if (dl)
            {
                dl->setError("905 Attribute Mismatch");
                dl->cancel();
                dl->setDownloadState(DlFailed);
                dl->postEvent(Error, NULL);
            } 
            return true;
        }*/
        // Check the mismatch in content type returned by server with the content type given in the descriptor.
        QString contentType = priv->m_mediaDownload->getAttribute(DlContentType).toString();
        if (priv->m_downloadDesc->getAttribute(OMADownloadDescType).toString() != contentType) {
            // media object download cannot be proceeded
            if (priv->m_mediaDownload) {
                priv->m_mediaDownload->setError("905 Attribute Mismatch");
                priv->m_mediaDownload->cancel();
                priv->m_mediaDownload->setDownloadState(DlFailed);
                priv->m_mediaDownload->postEvent(Error, NULL);
            } 
            break;
        } 
    }
    case Progress:
    {
        QVariant tSize = priv->m_mediaDownload->getAttribute(DlTotalSize);
        setTotalSize(tSize.toInt());
        QVariant curDlsize = priv->m_mediaDownload->getAttribute(DlDownloadedSize);
        setDownloadedDataSize(curDlsize.toInt());
        setDownloadState(DlInprogress);
        postEvent(Progress, NULL);
        break;
    }
    case NetworkLoss:
    {
        postEvent(NetworkLoss, NULL);
        break;
    }
    case Cancelled:
    {
        if (priv->m_isUserCancelled)
            priv->m_downloadCore->post(priv->m_downloadDesc->getAttribute(OMADownloadDescInstallNotifyURI).toString(), QByteArray("902 User Cancelled"));
        setDownloadState(DlCancelled);
        postEvent(Cancelled, NULL);
        break;
    }
    case Completed:
    {
        priv->m_downloadCore->post(priv->m_downloadDesc->getAttribute(OMADownloadDescInstallNotifyURI).toString(), QByteArray("900 Success "));
        setDownloadState(DlCompleted);
        postEvent(Completed, NULL); 
        break;
    }
    case Error:
    {
        priv->m_downloadCore->post(priv->m_downloadDesc->getAttribute(OMADownloadDescInstallNotifyURI).toString(), QByteArray("905 Attribute Mismatch"));
        priv->m_downloadCore->setLastError(QNetworkReply::UnknownContentError);
        priv->m_downloadCore->setLastErrorString(tr("905 Attribute Mismatch"));
        setDownloadState(DlFailed);
        postEvent(Error, NULL);
        break;
    } 
    default:
        break;
    }
    return true;
}


