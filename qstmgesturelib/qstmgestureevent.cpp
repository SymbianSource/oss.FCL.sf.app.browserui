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

#include "qstmgestureevent.h"
#include "qstmuievent_if.h"

#include <QGraphicsSceneMouseEvent>

using namespace qstmUiEventEngine;
using namespace qstmGesture;

static Qt::GestureType s_assignedType = Qt::CustomGesture;
static int s_stmGestureEventType = QEvent::User;
static QGraphicsItem* s_gestureGrabberItem = NULL;
static QGraphicsItem* s_gestureFocusedItem = NULL;


QStm_GestureEventFilter* QStm_GestureEventFilter::m_instance = 0;

QStm_GestureEventFilter* QStm_GestureEventFilter::instance()
{
    if (!m_instance) {
        m_instance = new QStm_GestureEventFilter();
    }
    return m_instance;
}

bool QStm_GestureEventFilter::eventFilter(QObject* receiver, QEvent* evt)
{
    return qstmIsGraphicsSceneMouseEvent(evt) || qstmIsMouseEvent(evt) || 
           qstmIsTouchEvent(evt) || qstmIsContextMenuEvent(evt);
}




bool QStm_GestureEventFilter::sendGraphicsSceneMouseEvent(QEvent* event, QGraphicsObject* go)
{
    bool ret = false;
    if (event->type() == QEvent::Gesture) {
        QStm_Gesture* gesture = getQStmGesture(event);
        if (gesture) {
            ret = gesture->sendGraphicsSceneMouseEvent(go);
        }

    }
    return ret;
}


bool QStm_GestureEventFilter::event(QEvent* event)
{
    if (event->type() == QEvent::Gesture) {
        QStm_Gesture* gesture = getQStmGesture(event);
        if (gesture) {
            QStm_GestureType gtype = gesture->getGestureStmType();
            
            /*
             * Touch mapped to mouse press, Tap mapped to mouse release
             * UpDown, LeftRight and Pan are mapped to mouse move.
             */
            if (gtype == QStmMaybeTapGestureType) {// in case of tap send mouse release
                gesture->setGestureStmType(QStmReleaseGestureType);
            }
            
            bool ret = (gtype == QStmTouchGestureType ||
                        gtype == QStmMaybeTapGestureType ||
                        gtype == QStmLeftRightGestureType || 
                        gtype == QStmUpDownGestureType ||
                        gtype == QStmPanGestureType ||
                        gtype == QStmReleaseGestureType ||
                        gtype == QStmFlickGestureType ||
                        gtype == QStmUknownGestureType) ;
                         
            if (ret) {
                gesture->sendMouseEvents();
                (static_cast<QGestureEvent*>(event))->accept(QStm_Gesture::assignedType());
                (static_cast<QGestureEvent*>(event))->accept();
            }
            
            if (gtype == QStmTapGestureType) {// restore gesture type. 
                gesture->setGestureStmType(QStmTapGestureType);
            }
            
            return ret;
        }
    }
    return false;
}






QStm_GestureEvent::QStm_GestureEvent():
                   QEvent(QStm_GestureEvent::stmGestureEventType())

{
}

QStm_GestureEvent::~QStm_GestureEvent()
{
}


QEvent::Type QStm_GestureEvent::stmGestureEventType() 
{ 
    if (s_stmGestureEventType == QEvent::User) {
        s_stmGestureEventType = QEvent::registerEventType(QStmGestureEventType);    
    }
    return static_cast<QEvent::Type>(QStmGestureEventType); 
}



void QStm_Gesture::setAssignedGestureType(Qt::GestureType type) 
{ 
    s_assignedType = type; 
}

Qt::GestureType QStm_Gesture::assignedType() 
{ 
    return s_assignedType; 
} 


QGraphicsItem* QStm_Gesture::gestureGrabberItem()
{ 
    return s_gestureGrabberItem; 
}

void QStm_Gesture::setGestureGrabberItem(QGraphicsItem* item)
{ 
    s_gestureGrabberItem = item; 
}


QGraphicsItem* QStm_Gesture::gestureFocusedItem()
{
    return s_gestureFocusedItem;
}

void QStm_Gesture::setGestureFocusedItem(QGraphicsItem* item)
{
    s_gestureFocusedItem = item;
}

