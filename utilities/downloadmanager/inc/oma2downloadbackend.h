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

#ifndef OMA2DOWNLOADBACKEND_H
#define OMA2DOWNLOADBACKEND_H

#include "dmcommon.h"
#include "dmcommoninternal.h"
#include "dmpimpl.h"
#include "downloadbackend.h"

// forward declarations
class OMA2DownloadBackendPrivate;
class DownloadCore;
class ClientDownload;

typedef QList<ClientDownload*> MediaDownloadList;

// class declaration

// concrete download implementation for OMA2 downloads
class OMA2DownloadBackend : public DownloadBackend
{
    Q_OBJECT
    DM_DECLARE_PRIVATE(OMA2DownloadBackend); // private implementation
public:
    OMA2DownloadBackend(DownloadCore *dlCore, ClientDownload *dl);
    ~OMA2DownloadBackend();

    // fetches the oma2 download attributes
    QVariant getAttribute(DownloadAttribute attr);
    // sets the oma2 download specific attributes
    int setAttribute(DownloadAttribute attr, const QVariant& value);
    // overloaded function for pausing the download
    int pause();
    // overloaded function for resuming paused download
    int resume();
    // overloaded function for cancelling the download
    int cancel();
    // overloaded function for getting child downloads
    void getChildren(QList<Download*>& list);

    // stores the data in storage
    void store(QByteArray data, bool lastChunk=false);
    // deletes the storage
    void deleteStore();
    // returns the size of stored data
    qint64 storedDataSize();
    // re-constructing all the values as part of persistent storage
    void init();


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
    // suppress user confirmation
    bool suppressUserConfirmation();
    // handle status code
    void handleStatusCode(const int& statusCode);
    // handle precondition failed
    void handlePreconditionFailed();
    // adds the downloads to downloadList
    void addtoDownloadList(ClientDownload *dl);
    // finds the download for the given id
    ClientDownload* findDownload(int id);
    // posting the install notify event to the web server
    void postInstallNotifyEvent(const char* statusMessage);
    // verifying downloads by going through the MediaObject map and setting OMA2'S state at the end.
    void verifyDownloads();
    // returns current MediaObject index
    int currentIndex();
    // setting values in QSettings for persistent storage
    void serializeData(ClientDownload* dl, int index);
};

#endif   
