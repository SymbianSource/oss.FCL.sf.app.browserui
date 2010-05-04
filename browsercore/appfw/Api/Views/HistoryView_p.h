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


#ifndef __HISTORYVIEW_P_H__
#define __HISTORYVIEW_P_H__

#include <QWidget>
#include <QGraphicsWidget>
#include <QAction>

class QWebHistory;

namespace WRT {
    class WrtBrowserContainer;
    class WrtPageManager;
    class GraphicsFlowInterface;

    class HistoryViewPrivate
    {
    public: // public actions available for this view
        QAction * m_actionForward;
        QAction * m_actionBack;
        QAction * m_actionOK;
        QAction * m_actionCancel;

    public:
        HistoryViewPrivate(WrtPageManager * mgr, QWidget* parent);
        HistoryViewPrivate(WrtPageManager * mgr, QGraphicsWidget* parent);
        ~HistoryViewPrivate();

        GraphicsFlowInterface * m_flowInterface;
        QWidget * m_widgetParent;
        QGraphicsWidget * m_graphicsWidgetParent;

        WrtPageManager * m_pageManager; // not owned
        WrtBrowserContainer * m_activePage; // not owned
        int m_historyIndex; 

        bool m_isActive;
    private:
        void init();
    };
}
#endif // __HISTORYVIEW_P_H__
