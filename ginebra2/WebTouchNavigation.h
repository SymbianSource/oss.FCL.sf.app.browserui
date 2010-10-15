/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not,
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#ifndef __WEBTOUCHNAVIGATION_H__
#define __WEBTOUCHNAVIGATION_H__


#define square(x) (x)*(x)

#include "ScrollHelper.h"

class QGraphicsWebView;
class QWebPage;
class QWebFrame;
class QStm_Gesture;

namespace GVA {



class WebTouchNavigation : public QObject
{
    Q_OBJECT
public:
    WebTouchNavigation(QGraphicsWebView* view);
    virtual ~WebTouchNavigation();
    void handleQStmGesture(QStm_Gesture* gesture);
    virtual bool eventFilter(QObject* obj, QEvent* event);
    void setScrollHelper(ScrollHelper* helper) { m_scrollHelper = helper; }
    ScrollHelper*  scrollHelper() { return m_scrollHelper; }
    QPoint scrollPosition();
    void setScrollPosition(QPoint pos);
private:
    void doTap(QStm_Gesture* gesture);
    void doPan(QStm_Gesture* gesture);
    void doFlick(QStm_Gesture* gesture);
    void doTouch(QStm_Gesture* gesture);
    void stopScrolling();

    QPointF mapFromGlobal(const QPointF& gpos);

signals:
    void scroll(QPoint& pos);
    
public slots:
    void pan();

private:
    QTimer* m_scrollTimer;
    QPointF  m_scrollDelta;
    bool    m_scrolling;
    QPointF m_kineticSpeed;
    QGraphicsWebView* m_view;
    QWebPage*         m_webPage;
    QWebFrame* m_frame;
    ScrollHelper* m_scrollHelper;
};

}


#endif //__WEBTOUCHNAVIGATION_H__

