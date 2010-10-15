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

#include "downloadmanager.h"
#include "download.h"
#include "clientdownload.h"
#include "downloadevent.h"
#include "downloadinfo.h"
#include "downloadcoremanager.h"
#include "backgrounddownloadmanager.h"
#include "paralleldownloadmanager.h"
#include "sequentialdownloadmanager.h"
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QList>
#include <QDir>
#include <QCoreApplication>

// defualt download path
#define DOWNLOAD_PATH QDir::homePath() + QObject::tr("/Downloads")

class DownloadManagerPrivate
{
    //declare public implementation
    DM_DECLARE_PUBLIC(DownloadManager);
public:
    DownloadManagerPrivate();
    ~DownloadManagerPrivate();

    QString m_clientName; // client name
    QNetworkProxy *m_proxy;
    QObject *m_receiver; // event reciever
    DownloadInfo *m_dlInfo;
    DownloadCoreManager *m_dlCoreManager; // manages all the download cores
    QString m_destPath; // download destination path
    BackgroundDownloadManager* m_backgroundManager;
    ParallelDownloadManager *m_parallelManager; // manages parallel downloads
    SequentialDownloadManager *m_sequentialManager; // manages sequential downloads
    QList<Download*> m_totalDownloads; // has a list of both parallel and sequential downloads, but doesnt own it
    DownloadMgrProgressMode m_progressMode;
    DownloadMgrPersistantMode m_persistantMode;
};

DownloadManagerPrivate::DownloadManagerPrivate()
{
    m_clientName = "";
    m_proxy = 0;
    m_receiver = 0;
    m_dlInfo = 0;
    m_dlCoreManager = 0;
    // set the download destination path to default path
    m_destPath = DOWNLOAD_PATH;
    m_backgroundManager = 0;
    m_parallelManager = 0;
    m_sequentialManager = 0;
    m_progressMode = NonQuiet;
    m_persistantMode = Active;
}

DownloadManagerPrivate::~DownloadManagerPrivate()
{
    if(m_sequentialManager)
    {
        delete m_sequentialManager;
        m_sequentialManager = 0;
    }
    if(m_parallelManager)
    {
        delete m_parallelManager;
        m_parallelManager = 0;
    }
    if (m_backgroundManager)
    {
        delete m_backgroundManager;
        m_backgroundManager = 0;
    }
    if(m_proxy)
    {
        delete m_proxy;
        m_proxy = 0;
    }
    if(m_dlInfo)
    {
        m_dlInfo->update();
        delete m_dlInfo;
        m_dlInfo = 0;
    }
    if(m_dlCoreManager)
    {
        delete m_dlCoreManager;
        m_dlCoreManager = 0;
    }
}

/*!
 * \class DownloadManager
 *
 * \brief The public APIs for managing the downloads
 *
 * This class has the public APIs for managing the downloads
 */

/*!
  creates an instance of download manager
  \a clientName indicates the name of the client
*/
Q_DECL_EXPORT DownloadManager::DownloadManager(const QString& clientName)
{
    DM_INITIALIZE(DownloadManager);
    priv->m_clientName = clientName;
    //create a download info to save download informations
    priv->m_dlInfo = new DownloadInfo(clientName);
    // create download core manager
    priv->m_dlCoreManager = new DownloadCoreManager(clientName);
    // create background download manager
    priv->m_backgroundManager = new BackgroundDownloadManager(this);
    // create parallel download manager
    priv->m_parallelManager = new ParallelDownloadManager();
    // create sequential download manager
    priv->m_sequentialManager = new SequentialDownloadManager();
}

/*!
  destructor for the download manager
*/
Q_DECL_EXPORT DownloadManager::~DownloadManager()
{
    DM_UNINITIALIZE(DownloadManager);
}

/*!
  initialises all the downloads which belongs to last download manager session
*/
Q_DECL_EXPORT void DownloadManager::init()
{
    //load all downloads which were created in the last session
    loadAllDownloads();
}

