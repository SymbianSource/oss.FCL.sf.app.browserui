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


#ifndef __THUMBNAILVIEW_P_H__
#define __THUMBNAILVIEW_P_H__

#include <QWidget>
#include <QGraphicsWidget>
#include <QAction>

namespace WRT {

    class WrtBrowserContainer;
    class WrtPageManager;
    class TnEngineHandler;
    class TnEngineView;

    class ThumbnailViewPrivate
    {
    public: // public actions available for this view
        QAction * m_actionOK;
        QAction * m_actionCancel;

    public:
        ThumbnailViewPrivate(WrtPageManager * mgr, QWidget* parent);
        ThumbnailViewPrivate(WrtPageManager * mgr, QGraphicsWidget* parent);
        ~ThumbnailViewPrivate();

        QWidget * m_widgetParent;
        QGraphicsWidget * m_graphicsWidgetParent;
        WrtPageManager* m_pageManager;
        WrtBrowserContainer* m_activePage;
        TnEngineHandler * m_tnEngineHandler;
        TnEngineView * m_tnEngineView;
        bool m_isActive;
        
    private:
        void init();
    };
}
#endif // __THUMBNAILVIEW_P_H__
