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

#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "dmcommon.h"
#include "dmpimpl.h"
#include "downloadevent.h"
#include <QObject>

#ifndef DOWNLOAD_MGR_EXPORT
# ifdef BUILD_DOWNLOAD_MGR_LIB
#  define DOWNLOAD_MGR_EXPORT Q_DECL_EXPORT
# else
#  define DOWNLOAD_MGR_EXPORT Q_DECL_IMPORT
# endif
#endif

// forward declarations
class Download;
class QNetworkReply; 
class QNetworkProxy;
class DownloadInfo;
class DownloadCoreManager;
class BackgroundDownloadManager;
class DownloadManagerPrivate;
class SequentialDownloadManager;

// class declaration
class DownloadManager
{
    DM_DECLARE_PRIVATE(DownloadManager); // private implementation
public:
    DOWNLOAD_MGR_EXPORT DownloadManager(const QString& clientName);
    DOWNLOAD_MGR_EXPORT virtual ~DownloadManager();

    // to initialise all the downloads which belongs to last
    // download manager session
    DOWNLOAD_MGR_EXPORT void init();
    // creates new download by passing url
    DOWNLOAD_MGR_EXPORT Download* createDownload(const QString& url, DownloadType type = Parallel, DownloadScope scope = Normal);
    // creates a download by passing network reply
    DOWNLOAD_MGR_EXPORT Download* createDownload(QNetworkReply *reply);    
    // sets the proxy
    DOWNLOAD_MGR_EXPORT void setProxy(const QString& proxyServer, int port);
    // registers event listener
    DOWNLOAD_MGR_EXPORT void registerEventReceiver(QObject *receiver);
    // unregister event listener
    DOWNLOAD_MGR_EXPORT void unregisterEventReceiver(QObject *receiver);
    // sets download manager attributes
    DOWNLOAD_MGR_EXPORT int setAttribute(DownloadManagerAttribute attr, const QVariant& value);
    // fetches download manager attributes
    DOWNLOAD_MGR_EXPORT QVariant getAttribute(DownloadManagerAttribute attr);
    // returns all the current downloads
    DOWNLOAD_MGR_EXPORT QList<Download *>& currentDownloads();
    // finds a download provided id if exists
    DOWNLOAD_MGR_EXPORT Download* findDownload(int dlId);
    // cancels all the downloads and removes from the list
    DOWNLOAD_MGR_EXPORT void removeAll();
    // cancels and removes the download from list
    DOWNLOAD_MGR_EXPORT void removeOne(Download *dl);
    // pauses all the downloads which are in progress
    DOWNLOAD_MGR_EXPORT void pauseAll();
    // resumes all the downloads which are paused
    DOWNLOAD_MGR_EXPORT void resumeAll();
    // returns the network proxy
    DOWNLOAD_MGR_EXPORT QNetworkProxy* proxy();
    
    // returns downloadinfo object
    DownloadInfo* downloadInfo();
    // returns downloadcoremanager object
    DownloadCoreManager* downloadCoreManager();
    // returns sequentialmanager object
    SequentialDownloadManager* sequentialManager();
    // returns background download manager object
    BackgroundDownloadManager* backgroundManager();

    // post the events
    void postEvent(DEventType type, DlManagerEventAttributeMap* attrMap);

private:
    // creates download by passing id
    // used while loading the saved downloads
    Download* createDownload(int dlId, DownloadScope scope);
    // loads the saved downloads
    void loadAllDownloads();
    // generated unique id for download
    long generateDlId();
};

#endif
