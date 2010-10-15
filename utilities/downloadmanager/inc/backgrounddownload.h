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

#ifndef BACKGROUNDDOWNLOAD_H
#define BACKGROUNDDOWNLOAD_H

#include "download.h"
#include "dmcommon.h"
#include "dmpimpl.h"
#include <QObject>

class DownloadManager;
class BackgroundDownloadManager;
class BackgroundDownloadPrivate;

typedef QList<QObject*> EventReceiverList;

// class declaration
class BackgroundDownload : public Download
{
    Q_OBJECT
    DM_DECLARE_PRIVATE(BackgroundDownload);
public:
    BackgroundDownload(DownloadManager* mgr, const QString& url, int dlId);
    virtual ~BackgroundDownload();

    // starts the download
    int start();
    // id of the download
    int id();
    // sets the attributes for the download
    int setAttribute(DownloadAttribute attr, const QVariant& value);
    // fetches the attributes of download
    QVariant getAttribute(DownloadAttribute attr);
    // pauses the download
    int pause();
    // resumes the download
    int resume();
    // cancels the download
    int cancel();
    // registers the event listener
    void registerEventReceiver(QObject *receiver);
    // unregisters the event listener
    void unregisterEventReceiver(QObject *receiver);
    // returns the download manager
    DownloadManager* downloadManager();
    // returns the list of child downloads
    void getChildren(QList<Download*>& list);
    // posts the event to recievers event loop
    void postEvent(DEventType type, DlEventAttributeMap* attrMap);
    // remove info of this download
    void removeDownloadInfo();

private: // copy constructor and assaignment operator
    BackgroundDownload(const BackgroundDownload &);
    BackgroundDownload &operator=(const BackgroundDownload &);

    // returns the event listeners
    EventReceiverList& eventReceivers();
};

#endif
