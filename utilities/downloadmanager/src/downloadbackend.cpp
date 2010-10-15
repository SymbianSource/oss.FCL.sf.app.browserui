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
#include "downloadmanager.h"
#include "downloadbackend.h"
#include "downloadcore.h"
#include "downloadfactory.h"
#include "downloadstore.h"
#include <QCoreApplication>
#include <QDateTime>

//private implementation
class DownloadBackendPrivate
{
    DM_DECLARE_PUBLIC(DownloadBackend);
public:
    DownloadBackendPrivate();
    ~DownloadBackendPrivate();

    DownloadCore *m_downloadCore; // not owned
    ClientDownload *m_download; //// not owned, only reference 
    DownloadInfo *m_dlInfo; // not owned
    qint64 m_totalSize; // total size of the download
    qint64 m_currentDownloadedSize; // current downloaded size
    qint64 m_lastPausedSize;
    DownloadState m_downloadState; // state of the download
    bool m_infoDeleted; // flag to indicate the info deletion    
    QDateTime m_startTime; // download start/resumed time
    QDateTime m_endTime; // download completed time
    int m_progressCounter;
};  

DownloadBackendPrivate::DownloadBackendPrivate()
{
    m_downloadCore = 0;
    m_download = 0;
    m_dlInfo = 0;
    m_totalSize = 0;
    m_currentDownloadedSize = 0;
    m_lastPausedSize =0;
    m_infoDeleted = false;
    m_progressCounter = 1;
}

DownloadBackendPrivate::~DownloadBackendPrivate()
{
#if 0 //m_downloadCore may be stale.
    if(m_downloadCore)
    {     
        // cancel if there is any transaction
        m_downloadCore->abort();
    }
#endif
}

