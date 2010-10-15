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

#ifndef FILESTORAGE_H
#define FILESTORAGE_H

#include "dmcommon.h"
#include "dmpimpl.h"
#include "downloadstore.h"
#include "clientdownload.h"
#include <QObject>

// forward declarations
class FileStoragePrivate;

// class declaration

// concrete implementation class
class FileStorage : public DownloadStore
{
    DM_DECLARE_PRIVATE(FileStorage); // private implementation
public:
    FileStorage(const QString& clientName, ClientDownload* download);
    virtual ~FileStorage();

    // writes to the file
    int write(const QByteArray& data, bool lastChunk=0);
    // closes the file
    int close();
    // opnes the file
    int open(QIODevice::OpenMode mode);
    // creates the storage
    int createStore();
    // deletes the file storage
    int deleteStore();
     //returns the size of loaded file
    int storedDataSize();

private:
    // moves the file
    void moveDlFile();
};    

#endif
