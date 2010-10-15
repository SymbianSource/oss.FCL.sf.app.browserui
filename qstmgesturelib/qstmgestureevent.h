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
#ifndef QSTMGESTUREEVENT_H_
#define QSTMGESTUREEVENT_H_

#include <QEvent>
#include <QPoint>
#include <QString>
#include <QVarLengthArray>
#include <QMouseEvent>
#include <QWidget>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGesture>
#include <QString>
#include <QGraphicsWidget>

#include "qstmgesturedefs.h"
#include "qstmgesture_if.h"

#define QStmGestureEventType QEvent::User + 1 
#define QStmGestureType Qt::CustomGesture

enum QStm_GestureType
	{
	    QStmUknownGestureType = 0,
	    QStmTouchGestureType,
	    QStmTapGestureType,
	    QStmDoubleTapGestureType,
	    QStmLongPressGestureType,
	    QStmHoverGestureType,
	    QStmPanGestureType,
	    QStmReleaseGestureType,
	    
	    QStmLeftRightGestureType,
	    QStmUpDownGestureType,
	    QStmFlickGestureType,     
	    QStmEdgeScrollGestureType,
	    
	    QStmPinchGestureType, 
	    QStmCornerZoomGestureType,
	    QStmMaybeTapGestureType
	};



class QSTMGESTURELIB_EXPORT QStm_Gesture : public QGesture
{
	Q_OBJECT
	
public:	

	QStm_Gesture(QObject* parent = 0);
    
    virtual ~QStm_Gesture() {}
    
    QStm_Gesture& operator=(QStm_Gesture& other);
    QPoint  position() const { return m_pos; }
    QPointF scenePosition(QGraphicsItem* i);
    void setPosition(const QPoint &pos) { m_pos = pos; /*setHotSpot(QPointF(pos)); */}
    QPoint  position2() const { return m_pos2; }
    QPointF scenePosition2(QGraphicsItem* i);
    void setPosition2(const QPoint &pos) { m_pos2 = pos; /*setHotSpot(QPointF(pos)); */}
    
    void setGestureStmType(QStm_GestureType type) { m_gstType = type; }    
    QStm_GestureType  getGestureStmType() const { return m_gstType; }
    QStm_GestureType  gestureUidToStmType(qstmGesture::QStm_GestureUid uid, int stmGestSubType);
    
    void setGestureState(Qt::GestureState state) { m_state = state; }
    Qt::GestureState gestureState() { return m_state; }
    
    void setGestureSubType(int type) { m_gstSubType = type; }
    int  getGestureSubType() { return m_gstSubType; }    
    
    qreal getSpeed() { return m_speed; }
    void setSpeed(qreal speed) { m_speed = speed; }
    
    QPointF getSpeedVec() { return m_speedVec; }
    QPointF sceneSpeedVec(QGraphicsItem* i);
    void setSpeedVec(QPointF speedVec) { m_speedVec = speedVec; }
    
    QPoint getLengthAndDirection() { return m_vector; }
    QPointF sceneLengthAndDirection(QGraphicsItem* i);

    void setLengthAndDirection(QPoint vec) { m_vector = vec; }
    
    int getDirection() { return m_direction; }
    int sceneDirection(QGraphicsItem* i);
    void setDirection(int dir) { m_direction = dir; }
    QString  gestureName();
    
    void* getDetails() { return m_details; }
    void  setDetails(void* details ) { m_details = details; }
    QTime timestamp() { return m_timestamp; }
    void  setTimestamp(QTime ts) { m_timestamp.setHMS(ts.hour(),
                                   ts.minute(), ts.second(), ts.msec()); }
    
    bool sendEvent(QEvent* event);
    QList<QMouseEvent*>   getMouseEvents();
    bool sendMouseEvents(QObject* receiver, Qt::KeyboardModifier modifier = Qt::NoModifier);
    bool sendMouseEvents(Qt::KeyboardModifier modifier = Qt::NoModifier);
    bool postMouseEvents(QObject* receiver, Qt::KeyboardModifier modifier = Qt::NoModifier);
    bool sendGraphicsSceneMouseEvent(QGraphicsObject* go);
    void setTarget(QWidget* target) { m_target = target; }
    QWidget* target() { return m_target; }
    
    static void setAssignedGestureType(Qt::GestureType type);
    static Qt::GestureType assignedType(); 
    static QGraphicsItem* gestureGrabberItem();
    static void setGestureGrabberItem(QGraphicsItem* item);
    static QGraphicsItem* gestureFocusedItem();
    static void setGestureFocusedItem(QGraphicsItem* item);
    bool clearGestureFocusedItemIfNeeded();
    bool isGestureEnded();
    void updateGestureFocusedItemIfNeeded(QGraphicsItem* gi);
    QEvent::Type gestureType2GraphicsSceneMouseType();

private:
    bool sendOrPostMouseEvents(QObject* receiver, Qt::KeyboardModifier modifier, bool send);
    void gestureTypeToMouseTypes(QVarLengthArray<int, 4>& types);     
    
    
    qreal             m_speed;    
    int               m_direction;
    QPoint            m_vector;
    QStm_GestureType  m_gstType;
    Qt::GestureState  m_state;
    int               m_gstSubType;
    QPoint            m_pos;
    QPoint            m_pos2;
    void*             m_details;
    QPointF           m_speedVec;
    QWidget*          m_target;
    QTime             m_timestamp;
};

QSTMGESTURELIB_EXPORT QStm_Gesture* getQStmGesture(QEvent* event);
QSTMGESTURELIB_EXPORT QPointF qstmMapFromGlobal(const QPointF& gpos, QGraphicsObject* graphicsObj);
QSTMGESTURELIB_EXPORT QPoint qstmMapFromScene(const QPointF& gpos, QGraphicsObject* graphicsObj);
QSTMGESTURELIB_EXPORT QPointF qstmMapToScene(const QPointF& gpos, QGraphicsObject* graphicsObj);
QSTMGESTURELIB_EXPORT QGraphicsView* qstmGetGraphicsView(QGraphicsObject* graphicsObj);
QSTMGESTURELIB_EXPORT bool qstmDeliverGestureEventToGraphicsItem(QGraphicsView* gv, QEvent* event);
QSTMGESTURELIB_EXPORT void qstmSetGraphicsSceneMouseEvent(const QPointF& scenePos, QGraphicsObject* graphicsObj,
                                                          QGraphicsSceneMouseEvent& event, bool select = false);

QSTMGESTURELIB_EXPORT bool qstmIsGraphicsSceneMouseEvent(QEvent* event);
QSTMGESTURELIB_EXPORT bool qstmIsMouseEvent(QEvent* event);
QSTMGESTURELIB_EXPORT bool qstmIsTouchEvent(QEvent* event);
QSTMGESTURELIB_EXPORT bool qstmIsContextMenuEvent(QEvent* event);

class QSTMGESTURELIB_EXPORT QStm_GestureEvent : public QEvent
{
public:	
	QStm_GestureEvent();
	virtual ~QStm_GestureEvent();
	static QEvent::Type stmGestureEventType();
};


class QSTMGESTURELIB_EXPORT QStm_GestureEventFilter : public QObject
{
public:
    QStm_GestureEventFilter() {}
    virtual ~QStm_GestureEventFilter() {}
    static  QStm_GestureEventFilter* instance();
    bool eventFilter(QObject* receiver, QEvent* event);
    bool event(QEvent* event);
    bool sendGraphicsSceneMouseEvent(QEvent* event, QGraphicsObject* go);


private:
    static QStm_GestureEventFilter* m_instance;
};



#endif /* QSTMGESTUREEVENT_H_ */