DownloadBackend::DownloadBackend(DownloadCore *dlCore, ClientDownload* dl)
{
    DM_INITIALIZE(DownloadBackend);
    priv->m_downloadCore = dlCore;
    priv->m_download = dl;
    priv->m_dlInfo = dl->downloadInfo();
    // connect all the signals from network
    connect(dlCore, SIGNAL(downloadProgress(qint64 , qint64 )), this, SLOT(bytesRecieved(qint64 , qint64 )));
    connect(dlCore, SIGNAL(finished()), this, SLOT(handleFinished()));
    connect(dlCore, SIGNAL(metaDataChanged()), this, SLOT(headerReceived()));
    connect(dlCore, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
    connect(dlCore, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(bytesUploaded(qint64, qint64)));

    // save the content type and url
    setValue(DownloadInfo::EContentType, priv->m_downloadCore->contentType()); 
    setValue(DownloadInfo::EUrl, priv->m_downloadCore->url()); 
    setValue(DownloadInfo::EETag, priv->m_downloadCore->entityTag());
    postEvent(Started, NULL);
}

DownloadBackend::~DownloadBackend()
{
    DM_UNINITIALIZE(DownloadBackend);
}

int DownloadBackend::pause()
{
    DM_PRIVATE(DownloadBackend);
    setDownloadState(DlPaused);
    priv->m_downloadCore->abort();
    return 0;
}

int DownloadBackend::resume()
{
    DM_PRIVATE(DownloadBackend);
    setDownloadState(DlInprogress);
    // save the content type and url
    setValue(DownloadInfo::EUrl, priv->m_downloadCore->url());
    setValue(DownloadInfo::EContentType, priv->m_downloadCore->contentType());
    priv->m_lastPausedSize = priv->m_currentDownloadedSize;
    priv->m_downloadCore->resumeDownload(priv->m_currentDownloadedSize);
    priv->m_startTime = QDateTime::currentDateTime();
    postEvent(Progress, NULL);
    return 0;
}

int DownloadBackend::cancel()
{
    DM_PRIVATE(DownloadBackend);   
    setDownloadState(DlCancelled);
    // cancel the transaction
    priv->m_downloadCore->abort();
    // delete the temporary storage
    deleteStore();
    // reset the states
    priv->m_totalSize = 0;
    priv->m_currentDownloadedSize = 0;
    priv->m_lastPausedSize = 0;
    return 0;
}

QVariant DownloadBackend::getAttribute(DownloadAttribute attr)
{
    DM_PRIVATE(DownloadBackend); 
    switch(attr)
    {
         case DlDownloadedSize:
         {
             return QVariant(priv->m_currentDownloadedSize);
         }
         case DlTotalSize:
         {
             return QVariant(priv->m_totalSize);
         }
         case DlDownloadState:
         {
             return QVariant(priv->m_downloadState);
         }
         case DlSourceUrl:
         {
             return QVariant(priv->m_downloadCore->url());
         }
         case DlContentType:
         {
             return QVariant(priv->m_downloadCore->contentType());
         }
         case DlStartTime:
         {
             return QVariant(priv->m_startTime);
         }
         case DlEndTime:
         {
             if (priv->m_downloadState == DlCompleted)
                 return priv->m_endTime;
             else 
                 return QVariant();
         }
         case DlElapsedTime:
         {
             if (priv->m_downloadState == DlCompleted)
                 return QVariant((priv->m_endTime.toTime_t()-priv->m_startTime.toTime_t()));

             if (priv->m_downloadState != DlInprogress)
                 return QVariant();
             QDateTime currentTime = QDateTime::currentDateTime();
             uint timeElasped = currentTime.toTime_t() - priv->m_startTime.toTime_t();
             return QVariant(timeElasped);
         }
         case DlRemainingTime:
         {
             if (priv->m_downloadState != DlInprogress)
                 return QVariant();
             QDateTime currentTime = QDateTime::currentDateTime();
             uint timeElasped = currentTime.toTime_t() - priv->m_startTime.toTime_t();
             // total bytes recieved since last start/resume
             qint64 totalBytesRecieved = priv->m_currentDownloadedSize - priv->m_lastPausedSize;
             qint64 remainingSize = priv->m_totalSize - priv->m_currentDownloadedSize;
             if (totalBytesRecieved > 0)
                 return QVariant((timeElasped*remainingSize)/totalBytesRecieved);
             else
                 return QVariant();
         }
         case DlSpeed:
         {
             if (priv->m_downloadState != DlInprogress)
                 return QVariant();
             QDateTime currentTime = QDateTime::currentDateTime();
             uint timeElasped = currentTime.toTime_t() - priv->m_startTime.toTime_t();
             qint64 totalBytesRecieved = priv->m_currentDownloadedSize - priv->m_lastPausedSize;
             if (timeElasped > 0)
                 return QVariant(totalBytesRecieved/timeElasped);
             else
                 return QVariant();
         }
         case DlPercentage:
         {
             if (priv->m_totalSize > 0)
                 return QVariant((priv->m_currentDownloadedSize*100)/priv->m_totalSize);
             else
                 return QVariant();
         }
         case DlLastError:
         {
             return QVariant(priv->m_downloadCore->lastError());
         }
         case DlLastErrorString:
         {
             return QVariant(priv->m_downloadCore->lastErrorString());
         }
         default:
         {
             break;
         }
     }
    return QVariant();
     
}

int DownloadBackend::setAttribute(DownloadAttribute /*attr*/, const QVariant& /*value*/)
{
    return -1;
}

void DownloadBackend::bytesRecieved(qint64 bytesRecieved, qint64 bytesTotal)
{
    DM_PRIVATE(DownloadBackend);
    if((priv->m_downloadState == DlFailed) || (bytesRecieved == 0))
    {
        return;
    }
 
    // in case of resumed downloads, we recieve total size remained to download
    setTotalSize(priv->m_lastPausedSize + bytesTotal);
    priv->m_currentDownloadedSize = priv->m_lastPausedSize + bytesRecieved;
    setDownloadState(DlInprogress);
    // store the recieved chunk
    store(priv->m_downloadCore->reply()->readAll(), false); 
    postEvent(Progress, NULL);
}

void DownloadBackend::handleFinished()
{
    DM_PRIVATE(DownloadBackend);

    DownloadState state = priv->m_downloadState;
    if( state == DlFailed )
    {
        postEvent(Failed, NULL);  
        return;
    }
    if( state == DlPaused )
    {
        postEvent(Paused, NULL);  
        return;
    }
    if( state == DlCancelled )
    {
        postEvent(Cancelled, NULL);  
        return;
    }
    if(priv->m_currentDownloadedSize < priv->m_totalSize)
    {
        // all packets are not recieved, so it is not last chunk 
        // should be some network problem
        store(priv->m_downloadCore->reply()->readAll(), false); 
        postEvent(NetworkLoss, NULL);
    }
    else
    {
        //finish is successful
        store(priv->m_downloadCore->reply()->readAll(), true);
        //finish is successful
        setDownloadState(DlCompleted);
        priv->m_endTime = QDateTime::currentDateTime();
        postEvent(Completed, NULL);
    } 
}

void DownloadBackend::error(QNetworkReply::NetworkError code)
{
    DM_PRIVATE(DownloadBackend);
    if((code == QNetworkReply::OperationCanceledError) && (priv->m_downloadState == DlCancelled))
    { 
        return;
    }   
    if(code == QNetworkReply::OperationCanceledError) 
    {
        // this means user has paused the download
        setDownloadState(DlPaused);
    }   
    else if(code != QNetworkReply::NoError)
    {
        priv->m_downloadCore->setLastError(code);
        if(priv->m_downloadCore->reply())
        {
            priv->m_downloadCore->setLastErrorString(priv->m_downloadCore->reply()->errorString());
        }
        setDownloadState(DlFailed);
        postEvent(Error, NULL);
    }
}

void DownloadBackend::postEvent(DEventType type, DlEventAttributeMap* attrMap)
{
    DM_PRIVATE(DownloadBackend);
    // The client if it doesn't want progress events then it can set the DownloadMgrProgress mode as quiet
    // If it wants progress events at regular intervals then it has to specify the KiloBytes at which it requires the progress event.
    // By default, the DownloadMgrProgress Mode is non-quiet with progress events being sent at every 5KB downloaded. 
    DownloadMgrProgressMode mode = (DownloadMgrProgressMode)(priv->m_download->downloadManager()->getAttribute(DlMgrProgressMode)).toInt();
    if ((mode == Quiet) && (type == Progress))
        return;

    qlonglong kBytes = priv->m_download->getAttribute(DlProgressInterval).toLongLong() * 1024;
    if (mode == NonQuiet && type == Progress)
    {
        if ((priv->m_currentDownloadedSize/(kBytes * priv->m_progressCounter)) > 0)
        {
            priv->m_progressCounter++;
            postDownloadEvent(type, attrMap);
        }
    }
    else
        postDownloadEvent(type, attrMap);
}

DownloadState DownloadBackend::downloadState(void)
{
    DM_PRIVATE(DownloadBackend);
    return priv->m_downloadState;
}

void DownloadBackend::setDownloadState(DownloadState state)
{
    DM_PRIVATE(DownloadBackend);
    priv->m_downloadState = state;
    // save the download state
    setValue(DownloadInfo::EDlState, priv->m_downloadState);
    if((state == DlFailed) || (state == DlCompleted) || (state == DlCancelled))
    {
        // remove dl info
            deleteInfo();
    }
}

void DownloadBackend::setDownloadedDataSize(qint64 size)
{
    DM_PRIVATE(DownloadBackend);
    priv->m_currentDownloadedSize = size;
    return;
}

void DownloadBackend::setTotalSize(qint64 size)
{
    DM_PRIVATE(DownloadBackend);
    priv->m_totalSize = size;
    setValue(DownloadInfo::ETotalSize, priv->m_totalSize);
    return;
}

void DownloadBackend::setStartTime()
{
    DM_PRIVATE(DownloadBackend);
    priv->m_startTime = QDateTime::currentDateTime();
}

ClientDownload* DownloadBackend::download(void)
{
    DM_PRIVATE(DownloadBackend);
    return priv->m_download;
}

/* Helper functions to access download info */

int DownloadBackend::setValue(DownloadInfo::Key aKey, const QString& aStrValue)
{
    DM_PRIVATE(DownloadBackend);
    if(priv->m_infoDeleted)
        return 0;
    return priv->m_dlInfo->setValue(priv->m_download->id(), aKey, aStrValue, priv->m_download->parentId());
}

int DownloadBackend::setValueForChild(DownloadInfo::Key aKey, const QString& aStrValue, int aChildId)
{
    DM_PRIVATE(DownloadBackend);
    if(priv->m_infoDeleted)
        return 0;
    return priv->m_dlInfo->setValueForChild(priv->m_download->id(), aKey, aStrValue, aChildId);
}

int DownloadBackend::setValue(DownloadInfo::Key aKey, long aIntValue)
{
    DM_PRIVATE(DownloadBackend);
    if(priv->m_infoDeleted)
        return 0;
    return priv->m_dlInfo->setValue(priv->m_download->id(), aKey, aIntValue, priv->m_download->parentId());
}

int DownloadBackend::setValueForChild(DownloadInfo::Key aKey, long aIntValue, int aChildId)
{
    DM_PRIVATE(DownloadBackend);
    if(priv->m_infoDeleted)
        return 0;
    return priv->m_dlInfo->setValueForChild(priv->m_download->id(), aKey, aIntValue, aChildId);
}

int DownloadBackend::setValue(DownloadInfo::Key aKey, const QList<QVariant>& aChildIds)
{
    DM_PRIVATE(DownloadBackend);
    if(priv->m_infoDeleted)
        return 0;
    return priv->m_dlInfo->setValue(priv->m_download->id(), aKey, aChildIds);
}

int DownloadBackend::getValue(DownloadInfo::Key aKey, QString& aStrValue)
{
    DM_PRIVATE(DownloadBackend);
    if(priv->m_infoDeleted)
        return 0;
    return priv->m_dlInfo->getValue(priv->m_download->id(), aKey, aStrValue, priv->m_download->parentId());
}

int DownloadBackend::getValueForChild(DownloadInfo::Key aKey, QString& aStrValue, int aChildId)
{
    DM_PRIVATE(DownloadBackend);
    if(priv->m_infoDeleted)
        return 0;
    return priv->m_dlInfo->getValueForChild(priv->m_download->id(), aKey, aStrValue, aChildId);
}

int DownloadBackend::getValue(DownloadInfo::Key aKey, long& aIntValue)
{
    DM_PRIVATE(DownloadBackend);
    if(priv->m_infoDeleted)
        return 0;
    return priv->m_dlInfo->getValue(priv->m_download->id(), aKey, aIntValue, priv->m_download->parentId());
}

int DownloadBackend::getValueForChild(DownloadInfo::Key aKey, long& aIntValue, int aChildId)
{
    DM_PRIVATE(DownloadBackend);
    if(priv->m_infoDeleted)
        return 0;
    return priv->m_dlInfo->getValueForChild(priv->m_download->id(), aKey, aIntValue, aChildId);
 
}

int DownloadBackend::getValue(DownloadInfo::Key aKey, QList<QVariant>& aChildIds)
{
    DM_PRIVATE(DownloadBackend);
    if(priv->m_infoDeleted)
        return 0;
    return priv->m_dlInfo->getValue(priv->m_download->id(), aKey, aChildIds);
}

int DownloadBackend::deleteInfo()
{
    DM_PRIVATE(DownloadBackend);
    if ((InActive == priv->m_download->downloadManager()->getAttribute(DlMgrPersistantMode))
        && (DlCompleted == priv->m_downloadState))
        return 0;
    priv->m_infoDeleted = true;
    return priv->m_dlInfo->remove(priv->m_download->id(), priv->m_download->parentId()); 
}
void DownloadBackend::postDownloadEvent(DEventType type, DlEventAttributeMap* attrMap)
{
    DM_PRIVATE(DownloadBackend);
    EventReceiverList list = priv->m_download->eventReceivers();
    for(int i=0; i<list.size(); i++)
    {
        if(list[i])
        {
            DownloadEvent *event = new DownloadEvent(type, attrMap, priv->m_download->id());
            QCoreApplication::postEvent(list[i], event);
        }
    }
}