/*!
  returns new download
  \a url indicates download url
  \a type indicates whether it is sequential or parallel download
*/
Q_DECL_EXPORT Download* DownloadManager::createDownload(const QString& url, DownloadType type/*=Parallel*/, DownloadScope scope/*=Normal*/)
{
    DM_PRIVATE(DownloadManager);
    if(url == "")
        return NULL;
    Download* dl(0);

    if (scope == Normal) {
        dl = new ClientDownload(this, url, generateDlId());
        if(type == Parallel)
            priv->m_parallelManager->addToParallelDownload(dl);
        else if(type == Sequential) {
            // by default the priority is low
            dl->setAttribute(DlPriority, (int)Low);
            priv->m_sequentialManager->addToSequentialDownload(dl);
        }
        postEvent(DownloadCreated, NULL);
    }
    else if (scope == Background) {
        dl = priv->m_backgroundManager->createDownload(url, type);
    }
    return dl;
}

/*!
  returns new download
  \a reply indicates network reply which is already initiated
*/
Q_DECL_EXPORT Download* DownloadManager::createDownload(QNetworkReply *reply)
{
    DM_PRIVATE(DownloadManager);
    if(!reply)
        return NULL;
    ClientDownload *dl = new ClientDownload(this, reply, generateDlId());
    priv->m_parallelManager->addToParallelDownload(dl);
    postEvent(DownloadCreated, NULL);
    return dl;
}

Download* DownloadManager::createDownload(int dlId, DownloadScope scope)
{
    DM_PRIVATE(DownloadManager);
    if(dlId == INVALID_DL_ID)
        return NULL;
    Download *dl = NULL;
    if (scope == Normal){
        dl = new ClientDownload(this, dlId);
        DownloadType type = (DownloadType)(dl->getAttribute(DlDownloadType).toInt());
        if(type == Parallel)
            priv->m_parallelManager->addToParallelDownload(dl);
        else
            priv->m_sequentialManager->addToSequentialDownload(dl);
    } else if (scope == Background){
        dl = priv->m_backgroundManager->createDownload(dlId);
    } else
        return NULL;
     
    if (dl)
         postEvent(DownloadCreated, NULL);
    return dl;
}

/*!
  sets the proxy
  \a proxyServer indicates proxy server name
  \a port indicates port number
*/
Q_DECL_EXPORT void DownloadManager::setProxy(const QString& proxyServer, int port)
{
    DM_PRIVATE(DownloadManager);
    if(priv->m_proxy)
    {
        delete priv->m_proxy;
        priv->m_proxy = 0;
    }
    priv->m_proxy = new QNetworkProxy(QNetworkProxy::HttpCachingProxy, proxyServer, port );
    return;
}

/*!
  registers event listener
  \a reciever indicates reciever object to be registered for download events
*/
Q_DECL_EXPORT void DownloadManager::registerEventReceiver(QObject *receiver)
{
    DM_PRIVATE(DownloadManager);
    if (receiver)
        priv->m_receiver = receiver;
    return;
}

/*!
  unregister event listener
  \a reciever indicates reciever object to be unregistered
*/
Q_DECL_EXPORT void DownloadManager::unregisterEventReceiver(QObject *receiver)
{
    DM_PRIVATE(DownloadManager);
    if(receiver == priv->m_receiver)
    {
        priv->m_receiver = 0;
    }
    return;
}

/*!
  sets download manager attribute
  \a attr indicates download manager attribute
  \a value indicates value to be set
*/
Q_DECL_EXPORT int DownloadManager::setAttribute(DownloadManagerAttribute attr, const QVariant& value)
{
    DM_PRIVATE(DownloadManager);
    switch(attr)
    {
        case DlMgrDestPath:
        {
            priv->m_destPath = value.toString();
            return 0;
        }
        case DlMgrProgressMode:
        {
            priv->m_progressMode = (DownloadMgrProgressMode)value.toInt();
            return 0;
        }
        case DlMgrPersistantMode:
        {
            priv->m_persistantMode = (DownloadMgrPersistantMode)value.toInt();
            return 0;
        }
        default :
            return -1;
    }
    return -1;
}

