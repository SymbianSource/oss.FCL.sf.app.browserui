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

#ifndef PROGRESSIVE_DOWNLOAD_SERVER_H
#define PROGRESSIVE_DOWNLOAD_SERVER_H

#include "dmpimpl.h"
#include <QObject>
#include <QTcpServer>

typedef QAbstractSocket::SocketError ProgressiveDlServerError;

// forward declarations
class ProgressiveDownloadServerPrivate;
class Download;

// class declaration 
class ProgressiveDownloadServer : public QObject {
    Q_OBJECT
    DM_DECLARE_PRIVATE(ProgressiveDownloadServer);
public:
    Q_DECL_EXPORT ProgressiveDownloadServer(Download* download); 
    virtual ~ProgressiveDownloadServer();

    // starts the server
    Q_DECL_EXPORT int startServer(void);
    // stops the server
    Q_DECL_EXPORT int stopServer(void);
    // returns the port number
    Q_DECL_EXPORT quint16 serverPort(void);
    // last error occurred
    Q_DECL_EXPORT ProgressiveDlServerError lastError(void);
    // last error occurred
    Q_DECL_EXPORT QString lastErrorString(void);

private: // copy constructor and assaignment operator
    ProgressiveDownloadServer();
    ProgressiveDownloadServer(const ProgressiveDownloadServer&);
    ProgressiveDownloadServer &operator=(const ProgressiveDownloadServer&);
    bool event(QEvent *event);
signals:

private slots:
    void handleConnection(void);
    void handleRequest(void);

};

#endif
