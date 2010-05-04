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


#ifndef __WEBNETWORKACCESSMANAGER_H__
#define __WEBNETWORKACCESSMANAGER_H__

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "messageboxproxy.h"

#if QT_VERSION >= 0x040500
#include <QNetworkDiskCache>
#endif

namespace WRT {

class WrtBrowserContainer;
class CookieJar;


class WebNetworkAccessManager : public QNetworkAccessManager,
                                public MessageBoxProxy
{
    Q_OBJECT

public:
    WebNetworkAccessManager(WrtBrowserContainer* page, QObject* parent = 0);
    virtual ~WebNetworkAccessManager();

    void onMessageBoxResponse(int retValue);

public slots:

protected:
    virtual QNetworkReply *createRequest(Operation op, const QNetworkRequest &request,
                                         QIODevice *outgoingData = 0);
private:
    void setupCache();
    void setupNetworkProxy();
    //Handle connection request.
    QNetworkReply* createRequestHelper(Operation op, const QNetworkRequest &request, QIODevice *outgoingData = 0);

private slots:
#ifdef NETWORK_DEBUG	
    void error(QNetworkReply::NetworkError code);
#endif 
    
private:
    WrtBrowserContainer* m_browserContainer;
    CookieJar* m_cookieJar;
    QNetworkReply* m_reply;
    QNetworkReply* n_reply; // Not owned, do not delete 
    QNetworkRequest* m_req;

#if QT_VERSION >= 0x040500
    QNetworkDiskCache *qDiskCache;
#endif

signals:
    void showMessageBox(WRT::MessageBoxProxy* data);

};
}
#endif
