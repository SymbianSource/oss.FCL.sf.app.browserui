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

#include "clientdownload.h"
#include "downloadbackend.h"
#include "downloadfactory.h"
#include "downloadcoremanager.h"
#include "downloadcore.h"
#include "downloadmanager.h"
#include "downloadevent.h"
#include "downloadinfo.h"
#include "sequentialdownloadmanager.h"
#include <QNetworkReply>
#include <QFileInfo>
#include <QCoreApplication>

#define PROGRESS_MINKB 5120 //Minimum KB to send progress events

// private implementation
class ClientDownloadPrivate
{
    DM_DECLARE_PUBLIC(ClientDownload);
public:
    ClientDownloadPrivate();
    ~ClientDownloadPrivate();
    int m_downloadId;
    QString m_url;  // url
    DownloadManager *m_downloadManager; // not owned, only reference
    DownloadBackend *m_downloadBackend;
    DownloadCore *m_downloadCore; // not owned
    EventReceiverList m_eventReceiverList; // list of event listeners
    DownloadInfo *m_dlInfo;         // not owned  
    bool m_createdByDlInfo; // flag to indicate persistant ClientDownload
    int m_parentId; // to set parent id
    QMap<DownloadAttribute, QVariant> m_downloadAttrMap; // the map of attributes can be set by client
};

ClientDownloadPrivate::ClientDownloadPrivate():m_downloadId(-1)
                                              ,m_url("")
                                              ,m_downloadManager(0)
                                              ,m_downloadBackend(0)
                                              ,m_downloadCore(0)
                                              ,m_dlInfo(0)
                                              ,m_createdByDlInfo(false)
                                              ,m_parentId(INVALID_DL_ID)
{ }

ClientDownloadPrivate::~ClientDownloadPrivate()
{
    if(m_downloadBackend)
    {
         delete m_downloadBackend;
         m_downloadBackend = 0;
    }
}

/*!
 * \class ClientDownload
 *
 * \brief The public APIs for managing a ClientDownload
 *
 * This class has the public APIs for managing a single ClientDownload
 */

// note that parentdlId is id of the parent download which is applicable in the case
// where parent download is oma download and child is media download
ClientDownload::ClientDownload(DownloadManager *mgr, const QString& url
                                                   , int dlId
                                                   , DownloadType type/*=Parallel*/
                                                   , int parentdlId /*=INVALID_DL_ID*/)
{
    DM_INITIALIZE(ClientDownload);
    priv->m_downloadId = dlId;
    priv->m_downloadManager = mgr;
    priv->m_url = url;
    priv->m_dlInfo = priv->m_downloadManager->downloadInfo();
    if (parentdlId > INVALID_DL_ID)
        setParentId(parentdlId);

    // set the default destination path
    priv->m_downloadAttrMap.insert(DlDestPath, mgr->getAttribute(DlMgrDestPath));
    // set the default filename from url
    QUrl urlToDownload(url);
    QFileInfo fileUrl(urlToDownload.path());
    QString fileName = fileUrl.fileName();
    priv->m_downloadAttrMap.insert(DlFileName, fileName);
    priv->m_dlInfo->setValue(priv->m_downloadId, DownloadInfo::EFileName, fileName, parentdlId);
    // set 5Kb as minimum size to send the progress event
    priv->m_downloadAttrMap.insert(DlProgressInterval, PROGRESS_MINKB);
    // set the download type
    priv->m_downloadAttrMap.insert(DlDownloadType, type);
    priv->m_dlInfo->setValue(priv->m_downloadId, DownloadInfo::EType, (long)type, parentdlId);
    // set the download scope
    priv->m_downloadAttrMap.insert(DlDownloadScope, Normal);
    priv->m_dlInfo->setValue(priv->m_downloadId, DownloadInfo::EScope, (long)Normal, parentdlId);

    // create ClientDownload core for network transactions
    priv->m_downloadCore = priv->m_downloadManager->downloadCoreManager()->createDownloadCore(url);
    // set the proxy
    priv->m_downloadCore->setProxy(mgr->proxy());
    connect(priv->m_downloadCore, SIGNAL(metaDataChanged()), this, SLOT(createDownloadImplementation()));
}

