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
*/

#include "ExternalEventCharm.h"
#include "qstmgestureevent.h"

namespace GVA {

const QString ExternalEventCharm::s_mouseClick = "MouseClick";
static const int KLongPressThreshold = 30;

inline QGraphicsScene *ExternalEventCharm::scene() {
    QGraphicsObject *item = static_cast<QGraphicsObject*>(m_object);
    return item->scene();
}

ExternalEventCharm::ExternalEventCharm(QGraphicsObject *object)
  : ObjectCharm(object),
    m_pressed(false)
{
}

bool ExternalEventCharm::eventFilter(QObject *object, QEvent *event) {
    checkForExternalEvent(this, event);
    checkForExternalGesture(event);

    switch (event->type()) {
      case QEvent::Show:
      {
          if(scene())
          	scene()->installEventFilter(this);
          break;
      }
      case QEvent::Hide:
      {
          if(scene())
          	scene()->removeEventFilter(this);
          break;
      }
      default: break;
    }

    // standard event processing
    return object->eventFilter(object, event);
}

void ExternalEventCharm::checkForExternalEvent(QObject * o, QEvent * e)
{
    Q_UNUSED(o);
    bool emitClick = false;

    switch (e->type()) {
        case QEvent::GraphicsSceneMousePress: {
            QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent*>(e);
            m_pressPos = me->scenePos();
            m_pressed = true;
            break;
        }
        case QEvent::GraphicsSceneMouseRelease: {
            // See if we need to send a mouse-click signal.
            // TODO: When gesture handling is implemented this should be replaced.
            if(m_pressed) {
                QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent*>(e);
                QPointF diff = m_pressPos - me->scenePos();
                if(diff.manhattanLength() < KLongPressThreshold) {
                    emitClick = true;
                }
                m_pressed = false;
            }
            break;
        }
        case QEvent::GraphicsSceneMouseDoubleClick:
            m_pressed = false;
            break;
        default:
            // Ignore other events.
            return;
    }

    QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent*>(e);
    QGraphicsObject *item = static_cast<QGraphicsObject*>(m_object);
    QRectF itemGeometry = item->sceneBoundingRect();

    if (!itemGeometry.contains(me->scenePos())) {
        if(emitClick)
            emit externalMouseEvent(me, s_mouseClick, "");
        else
            emitExternalEvent(e);
    }
}


bool ExternalEventCharm::checkForExternalGesture(QEvent* event)
{
    if (event->type() != QEvent::Gesture) return false;
    bool emitClick = false;
    bool isGestureStarted = false;
    bool isGestureEnded = false;
    QStm_Gesture* gesture = getQStmGesture(event);
    if (gesture) {
        QStm_GestureType gtype = gesture->getGestureStmType();
        QPoint gpos = gesture->position();
        isGestureStarted = (gtype == QStmTouchGestureType);
        isGestureEnded = (gtype == QStmMaybeTapGestureType ||
                               gesture->isGestureEnded());

        if (isGestureStarted || isGestureEnded) {
            QGraphicsObject *item = static_cast<QGraphicsObject*>(m_object);
            QPointF pos = qstmMapToScene(gpos, item);
            if (isGestureStarted) {
                m_pressed = true;
            }
            else if (isGestureEnded) {
                if (m_pressed) {
                    emitClick = true;
                    m_pressed = false;
                }
            }

            if (!item->sceneBoundingRect().contains(pos)) {

                QGraphicsSceneMouseEvent me(emitClick ? QEvent::GraphicsSceneMouseRelease :
                                                        QEvent::GraphicsSceneMousePress);

                qstmSetGraphicsSceneMouseEvent(pos, item, me);
                if(emitClick) {
                    emit externalMouseEvent(&me, s_mouseClick, "");
                }
                else {
                    emitExternalEvent(&me);
                }
            }
        }
    }
    return isGestureStarted || isGestureEnded;
}

void ExternalEventCharm::emitExternalEvent(QEvent * e)
{
    QString description;

    QDebug stream(&description);
    stream << e;

    QString name = description;
    name.truncate(name.indexOf('('));

    emit externalMouseEvent(e, name, description.trimmed());
}


}   // end GVA namepace
