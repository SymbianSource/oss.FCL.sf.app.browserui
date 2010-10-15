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
#include "sequentialdownloadmanager.h"
#include "downloadevent.h"
#include "clientdownload.h"

class SequentialDownloadManagerPrivate
{
    //declare public implementation
    DM_DECLARE_PUBLIC(SequentialDownloadManager);
public:
    SequentialDownloadManagerPrivate();
    ~SequentialDownloadManagerPrivate();

    QList<Download*> m_createdDownloads; // list of downloads which have been created by the client and owns them
    QQueue<Download*> m_downloadsQueue; // queue of downloads scheduled for downloading and owns them
    QList<Download*> m_completedDownloads; // list of downloads which have been completed and owns them
    QList<Download*> m_failedDownloads; // list of failed downloads
    QList<Download*> m_totalDownloads; // total of all the above downloads, but this doesnt own downloads
};

SequentialDownloadManagerPrivate::SequentialDownloadManagerPrivate()
{ }

SequentialDownloadManagerPrivate::~SequentialDownloadManagerPrivate()
{
    int createdSize = m_createdDownloads.size();
    for (int i = 0; i < createdSize; i++)
        delete m_createdDownloads[i];
    m_createdDownloads.clear();

    int completedSize = m_completedDownloads.size();
    for (int i = 0; i < completedSize; i++)
        delete m_completedDownloads[i];
    m_completedDownloads.clear();

    int size = m_downloadsQueue.size();
    for (int i = 0; i < size; i++) {
        DownloadState state = (DownloadState)((m_downloadsQueue[i]->getAttribute(DlDownloadState)).toInt());
        if (state == DlInprogress) {
            // pause the download if it is in progress
            m_downloadsQueue[i]->pause();
        } else if (state == DlFailed) {
            // cancel the download if it has failed
            m_downloadsQueue[i]->cancel();
        }
        delete (m_downloadsQueue[i]);
    }
    m_downloadsQueue.clear();
}

/*
  constructor
*/
SequentialDownloadManager::SequentialDownloadManager()
{
    DM_INITIALIZE(SequentialDownloadManager);
}

/*
  destructor
*/
SequentialDownloadManager::~SequentialDownloadManager()
{
    DM_UNINITIALIZE(SequentialDownloadManager);
}

/*
  returns all the current downloads
*/
QList<Download*>& SequentialDownloadManager::currentDownloads()
{
    DM_PRIVATE(SequentialDownloadManager);
    priv->m_totalDownloads.clear();
    for (int i = 0; i < priv->m_completedDownloads.size(); ++i)
         priv->m_totalDownloads.append(priv->m_completedDownloads[i]);
    for (int i = 0; i < priv->m_downloadsQueue.size(); ++i)
        priv->m_totalDownloads.append(priv->m_downloadsQueue[i]);
    for (int i = 0; i < priv->m_createdDownloads.size(); ++i)
        priv->m_totalDownloads.append(priv->m_createdDownloads[i]);
    for (int i = 0; i < priv->m_failedDownloads.size(); ++i)
        priv->m_totalDownloads.append(priv->m_failedDownloads[i]);
    return priv->m_totalDownloads;
}

/*
  finds a download provided id if exists
  \a id indicates identifier for download
*/
Download* SequentialDownloadManager::findDownload(int dlId)
{
    DM_PRIVATE(SequentialDownloadManager);
    for (int i = 0; i < priv->m_completedDownloads.size(); ++i) {
        if (priv->m_completedDownloads[i]->id() == dlId)
            return priv->m_completedDownloads[i];
    }
    for (int i = 0; i < priv->m_downloadsQueue.size(); ++i) {
        if (priv->m_downloadsQueue[i]->id() == dlId)
            return priv->m_downloadsQueue[i];
    }
    for (int i = 0; i < priv->m_createdDownloads.size(); ++i) {
        if (priv->m_createdDownloads[i]->id() == dlId)
            return priv->m_createdDownloads[i];
    }
    return 0;
}

