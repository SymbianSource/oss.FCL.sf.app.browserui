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

#ifndef OMADOWNLOADBACKEND_H
#define OMADOWNLOADBACKEND_H

#include "dmcommon.h"
#include "dmcommoninternal.h"
#include "dmpimpl.h"
#include "downloadbackend.h"

// forward declarations
class OMADownloadBackendPrivate;
class DownloadCore;
class ClientDownload;

typedef QList<ClientDownload*> MediaDownloadList;

// class declaration

// concrete download implementation for OMA downloads
class OMADownloadBackend : public DownloadBackend
{
    Q_OBJECT
    DM_DECLARE_PRIVATE(OMADownloadBackend); // private implementation
public:
    OMADownloadBackend(DownloadCore *dlCore, ClientDownload *dl);
    ~OMADownloadBackend();

    // fetches the oma download attributes
    QVariant getAttribute(DownloadAttribute attr);
    // sets the oma download specific attributes
    int setAttribute(DownloadAttribute attr, const QVariant& value);
    // overloaded function for pausing the download
    int pause();
    // overloaded function for resuming paused download
    int resume();
    // overloaded function for cancelling the download
    int cancel();

    // stores the data in storage
    void store(QByteArray data, bool lastChunk=false);
    // deletes the storage
    void deleteStore();
    // returns the size of stored data
    qint64 storedDataSize();

private slots:
    void bytesRecieved(qint64 bytesRecieved, qint64 bytesTotal);
    void handleFinished();    
    void bytesUploaded(qint64, qint64); 
private:
    // parses download descriptor
    bool parseDownloadDescriptor();
    // does the capability check on receiving DD
    bool checkDownloadDescriptor();
    // event handler
    bool event(QEvent *event);
};

#endif   
