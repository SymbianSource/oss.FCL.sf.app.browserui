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

#include <QTimer>
#include <QGraphicsWebView>
#include <QWebPage>
#include <QWebFrame>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneContextMenuEvent>
#include <QApplication>

#include "WebTouchNavigation.h"
#include "GWebContentViewWidget.h"
#include "GWebContentView.h"
#include "wrtperftracer.h"

#ifndef NO_QSTM_GESTURE
#include "qstmutils.h"
#include "qstmgestureevent.h"
#include "qstmfilelogger.h"
#endif
#define SCROLL_TIMEOUT   40

namespace GVA {
#ifndef NO_QSTM_GESTURE
using namespace qstmGesture;
#endif


WebTouchNavigation::WebTouchNavigation(QGraphicsWebView* view) :
         m_view(view),
         m_scrollHelper(0),
         m_frame(0)

{
    m_webPage = m_view->page();
}

WebTouchNavigation::~WebTouchNavigation()
{
}


bool WebTouchNavigation::eventFilter(QObject* obj, QEvent* event)
{
    if (obj != m_view) return false;
#ifndef NO_QSTM_GESTURE    
    if (event->type() == QEvent::Gesture) {
        QStm_Gesture* gesture = getQStmGesture(event);
        if (gesture) {
            handleQStmGesture(gesture);
            return true;
        }
    }
#endif
    return false;
}

#ifndef NO_QSTM_GESTURE
void WebTouchNavigation::handleQStmGesture(QStm_Gesture* gesture)
{
    QStm_GestureType type = gesture->getGestureStmType();
    qstmDebug() << " WTN::handleQStmGesture - type=" << type << "\n";
    
    switch (type) {
        case QStmTapGestureType:
        {
            doTap(gesture);
            //gesture->sendMouseEvents();
            break;
        }
        case QStmLeftRightGestureType:
        {
            qstmDebug() << " handleQStmGesture LEFTRIGHT_begin: pos: " << gesture->position() << "\n";
            //gesture->sendMouseEvents();
            doPan(gesture);
            qstmDebug() << " handleQStmGesture LEFTRIGHT_end" << "\n";
            break;    
        }
        case QStmUpDownGestureType:
        {
            qstmDebug() << " handleQStmGesture UPDOWN_begin pos: " << gesture->position() << "\n";
            //gesture->sendMouseEvents();
            doPan(gesture);
            qstmDebug() << " handleQStmGesture UPDOWN_end"<< "\n";
            break;
        }
        case QStmPanGestureType:
        {
            //
            qstmDebug() << " handleQStmGesture PAN_begin pos: " << gesture->position() << "\n";
            //gesture->sendMouseEvents();
            doPan(gesture);
            qstmDebug() << " handleQStmGesture PAN_end" << "\n";
            break;
        }
        case QStmFlickGestureType:
        {
            qstmDebug() << " handleQStmGesture FLICK_begin pos: " << gesture->position() << ", speed: " << gesture->getSpeedVec() << "\n";
            doFlick(gesture);
            //gesture->sendMouseEvents();
            qstmDebug() << " handleQStmGesture FLICK_end" << "\n";
            break;
        }
        case QStmDoubleTapGestureType:
        {
            //qstmDebug() << "WTN::handleQStmGesture doubletap";
            //m_decelEdit->show();
            //gesture->sendMouseEvents();
            break;
        }
        case QStmTouchGestureType:
        {
            qstmDebug() << " handleQStmGesture TOUCH_begin pos: " << gesture->position() << "\n";
            //doTouch(gesture);
            //gesture->sendMouseEvents(m_view);
            qstmDebug() << " handleQStmGesture TOUCH_end" << "\n";
            break;
        }
        case QStmReleaseGestureType:
        {
            qstmDebug() << " handleQStmGesture RELEASE_begin pos: " << gesture->position() << "\n";
            //gesture->sendMouseEvents(m_view);
            m_scrollHelper->stopScroll();
            qstmDebug() << " handleQStmGesture RELEASE_end" << "\n" << "\n";
            break;
    }
        case QStmPinchGestureType:
        {
            qstmDebug() << " handleQStmGesture PINCH_begin" << "\n";
            qstmDebug() << " handleQStmGesture PINCH_end" << "\n";    
            break;
        }
        case QStmLongPressGestureType:
        {
            qstmDebug() << " handleQStmGesture LONGPRESS_begin pos: " << gesture->position() << "\n";
            QPoint gpos = gesture->position();
            QPoint pos = mapFromGlobal(gpos).toPoint();
            //QContextMenuEvent cmEvent(QContextMenuEvent::Mouse, pos, gpos);

            QGraphicsSceneContextMenuEvent cmEvent(QEvent::GraphicsSceneContextMenu);
            cmEvent.setPos(pos);
            cmEvent.setScenePos(gpos);
            cmEvent.setReason(QGraphicsSceneContextMenuEvent::Mouse);
            
            QApplication::sendEvent(m_view, &cmEvent);
            qstmDebug() << " handleQStmGesture LONGPRESS_end" << "\n";
        }
        
        default: 
        {
//            gesture->sendMouseEvents();
        }
    }
}

void WebTouchNavigation::doTouch(QStm_Gesture* gesture)
{
    qstmDebug() << "WTN::doTouch IN";
    stopScrolling();
    }

void WebTouchNavigation::stopScrolling()
{
}


void WebTouchNavigation::doFlick(QStm_Gesture* gesture)
{
    m_kineticSpeed = gesture->getSpeedVec();
    m_scrollHelper->kineticScroll(m_kineticSpeed);
}

void WebTouchNavigation::doPan(QStm_Gesture* gesture)
{   
    m_scrollDelta = gesture->getLengthAndDirection();
#ifdef BEDROCK_TILED_BACKING_STORE
    m_scrollDelta.setY(-1 * m_scrollDelta.y());
#endif
    QPointF p = QPointF(gesture->position());
//    if (m_scrollDelta.x() != 0 || m_scrollDelta.y() != 0) {
        m_scrollHelper->scroll(m_scrollDelta, p);
//    }
}


void WebTouchNavigation::doTap(QStm_Gesture* gesture)
{
    qstmDebug() << "WTN::doTap IN";
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


#endif

QPoint WebTouchNavigation::scrollPosition()
{
    if (!m_frame) {
        m_frame = m_webPage->mainFrame();
    }
    return m_frame->scrollPosition();
}


void WebTouchNavigation::setScrollPosition(QPoint pos)
{
    if (!m_frame) {
        m_frame = m_webPage->mainFrame();
    }
    m_frame->setScrollPosition(pos);
}

void WebTouchNavigation::pan()
{
    if (m_scrollDelta.x() != 0 || m_scrollDelta.y() != 0) {
        m_frame->scroll(-m_scrollDelta.x(), m_scrollDelta.y());
        m_scrollDelta = QPoint(0,0);
    }
}


QPointF WebTouchNavigation::mapFromGlobal(const QPointF& gpos)
{
    QGraphicsScene* scene = m_view->scene();
    QList<QGraphicsView*> gvList = scene->views();
    QList<QGraphicsView*>::iterator it;

    for (it = gvList.begin(); it != gvList.end(); it++) {
        if (static_cast<QGraphicsView*>(*it)->hasFocus()) {
            QWidget* viewport = static_cast<QGraphicsView*>(*it)->viewport();
            return m_view->mapFromScene(viewport->mapFromGlobal(gpos.toPoint()));
        }
    }

    return QPointF(0.0, 0.0);
}

}

