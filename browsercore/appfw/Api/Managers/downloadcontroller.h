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


#ifndef __DOWNLOAD_CONTROLLER_H__
#define __DOWNLOAD_CONTROLLER_H__

#include <QObject>
#include "BWFGlobal.h"

class QNetworkProxy;
class QNetworkReply;
class QNetworkRequest;
class QString;
class QUrl;
class QWebPage;

class Download;

class DownloadControllerPrivate;

class BWF_EXPORT DownloadController : public QObject
{
    Q_OBJECT

public:
    DownloadController(const QString & client, const QNetworkProxy & proxy);
    ~DownloadController();

    static void debugDownload(Download * download);

public slots:
    bool handlePage(QWebPage * page);

private slots:
    void startDownload(QNetworkReply * reply);
    void startDownload(const QNetworkRequest & request);

signals:
    void downloadCreated(Download * download);

    void downloadStarted(Download * download);

    void downloadHeaderReceived(Download * download);

    void downloadProgress(Download * download);

    void downloadFinished(Download * download);

    void downloadPaused(Download * download, const QString & error);

    void downloadCancelled(Download * download, const QString & error);

    void downloadFailed(Download * download, const QString & error);

    void downloadNetworkLoss(Download * download, const QString & error);

    void downloadError(Download * download, const QString & error);

    void downloadsCleared();

private:
    DownloadControllerPrivate * d;

    // Signals are protected, so they can't normally be emitted from methods
    // in unrelated classes.  Therefore to emit DownloadController signals
    // from methods in DownloadControllerPrivate, DownloadController must
    // declare DownloadControllerPrivate as a friend.

    friend class DownloadControllerPrivate;
};

#endif // __DOWNLOAD_CONTROLLER_H__
