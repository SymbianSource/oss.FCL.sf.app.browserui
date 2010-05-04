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


#ifndef __DOWNLOADS_H__
#define __DOWNLOADS_H__

#include <QObject>

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

private slots:
    void reportDownloadCreated(Download * download);
    void reportDownloadStarted(Download * download);
    void reportDownloadSuccess(Download * download);
    void reportDownloadFailure(Download * download, const QString & error);

signals:
    void downloadCreated(const QString & messageHTML);
    void downloadStarted(const QString & messageHTML);
    void downloadSuccess(const QString & messageHTML);
    void downloadFailure(const QString & messageHTML);
    void downloadsCleared();

private:
    DownloadController * m_downloadController;
};

} // namespace GVA

#endif // __DOWNLOADS_H__
