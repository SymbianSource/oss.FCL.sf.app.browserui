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


#ifndef __IMAGEVIEW_H__
#define __IMAGEVIEW_H__

#include <QWidget>
#include <QGraphicsWidget>
#include <QAction>
#include "controllableviewimpl.h"

class QWebHistory;

namespace WRT {

    class WrtBrowserContainer;
    class WrtPageManager;
    class FlowInterface;
    class ImageViewPrivate;

    class BWF_EXPORT ImageView : public ControllableViewQWidgetBase
    {
        Q_OBJECT
    public:
        ImageView(WrtPageManager * mgr, QWidget* parent);
        ImageView(WrtPageManager * mgr, QGraphicsWidget* parent);
        ~ImageView();
        WrtPageManager* wrtPageManager();
        QImage getCurrent() const;
        int getCurrentImageCount();

    public: // public actions available for this view
        QAction * getActionForward();
        QAction * getActionBack();
        QAction * getActionOK();
        QAction * getActionCancel();


    public: // Pure Virtual from ControllableView
        QString title();
        bool isActive();
        QList<QAction*> getContext();

    private:
        void refreshFlowInterface();

    signals:
        void centerIndexChanged(int index);
        void ok(QImage img);
        void cancel();

    public slots:
        void activate();
        void deactivate();
        void setCenterIndex(int i);
        void forward();
        void back();

    private slots:
        void updateActions(int centerIndex);
        void chooseImage(int index);
        void updateImageList(bool status);

    protected:
        QWidget* qWidget() const;
        ImageViewPrivate * const d;
    };
}
#endif // __IMAGEVIEW_H__
