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


#ifndef __SECUREUICONTROLLER_P_H__
#define __SECUREUICONTROLLER_P_H__

#include <QWidget>
#include "BWFGlobal.h"
#include <QNetworkReply>

namespace WRT {

    class SecureUIController;
    class SecureUIControllerPrivate
    {
     public:
        SecureUIControllerPrivate(SecureUIController* qq);
        ~SecureUIControllerPrivate();

        SecureUIController * const q;

        QWidget * m_widgetParent; // owned
        int m_internalState;
        int m_secureState;
        QList<QString> m_sslTrustedHostList;
        QNetworkReply* m_reply;
    };
}
#endif // __SECUREUICONTROLLER_P_H__