QStm_Gesture::QStm_Gesture(QObject* parent) : QGesture(parent)
{
	    m_speed = 0.0;    
	    m_direction = -1;
	    m_vector = QPoint(0,0);
	    m_gstType = QStmUknownGestureType; 
	    m_state = Qt::NoGesture;
	    m_gstSubType = 0;
	    m_pos = QPoint(INT_MIN,INT_MIN);
        m_pos2 = QPoint(INT_MIN,INT_MIN);
	    m_details = NULL;
	    m_speedVec = QPointF(0.0, 0.0);
	    m_target = NULL;
        m_timestamp = QTime::currentTime();
};


QStm_Gesture& QStm_Gesture::operator=(QStm_Gesture& other)
{
    m_speed = other.getSpeed();    
    m_direction = other.getDirection();
    m_vector = other.getLengthAndDirection();
    m_gstType = other.getGestureStmType(); 
    m_state = other.gestureState();
    m_gstSubType = other.getGestureSubType();
    m_pos = other.position();
    m_pos2 = other.position2();
    m_details = other.getDetails();
    m_speedVec = other.getSpeedVec();
    m_target = other.target();
    m_timestamp = other.timestamp();
    setHotSpot(other.hotSpot());
	return *this;
}


QString  QStm_Gesture::gestureName()
{
    switch (m_gstType) {
        case QStmUknownGestureType:
            return QString("QStmUknownGestureType");    
        case QStmTouchGestureType:
            return QString("QStmTouchGestureType");
        case QStmTapGestureType:
            return QString("QStmTapGestureType");
        case QStmDoubleTapGestureType:
            return QString("QStmDoubleTapGestureType");
        case QStmLongPressGestureType:
            return QString("QStmLongPressGestureType");
        case QStmHoverGestureType:
            return QString("QStmHoverGestureType");
        case QStmPanGestureType:
            return QString("QStmPanGestureType");
        case QStmReleaseGestureType:
            return QString("QStmReleaseGestureType");
        case QStmLeftRightGestureType:
            return QString("QStmLeftRightGestureType");
        case QStmUpDownGestureType:
            return QString("QString");
        case QStmFlickGestureType:
            return QString("QStmFlickGestureType");
        case QStmEdgeScrollGestureType:
            return QString("QStmEdgeScrollGestureType");
        case QStmPinchGestureType:
            return QString("QStmPinchGestureType");
        case QStmCornerZoomGestureType:
            return QString("QStmCornerZoomGestureType");
        default:
            return QString("XZ Gesture");
    }	
}



QStm_GestureType  QStm_Gesture::gestureUidToStmType(QStm_GestureUid uid, int stmGestSubType)
{
    QStm_GestureType  gtype = QStmUknownGestureType;
    
    switch (uid) {
        case EGestureUidTap:
        {
            QStm_TapType type = qstmGesture::QStm_TapType(stmGestSubType);
            if (type == qstmGesture::ETapTypeDouble) {
                gtype = QStmDoubleTapGestureType;
            }
            else  {
                gtype = QStmTapGestureType;
            }
            break;
        }

        case EGestureUidTouch:
        {
            gtype = QStmTouchGestureType;
            break;
        }

        case EGestureUidRelease:
        {
            gtype = QStmReleaseGestureType;
            break;
        }

        case EGestureUidFlick:
        {
            gtype = QStmFlickGestureType;
            break;
        }

        case EGestureUidLeftRight:
        {
            gtype = QStmLeftRightGestureType;
            break;
        }

        case EGestureUidUpDown:
        {
            gtype = QStmUpDownGestureType;
            break;
        }

        case EGestureUidPan:
        {
            gtype = QStmPanGestureType;
            break;
        }

        case EGestureUidHover:
        {
            gtype = QStmHoverGestureType;
            break;
        }

        case EGestureUidLongPress:
        {
            gtype = QStmLongPressGestureType;
            break;
        }

        case EGestureUidEdgeScroll:
        {
            gtype = QStmEdgeScrollGestureType;
            break;
        }

        case EGestureUidCornerZoom:
        {
            gtype = QStmCornerZoomGestureType;
            break;
        }

        case EGestureUidPinch:
        {
            gtype = QStmPinchGestureType;
            break;
        }
        case EGestureUidMaybeTap:
        {
            gtype = QStmMaybeTapGestureType;
            break;
        }
        default:
        {
            gtype = QStmUknownGestureType;
            break;
        }
    }    
    return gtype;
}

