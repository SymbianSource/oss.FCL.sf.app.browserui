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


#ifndef __HISTORYVIEW_H__
#define __HISTORYVIEW_H__

#include <QWidget>
#include <QGraphicsWidget>
#include <QAction>
#include <QString>
#include "qwebframe.h"
#include "controllableviewimpl.h"

class QWebHistory;

namespace WRT {

    class WrtBrowserContainer;
    class WrtPageManager;
    class FlowInterface;
    class HistoryViewPrivate;

    class BWF_EXPORT HistoryView : public ControllableViewBase
    {
        Q_OBJECT
    public:
        HistoryView(WrtPageManager * mgr, QWidget* parent);
        HistoryView(WrtPageManager * mgr, QGraphicsWidget* parent);
        ~HistoryView();
        WrtPageManager* wrtPageManager();

    public: // public actions available for this view
        QAction * getActionForward();
        QAction * getActionBack();
        QAction * getActionOK();
        QAction * getActionCancel();
        int currentIndex();

    public: // Pure Virtual from ControllableView
        QString title() const;
        bool isActive();
        QList<QAction*> getContext();
        QRect centralRect();
        QGraphicsWidget* widget() const;

    signals:
        void centerIndexChanged(int index);
        void ok(int item);
        void cancel();
        void contextEvent(QObject *context);
        void activated();
        void deactivated();

    public slots:
        void activate();
        void deactivate();
        void setCenterIndex(int i);
        void forward();
        void back();
    
    private slots:
        void updateActions(int centerIndex);
        void updateHistory(bool status=true);

    private:
        bool eventFilter(QObject *obj, QEvent *event);
        bool handleWidgetContextMenuEvent(class QContextMenuEvent *event);
        QString itemTitle(int index) const;

    protected:
        HistoryViewPrivate * const d;
    };

    class BWF_EXPORT HistoryViewJSObject : public ControllableViewJSObject {
        Q_OBJECT
      public:
        HistoryViewJSObject(HistoryView* view, QWebFrame* webFrame, const QString& objectName);
        ~HistoryViewJSObject();

      int getCurrentHistoryIndex() const { return historyViewConst()->currentIndex(); }
      Q_PROPERTY(int currentHistIndex READ getCurrentHistoryIndex)

    public slots:
        void ok(int item);
        void cancel();
        void onContextEvent(QObject *context);
    
    signals:
       void done(int);
       void contextEvent(QObject *context);
       void activated();
       void deactivated();

    protected:
        HistoryView * historyView() { return static_cast<HistoryView *>(m_contentView); }
        HistoryView * historyViewConst() const { return static_cast<HistoryView *>(m_contentView); }
    };
}

#endif // __HISTORYVIEW_H__
