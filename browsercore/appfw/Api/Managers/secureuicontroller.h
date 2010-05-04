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


#ifndef __SECUREUICONTROLLER_H__
#define __SECUREUICONTROLLER_H__

#include <QWidget>
#include <QSslError>
#include <QNetworkReply>
#include "BWFGlobal.h"
#include <qwebframe.h>
#include <QMessageBox>
#include "messageboxproxy.h"

#define NOLOAD 0x0
#define TOPLEVELLOADED 0x01
#define TOPLEVELSECURE 0x02
#define TOPLEVELUNTRUSTED 0x04
#define SUBLOADUNSECURE 0x08

namespace WRT {

    class SecureUIControllerPrivate;

    class BWF_EXPORT SecureUIController : public QObject,
                                          public MessageBoxProxy
    {
        Q_OBJECT
    public:
        enum SecureState{
            unsecureLoadFinished = 0,
            secureLoadFinished,
            untrustedLoadFinished,
            mixedLoadFinished,
            untrustedMixedLoadFinished,
            secureInvalid
        };
       /*!
        * SecureUIController Constructor
        * @param parent: parent QObject 
        */  
        SecureUIController(QObject* parent = 0);
        /*!
         * SecureUIController Destructor
         */ 

        ~SecureUIController();

        QList<QString> * sslTrustedHostList();
        int secureState();
        void setSecureState(int state);
        QNetworkReply* getReply();
        void setReply(QNetworkReply* reply);

        void onMessageBoxResponse(int retValue);

    public slots:

        void setTopLevelScheme(const QUrl & url);
        void secureCheck(QWebFrame* frame, QNetworkRequest* request);
        void endSecureCheck(bool);
        void onSslErrors(QNetworkReply*,const QList<QSslError> &);
                
    signals:
        void pageSecureState(int);
        void loadStopped();
        void showMessageBox(WRT::MessageBoxProxy* data);
        
    private:
        void determineSecureState();
        SecureUIControllerPrivate * const d;
    };
}
#endif // __SECUREUICONTROLLER_H__
