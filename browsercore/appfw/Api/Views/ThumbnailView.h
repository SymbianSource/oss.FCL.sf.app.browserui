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


#ifndef __THUMBNAILVIEW_H__
#define __THUMBNAILVIEW_H__

#include <QWidget>
#include <QGraphicsWidget>
#include <QAction>
#include "controllableviewimpl.h"

namespace WRT {

    class WrtBrowserContainer;
    class WrtPageManager;
    class TnEngineHandler;
    class TnEngineView;
    class ThumbnailViewPrivate;

    class BWF_EXPORT ThumbnailView : public ControllableViewQWidgetBase
    {
        Q_OBJECT
    public:
        ThumbnailView(WrtPageManager * mgr, QWidget* parent);
        ThumbnailView(WrtPageManager * mgr, QGraphicsWidget* parent);
        ~ThumbnailView();
        
        WrtPageManager* wrtPageManager();

    public: // public actions available for this view
        QAction * getActionOK();
        QAction * getActionCancel();

    public: // Pure Virtual from ControllableView

        QString title();
        bool isActive();
        QList<QAction*> getContext();
        static const QString Type() { return "ImageFlowView"; }
        virtual QString type() const { return Type(); }

    signals:
        void scrollStarted();  
        void scrollBy(int x, int y);
        void scrollEnded();
        void ok(int x, int y);
        void cancel();

    public slots:
        void activate();
        void deactivate();
        void scrollViewBy(int, int);

    protected:
        QWidget* qWidget() const;
    private:
        ThumbnailViewPrivate * const d;
    };
}
#endif // __THUMBNAILVIEW_H__
