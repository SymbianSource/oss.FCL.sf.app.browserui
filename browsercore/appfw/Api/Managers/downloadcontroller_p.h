/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
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
