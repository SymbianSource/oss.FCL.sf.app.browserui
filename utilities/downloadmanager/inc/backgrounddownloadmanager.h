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

#ifndef BACKGROUNDDOWNLOADMANAGER_H
#define BACKGROUNDDOWNLOADMANAGER_H

#include "dmcommon.h"
#include "dmpimpl.h"
#include <QObject>

// forward declarations
class DownloadManager;
class Download;
class DownloadManagerClient;
class BackgroundDownloadManagerPrivate;

// class declaration
class BackgroundDownloadManager : public QObject
{
    Q_OBJECT
    DM_DECLARE_PRIVATE(BackgroundDownloadManager); // private implementation
public:
    BackgroundDownloadManager(DownloadManager* downloadMgr);
    virtual ~BackgroundDownloadManager();

    // to initialise all the downloads which belongs to last
    // download manager session
    void init();
    // creates new download by passing url
    Download* createDownload(const QString& url, DownloadType type = Parallel);
    // creates new download by reading from persistant download information
    Download* createDownload(int dlId);
    // sets the proxy
    void setProxy(const QString& proxyServer, int port);
    // sets download manager attributes
    int setAttribute(DownloadManagerAttribute attr, const QVariant& value);
    // fetches download manager attributes
    QVariant getAttribute(DownloadManagerAttribute attr);
    // returns all the current downloads
    QList<Download*>& currentDownloads();
    // finds a download provided id if exists
    Download* findDownload(int dlId);
    // cancels all the downloads and removes from the list
    void removeAll();
    // cancels and removes the download from list
    void removeOne(Download *dl);
    // pauses all the downloads which are in progress
    void pauseAll();
    // resumes all the downloads which are paused
    void resumeAll();
    // returns download manager client object
    DownloadManagerClient* downloadManagerClient();
};

#endif