// note that parentdlId is id of the parent download which is applicable in the case
// where parent download is oma download and child is media download
ClientDownload::ClientDownload(DownloadManager *mgr, QNetworkReply *reply
                                                   , int dlId
                                                   , int parentdlId /*=INVALID_DL_ID*/)
{
    DM_INITIALIZE(ClientDownload);
    priv->m_downloadManager = mgr;
    priv->m_downloadId = dlId;
    priv->m_dlInfo = priv->m_downloadManager->downloadInfo();
    if (parentdlId > INVALID_DL_ID)
        setParentId(parentdlId);

    // set the default destination path
    priv->m_downloadAttrMap.insert(DlDestPath, mgr->getAttribute(DlMgrDestPath));
    if (reply) {
        // set the default filename from url
        QUrl urlToDownload(reply->url());
        QFileInfo fileUrl(urlToDownload.path());
        QString fileName = fileUrl.fileName();
        priv->m_downloadAttrMap.insert(DlFileName, fileName);
        priv->m_dlInfo->setValue(priv->m_downloadId, DownloadInfo::EFileName, fileName, parentdlId);
    }
    // set 5Kb as minimum size to send the progress event
    priv->m_downloadAttrMap.insert(DlProgressInterval, PROGRESS_MINKB);
    // set the download type as parallel
    priv->m_downloadAttrMap.insert(DlDownloadType, Parallel);
    priv->m_dlInfo->setValue(priv->m_downloadId, DownloadInfo::EType, (long)Parallel, parentdlId);
        // set the download scope
    priv->m_downloadAttrMap.insert(DlDownloadScope, Normal);
    priv->m_dlInfo->setValue(priv->m_downloadId, DownloadInfo::EScope, (long)Normal, parentdlId);
    // create ClientDownload core for network transactions
    priv->m_downloadCore = priv->m_downloadManager->downloadCoreManager()->createDownloadCore(reply);
    // create actual implementation class based on content type of the ClientDownload
    priv->m_downloadBackend = DownloadAbstractFactory::createDownloadImplementation(priv->m_downloadCore, this);
    priv->m_downloadBackend->setStartTime();
 }

// this is the case for reading the persistant  ClientDownload information of last session and 
// creating ClientDownload object
ClientDownload::ClientDownload(DownloadManager *mgr, int dlId, int parentdlId)
{
    DM_INITIALIZE(ClientDownload);
    priv->m_downloadManager = mgr;
    priv->m_downloadId = dlId;
    priv->m_createdByDlInfo =  true;
    priv->m_dlInfo = priv->m_downloadManager->downloadInfo();
    if (parentdlId > INVALID_DL_ID)
         setParentId(parentdlId);

    // read the path from persistant
    QString path;
    priv->m_dlInfo->getValue(priv->m_downloadId, DownloadInfo::EFinalPath, path, parentdlId);
    priv->m_downloadAttrMap.insert(DlDestPath, path);

    // set 5Kb as minimum size to send the progress event
    priv->m_downloadAttrMap.insert(DlProgressInterval, PROGRESS_MINKB);
    
    // set the download type parallel/sequential
    long type;
    priv->m_dlInfo->getValue(priv->m_downloadId, DownloadInfo::EType, type, parentdlId);
    priv->m_downloadAttrMap.insert(DlDownloadType, (DownloadType)type);

    // set the priority
    long priority;
    priv->m_dlInfo->getValue(priv->m_downloadId, DownloadInfo::EPriority, priority, parentdlId);
    priv->m_downloadAttrMap.insert(DlPriority, (DownloadPriority)priority);

    // fetching url
    QString url;
    priv->m_dlInfo->getValue(priv->m_downloadId, DownloadInfo::EUrl, url, parentdlId);
    // create ClientDownload core for network transactions
    priv->m_downloadCore = priv->m_downloadManager->downloadCoreManager()->createDownloadCore(url);
    // set the proxy
    QNetworkProxy *proxy = priv->m_downloadCore->proxy();
    if(!proxy && priv->m_downloadCore && priv->m_downloadManager) {
         //set ClientDownload core's proxy as ClientDownload manager's proxy, if its not already set
         priv->m_downloadCore->setProxy(priv->m_downloadManager->proxy());
    }
    // fetch the content type
    QString contentType;
    priv->m_dlInfo->getValue(priv->m_downloadId, DownloadInfo::EContentType, contentType, parentdlId);
    priv->m_downloadCore->setContentType(contentType);

    // fetch ETag header value
    QString entityTag;
    priv->m_dlInfo->getValue(priv->m_downloadId, DownloadInfo::EETag, entityTag, parentdlId);
    priv->m_downloadCore->setEntityTag(entityTag);

    // create actual implementation class based on content type of the ClientDownload
    priv->m_downloadBackend = DownloadAbstractFactory::createDownloadImplementation(priv->m_downloadCore, this);
    priv->m_downloadBackend->init();

    long size = 0;
    priv->m_dlInfo->getValue(priv->m_downloadId, DownloadInfo::ETotalSize, size, parentdlId);
    priv->m_downloadBackend->setTotalSize(size);

    long state = 0;
    priv->m_dlInfo->getValue(priv->m_downloadId, DownloadInfo::EDlState, state, parentdlId);
    if (state == DlCompleted) 
        priv->m_downloadBackend->setDownloadedDataSize(size);
    else {
        // fetch the already downloaded data size
        qint64 currentSize = priv->m_downloadBackend->storedDataSize();    
        priv->m_downloadBackend->setDownloadedDataSize(currentSize);
    }
    
    if (state  == DlInprogress)
        priv->m_downloadBackend->setDownloadState(DlPaused);
    else
        priv->m_downloadBackend->setDownloadState((DownloadState)state);       
}
 
