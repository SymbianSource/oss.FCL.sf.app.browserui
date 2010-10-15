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
#include "paralleldownloadmanager.h"
#include "clientdownload.h"

class ParallelDownloadManagerPrivate
{
    //declare public implementation
    DM_DECLARE_PUBLIC(ParallelDownloadManager);
public:
    ParallelDownloadManagerPrivate();
    ~ParallelDownloadManagerPrivate();
   
    QList<Download*> m_downloads; // list of downloads
};

ParallelDownloadManagerPrivate::ParallelDownloadManagerPrivate()
{ }

ParallelDownloadManagerPrivate::~ParallelDownloadManagerPrivate()
{
    int size = m_downloads.size();
    for(int i=0; i<size; i++) {
        DownloadState state = (DownloadState)((m_downloads[i]->getAttribute(DlDownloadState)).toInt());
        if(state == DlInprogress) {
            // pause the download if it is in progress
            m_downloads[i]->pause();
        } else if(state == DlFailed) {
            // cancel the download if it has failed
            m_downloads[i]->cancel();
        }
        delete (m_downloads[i]);
    }
    m_downloads.clear();
}

/*
  constructor
*/
ParallelDownloadManager::ParallelDownloadManager()
{
    DM_INITIALIZE(ParallelDownloadManager);
}

/*
  destructor
*/
ParallelDownloadManager::~ParallelDownloadManager()
{
    DM_UNINITIALIZE(ParallelDownloadManager);
}

/*
  returns all the current downloads
*/
QList<Download*>& ParallelDownloadManager::currentDownloads()
{
    DM_PRIVATE(ParallelDownloadManager);
    return priv->m_downloads;
}

/*
  finds a download provided id if exists
  \a id indicates identifier for download
*/
Download* ParallelDownloadManager::findDownload(int dlId)
{
    DM_PRIVATE(ParallelDownloadManager);
    for (int i = 0; i < priv->m_downloads.size(); ++i) {
        if (priv->m_downloads[i]->id() == dlId)
            return priv->m_downloads[i];
    }
    return 0;
}

/*
  cancels all the downloads
*/
void ParallelDownloadManager::removeAll()
{
    DM_PRIVATE(ParallelDownloadManager);
    int count = priv->m_downloads.count();
    //traverse the list and delete each download in the list
    for(int i=0; i<count; i++) {
        // cancel the download
        priv->m_downloads[i]->cancel();
        priv->m_downloads[i]->deleteLater();
    }
    priv->m_downloads.clear();
}

/*
  cancels and removes the download
  \a dl indicates the download to be canceled and removed
*/
void ParallelDownloadManager::removeOne(Download *dl)
{
    if(!dl)
        return;
    DM_PRIVATE(ParallelDownloadManager);
    if (priv->m_downloads.contains(dl)) {
        dl->cancel();
        priv->m_downloads.removeOne(dl);
        delete dl;
        dl = 0;
    }
}

/*
  pauses all the downloads which are in progress
*/
void ParallelDownloadManager::pauseAll()
{
    DM_PRIVATE(ParallelDownloadManager);
    int count = priv->m_downloads.size();
    //traverse the list and pause downloads which are on-going
    for(int i=0; i<count; i++) {
        DownloadState state = (DownloadState)((priv->m_downloads[i]->getAttribute(DlDownloadState)).toInt());
        if((state == DlInprogress) || (state == DlStarted))
            priv->m_downloads[i]->pause();
    }    
}

/*
  resumes all the downloads which are paused
*/
void ParallelDownloadManager::resumeAll()
{
    DM_PRIVATE(ParallelDownloadManager);
    int count = priv->m_downloads.size();
    //traverse the list and resume downloads which are paused
    for(int i=0; i<count; i++) {
        DownloadState state = (DownloadState)((priv->m_downloads[i]->getAttribute(DlDownloadState)).toInt());
        if((state == DlPaused) || (state == DlCancelled)) {
            // note that in case of cancelled downloads, it will start from begining
            priv->m_downloads[i]->resume();
        }              
    }
}

void ParallelDownloadManager::addToParallelDownload(Download* dl)
{
    DM_PRIVATE(ParallelDownloadManager);
    // set the download type
    ClientDownload* download;
    download = dynamic_cast<ClientDownload*>(dl);
    if(download)
        download->attributes().insert(DlDownloadType, (int)Parallel);
    priv->m_downloads.append(dl);
}
