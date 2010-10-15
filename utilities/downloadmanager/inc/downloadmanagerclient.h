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
#ifndef DOWNLOADMANAGERCLIENT_H
#define DOWNLOADMANAGERCLIENT_H

#include <QObject>
#include "downloadmanager.h"
#include "dmcommoninternal.h"
#include "dmpimpl.h"

class DownloadManagerClientPrivate;

class DownloadManagerClient : public QObject
{
    Q_OBJECT
    DM_DECLARE_PRIVATE(DownloadManagerClient);

public:
    DownloadManagerClient(DownloadManager* downloadManager);
    ~DownloadManagerClient();
    
    // low-level implementation of functions present in BackgroundDownloadManager class
    void setStartupInfo();
    void setProxy(const QString& proxyServer, const int port);
    int createDownload(const QString& url, DownloadType type);
    bool attachToDownload(int dlId);
    void removeDownload(int downloadId);
    void pauseAll();
    void resumeAll();
    void removeAll();
    QVariant getDownloadManagerAttribute(DownloadManagerAttribute attr);
    int setDownloadManagerAttribute(DownloadManagerAttribute attr, const QVariant& value);

    // low-level implementation of functions present in BackgroundDownload class
    int startDownload(int dlId);
    int pauseDownload(int dlId);
    int resumeDownload(int dlId);
    int cancelDownload(int dlId);
    QVariant getDownloadAttribute(int dlId, DownloadAttribute attr);
    int setDownloadAttribute(int dlId, DownloadAttribute attr, const QVariant& value);

    // server error functions
    int serverError();

private:
    // start server if required and connect to it; set proxy info in server
    bool initServer();
    // connect to download manager server
    bool connectToServer();
    // process Download and DownloadManager events
    void processEvents(QString eventMsg);
    // encodes the strings (client name, url, etc) so that it does not contain
    // message delimiter (DM_DELIMITER)
    QString encodeString(const QString& inputString);

private slots:
    // poll server for events
    void getEvents();
    // set server IPC error
    void setServerError(int );
};

#endif 