ClientDownload::~ClientDownload()
{ 
    DM_UNINITIALIZE(ClientDownload);
}

/*!
  returns id of the ClientDownload
*/
int ClientDownload::id()
{
    DM_PRIVATE(ClientDownload);
    return priv->m_downloadId;
}

/*!
  starts the ClientDownload, returns the success status
*/
int ClientDownload::start()
{
    DM_PRIVATE(ClientDownload);
    DownloadType type = (DownloadType)((priv->m_downloadAttrMap.value(DlDownloadType)).toInt());
    if(type == Sequential)
        priv->m_downloadManager->sequentialManager()->process(priv->m_downloadId);
    else
        startDownload(); // starts the download parallely
    return 0;
}

/*!
  sets the attribute for the ClientDownload
  \a attr indicates attribute
  \a value indicates value for the ClientDownload
*/
int ClientDownload::setAttribute(DownloadAttribute attr, const QVariant& value)
{
    //sets the attribute to ClientDownload backend
    DM_PRIVATE(ClientDownload);
    switch(attr)
    {
        case DlFileName:
        case DlDestPath:
        {            
            // download is just created but not started
            // filename, destination path can be set only before the download has started
            if(!priv->m_downloadBackend)
            {
                QString strValue = value.toString();       
                if(strValue.length() != 0)
                    priv->m_downloadAttrMap.insert(attr, value);
                return 0;
            }
            else
                return -1;
        }
        case DlPriority:
            {
                // cannot change the priority once the download has started
                if(!priv->m_downloadBackend)
                {
                    priv->m_downloadAttrMap.insert(attr, value);
                    priv->m_dlInfo->setValue(priv->m_downloadId, DownloadInfo::EPriority, (long)(value.toInt()), parentId());
                    // reshuffle the download queue based on the priority
                    if((DownloadType)(priv->m_downloadAttrMap.value(DlDownloadType).toInt()) == Sequential)
                        priv->m_downloadManager->sequentialManager()->addToSequentialDownload(this);
                    return 0;
                }
            }
        case DlProgressInterval:
        {
            qlonglong val = value.toLongLong() * 1024;
            if (val >= PROGRESS_MINKB) {
                priv->m_downloadAttrMap.insert(attr, val);
                return 0;
            }
        }
                
        default:
            if(priv->m_downloadBackend)
                return priv->m_downloadBackend->setAttribute(attr, value);
    }
    return 0;
}

