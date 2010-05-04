/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef COOKIEJAR_H
#define COOKIEJAR_H

#include <qnetworkcookie.h>

#include <qstringlist.h>

namespace WRT {

class CookieJar : public QNetworkCookieJar
{
    Q_OBJECT

public:
    CookieJar(QObject *parent = 0);
    ~CookieJar();

    QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const;
    bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);

    void clear();

private slots:
    void save();

private:
    void purgeOldCookies();
    void load();
    bool m_loaded;
    QString m_cookiesFile;
    QString m_cookiesDir;
};
}

#endif // COOKIEJAR_H