void QStm_Gesture::gestureTypeToMouseTypes(QVarLengthArray<int, 4>& types)
{
	switch (m_gstType) {
		case QStmTouchGestureType:
		{
			types.append(QEvent::MouseButtonPress);
			break;
		}
		case QStmDoubleTapGestureType:
		{
			types.append(QEvent::MouseButtonDblClick);
			break;
		}

		case QStmHoverGestureType:
		case QStmPanGestureType:
		case QStmLeftRightGestureType:
		case QStmUpDownGestureType:
		case QStmCornerZoomGestureType:
		{
			types.append(QEvent::MouseMove);
			break;
		}
		case QStmReleaseGestureType:
		case QStmFlickGestureType:
		{
			types.append(QEvent::MouseButtonRelease);
			break;
		}
		case QStmTapGestureType:
        //case QStmMaybeTapGestureType:
		{
			types.append(QEvent::MouseButtonPress);
			types.append(QEvent::MouseButtonRelease); 
			break;
		}
		case QStmUknownGestureType:
		{
		    QStm_UiEventIf* uiEvent = static_cast<QStm_UiEventIf*>(m_details);
		    if (uiEvent) {
		        QEvent::Type evType = uiEvent->mapToMouseEventType();
		        types.append(evType);
		    }
		    break;
		}
		case QStmEdgeScrollGestureType:
		case QStmPinchGestureType:
		case QStmLongPressGestureType:
		default:
			break;
	}
	return;
}




bool QStm_Gesture::sendOrPostMouseEvents(QObject* receiver, Qt::KeyboardModifier modifier, bool send)
{
    bool ret = false;
    QPoint pos = position();
    QPoint gpos = pos;
    QWidget* w = NULL;
    Qt::MouseButtons buttons = Qt::LeftButton;
    
    
    buttons &= ~Qt::RightButton;
    buttons &= ~Qt::MidButton; 
    buttons &= Qt::MouseButtonMask;
    
    if (receiver->isWidgetType()) {
        w = static_cast<QWidget*>(receiver);
    	pos = w->mapFromGlobal(gpos);
    }    
    

    QVarLengthArray<int, 4> mouseTypes;
    gestureTypeToMouseTypes(mouseTypes);   
    
    for (int i = 0; i < mouseTypes.size(); i++) {
    	QEvent::Type mtype = static_cast<QEvent::Type>(mouseTypes[i]);

        if (mtype == QEvent::None) {
            continue;
        }
        else if (mtype != QEvent::MouseButtonRelease && w) {
            QPoint wtl = w->mapToGlobal(w->pos());
            QRect wgeom = QRect(wtl, w->size());
            if (!wgeom.contains(m_pos)) {
                continue;
            }
        }

    	if (mtype == QEvent::MouseButtonRelease) {
    		buttons = 0;
    	}
    	
    	if (send) {
    	    QMouseEvent evt(mtype, pos, gpos, Qt::LeftButton, buttons, modifier);
            //if (w) w->grabMouse();
    	    QApplication::sendEvent(receiver, &evt);
            //if (w) w->releaseMouse();
    	}
    	else {
    		QMouseEvent* evt = new QMouseEvent(mtype, pos, gpos, Qt::LeftButton, buttons, modifier);
    		QApplication::postEvent(receiver, evt);
    	}
    }
    
    return ret;
}


bool QStm_Gesture::sendEvent(QEvent* event)
{
    QWidget* target = static_cast<QWidget*>(m_target);
    QWidget* w = NULL;
    QWidget* modal = QApplication::activeModalWidget();

    
    
    if (modal) {
        target = modal;
    }
    
    if (target) {
        QPoint pos = target->mapFromGlobal(m_pos);
        w = target->childAt(pos);
        if (!w) {
            w = target;
        }
    }
    
    if (w) {
        if (modal) { //we send mouse events to modal dialogs.
            return(sendMouseEvents(w));
        }
        /*
        else if (event->type() == QStm_GestureEvent::stmGestureEventType() && 
            m_gstType == QStmUknownGestureType) {
            QStm_UiEventIf* uiEvent = static_cast<QStm_UiEventIf*>(m_details);
            
            if (uiEvent && m_target == modal) {
            //re-generate mouse events
                return(sendMouseEvents(w));
            }

        else {
                return(QApplication::sendEvent(w, event));
        }
    }
		*/
        else {
            if (!target->hasFocus()) {
                target->setFocus(Qt::MouseFocusReason);
            }
            return(QApplication::sendEvent(w, event));
        }
    }
    return false;
}


