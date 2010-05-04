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


#ifndef __IMAGEVIEW_P_H__
#define __IMAGEVIEW_P_H__

#include <QWidget>
#include <QGraphicsWidget>
#include <QAction>

class QWebHistory;

namespace WRT {

    class WrtBrowserContainer;
    class WrtPageManager;
    class FlowInterface;

    class ImageViewPrivate
    {
    public: // public actions available for this view
        QAction * m_actionForward;
        QAction * m_actionBack;
        QAction * m_actionOK;
        QAction * m_actionCancel;

    public:
        ImageViewPrivate(WrtPageManager * mgr, QWidget* parent);
        ImageViewPrivate(WrtPageManager * mgr, QGraphicsWidget* parent);
        ~ImageViewPrivate();

        FlowInterface* m_flowInterface;
        QWidget * m_widgetParent;
        QGraphicsWidget * m_graphicsWidgetParent;

        WrtPageManager* m_pageManager;
        int m_imageIndex;
        WrtBrowserContainer* m_activePage;
        QList<QImage> m_imageList;

        bool m_isActive;

    private:
        void init();

    };
}
#endif // __IMAGEVIEW_P_H__
