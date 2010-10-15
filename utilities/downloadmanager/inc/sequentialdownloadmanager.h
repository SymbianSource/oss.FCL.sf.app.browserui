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

#ifndef SEQUENTIALDOWNLOADMANAGER_H
#define SEQUENTIALDOWNLOADMANAGER_H

#include "dmcommon.h"
#include "dmpimpl.h"
#include "downloadevent.h"
#include <QObject>
#include <QQueue>

// forward declarations
class ClientDownload;
class QNetworkReply; 
class SequentialDownloadManagerPrivate;

// class declaration
class SequentialDownloadManager : public QObject
{
    DM_DECLARE_PRIVATE(SequentialDownloadManager); // private implementation
public:
    SequentialDownloadManager();
    virtual ~SequentialDownloadManager();
    
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
    void addToSequentialDownload(Download* dl);

private:
    // removes adownload from created download list to download queue for downloading
    void addToDownloadQueue(Download* dl);
    // adds downloads to completed list of downloads
    void addToCompletedList(Download* dl);
    // processes a download when download start fired
    void process(int dlId);
    // event handler
    bool event(QEvent *event);
    // starts the next download in downloadQueue
    void startNext();
    // pauses the download if it is the first one in the queue
    void pauseDownload(int dlId);
    // resumes the download if it is the first one in the queue
    void resumeDownload(int dlId);
    // cancels the download if it is the first one in the queue
    void cancelDownload(int dlId);

    friend class ClientDownload;
};

#endif