bool QStm_Gesture::sendGraphicsSceneMouseEvent(QGraphicsObject* go)
{
    QEvent::Type eventType = gestureType2GraphicsSceneMouseType();
    bool ret = false;

    if (eventType != QEvent::None) {
        QGraphicsSceneMouseEvent gsme(eventType);
        QPointF scenePos = qstmMapToScene(m_pos, go);
        qstmSetGraphicsSceneMouseEvent(scenePos, go, gsme);
        ret = go->event(&gsme);
    }
    return ret;
}


QEvent::Type QStm_Gesture::gestureType2GraphicsSceneMouseType()
{
    QEvent::Type type = QEvent::None;
    switch(m_gstType)
    {
        case QStmTouchGestureType:
            type = QEvent::GraphicsSceneMousePress;
            break;
        case QStmMaybeTapGestureType:
        case QStmReleaseGestureType:
        case QStmFlickGestureType:
            type = QEvent::GraphicsSceneMouseRelease;
            break;
        case QStmLeftRightGestureType:
        case QStmUpDownGestureType:
        case QStmPanGestureType:
            type = QEvent::GraphicsSceneMouseMove;
            break;
        case  QStmUknownGestureType:
        {
            QVarLengthArray<int, 4> mouseTypes;
            gestureTypeToMouseTypes(mouseTypes);
            switch (mouseTypes[0])
            {
                case QEvent::MouseButtonPress:
                    type = QEvent::GraphicsSceneMousePress;
                    break;
                case QEvent::MouseButtonRelease:
                    type = QEvent::GraphicsSceneMouseRelease;
                    break;
                case QEvent::MouseMove:
                    type = QEvent::GraphicsSceneMouseMove;
                    break;
            }
        }
    }
    return type;
}

bool QStm_Gesture::sendMouseEvents(Qt::KeyboardModifier modifier)
{
    QWidget* target = static_cast<QWidget*>(m_target);

    if (!target) {
        target = QApplication::widgetAt(m_pos);
    }
    if (!target) return false;
    QPoint pos = target->mapFromGlobal(m_pos);
    QWidget* w = target->childAt(pos);
    if (!w) {
        w = target;
    }
    
    return sendMouseEvents(w);
}

bool QStm_Gesture::sendMouseEvents(QObject* receiver, Qt::KeyboardModifier modifier)
{
	return sendOrPostMouseEvents(receiver, modifier, true);
}

bool QStm_Gesture::postMouseEvents(QObject* receiver, Qt::KeyboardModifier modifier)
{
	return sendOrPostMouseEvents(receiver, modifier, false);
}

QList<QMouseEvent*>   QStm_Gesture::getMouseEvents()
{
	QList<QMouseEvent*> mouseEventsList;
    QPoint pos = position(); 
    QPoint gpos = pos;
    Qt::MouseButtons buttons = Qt::LeftButton;        
    buttons &= ~Qt::RightButton;
    buttons &= ~Qt::MidButton; 
    buttons &= Qt::MouseButtonMask;
        
    
    
    QVarLengthArray<int, 4> mouseTypes;
    gestureTypeToMouseTypes(mouseTypes);
    
    for (int i = 0; i < mouseTypes.size(); i++) {
        QMouseEvent* evt = new QMouseEvent(static_cast<QEvent::Type>(mouseTypes[i]), 
        		               pos, gpos, Qt::LeftButton, buttons, Qt::NoModifier);
        mouseEventsList.append(evt);
    }
	return mouseEventsList;
}


bool QStm_Gesture::isGestureEnded()
{
    bool gestureEnded = false;
    
    if (m_gstType == QStmUknownGestureType) {
        QStm_UiEventIf* uiEvent = static_cast<QStm_UiEventIf*>(m_details);
        if (uiEvent) {
            QEvent::Type evType = uiEvent->mapToMouseEventType();
            gestureEnded = (evType == ERelease);
        }
    }
    else if (m_gstType == QStmReleaseGestureType ||
             m_gstType == QStmFlickGestureType ||
             m_gstType == QStmTapGestureType ||
             m_gstType == QStmDoubleTapGestureType) {
        gestureEnded = true;
    }
    return gestureEnded;
}

bool QStm_Gesture::clearGestureFocusedItemIfNeeded()
{
    bool reset = isGestureEnded();
    if (reset) {
        QStm_Gesture::setGestureFocusedItem(NULL);
    }
    return reset;
}

