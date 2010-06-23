/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#ifndef __DOWNLOAD_CONTROLLER_PRIVATE_H__
#define __DOWNLOAD_CONTROLLER_PRIVATE_H__

#include <QObject>
#include "BWFGlobal.h"

class QNetworkProxy;
class QString;
class QUrl;

class DownloadEvent;
class DownloadManager;

class DownloadController;

class BWF_EXPORT DownloadControllerPrivate : public QObject
{
public:
    DownloadControllerPrivate(
            DownloadController * controller,
            const QString & client,
            const QNetworkProxy & proxy);

    ~DownloadControllerPrivate();

    void startDownload(const QUrl & url, const QFileInfo & info);
    void startDownload(QNetworkReply * reply);
    void startDownload(const QNetworkRequest & request);

private:
    void startDownload(Download * download, const QUrl & url);

    bool handleDownloadManagerEvent(DownloadEvent * event);
    bool handleDownloadEvent(DownloadEvent * event);

protected:
    bool event(QEvent * event);

private:
    DownloadController * m_downloadController;
    DownloadManager * m_downloadManager; // owned
};

#endif // __DOWNLOAD_CONTROLLER_PRIVATE_H__
