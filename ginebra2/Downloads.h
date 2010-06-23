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

#ifndef __DOWNLOADS_H__
#define __DOWNLOADS_H__

#include <QObject>

class QUrl;
class QWebPage;

class DownloadController;
class Download;

namespace GVA {

class Downloads : public QObject
{
    Q_OBJECT

public:
    Downloads();
    virtual ~Downloads();

    void handlePage(QWebPage * page);

public slots:
    void downloadImage(const QString & imageUrl);

private slots:
#ifdef USE_DOWNLOAD_MANAGER
    void reportDownloadCreated(Download * download);
    void reportDownloadStarted(Download * download);
    void reportDownloadSuccess(Download * download);
    void reportDownloadFailure(Download * download, const QString & error);
#endif
    void reportUnsupportedDownload(const QUrl & url);

signals:
    void downloadCreated(const QString & messageHTML);
    void downloadStarted(const QString & messageHTML);
    void downloadSuccess(const QString & messageHTML);
    void downloadFailure(const QString & messageHTML);
    void downloadsCleared();
    void unsupportedDownload(const QString & messageHTML);

private:
    DownloadController * m_downloadController;
};

} // namespace GVA

#endif // __DOWNLOADS_H__
