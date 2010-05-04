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


#ifndef __BROWSERPAGEFACTORY_H__
#define __BROWSERPAGEFACTORY_H__

#include "BWFGlobal.h"

class QWebPage;

struct BWF_EXPORT BrowserPageFactory 
{
    virtual QWebPage* openPage() = 0;

    static QWebPage* openBrowserPage();
    bool m_bRestoreSession;
};

#endif // __BROWSERPAGEFACTORY_H__