void QStm_Gesture::updateGestureFocusedItemIfNeeded(QGraphicsItem* gi)
{
    if (isGestureEnded()) {
        QStm_Gesture::setGestureFocusedItem(NULL);        
    }
    else if (gestureState() != Qt::GestureFinished) { 
        QStm_Gesture::setGestureFocusedItem(gi);
    }    
}

QPointF QStm_Gesture::scenePosition(QGraphicsItem* i)
{
    return qstmMapToScene(m_pos, static_cast<QGraphicsObject*>(i));
}

QPointF QStm_Gesture::scenePosition2(QGraphicsItem* i)
{
    return qstmMapToScene(m_pos2, static_cast<QGraphicsObject*>(i));
}

QPointF QStm_Gesture::sceneSpeedVec(QGraphicsItem* i)
{
    QGraphicsObject* o = static_cast<QGraphicsObject*>(i);

    return qstmMapToScene(m_speedVec,o) - qstmMapToScene(QPointF(0, 0), o);
}

QPointF QStm_Gesture::sceneLengthAndDirection(QGraphicsItem* i)
{
    QGraphicsObject* o = static_cast<QGraphicsObject*>(i);

    return qstmMapToScene(QPointF(-m_vector.x(), m_vector.y()),o) - qstmMapToScene(QPointF(0, 0), o);
}

int QStm_Gesture::sceneDirection(QGraphicsItem *i)
{
    QStm_GestureDirection dir = ENorth ;
    QPointF v = sceneLengthAndDirection(i);

    qreal x = qAbs(v.x()) ;
    qreal y = qAbs(v.y()) ;

    if (y == 0 && x == 0) {
        dir = ENoDirection;
    }
    else if (y <= x/2) {
        if (v.x() < 0)
            dir = EWest ;
        else
            dir = EEast ;
    }
    else if (y > x/2 && y <= (x+x/2)) {
        if (v.x() < 0)  {
            if (v.y() < 0 )
                dir = ESouthWest ;
            else
                dir = ENorthWest ;
        }
        else {
            if (v.y() < 0 )
                dir = ESouthEast ;
            else
                dir = ENorthEast ;
        }
    }
    else if (y > x+x/2) {
        if (v.y() < 0)
            dir = ESouth ;
        else
            dir = ENorth ;
    }

    return dir ;
}

QSTMGESTURELIB_EXPORT QPoint qstmMapFromScene(const QPointF& gpos, QGraphicsObject* graphicsObj)
{
    QGraphicsView* grView = qstmGetGraphicsView(graphicsObj);
    QPointF pos = QPointF(0.0, 0.0);
    if (grView) {
        pos = grView->mapFromScene(gpos);
        return grView->mapToGlobal(pos.toPoint());
    }
    return QPoint(0, 0);
}


QSTMGESTURELIB_EXPORT QPointF qstmMapToScene(const QPointF& gpos, QGraphicsObject* graphicsObj)
{
    QGraphicsView* grView = qstmGetGraphicsView(graphicsObj);
    QPointF pos = QPointF(0.0, 0.0);
    if (grView) {
        pos = grView->mapFromGlobal(gpos.toPoint());
        pos = grView->mapToScene(pos.toPoint());
    }
    return pos;
}


QSTMGESTURELIB_EXPORT QPointF qstmMapFromGlobal(const QPointF& gpos, QGraphicsObject* graphicsObj)
{
    QGraphicsView* grView = qstmGetGraphicsView(graphicsObj);
    QPointF pos = QPointF(0.0, 0.0);
    if (grView) {
        pos = grView->mapFromGlobal(gpos.toPoint());
    }
    return pos;
}

QSTMGESTURELIB_EXPORT QGraphicsView* qstmGetGraphicsView(QGraphicsObject* graphicsObj)
{
    QGraphicsScene* scene = graphicsObj->scene();
    QList<QGraphicsView*> gvList = scene->views();
    QList<QGraphicsView*>::iterator it;
    
    /*
    for (it = gvList.begin(); it != gvList.end(); it++) {
        if (static_cast<QGraphicsView*>(*it)->hasFocus()) {
            return static_cast<QGraphicsView*>(*it);
        }
    }
    */
    return gvList.isEmpty() ? NULL : gvList[0];
}

QSTMGESTURELIB_EXPORT QStm_Gesture*  getQStmGesture(QEvent* event)
{	
	QStm_Gesture* gesture = NULL;
	if (event->type() == QEvent::Gesture) {
	    QGestureEvent* ge = static_cast<QGestureEvent*>(event);
	    QList<QGesture*> l = ge->gestures();
	    QGesture* g = !l.isEmpty() ? l.at(0) : NULL;
	    if (g && g->gestureType() == QStm_Gesture::assignedType()) {
	        gesture = static_cast<QStm_Gesture*>(g);
	    }
	}
	return gesture;
}

