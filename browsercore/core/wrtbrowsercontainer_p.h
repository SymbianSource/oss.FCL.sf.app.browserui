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


#ifndef __WRTBROWSERCONTAINER_P_H__
#define __WRTBROWSERCONTAINER_P_H__
#include "ZoomMetaData.h"

class QGraphicsWidget;
struct BrowserPageFactory;
struct ZoomMetaData;

namespace WRT {
    class SchemeHandler;
    class SecSession;
    class WrtBrowserContainer;
    class SecureUIController;
    class LoadController;

    class WrtBrowserContainerPrivate
    {
    public:
        WrtBrowserContainerPrivate(QObject* parent=0 , WrtBrowserContainer* page = NULL);
        ~WrtBrowserContainerPrivate();
        WrtBrowserContainer* m_page;
        SchemeHandler* m_schemeHandler; 
        QString m_pageGroupName;
        BrowserPageFactory* m_pageFactory;
        QGraphicsWidget* m_widget;
        WRT::SecureUIController *m_secureController; //Owned
        WRT::LoadController * m_loadController; //Owned
        ZoomMetaData m_zoomData;
    };
}
#endif
