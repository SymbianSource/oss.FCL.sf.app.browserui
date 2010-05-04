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


#include <QTimer>
#include <QGraphicsWebView>
#include <QWebPage>
#include <QWebFrame>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "WebTouchNavigation.h"
#include "KineticHelper.h"

#define SCROLL_TIMEOUT   40

namespace GVA {
using namespace qstmGesture;




DecelEdit::DecelEdit(WebTouchNavigation* nav) : QLineEdit(), m_nav(nav)
{
    setMaxLength(6);
    connect(this, SIGNAL(editingFinished()), this, SLOT(setDecel()));
//    connect(this, SIGNAL(returnPressed ()), this, SLOT(setDecel()));
}

void DecelEdit::setDecel()
{
    QString txt = text();
    bool ok = false;
    int decel = txt.toInt(&ok);
    m_nav->m_kinetic->setDeceleration((qreal)decel);
    hide();
}


WebTouchNavigation::WebTouchNavigation(QGraphicsWebView* view) :
		 m_view(view),
		 m_scrollTimer(0)

{
	m_webPage = m_view->page();
	m_kinetic = new KineticHelper(this);
	m_decelEdit = new DecelEdit(this);
}

WebTouchNavigation::~WebTouchNavigation()
{
	disconnect(m_scrollTimer, SIGNAL(timeout()),this,SLOT(pan()));
	delete m_scrollTimer;
	    
	delete m_kinetic;
}


void WebTouchNavigation::handleQStmGesture(QStm_Gesture* gesture)
{
	QStm_GestureType type = gesture->getGestureStmType();
	
	switch (type) {
	    case QStmTapGestureType:
	    {
	    	doTap(gesture);
	    	break;
	    }
	    case QStmPanGestureType:
	    {
	    	doPan(gesture);
	    	break;
	    }
	    case QStmFlickGestureType:
	    {
	    	doFlick(gesture);
	    	break;
	    }
	    case QStmDoubleTapGestureType:
	    {
	        m_decelEdit->show();
	    	break;
	    }
	    case QStmTouchGestureType:
	    {
	    	doTouch(gesture);
	    	break;
	    }
	}
}

void WebTouchNavigation::doTouch(QStm_Gesture* gesture)
{
	stopScrolling();
	if (m_kinetic->isScrolling()) {
	    m_kinetic->stopScrolling();
	}
}

void WebTouchNavigation::stopScrolling()
{
	if (m_scrollTimer && m_scrollTimer->isActive()) {
	    m_scrollTimer->stop();
	    m_scrollDelta = QPoint(0,0);
	}
}


void WebTouchNavigation::doFlick(QStm_Gesture* gesture)
{
	
	QPointF pos = mapFromGlobal(gesture->position());
	m_kineticSpeed = gesture->getSpeedVec();
	QStm_GestureDirection direction = static_cast<QStm_GestureDirection>(gesture->getDirection()); 
	
	if (direction == ENorth || direction == ESouth) {
		m_kineticSpeed.setY(0.0);
	}
	else if (direction == EEast || direction == EWest) {
		m_kineticSpeed.setX(0.0);
	}
	
	m_kineticSpeed *= -1.0;
	m_frame = m_webPage->frameAt(pos.toPoint());
	m_kinetic->startScrolling();
}

void WebTouchNavigation::doPan(QStm_Gesture* gesture)
{
	if (gesture->gestureState() == Qt::GestureFinished) {
		stopScrolling();
	}
	else {
	  m_scrollDelta += gesture->getLengthAndDirection();
	  Q_ASSERT(m_scrollDelta.x() != 0 || m_scrollDelta.y() != 0);
	  QPointF pos = mapFromGlobal(gesture->position());
	  m_frame = m_webPage->frameAt(pos.toPoint());
      if(!m_scrollTimer) {
          m_scrollTimer = new QTimer(this);
          m_scrollTimer->setSingleShot(false);
          QObject::connect(m_scrollTimer, SIGNAL(timeout()), this, SLOT(pan()));
      }
      
      if (!m_scrollTimer->isActive()) {
          m_scrollTimer->stop();
          m_scrollTimer->start(SCROLL_TIMEOUT);
      }
	}
}

void WebTouchNavigation::pan()
{
	if (m_scrollDelta.x() != 0 || m_scrollDelta.y() != 0) {
	    m_frame->scroll(-m_scrollDelta.x(), m_scrollDelta.y());
	    m_scrollDelta = QPoint(0,0);
	}
}

void WebTouchNavigation::doTap(QStm_Gesture* gesture)
{
	QPoint gpos = gesture->position(); 
	QPoint pos = mapFromGlobal(gpos).toPoint();
	Qt::MouseButtons buttons = Qt::LeftButton;        
	buttons &= ~Qt::RightButton;
	buttons &= ~Qt::MidButton; 
	buttons &= Qt::MouseButtonMask;
	
	QMouseEvent mdown (QEvent::MouseButtonPress, pos, gpos, Qt::LeftButton, buttons, Qt::NoModifier);
	QMouseEvent mup (QEvent::MouseButtonRelease, pos, gpos, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
	
	m_webPage->event(&mdown);
	m_webPage->event(&mup);
}




QPointF WebTouchNavigation::mapFromGlobal(const QPointF& gpos)
{
	QGraphicsScene* scene = m_view->scene();
	QList<QGraphicsView*> gvList = scene->views();
	QList<QGraphicsView*>::iterator it;
	
	for(it = gvList.begin(); it != gvList.end(); it++) {
	    if (static_cast<QGraphicsView*>(*it)->hasFocus()) {
	        QWidget* viewport = static_cast<QGraphicsView*>(*it)->viewport();
	        return m_view->mapFromScene(viewport->mapFromGlobal(gpos.toPoint()));
        }
	}

	return QPointF(0.0, 0.0);	
}


void WebTouchNavigation::scrollTo(QPoint& pos)
{
    m_frame->setScrollPosition(pos);
}


QPoint WebTouchNavigation::getScrollPosition()
{
    return m_frame->scrollPosition();
}

QPoint WebTouchNavigation::getInitialPosition()
{
    return m_frame->scrollPosition();    
}

QPointF WebTouchNavigation::getInitialSpeed()
{
    return m_kineticSpeed;    
}



}