/*!
  fetches the attribute of the ClientDownload
  \a attr indicates ClientDownload attribute
*/
QVariant ClientDownload::getAttribute(DownloadAttribute attr)
{
    //gets attribute
    DM_PRIVATE(ClientDownload);
    switch(attr)
    {
        case DlFileName:
        {
            if(priv->m_downloadBackend) // download is already started
                return priv->m_downloadBackend->getAttribute(attr);
            else
                return priv->m_downloadAttrMap.value(DlFileName);
        }
        case DlDestPath:
        {
            return priv->m_downloadAttrMap.value(DlDestPath);
        }
        case DlPriority:
        {
            return priv->m_downloadAttrMap.value(DlPriority);
        }
        case DlDownloadType:
        {
            return priv->m_downloadAttrMap.value(DlDownloadType);
        }
        case DlProgressInterval:
        {
            qlonglong val = priv->m_downloadAttrMap.value(DlProgressInterval).toLongLong() / 1024;
            return val;
        }
        default:
        {
            if(priv->m_downloadBackend)
                return priv->m_downloadBackend->getAttribute(attr);
        }
    }  
    return QVariant();
}

/*!
  pauses the ClientDownload
*/
int ClientDownload::pause()
{
    DM_PRIVATE(ClientDownload);
    DownloadType type = (DownloadType)((priv->m_downloadAttrMap.value(DlDownloadType)).toInt());
    if(type == Sequential)
        priv->m_downloadManager->sequentialManager()->pauseDownload(priv->m_downloadId);
    else
        pauseDownload(); // pauses the download parallely
    return 0;
}

/*!
  resumes the ClientDownload
*/
int ClientDownload::resume()
{
    DM_PRIVATE(ClientDownload);
    DownloadType type = (DownloadType)((priv->m_downloadAttrMap.value(DlDownloadType)).toInt());
    if(type == Sequential)
        priv->m_downloadManager->sequentialManager()->resumeDownload(priv->m_downloadId);
    else
        resumeDownload(); // resumes the download parallely
    return 0;
}

/*!
  cancels the ClientDownload
*/
int ClientDownload::cancel()
{
    DM_PRIVATE(ClientDownload);
    DownloadType type = (DownloadType)((priv->m_downloadAttrMap.value(DlDownloadType)).toInt());
    if(type == Sequential)
        priv->m_downloadManager->sequentialManager()->cancelDownload(priv->m_downloadId);
    else
        cancelDownload(); // cancels the download parallely
    return 0;
}

/*!
  registers receiver for the ClientDownload events
  \a reciever indicates reciever which listen to ClientDownload events
*/
void ClientDownload::registerEventReceiver(QObject *receiver)
{
    DM_PRIVATE(ClientDownload);
    if(receiver)
        if (!priv->m_eventReceiverList.contains(receiver))
            priv->m_eventReceiverList.append(receiver);
}

/*!
  unregisters the event listener
  \a receiver indicates listener which will be unregistered
*/
void ClientDownload::unregisterEventReceiver(QObject *receiver)
{
    DM_PRIVATE(ClientDownload);
    priv->m_eventReceiverList.removeOne(receiver);
}

EventReceiverList& ClientDownload::eventReceivers()
{
    DM_PRIVATE(ClientDownload);
    return priv->m_eventReceiverList;
}

/*!
  returns ClientDownload manager
*/
DownloadManager* ClientDownload::downloadManager()
{
    DM_PRIVATE(ClientDownload);
    return priv->m_downloadManager;
}

/*!
  returns the child downloads i.e if ClientDownload has any media objects
  \a list indicates list of child downloads
*/
void ClientDownload::getChildren(QList<Download*>& list)
{
    DM_PRIVATE(ClientDownload);
    if(priv->m_downloadBackend)
       priv->m_downloadBackend->getChildren(list);
}

DownloadInfo* ClientDownload::downloadInfo()
{
    DM_PRIVATE(ClientDownload);
    return priv->m_dlInfo;
}

