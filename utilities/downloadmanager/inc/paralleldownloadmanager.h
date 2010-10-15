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

#ifndef PARALLELDOWNLOADMANAGER_H
#define PARALLELDOWNLOADMANAGER_H

#include "dmcommon.h"
#include "dmpimpl.h"
#include "downloadevent.h"
#include <QObject>

// forward declarations
class ClientDownload;
class QNetworkReply; 
class ParallelDownloadManagerPrivate;

// class declaration
class ParallelDownloadManager
{
    DM_DECLARE_PRIVATE(ParallelDownloadManager); // private implementation
public:
    ParallelDownloadManager();
    virtual ~ParallelDownloadManager();
    
    // returns all the current downloads
    QList<Download *>& currentDownloads();
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
    // Adds to the download list
    void addToParallelDownload(Download* dl);
};

#endif
