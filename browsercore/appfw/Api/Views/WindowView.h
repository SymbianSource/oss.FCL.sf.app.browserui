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


#ifndef __WINDOWVIEW_H__
#define __WINDOWVIEW_H__

#include <QWidget>
#include <QGraphicsWidget>
#include <QAction>
#include "controllableviewimpl.h"

class QWebHistory;

namespace WRT {

    class WrtBrowserContainer;
    class WrtPageManager;
    class WindowViewPrivate;

    class BWF_EXPORT WindowView : public ControllableViewBase
    {
        Q_OBJECT
    public:
    	 enum State {
            WindowViewNotActive,
            WindowViewActive,
            WindowViewAddPage,
            WindowViewDeletePage,
            WindowViewInvalid
        };
        WindowView(WrtPageManager * mgr, QWidget* parent);
        WindowView(WrtPageManager * mgr, QGraphicsWidget* parent);
        ~WindowView();

        virtual void show() {
            widget()->show();
        }

        virtual void hide() {
            widget()->hide();
        }

        WrtPageManager* wrtPageManager();

    public: // public actions available for this view
        QAction * getActionForward();
        QAction * getActionBack();
        QAction * getActionOK();
        QAction * getActionCancel();
        QAction * getActionAddWindow();
        QAction * getActionDelWindow();

        QRect centralRect();
        QImage currentSlide();
        void setBlankWindowImg(QImage * img);
        void hideWidget();
        void showWidget();
        void setSize(QSize& size);

    public: // Pure Virtual from ControllableView
        QString title() const;
        bool isActive();
        QList<QAction*> getContext();
        void displayModeChanged(QString& newMode);
        QGraphicsWidget* widget() const;

    signals:
        void centerIndexChanged(int index);
        void ok(WrtBrowserContainer * newPage);
        void cancel();
        void animateComplete();
        void addPageComplete();

    public slots:
        void activate();
        void deactivate();
        void forward();
        void back();
        void updateWindows();

        void animate(WrtBrowserContainer *, WrtBrowserContainer*);

    private slots:
        void changedCenterIndex(int);
        void updateActions();    
        void okTriggered(int index);
        void okTriggeredCplt();
        void pageLoadCplt(bool ok);
        void addPage();
        void addPageCplt(int);
        void addPageCplt();
        void delPage();
        void delPage(int);
        void delPageCplt(int);

    private:
        void init();

        void setCenterIndex();
        void setCenterIndex(int i);
        void setCenterIndex(WrtBrowserContainer *);
        void indexChangeInActiveState(int);
        void updateImages();

    protected:
        WindowViewPrivate * const d;
    };

    class BWF_EXPORT WindowViewJSObject : public ControllableViewJSObject {
        Q_OBJECT
      public:
        WindowViewJSObject(WindowView* view, QWebFrame* webFrame, const QString& objectName);
        ~WindowViewJSObject();
    

    public slots:
        void ok(WrtBrowserContainer * newPage);
        void addPageComplete();
        void changedCenterIndex(int);
    
    signals:
       void done(WrtBrowserContainer * newPage);
       void pageAdded();
       void centerIndexChanged(int index);

    protected:
        WindowView * windowView() { return static_cast<WindowView *>(m_contentView); }
        WindowView * windowViewConst() const { return static_cast<WindowView *>(m_contentView); }
    };

}
#endif // __WINDOWVIEW_H__
