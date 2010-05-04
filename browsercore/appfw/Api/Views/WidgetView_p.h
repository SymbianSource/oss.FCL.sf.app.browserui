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


#ifndef __WIDGETVIEW_P_H__
#define __WIDGETVIEW_P_H__

#include <QWidget>
#include <QGraphicsWidget>

namespace WRT {

    class WrtPageManager;
    class WrtBrowserContainer;

    class WidgetViewPrivate
    {
    public:
        WidgetViewPrivate(WrtPageManager * mgr, QWidget* parent);
        WidgetViewPrivate(WrtPageManager * mgr, QGraphicsWidget* parent);
        ~WidgetViewPrivate();

        WrtPageManager * m_pageManager;
        QWidget * m_widgetParent;
        QGraphicsWidget * m_graphicsWidgetParent;

        bool m_isActive;
    };
}

#endif // __WIDGETVIEW_P_H__

