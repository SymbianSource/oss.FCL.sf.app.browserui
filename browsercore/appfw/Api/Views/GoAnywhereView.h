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


#ifndef __GOANYWHEREVIEW_H__
#define __GOANYWHEREVIEW_H__

#include "controllableviewimpl.h"

#include <QLabel>
#include <QTimer>
#include <QWidget>
#include <QTabWidget>

class QAction;
class QWidget;
class QScrollArea;

namespace WRT {

    class WrtPage;
    class WrtPageManager;
    class GoAnywhereViewPrivate;
    class GoAnywhereWidget;

    class BWF_EXPORT GoAnywhereView : public ControllableViewQWidgetBase
    {
    friend class GoAnywhereWidget;
        Q_OBJECT
    public:
        GoAnywhereView(WrtPageManager* mgr, QWidget* parent,const QString& aType);
        ~GoAnywhereView(); 

        QAction* getActionBack();
        void setPageInfo(const QUrl& linkUrl, const QUrl& imageUrl);
        void showAnimation(bool show);
        void setAnimateDirection(const QString& direction);
        void setAnimateDuration(int millisecond);
        void addView(ControllableView* view);
        QRect centralRect() const;
        QImage currentSlide();
        static ControllableView *createNew(QWidget *parent);
        static const QString Type() { return "goAnywhereView"; }
        bool eventFilter(QObject* receiver, QEvent* event);
        
    signals:
        void ok(int ret);
        void goAnywhereIsReady();
        void goAnywhereViewChanged(QString viewId);

        void saveBookmarkBtnClicked();
        void viewImageBtnClicked();
        void sendPageBtnClicked();
        void pageSettingBtnClicked();

    private slots: 
        void back();
        void tabChanged(int index);
   
    private:
        void init();

    public: // Pure Virtual from ControllableView
        QString title();
        bool isActive();
        QList<QAction*> getContext();
        WrtPageManager* wrtPageManager();
        QString type() const{ return Type(); }

    public slots: // Pure Virtual from ControllableView
        void activate();
        void deactivate();

    protected:
        QWidget* qWidget() const;
        GoAnywhereViewPrivate * const d;
    };

    class GoAnywhereWidget: public QTabWidget
    {
        Q_OBJECT

    public:
        GoAnywhereWidget(QWidget *parent, GoAnywhereView *view);
        
    protected:
     //   void paintEvent(QPaintEvent* event);
     //   void resizeEvent(QResizeEvent* event);

    private:
        GoAnywhereView *m_view;
        QScrollArea* m_scrollArea;
        QWidget* m_inWidgt;
        QLabel* m_pageTileLabel;
        QLabel* m_pageUrlLabel;
    };
}
#endif // __GOANYWHEREVIEW_H__