void ClientDownload::createDownloadImplementation()
{
    DM_PRIVATE(ClientDownload);
    //ClientDownload backend is created based on the content type
    if(!priv->m_downloadBackend) {
        priv->m_downloadBackend = DownloadAbstractFactory::createDownloadImplementation(priv->m_downloadCore, this);
        priv->m_downloadBackend->setStartTime();
        postEvent(HeaderReceived, NULL);
    }
}

void ClientDownload::postEvent(DEventType type, DlEventAttributeMap* attrMap)
{
    DM_PRIVATE(ClientDownload);
    EventReceiverList list = eventReceivers();
    for(int i=0; i<list.size(); i++) {
        if(list[i]) {
            DownloadEvent *event = new DownloadEvent(type, attrMap, priv->m_downloadId);
            QCoreApplication::postEvent(list[i], event);
        }
    }
}

void ClientDownload::setError(const QString& errorStr)
{
    DM_PRIVATE(ClientDownload);
    priv->m_downloadCore->setLastErrorString(errorStr);
    priv->m_downloadCore->setLastError(QNetworkReply::UnknownContentError);
}

void ClientDownload::setDownloadState(DownloadState state)
{
    DM_PRIVATE(ClientDownload);
    priv->m_downloadBackend->setDownloadState(state);
}

bool ClientDownload::isCreatedByDlInfo(void)
{
    DM_PRIVATE(ClientDownload);
    return priv->m_createdByDlInfo;
}

void ClientDownload::setParentId(int parentId)
{
    DM_PRIVATE(ClientDownload);
    priv->m_parentId = parentId;
}

int ClientDownload::parentId()
{
    DM_PRIVATE(ClientDownload);
    return priv->m_parentId;
}

QMap<DownloadAttribute, QVariant>& ClientDownload::attributes(void)
{
    DM_PRIVATE(ClientDownload);
    return priv->m_downloadAttrMap;
}

// starts the ClientDownload, returns the success status
int ClientDownload::startDownload()
{
    DM_PRIVATE(ClientDownload);
    if (!(priv->m_downloadCore))
        return -1;  
    QNetworkProxy *proxy = priv->m_downloadCore->proxy();
    if(!proxy && priv->m_downloadManager) {
         //set ClientDownload core's proxy as ClientDownload manager's proxy, if its not already set
         priv->m_downloadCore->setProxy(priv->m_downloadManager->proxy());
    }
    priv->m_downloadCore->doDownload();

    return 0;
}

int ClientDownload::pauseDownload()
{
    DM_PRIVATE(ClientDownload);
    if( priv->m_downloadBackend ) {
        DownloadState state = (DownloadState)priv->m_downloadBackend->getAttribute(DlDownloadState).toInt();
        if( state != DlInprogress)
            return 0;
        return priv->m_downloadBackend->pause();
    }
    else
        priv->m_downloadCore->abort();
    return 0;
}

int ClientDownload::resumeDownload()
{
    DM_PRIVATE(ClientDownload);
    QNetworkProxy *proxy = priv->m_downloadCore->proxy();
    if(!proxy && priv->m_downloadCore && priv->m_downloadManager) {
         //set ClientDownload core's proxy as ClientDownload manager's proxy, if its not already set
         priv->m_downloadCore->setProxy(priv->m_downloadManager->proxy());
    }
    if(priv->m_downloadBackend) {
        DownloadState state = (DownloadState)priv->m_downloadBackend->getAttribute(DlDownloadState).toInt();
        if((state != DlPaused) && (state != DlCancelled))
            return 0;
        return priv->m_downloadBackend->resume();
    }
    else
        startDownload(); // This means, ClientDownload has just been created but never started
    return 0;
}

int ClientDownload::cancelDownload()
{
    // cancels the ClientDownload
    DM_PRIVATE(ClientDownload);
    if( priv->m_downloadBackend ) {
        DownloadState state = (DownloadState)priv->m_downloadBackend->getAttribute(DlDownloadState).toInt();
        if((state == DlCompleted) || (state == DlFailed) || (state == DlCancelled))
            return 0; 
        return priv->m_downloadBackend->cancel();
    }
    return 0;
}

