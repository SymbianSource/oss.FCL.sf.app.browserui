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

#include "backgrounddownload.h"
#include "download.h"
#include "downloadmanager.h"
#include "backgrounddownloadmanager.h"
#include "downloadmanagerclient.h"
#include "downloadevent.h"
#include "downloadinfo.h"
#include <QNetworkReply>
#include <QCoreApplication>

// private implementation
class BackgroundDownloadPrivate
{
    DM_DECLARE_PUBLIC(BackgroundDownload);
public:
    BackgroundDownloadPrivate();
    ~BackgroundDownloadPrivate();
    int m_downloadId;
    QString m_url;
    DownloadManager *m_downloadManager; // not owned
    BackgroundDownloadManager* m_backgroundMgr; // not owned
    EventReceiverList m_eventReceiverList; // list of event listeners
    DownloadManagerClient* m_downloadMgrClient; // not owned
    DownloadInfo *m_dlInfo;         // not owned
};

BackgroundDownloadPrivate::BackgroundDownloadPrivate()
{
    m_downloadId = INVALID_DL_ID;
    m_downloadManager = 0;
    m_backgroundMgr = 0;
    m_downloadMgrClient = 0;
}

BackgroundDownloadPrivate::~BackgroundDownloadPrivate()
{
}

/*!
 * \class BackgroundDownload
 *
 * \brief The public APIs for managing a background download
 *
 * This class has the public APIs for managing a single background download
 */

BackgroundDownload::BackgroundDownload(DownloadManager *mgr, const QString& url, int dlId)
{
    DM_INITIALIZE(BackgroundDownload);
    priv->m_downloadId = dlId;
    priv->m_url = url;
    priv->m_downloadManager = mgr;
    priv->m_backgroundMgr = mgr->backgroundManager();
    priv->m_downloadMgrClient = priv->m_backgroundMgr->downloadManagerClient();
    if (mgr)
        priv->m_dlInfo = mgr->downloadInfo();
    if (priv->m_dlInfo) {
        priv->m_dlInfo->setValue(priv->m_downloadId, DownloadInfo::EUrl, (const QString&)url);
        priv->m_dlInfo->setValue(priv->m_downloadId, DownloadInfo::EScope, (long)Background);
    }
}

BackgroundDownload::~BackgroundDownload()
{
    DM_UNINITIALIZE(BackgroundDownload);
}

/*!
  starts the background download, returns the success status
*/
int BackgroundDownload::start()
{
    DM_PRIVATE(BackgroundDownload);
    if (priv->m_downloadMgrClient)
        return priv->m_downloadMgrClient->startDownload(priv->m_downloadId);
    return -1;
}

/*!
  returns id of the background download
*/
int BackgroundDownload::id()
{
    DM_PRIVATE(BackgroundDownload);
    return priv->m_downloadId;
}

/*!
  sets the attribute for the background download
  \a attr indicates attribute
  \a value indicates value for the download
*/
int BackgroundDownload::setAttribute(DownloadAttribute attr, const QVariant& value)
{
    DM_PRIVATE(BackgroundDownload);
    if (priv->m_downloadMgrClient)
        return priv->m_downloadMgrClient->setDownloadAttribute(priv->m_downloadId, attr, value);
    return -1;
}

/*!
  fetches the attribute of the background download
  \a attr indicates download attribute
*/
QVariant BackgroundDownload::getAttribute(DownloadAttribute attr)
{
    DM_PRIVATE(BackgroundDownload);
    if (priv->m_downloadMgrClient)
        return priv->m_downloadMgrClient->getDownloadAttribute(priv->m_downloadId, attr);
    return QVariant();
}

/*!
  pauses the background download
*/
int BackgroundDownload::pause()
{
    DM_PRIVATE(BackgroundDownload);
    if (priv->m_downloadMgrClient)
        return priv->m_downloadMgrClient->pauseDownload(priv->m_downloadId);
    return -1;
}

/*!
  resumes the background download
*/
int BackgroundDownload::resume()
{
    DM_PRIVATE(BackgroundDownload);
    if (priv->m_downloadMgrClient)
        return priv->m_downloadMgrClient->resumeDownload(priv->m_downloadId);
    return -1;
}

/*!
  cancels the background download
*/
int BackgroundDownload::cancel()
{
    DM_PRIVATE(BackgroundDownload);
    if (priv->m_downloadMgrClient)
        return priv->m_downloadMgrClient->cancelDownload(priv->m_downloadId);
    return -1;
}

/*!
  registers receiver for the background download events
  \a reciever indicates reciever which listen to download events
*/
void BackgroundDownload::registerEventReceiver(QObject *receiver)
{
    DM_PRIVATE(BackgroundDownload);
    if (receiver)
       if (!priv->m_eventReceiverList.contains(receiver))
            priv->m_eventReceiverList.append(receiver);
}

/*!
  unregisters the event listener
  \a receiver indicates listener which will be unregistered
*/
void BackgroundDownload::unregisterEventReceiver(QObject *receiver)
{
    DM_PRIVATE(BackgroundDownload);
    priv->m_eventReceiverList.removeOne(receiver);
}

/*!
  returns download manager
*/
DownloadManager* BackgroundDownload::downloadManager()
{
    DM_PRIVATE(BackgroundDownload);
    return priv->m_downloadManager;
}

/*!
  returns the child downloads i.e if download has any media objects
  \a list indicates list of child downloads
*/
void BackgroundDownload::getChildren(QList<Download*>& /*list*/)
{
    // This feature is not supported at present
    return;
}

// posts the event to recievers event loop
void BackgroundDownload::postEvent(DEventType type, DlEventAttributeMap* attrMap)
{
    DM_PRIVATE(BackgroundDownload);
    if((type == Failed) || (type == Completed) || (type == Cancelled)) {
        if (!(type == Completed && InActive == priv->m_downloadManager->getAttribute(DlMgrPersistantMode)))
            removeDownloadInfo();
    }
    EventReceiverList list = eventReceivers();
    for (int i=0; i<list.size(); i++) {
        if (list[i]) {
            DownloadEvent *event = new DownloadEvent(type, attrMap, priv->m_downloadId);
            QCoreApplication::postEvent(list[i], event);
        }
    }
}

// returns the event listeners
EventReceiverList& BackgroundDownload::eventReceivers()
{
    DM_PRIVATE(BackgroundDownload);
    return priv->m_eventReceiverList;
}

// remove info of this download
void BackgroundDownload::removeDownloadInfo()
{
    DM_PRIVATE(BackgroundDownload);
    priv->m_dlInfo->remove(priv->m_downloadId);
}