QSTMGESTURELIB_EXPORT bool qstmDeliverGestureEventToGraphicsItem(QGraphicsView* gv, QEvent* event)
{
    if (event->type() != QEvent::Gesture) return false;
    
    bool ret = false;
    QStm_Gesture* gesture = getQStmGesture(event);
    if (gesture) {

        QGraphicsScene* gs = gv->scene();
        QGraphicsItem* gestureGrabber = QStm_Gesture::gestureGrabberItem();
        QGraphicsItem* gi = QStm_Gesture::gestureFocusedItem();
        QGraphicsItem* mgItem = gs->mouseGrabberItem();
        
        if (gestureGrabber) {
            gs->sendEvent(gestureGrabber, event);
            ret = true; //no fallback to mouse events   
        }
        else {
            if (!gi) {
                gi = mgItem;
            }
            if (!gi) {
                QPoint pos = gv->mapFromGlobal(gesture->position());
                pos = gv->mapToScene(pos).toPoint();
                QList<QGraphicsItem *> itemsList = gs->items(pos, Qt::IntersectsItemShape,
                                                             Qt::DescendingOrder);
                for(int i = 0; i < itemsList.size(); i++) {

                    if (itemsList[i] && (itemsList[i])->opacity() < qreal(0.001)) {
                        continue;
                    }
                    gi = itemsList[i];
                    gs->setFocusItem(gi, Qt::MouseFocusReason);
                    break;
                }
            }
            if (gi) {
                //gs->setFocusItem(gi, Qt::MouseFocusReason);
                //ret = gs->sendEvent(gi, event);
                //gs->setFocusItem(0, Qt::MouseFocusReason);

                QGraphicsObject* go = gi->toGraphicsObject();
                if (go) {
                    ret = go->event(event);
                }

            }
        }
        
        if (!ret) { // fallback to mouse events
            QStm_GestureEventFilter::instance()->event(event);
            ret = true;
        }
        else if (!gestureGrabber) {
            gesture->updateGestureFocusedItemIfNeeded(gi);
        }
    }
    return ret;
}


QSTMGESTURELIB_EXPORT void qstmSetGraphicsSceneMouseEvent(const QPointF& scenePos, QGraphicsObject* graphicsObj,
                                                         QGraphicsSceneMouseEvent& event, bool select)
{
    QPointF pos = scenePos;
    QPoint gpos = qstmMapFromScene(pos, graphicsObj);

    event.setScenePos(pos);
    event.setScreenPos(gpos);
    event.setPos(graphicsObj->mapFromScene(pos));
    if (!select && event.type() != QEvent::GraphicsSceneMouseMove) {
        event.setButton(Qt::LeftButton);
    }
    else {
        event.setButton(Qt::NoButton);
    }
    if (!select) {
        event.setButtons(Qt::NoButton);
    }
    else {
        event.setButtons(Qt::LeftButton);
    }
    event.setModifiers(Qt::NoModifier);
}

QSTMGESTURELIB_EXPORT bool qstmIsGraphicsSceneMouseEvent(QEvent* event)
{
    QEvent::Type  type = event->type();
    return type == QEvent::GraphicsSceneMouseMove ||
           type == QEvent::GraphicsSceneMousePress ||
           type == QEvent::GraphicsSceneMouseRelease ||
           type == QEvent::GraphicsSceneMouseDoubleClick;
}

QSTMGESTURELIB_EXPORT bool qstmIsMouseEvent(QEvent* event)
{
    QEvent::Type  type = event->type();
    return type == QEvent::MouseButtonPress ||
           type == QEvent::MouseButtonRelease ||
           type == QEvent::MouseMove ||
           type == QEvent::MouseButtonDblClick;
}

QSTMGESTURELIB_EXPORT bool qstmIsTouchEvent(QEvent* event)
{
    QEvent::Type  type = event->type();
    return type == QEvent::TouchBegin ||
           type == QEvent::TouchEnd ||
           type == QEvent::TouchUpdate;
}

QSTMGESTURELIB_EXPORT bool qstmIsContextMenuEvent(QEvent* event)
{
    QEvent::Type  type = event->type();
    return type == QEvent::ContextMenu ||
           type == QEvent::GraphicsSceneContextMenu;
}