/*
  cancels all the downloads
*/
void SequentialDownloadManager::removeAll()
{
    DM_PRIVATE(SequentialDownloadManager);
    //traverse the list and delete each download in the list
    for(int i = 0; i < priv->m_downloadsQueue.count(); ) {
        // cancel the download
        priv->m_downloadsQueue[0]->cancel();
        delete (priv->m_downloadsQueue[0]);
        priv->m_downloadsQueue.removeAt(0);
    }
    priv->m_downloadsQueue.clear();

    int createdDownloadsCount = priv->m_createdDownloads.count();
    //traverse the list and delete each download in the list
    for(int i = 0; i < createdDownloadsCount; i++)
        delete (priv->m_createdDownloads[i]);
    priv->m_createdDownloads.clear();

    int completedDownloadsCount = priv->m_completedDownloads.count();
    //traverse the list and delete each download in the list
    for(int i = 0; i < completedDownloadsCount; i++)
        delete (priv->m_completedDownloads[i]);
    priv->m_completedDownloads.clear();

    int failedDownloadsCount = priv->m_failedDownloads.count();
    //traverse the list and delete each download in the list
    for(int i = 0; i < failedDownloadsCount; i++)
        delete (priv->m_failedDownloads[i]);
    priv->m_failedDownloads.clear();
}

/*
  cancels and removes the download
  \a dl indicates the download to be canceled and removed
*/
void SequentialDownloadManager::removeOne(Download *dl)
{
    if(!dl)
        return;
    DM_PRIVATE(SequentialDownloadManager);
    if (currentDownloads().contains(dl)) {
        dl->cancel();
        // download might be there in any of the following list
        priv->m_downloadsQueue.removeOne(dl);
        priv->m_createdDownloads.removeOne(dl);
        priv->m_completedDownloads.removeOne(dl);
        delete dl;
        dl = 0;
    }
}

/*
  pauses all the downloads which are in progress
*/
void SequentialDownloadManager::pauseAll()
{
    DM_PRIVATE(SequentialDownloadManager);
    if (!priv->m_downloadsQueue.isEmpty()) {
        QQueue<Download*>::iterator i;
        i = priv->m_downloadsQueue.begin();
        DownloadState state = (DownloadState)(((*i)->getAttribute(DlDownloadState)).toInt());
        if ((state == DlInprogress || state == DlStarted))
            dynamic_cast<ClientDownload*>(*i)->pauseDownload();
    }
}

/*
  resumes all the downloads which are paused
*/
void SequentialDownloadManager::resumeAll()
{
    DM_PRIVATE(SequentialDownloadManager);
    if (!priv->m_downloadsQueue.isEmpty()) {
        QQueue<Download*>::iterator i;
        i = priv->m_downloadsQueue.begin();
        DownloadState state = (DownloadState)(((*i)->getAttribute(DlDownloadState)).toInt());
        if (state == DlPaused)
            dynamic_cast<ClientDownload*>(*i)->resumeDownload();
    }
}

void SequentialDownloadManager::addToSequentialDownload(Download* dl)
{
    // Here the downloads are added first to a download list. When the downloads are in this list,
    // the client can set some attributes on the download.
    // Later the client has to explicitly call start() on the download after which,
    // download gets added to the downloadsQueue and the download starts.

    DM_PRIVATE(SequentialDownloadManager);

    // set the download type
    ClientDownload* download = dynamic_cast<ClientDownload*>(dl);
    if(download)
        download->attributes().insert(DlDownloadType, (int)Sequential);

    // if the download is already in queue but the prority has been changed
    if( findDownload(dl->id()))
        priv->m_createdDownloads.removeOne(dl);

    DownloadPriority priority = (DownloadPriority)(dl->getAttribute(DlPriority)).toInt();
    if (priority == High) {
        QList<Download*>::iterator i;
        for (i = priv->m_createdDownloads.begin(); i != priv->m_createdDownloads.end(); i++) {
            if ((*i)->getAttribute(DlPriority).toInt() == Low) {
                priv->m_createdDownloads.insert(i, dl);
                return;
            }
        }
        priv->m_createdDownloads.insert(i, dl); // if no low priority download is found then just insert the high priority download
    }
    else
        priv->m_createdDownloads.append(dl);
}

void SequentialDownloadManager::addToDownloadQueue(Download* dl)
{
    DM_PRIVATE(SequentialDownloadManager);
    priv->m_downloadsQueue.append(dl);
}

void SequentialDownloadManager::addToCompletedList(Download* dl)
{
    DM_PRIVATE(SequentialDownloadManager);
    priv->m_completedDownloads.append(dl);
}

