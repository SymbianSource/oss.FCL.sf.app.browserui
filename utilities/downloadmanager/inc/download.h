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

#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include "dmcommon.h"
#include "downloadevent.h"
#include <QObject>

class DownloadManager;
class QNetworkReply;
class DownloadInfo;
class DownloadPrivate;

typedef QList<QObject*> EventReceiverList; 

// class declaration
class Download : public QObject
{
    Q_OBJECT
public:
    virtual ~Download() {};

    // starts the download
     virtual int start() = 0;
    // id of the download
     virtual int id() = 0;
    // sets the attributes for the download
     virtual int setAttribute(DownloadAttribute attr, const QVariant& value) = 0;
    // fetches the attributes of download
     virtual QVariant getAttribute(DownloadAttribute attr) = 0;
    // pauses the download
     virtual int pause() = 0;
    // resumes the download
     virtual int resume() = 0;
    // cancels the download
     virtual int cancel() = 0;
    // registers the event listener
     virtual void registerEventReceiver(QObject *receiver) = 0;
    // unregisters the event listener
     virtual void unregisterEventReceiver(QObject *receiver) = 0;
    // returns the download manager
     virtual DownloadManager *downloadManager() = 0;
    // returns the list of child downloads
     virtual void getChildren(QList<Download*>& list) = 0;
};

#endif
