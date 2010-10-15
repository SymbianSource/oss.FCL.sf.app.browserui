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

#ifndef HTTPDOWNLOADBACKEND_H
#define HTTPDOWNLOADBACKEND_H

#include "downloadbackend.h"
#include "dmcommon.h"

// forward declarations
class DownloadCore;
class ClientDownload;
class DownloadStore;
class HttpDownloadBackendPrivate;

// class declaration

// concrete download implementation class for normal http downloads
class HttpDownloadBackend : public DownloadBackend
{
    DM_DECLARE_PRIVATE(HttpDownloadBackend); // private implementation
public:
    HttpDownloadBackend(DownloadCore *dlCore, ClientDownload *dl, DownloadStore* store);
    ~HttpDownloadBackend();

    // overloaded function for resuming download
    int resume();
    // overloaded function for storing the data
    void store(QByteArray data, bool lastChunk=false);
    // overloaded function for deleting the storage
    void deleteStore();
    // overloaded function which returns the stored data size
    qint64 storedDataSize();
    // fetches the attributes
    QVariant getAttribute(DownloadAttribute attr);
    // sets the attributes
    int setAttribute(DownloadAttribute attr, const QVariant& value);
public slots:
    void headerReceived();
};

#endif


