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


#ifndef __WRTMESSAGEBOXPROXY_H__
#define __WRTMESSAGEBOXPROXY_H__

#include <QString>
#include <QMessageBox>
#include "BWFGlobal.h"

namespace WRT {
    class BWF_EXPORT MessageBoxProxy
    {
     public:

         QString m_text;
         QString m_informativeText;
         QString m_detailedText;
         QMessageBox::StandardButtons m_buttons;
         QMessageBox::StandardButton m_defaultButton;
         QMessageBox::Icon m_icon;

     public:

         MessageBoxProxy();
         ~MessageBoxProxy();
         virtual void onMessageBoxResponse(int retValue) = 0;
     };

}

#endif
