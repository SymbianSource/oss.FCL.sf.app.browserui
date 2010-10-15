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

#ifndef DOWNLOAD_CORE_MANAGER_H_
#define DOWNLOAD_CORE_MANAGER_H_

#include <QObject>
#include "dmpimpl.h"

// forward declarations
class DownloadCore;
class DownloadCoreManagerPrivate;  
class QNetworkReply;

// class declaration
class  DownloadCoreManager : public QObject
{
    Q_OBJECT
    DM_DECLARE_PRIVATE(DownloadCoreManager); // private implementation

public:
    DownloadCoreManager(const QString& clientName);
    virtual ~DownloadCoreManager();

private: // copy constructor and assaignment operator
      DownloadCoreManager(const DownloadCoreManager &);
      DownloadCoreManager &operator=(const DownloadCoreManager &);

public:
    // DownloadCore creation using url
    DownloadCore* createDownloadCore(const QString &url);
    // Download creation using network reply
    DownloadCore* createDownloadCore(QNetworkReply *reply);
    // retuns the client name
    QString& clientName(); 
};

#endif /*DOWNLOAD_CORE_MANAGER_H_*/
