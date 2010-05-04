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


#include "Downloads.h"
#include "Utilities.h"

#include <QtDebug>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QWebPage>

#include "bedrockprovisioning.h"
#include "downloadcontroller.h"
#include "download.h"
#include "downloadmanager.h"

namespace GVA {

Downloads::Downloads()
: m_downloadController(0)
{
    setObjectName("downloads");
}

Downloads::~Downloads()
{
}

void Downloads::handlePage(QWebPage * page)
{
    if (m_downloadController == 0) {
        QString client = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->applicationName();

        QNetworkAccessManager * accessManager = page->networkAccessManager();
        QNetworkProxy proxy = accessManager->proxy();

        m_downloadController = new DownloadController(client, proxy);

        safe_connect(m_downloadController, SIGNAL(downloadCreated(Download *)),
                this, SLOT(reportDownloadCreated(Download *)));

        safe_connect(m_downloadController, SIGNAL(downloadStarted(Download *)),
                this, SLOT(reportDownloadStarted(Download *)));

        safe_connect(m_downloadController, SIGNAL(downloadFinished(Download *)),
                this, SLOT(reportDownloadSuccess(Download *)));

        safe_connect(m_downloadController, SIGNAL(downloadFailed(Download *, const QString &)),
                this, SLOT(reportDownloadFailure(Download *, const QString &)));

        safe_connect(m_downloadController, SIGNAL(downloadPaused(Download *, const QString &)),
                this, SLOT(reportDownloadFailure(Download *, const QString &)));

        safe_connect(m_downloadController, SIGNAL(downloadCancelled(Download *, const QString &)),
                this, SLOT(reportDownloadFailure(Download *, const QString &)));

        safe_connect(m_downloadController, SIGNAL(downloadNetworkLoss(Download *, const QString &)),
                this, SLOT(reportDownloadFailure(Download *, const QString &)));

        // There is no Download* argument to extract data from so we may as well
        // just connect signal to signal without going through another slot.
        safe_connect(m_downloadController, SIGNAL(downloadsCleared()),
                this, SIGNAL(downloadsCleared()));
    }

    m_downloadController->handlePage(page);
}

void Downloads::reportDownloadCreated(Download * download)
{
    // Localize dialog message.

    QString fmt = qtTrId("fmt_browser_downloading_file");
    QString msg = fmt.arg(download->getAttribute(DlFileName).toString());

    emit downloadCreated(msg);
}

void Downloads::reportDownloadStarted(Download * download)
{
    // Localize dialog message.

    QString fmt = qtTrId("fmt_browser_downloading_file");
    QString msg = fmt.arg(download->getAttribute(DlFileName).toString());

    emit downloadCreated(msg);
}

void Downloads::reportDownloadSuccess(Download * download)
{
    // Localize dialog message.

    QString fmt = qtTrId("fmt_browser_file_has_finished_downloading");
    QString msg = fmt.arg(download->getAttribute(DlFileName).toString());

    emit downloadSuccess(msg);

    // Don't forget to remove the download; otherwise the download
    // controller won't know to emit the DownloadsCleared signal.

    DownloadManager * manager = download->downloadManager();

    manager->removeOne(download);
}

void Downloads::reportDownloadFailure(Download * download, const QString & error)
{
    // What went wrong?

    DownloadController::debugDownload(download);

    // Localize dialog message.

    QString fmt = qtTrId("fmt_browser_tag_error_tag_file_could_not_be_downloaded");
    QString msg = fmt.arg(
            "<span style=\"color:red\">",
            "</span>",
            download->getAttribute(DlFileName).toString());

    emit downloadFailure(msg);

    // Don't forget to remove the download; otherwise the download
    // controller won't know to emit the DownloadsCleared signal.

    DownloadManager * manager = download->downloadManager();

    manager->removeOne(download);
}

} // namespace GVA
