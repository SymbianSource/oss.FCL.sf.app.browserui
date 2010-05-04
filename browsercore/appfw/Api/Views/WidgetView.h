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


#ifndef __WIDGETVIEW_H__
#define __WIDGETVIEW_H__

#include <QWidget>
#include <QGraphicsWidget>
#include "controllableviewimpl.h"

namespace WRT {

    class WrtPageManager;
    class WrtBrowserContainer;
    class WidgetViewPrivate;

    class BWF_EXPORT WidgetView : public ControllableViewQWidgetBase
    {
        Q_OBJECT
        //public: // public actions available for this view        

    public:
        WidgetView(WrtPageManager * mgr, QWidget* parent);
        WidgetView(WrtPageManager * mgr, QGraphicsWidget* parent);
        ~WidgetView();
        WrtPageManager * wrtPageManager();
        QMenu* getMenu();

    public slots:
        void hide();
        void show();

    public: // Pure Virtual from ControllableView
        QWidget* widget();
        QString title();
        bool isActive();
        QList<QAction*> getContext();

    public slots: // Pure Virtual from ControllableView
        void activate();
        void deactivate();

    private:
        WidgetViewPrivate * const d;
    };
}

#endif // __CONTENTVIEW_H__

