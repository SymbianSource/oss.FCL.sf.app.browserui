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

#include "ActionButton.h"
#include "qstmgestureevent.h"
#include <QDebug>

namespace GVA {

  ActionButton::ActionButton(ChromeSnippet * snippet, const QString &objectName, QGraphicsItem* parent)
    : NativeChromeItem(snippet, parent),
      m_internalAction(NULL),
      m_triggerOnUp(true),
      m_triggerOnDown(false),
      m_active(false),
      m_activeOnPress(true)
  {
	setObjectName(objectName);
  }

  void ActionButton::paint( QPainter * painter, const QStyleOptionGraphicsItem * opt, QWidget * widget )
  {
    Q_UNUSED(opt)
    Q_UNUSED(widget)

    painter->save();
    QAction * action = defaultAction();
    QIcon::Mode mode = QIcon::Normal;

    if (action) {
        if (m_active) {
            mode = QIcon::Active;
        }
        else if (!action->isEnabled()) {
            mode = QIcon::Disabled;
        }
    }
    
    #ifndef  Q_WS_MAEMO_5
    m_icon.paint(painter, boundingRect().toRect(), Qt::AlignCenter, mode, QIcon::On);
    #else
    m_icon.paint(painter, boundingRect().toRect(), Qt::AlignLeft, mode, QIcon::On);
    #endif
    
    painter->restore();
#ifndef BROWSER_LAYOUT_TENONE
    NativeChromeItem::paint(painter, opt, widget);
#endif
  }

  void ActionButton::mousePressEvent( QGraphicsSceneMouseEvent * ev )
  {

    QAction * action = defaultAction();
    if (action && (action->isEnabled()) ) {
      //qDebug() << "mousePressEvent" << m_triggerOnDown <<  ev->type();
      // If m_activeOnPress is true, set active flag to set icon state to Selected
      if (m_activeOnPress )
        setActive(true);
      if (m_triggerOnDown == true) {
        if (ev->button() == Qt::LeftButton) {

              action->trigger();
              emit activated();
        }
      }

    }
    emit mouseEvent(ev->type() );
  }

  void ActionButton::mouseReleaseEvent( QGraphicsSceneMouseEvent * ev )
  {

    bool trigger = sceneBoundingRect().contains(ev->scenePos());

    QAction * action = defaultAction();
    //qDebug() << "ActionButton::mouseReleaseEvent " << m_snippet->elementId();

    if (   trigger && m_triggerOnUp == true) {
      if (ev->button() == Qt::LeftButton) {
        if (action && action->isEnabled()){
          action->trigger();
          emit activated();

        }
      }
    }
    // If m_activeOnPress is true, reset active flag to set icon state to Normal
    if (m_activeOnPress || !trigger)
      setActive(false);
    emit mouseEvent(ev->type() );
  }

  //Action buttons only have one action at a time, so whenever we add an action, we remove any previously set action
  //NB: The action is typically one of the available actions on a view (via ControllableView.getContext()).
  //ActionButtonSnippet provides the scriptable method connectAction() to create native connections to view actions

  void ActionButton::setAction ( QAction * action, bool triggerOnDown,  bool triggerOnUp )
  {
    QAction * currentAction = defaultAction();
    if (currentAction == action)
      return;
    if (currentAction){
      disconnect(currentAction, SIGNAL(changed()), this, SLOT(onActionChanged()));
      removeAction(currentAction);
    }
    addAction(action);
    connect(action, SIGNAL(changed()),this, SLOT(onActionChanged()));
    m_triggerOnUp = triggerOnUp;
    m_triggerOnDown = triggerOnDown;


    // Save the action as the internal action and set its properties
    m_internalAction = action;
    m_internalAction->setCheckable(false);

    update();
  }

  void ActionButton::setEnabled(bool enabled)
  {
    m_internalAction->setEnabled(enabled);
  }

  void ActionButton::setTriggerOnUp(bool triggeronup)
  {
     m_triggerOnUp = triggeronup;
  }
  
  void ActionButton::setActiveOnPress(bool active)
  {
    m_activeOnPress = active;
  }
  
  void ActionButton::setActive(bool active)
  {
    if (m_active != active ) {
        m_active = active;
        update();
    }
  }

  //NB: handle icon on/off states too?

  void ActionButton::addIcon( const QString & resource, QIcon::Mode mode )
  {
    m_icon.addPixmap( QPixmap(resource), mode, QIcon::On );
  }

  QAction * ActionButton::defaultAction()
  {
    if (actions().isEmpty())
      return 0;
    return actions()[0];
  }

  void ActionButton::onActionChanged(){
    //Repaint when the action changes state
    update();
  }

  void ActionButton::onHidden(){

    QAction * action = defaultAction();
    if (action && action->isChecked() ){
      action->setChecked(false);
    }
    setActive(false);

  }

  void ActionButton::onShown(){

    QAction * action = defaultAction();
    if (action && action->isEnabled() && !action->isChecked()){
      action->setChecked(true);
      setActive(true);
    }
  }

  
  bool ActionButton::handleGesture(QEvent* event)
    {
      /*
        if (event->type() == QEvent::Gesture) {
            QStm_Gesture* gesture = getQStmGesture(event);
            if (gesture) {
                QStm_GestureType type = gesture->getGestureStmType();
                QEvent::Type mouseEventType = gesture->gestureType2GraphicsSceneMouseType();
                
                if (mouseEventType == QEvent::GraphicsSceneMousePress || 
                    mouseEventType == QEvent::GraphicsSceneMouseRelease) {
                    QGraphicsSceneMouseEvent gsme(mouseEventType);
                    QPoint gpos = gesture->position();
                    qstmSetGraphicsSceneMouseEvent(gpos, this, gsme);
                    if (mouseEventType == QEvent::GraphicsSceneMousePress) {
                        mousePressEvent(&gsme);    
                    }
                    else if (mouseEventType == QEvent::GraphicsSceneMouseRelease) {
                        mouseReleaseEvent(&gsme);
                    }
                }
            }
        }
      */  
      return false;
    }

  
  bool ActionButton::eventFilter(QObject* receiver, QEvent* event)
  {
      /*
      if (receiver == this) {
          return QStm_GestureEventFilter::instance()->eventFilter(receiver, event);
      }
      */
      return false;
  }
  
}//end of name space
