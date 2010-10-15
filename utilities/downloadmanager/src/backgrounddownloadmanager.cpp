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
#include "backgrounddownloadmanager.h"
#include "download.h"
#include "backgrounddownload.h"
#include "downloadmanagerclient.h"
#include "downloadevent.h"
#include "downloadinfo.h"
#include <QHash>
#include <QCoreApplication>
#include <QNetworkReply>

class BackgroundDownloadManagerPrivate
{
    //declare public implementation
    DM_DECLARE_PUBLIC(BackgroundDownloadManager);
public:
    BackgroundDownloadManagerPrivate();
    ~BackgroundDownloadManagerPrivate();

    QList<Download*> m_downloads; // list of downloads
    QString m_clientName; // client name
    QObject *m_receiver; // event reciever
    DownloadManager* m_downloadMgr; // not owned
    DownloadManagerClient* m_downloadMgrClient; // owned
    DownloadInfo* m_dlInfo; // not owned
};

BackgroundDownloadManagerPrivate::BackgroundDownloadManagerPrivate()
{
    m_clientName = "";
    m_receiver = 0;
    m_downloadMgr = 0;
    m_downloadMgrClient = 0;
}

BackgroundDownloadManagerPrivate::~BackgroundDownloadManagerPrivate()
{
    if (m_downloadMgrClient) {
        delete m_downloadMgrClient;
        m_downloadMgrClient = 0;
    }
    m_downloadMgr = 0;
    m_downloads.clear();
}

BackgroundDownloadManager::BackgroundDownloadManager(DownloadManager* downloadMgr)
{
    DM_INITIALIZE(BackgroundDownloadManager);
    priv->m_downloadMgr = downloadMgr;
    if (downloadMgr) {
        priv->m_clientName = downloadMgr->getAttribute(DlMgrClientName).toString();
        priv->m_dlInfo = downloadMgr->downloadInfo();
    }
}

// destructor for the download manager
BackgroundDownloadManager::~BackgroundDownloadManager()
{
    DM_UNINITIALIZE(BackgroundDownloadManager);
}

// initialises all the downloads which belongs to last download manager session
void BackgroundDownloadManager::init()
{
    //load all downloads which were created in the last session
    return;
}

// returns new download
// url indicates download url
Download* BackgroundDownloadManager::createDownload(const QString& url, DownloadType type)
{
    if (url.isEmpty())
        return NULL;

    DM_PRIVATE(BackgroundDownloadManager);
    // connection to server will be established whrn DownloadManagerClient object is created
    // so we delay the constructon of this object till client calls some function
    if (!priv->m_downloadMgrClient)
        priv->m_downloadMgrClient = new DownloadManagerClient(priv->m_downloadMgr);
    // create downlaod at server
    int downloadId = priv->m_downloadMgrClient->createDownload(url, type);
    if (downloadId == INVALID_DL_ID)
        return NULL;
    Download* dl = new BackgroundDownload(priv->m_downloadMgr, url, downloadId);
    if (dl)
        priv->m_downloads.append(dl);
    return dl;
}

// creates the background download from the persistantly stored download info
Download* BackgroundDownloadManager::createDownload(int id)
{
    DM_PRIVATE(BackgroundDownloadManager);

    if ((id == INVALID_DL_ID) || (!priv->m_dlInfo))
        return NULL;

    // connection to server will be established when DownloadManagerClient object is created
    // so we delay the constructon of this object till client calls some function
    if (!priv->m_downloadMgrClient)
        priv->m_downloadMgrClient = new DownloadManagerClient(priv->m_downloadMgr);
    QString url;
    priv->m_dlInfo->getValue(id, DownloadInfo::EUrl, url);
    // create downlaod at server
    bool status = priv->m_downloadMgrClient->attachToDownload(id);
    if (status == false) {
        // this means, download no more exists in server side
        priv->m_dlInfo->remove(id);
        return NULL;
    }
    Download* dl = new BackgroundDownload(priv->m_downloadMgr, url, id);
    if (dl)               
        priv->m_downloads.append(dl);
    return dl;
}