void SequentialDownloadManager::process(int dlId)
{
    DM_PRIVATE(SequentialDownloadManager);
    // Start() has been called on a download present in the createdDownloads list. Move that download from the list to the queue
    ClientDownload* dl = dynamic_cast<ClientDownload*>(findDownload(dlId));
    dl->registerEventReceiver(this);
    priv->m_createdDownloads.removeOne(dl); // Once start() is called on download, add it to downloadQueue & remove it from createdDownloads list
    if (priv->m_downloadsQueue.isEmpty()) {
        addToDownloadQueue(dl);
        dl->startDownload();
        return;
    }
    DownloadPriority priority = (DownloadPriority)((dl->getAttribute(DlPriority)).toInt());
    // checking for priority
    if (priority == High) {
        // Add the download before the first low priority item
        QQueue<Download*>::iterator i;
        for (i = priv->m_downloadsQueue.begin(); i != priv->m_downloadsQueue.end(); i++) {
            if ((*i)->getAttribute(DlPriority).toInt() == Low) {
                if (i == priv->m_downloadsQueue.begin()) {
                    dynamic_cast<ClientDownload*>(*i)->pauseDownload();
                    priv->m_downloadsQueue.insert(i, dl); // low priority dl is in progress so pause it, insert the high priority dl and start it.
                    dl->startDownload();
                }
                else {
                    // low priority download is present in the queue but it's not the first one as it is not yet started,
                    // so there's already a high priority dl in progress. So this high priority dl should just be inserted into the queue.
                    priv->m_downloadsQueue.insert(i, dl);
                }
                return;
            }
        }
        priv->m_downloadsQueue.insert(i, dl); // insert the high priority download if there are no low priority downloads in the queue
    }
    else
        addToDownloadQueue(dl); // If priority is low then just append to the queue.
}

bool SequentialDownloadManager::event(QEvent *event)
{
    DM_PRIVATE(SequentialDownloadManager);
    DEventType type = (DEventType)event->type();
    switch(type) {
    case Completed:
    {
        int dlId = ((DownloadEvent*)event)->getId();
        ClientDownload *dl = dynamic_cast<ClientDownload*>(findDownload(dlId));
        addToCompletedList(dl);
        // After adding the download to completed list, remove it from downloadQueue
        priv->m_downloadsQueue.removeOne(dl);
        startNext();
        break;
    }
    case Error:
    {
        int dlId = ((DownloadEvent*)event)->getId();
        ClientDownload *dl = dynamic_cast<ClientDownload*>(findDownload(dlId));
        priv->m_failedDownloads.append(dl);
        priv->m_downloadsQueue.removeOne(dl);
        startNext();
        break;
    }
    default:
        break;
    }
    return true;
}

void SequentialDownloadManager::startNext()
{
    DM_PRIVATE(SequentialDownloadManager);
    QQueue<Download*>::iterator i;
    if (!priv->m_downloadsQueue.isEmpty()) {
        i = priv->m_downloadsQueue.begin();
        DownloadState state = (DownloadState)(((*i)->getAttribute(DlDownloadState)).toInt());
        if (state == DlPaused)
            (*i)->resume();
        else
            dynamic_cast<ClientDownload*>(*i)->startDownload();
    }
}

void SequentialDownloadManager::pauseDownload(int dlId)
{
    DM_PRIVATE(SequentialDownloadManager);
    QQueue<Download*>::iterator i;
    if (!priv->m_downloadsQueue.isEmpty()) {
        i = priv->m_downloadsQueue.begin();
        if (dlId == (*i)->id())
            dynamic_cast<ClientDownload*>(*i)->pauseDownload();
    }
}

void SequentialDownloadManager::resumeDownload(int dlId)
{
    DM_PRIVATE(SequentialDownloadManager);
    QQueue<Download*>::iterator i;
    if (!priv->m_downloadsQueue.isEmpty()) {
        i = priv->m_downloadsQueue.begin();
        if (dlId == (*i)->id())
            dynamic_cast<ClientDownload*>(*i)->resumeDownload();
    }
}

void SequentialDownloadManager::cancelDownload(int dlId)
{
    DM_PRIVATE(SequentialDownloadManager);
    QQueue<Download*>::iterator i;
    if (!priv->m_downloadsQueue.isEmpty()) {
        i = priv->m_downloadsQueue.begin();
        if (dlId == (*i)->id())
            dynamic_cast<ClientDownload*>(*i)->cancelDownload();
    }
}
