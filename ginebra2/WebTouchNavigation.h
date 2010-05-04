/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __WEBTOUCHNAVIGATION_H__
#define __WEBTOUCHNAVIGATION_H__

#include "qstmgestureevent.h"
#include "KineticHelper.h"
#include <QLineEdit>

class QGraphicsWebView;
class QWebPage;
class QWebFrame;

namespace GVA {

class WebTouchNavigation;

class DecelEdit : public QLineEdit
{
    Q_OBJECT
public:
    DecelEdit(WebTouchNavigation* nav);
    ~DecelEdit() {};
public slots:
    void setDecel();
private:    
    WebTouchNavigation* m_nav;
    
};



class WebTouchNavigation : public QObject, 
                           public KineticScrollable
{
    Q_OBJECT
public:
    WebTouchNavigation(QGraphicsWebView* view);
    virtual ~WebTouchNavigation();
    void handleQStmGesture(QStm_Gesture* gesture);
    
    //from KineticScrollable
    void scrollTo(QPoint& pos);    
    QPoint getScrollPosition();
    QPoint getInitialPosition();
    QPointF getInitialSpeed();
    
private:
    void doTap(QStm_Gesture* gesture);
    void doPan(QStm_Gesture* gesture);
    void doFlick(QStm_Gesture* gesture);
    void doTouch(QStm_Gesture* gesture);
    void stopScrolling();
    
    QPointF mapFromGlobal(const QPointF& gpos);
    
public slots:
    void pan();    

private:    
    QTimer* m_scrollTimer;
	QPoint  m_scrollDelta;
	bool    m_scrolling;
    QPointF m_kineticSpeed;
    QGraphicsWebView* m_view;
    QWebPage*         m_webPage;
    QWebFrame* m_frame;
    KineticHelper* m_kinetic;
    DecelEdit*   m_decelEdit;
    
    
    friend class DecelEdit;
};

}


#endif //__WEBTOUCHNAVIGATION_H__

