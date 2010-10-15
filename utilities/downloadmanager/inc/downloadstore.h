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

#ifndef DOWNLOADSTORE_H
#define DOWNLOADSTORE_H

#include <QObject>
#include <QByteArray>
#include <QIODevice>

// class declaration

// abstract class for all concrete storage implementations
class DownloadStore 
{
public:
    virtual ~DownloadStore(){}

    // to write the data to storage
    virtual int write(const QByteArray& data,bool lastChunk=0) = 0 ;
    // closes storage
    virtual int close() = 0 ;
    // opens storage
    virtual int open(QIODevice::OpenMode) = 0;
    // creates the storage
    virtual int createStore() = 0;
    // deletes the storage
    virtual int deleteStore() = 0;
    // returns the size of stored data in the storage
    virtual int storedDataSize() = 0;
};    

#endif