/*!
  fetches download manager attribute
  \a attr indicates download manager attribute whos value to be returned
*/
Q_DECL_EXPORT QVariant DownloadManager::getAttribute(DownloadManagerAttribute attr)
{
    DM_PRIVATE(DownloadManager);
    switch(attr)
    {
        case DlMgrDlCount:
        {
            int count(0);
            if(priv->m_parallelManager)
                count += priv->m_parallelManager->currentDownloads().count();
            if(priv->m_sequentialManager)
                count += priv->m_sequentialManager->currentDownloads().count();
            if(priv->m_backgroundManager)
                count += priv->m_backgroundManager->currentDownloads().count();
            return QVariant(count);
        }
        case DlMgrClientName:
        {
            return QVariant(priv->m_clientName);
        }
        case DlMgrDestPath:
        {
            return QVariant(priv->m_destPath);
        }
        case DlMgrServerError:
        {
            // These cases apply only for Background Download Manager as this attribute
            // is for getting download manager client-server communication related errors
            QVariant value;
            if(priv->m_backgroundManager)
                value = priv->m_backgroundManager->getAttribute(attr);
            return value;
        }
        case DlMgrProgressMode:
        {
            return QVariant(priv->m_progressMode);
        }
        case DlMgrPersistantMode:
        {
            return QVariant(priv->m_persistantMode);
        }
        default :
            return QVariant();
    }
    return QVariant();
}

/*!
  returns all the current downloads
*/
Q_DECL_EXPORT QList<Download*>& DownloadManager::currentDownloads()
{
    DM_PRIVATE(DownloadManager);
    priv->m_totalDownloads.clear();
    if(priv->m_parallelManager) {
        for(int i = 0;i < priv->m_parallelManager->currentDownloads().size(); ++i)
            priv->m_totalDownloads.append(priv->m_parallelManager->currentDownloads()[i]);
    }
    if(priv->m_sequentialManager) {
        for(int i = 0;i < priv->m_sequentialManager->currentDownloads().size(); ++i)
            priv->m_totalDownloads.append(priv->m_sequentialManager->currentDownloads()[i]);
    }
    if(priv->m_backgroundManager) {
        for(int i = 0;i < priv->m_backgroundManager->currentDownloads().size(); ++i)
            priv->m_totalDownloads.append(priv->m_backgroundManager->currentDownloads()[i]);
    }
    return priv->m_totalDownloads;
}

/*!
  finds a download provided id if exists
  \a id indicates identifier for download
*/
Q_DECL_EXPORT Download* DownloadManager::findDownload(int dlId)
{
    DM_PRIVATE(DownloadManager);
    Download* dl = NULL;
    if(priv->m_parallelManager)
    {
        dl = priv->m_parallelManager->findDownload(dlId);
        if(dl)
            return dl;
    }
    if(priv->m_sequentialManager)
    {
        dl = priv->m_sequentialManager->findDownload(dlId);
        if(dl)
            return dl;
    }
    if(priv->m_backgroundManager)
    {
        dl = priv->m_backgroundManager->findDownload(dlId);
        if(dl)
            return dl;
    }
    return NULL;
}

/*!
  cancels all the downloads
*/
Q_DECL_EXPORT void DownloadManager::removeAll()
{
    DM_PRIVATE(DownloadManager);
    if(priv->m_parallelManager)
        priv->m_parallelManager->removeAll();
    if(priv->m_sequentialManager)
        priv->m_sequentialManager->removeAll();
    if(priv->m_backgroundManager)
        priv->m_backgroundManager->removeAll();
    postEvent(DownloadsCleared, NULL);
    return;
}

/*!
  cancels and removes the download
  \a dl indicates the download to be canceled and removed
*/
Q_DECL_EXPORT void DownloadManager::removeOne(Download *dl)
{
    if (!dl)
        return;

    DM_PRIVATE(DownloadManager);
    if(priv->m_parallelManager)
        priv->m_parallelManager->removeOne(dl);
    if(priv->m_sequentialManager)
        priv->m_sequentialManager->removeOne(dl);
    if(priv->m_backgroundManager)
        priv->m_backgroundManager->removeOne(dl);

    if((priv->m_parallelManager->currentDownloads().count() == 0) &&
        (priv->m_sequentialManager->currentDownloads().count() == 0) &&
        (priv->m_backgroundManager->currentDownloads().count() == 0))
        postEvent(DownloadsCleared, NULL);
    return;
}

