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


#ifndef __WINDOWVIEW_P_H__
#define __WINDOWVIEW_P_H__

#include <QWidget>
#include <QGraphicsWidget>
#include <QAction>
#include <QTimer>

class QWebHistory;

namespace WRT {
    class WrtBrowserContainer;
    class WrtPageManager;
    class GraphicsFlowInterface;

    class WindowViewPrivate
    {
    public: // public actions available for this view
        QAction * m_actionForward;
        QAction * m_actionBack;
        QAction * m_actionOK;
        QAction * m_actionCancel;
        QAction * m_actionAddWindow;
        QAction * m_actionDelWindow;

    public:
        WindowViewPrivate(WrtPageManager * mgr, QWidget* parent);
        WindowViewPrivate(WrtPageManager * mgr, QGraphicsWidget* parent);
        ~WindowViewPrivate();

        GraphicsFlowInterface * m_flowInterface;
        QWidget * m_widgetParent;
        QGraphicsWidget * m_graphicsWidgetParent;

        WrtPageManager * m_pageManager; // not owned
        WrtBrowserContainer * m_activePage; // not owned

        int m_state;

        QTimer * m_animateTimer;
        int m_animateCount;
        WrtBrowserContainer * m_newCenterPage;

        QList<WrtBrowserContainer*>* m_pageList;

        int m_newPageIndex;
        QImage * m_blankWindowImg;
        QSize m_windowViewSize;

    private:
        void init();
    };
}
#endif // __WINDOWVIEW_P_H__