// sets the proxy
// proxyServer indicates proxy server name
// port indicates port number
void BackgroundDownloadManager::setProxy(const QString& proxyServer, int port)
{
    if (!proxyServer.isEmpty()) {
        DM_PRIVATE(BackgroundDownloadManager);
        // connection to server will be established whrn DownloadManagerClient object is created
    // so we delay the constructon of this object till client calls some function
        if (!priv->m_downloadMgrClient)
            priv->m_downloadMgrClient = new DownloadManagerClient(priv->m_downloadMgr);
        priv->m_downloadMgrClient->setProxy(proxyServer, port);
    }
}

// sets download manager attribute
// attr indicates download manager attribute
// value indicates value to be set
int BackgroundDownloadManager::setAttribute(DownloadManagerAttribute attr, const QVariant& value)
{
    // remove build warnings
    DM_PRIVATE(BackgroundDownloadManager);
    if (!priv->m_downloadMgrClient)
        return -1;
    int returnValue = priv->m_downloadMgrClient->setDownloadManagerAttribute(attr, value);
    return returnValue;
}

// fetches download manager attribute
// attr indicates download manager attribute whos value to be returned
QVariant BackgroundDownloadManager::getAttribute(DownloadManagerAttribute attr)
{
    DM_PRIVATE(BackgroundDownloadManager);
    if (!priv->m_downloadMgrClient)
        return QVariant();

    // if download manager error, return the last client-server error
    switch(attr) {
    case DlMgrServerError:
        return QVariant(priv->m_downloadMgrClient->serverError());
    default:
        break;
    }

    QVariant value = priv->m_downloadMgrClient->getDownloadManagerAttribute(attr);
    return value;
}

// returns all the current downloads
QList<Download*>& BackgroundDownloadManager::currentDownloads()
{
    DM_PRIVATE(BackgroundDownloadManager);
    return priv->m_downloads;
}

// finds a download provided id if exists
// id indicates identifier for download
Download* BackgroundDownloadManager::findDownload(int dlId)
{
    DM_PRIVATE(BackgroundDownloadManager);
    for (int i=0; i<priv->m_downloads.count(); ++i) {
        if (priv->m_downloads[i]->id() == dlId) {
            return priv->m_downloads[i];
        }
    }
    return 0;
}

// cancels all the downloads
void BackgroundDownloadManager::removeAll()
{
    DM_PRIVATE(BackgroundDownloadManager);
    if (!priv->m_downloadMgrClient)
        return;

    // before removing all downloads, remove their downloadinfo
    for (int i=0; i<priv->m_downloads.count(); ++i) {
        BackgroundDownload* download = dynamic_cast<BackgroundDownload*>(priv->m_downloads[i]);
        if (download)
            download->removeDownloadInfo();
    }

    priv->m_downloadMgrClient->removeAll();
    for (int i=0; i<priv->m_downloads.count(); ++i) {
        delete (priv->m_downloads[i]);
        priv->m_downloads[i] = 0;
    }
    priv->m_downloads.clear();
}

// cancels and removes the download
// dl indicates the download to be canceled and removed
void BackgroundDownloadManager::removeOne(Download* dl)
{
    // check if download exists
    if (!dl)
        return;

    DM_PRIVATE(BackgroundDownloadManager);
    // return if download manager client does not exist yet
    if (!priv->m_downloadMgrClient)
        return;
    // if this is a background download, remove it
    if (priv->m_downloads.contains(dl)) {
        priv->m_downloadMgrClient->removeDownload(dl->id());
        priv->m_downloads.removeOne(dl);
    }
}

// pauses all the downloads which are in progress
void BackgroundDownloadManager::pauseAll()
{
    DM_PRIVATE(BackgroundDownloadManager);
    if (priv->m_downloadMgrClient)
        priv->m_downloadMgrClient->pauseAll();
}

// resumes all the downloads which are paused
void BackgroundDownloadManager::resumeAll()
{
    DM_PRIVATE(BackgroundDownloadManager);
    if (priv->m_downloadMgrClient)
        priv->m_downloadMgrClient->resumeAll();
}

// get download manager client object
DownloadManagerClient* BackgroundDownloadManager::downloadManagerClient()
{
    DM_PRIVATE(BackgroundDownloadManager);
    return priv->m_downloadMgrClient;
}