/*!
  pauses all the downloads which are in progress
*/
Q_DECL_EXPORT void DownloadManager::pauseAll()
{
    DM_PRIVATE(DownloadManager);
    if(priv->m_parallelManager)
        priv->m_parallelManager->pauseAll();
    if(priv->m_sequentialManager)
        priv->m_sequentialManager->pauseAll();
    if(priv->m_backgroundManager)
        priv->m_backgroundManager->pauseAll();
    return;
}

/*!
  resumes all the downloads which are paused
*/
Q_DECL_EXPORT void DownloadManager::resumeAll()
{
    DM_PRIVATE(DownloadManager);
    if(priv->m_parallelManager)
        priv->m_parallelManager->resumeAll();
    if(priv->m_sequentialManager)
        priv->m_sequentialManager->resumeAll();
    if(priv->m_backgroundManager)
        priv->m_backgroundManager->resumeAll();
    return;
}

/*!
  returns the proxy used
*/
Q_DECL_EXPORT QNetworkProxy* DownloadManager::proxy()
{
    DM_PRIVATE(DownloadManager);
    return priv->m_proxy;
}

DownloadInfo* DownloadManager::downloadInfo()
{
    DM_PRIVATE(DownloadManager);
    return priv->m_dlInfo;
}

DownloadCoreManager* DownloadManager::downloadCoreManager()
{
    DM_PRIVATE(DownloadManager);
    return priv->m_dlCoreManager;
}

SequentialDownloadManager* DownloadManager::sequentialManager()
{
    DM_PRIVATE(DownloadManager);
    return priv->m_sequentialManager;
}

BackgroundDownloadManager* DownloadManager::backgroundManager()
{
    DM_PRIVATE(DownloadManager);
    return priv->m_backgroundManager;
}

void DownloadManager::loadAllDownloads()
{
    DM_PRIVATE(DownloadManager);
    // read the saved downloads information and create download
    QVector<int> ids = DownloadInfo::getAllDownloads(priv->m_clientName);
    for(int i=0; i<ids.size(); i++)
    {
        if(!findDownload(ids[i])) {
            long scope;
            priv->m_dlInfo->getValue(ids[i], DownloadInfo::EScope, scope);
            createDownload(ids[i], (DownloadScope)scope);
        }
    }
    return;
}

// generate unique id for download
long DownloadManager::generateDlId()
{
    DM_PRIVATE(DownloadManager);
    long id = 1;
    long totDls = (priv->m_parallelManager->currentDownloads().count() + 
                priv->m_sequentialManager->currentDownloads().count() +
                priv->m_backgroundManager->currentDownloads().count());

    if(totDls <= 0)
    {
        return id;
    }
    while(1)
    {
        int i;
        for(i = 0; i <= totDls ; ++i)
        {
            Download* parallelDl = NULL;
            parallelDl = priv->m_parallelManager ? priv->m_parallelManager->findDownload(i) : NULL;
            Download* sequentialDl = NULL;
            sequentialDl = priv->m_sequentialManager ? priv->m_sequentialManager->findDownload(i) : NULL;
            Download* backgroundDl = NULL;
            backgroundDl = priv->m_backgroundManager ? priv->m_backgroundManager->findDownload(i) : NULL;
            if((parallelDl && (parallelDl->id() == id)) ||
                (sequentialDl && (sequentialDl->id() == id)) ||
                (backgroundDl && (backgroundDl->id() == id)))
            {
                break;
            }
        }
        if(i>totDls)
        {
            return id;
        }
        id++;
    }
    return -1;
}

void DownloadManager::postEvent(DEventType type, DlManagerEventAttributeMap* attrMap)
{
    DM_PRIVATE(DownloadManager);
    if(priv->m_receiver)
    {
        DownloadManagerEvent *event = new DownloadManagerEvent(type, attrMap);
        QCoreApplication::postEvent(priv->m_receiver, event);
    }
}

