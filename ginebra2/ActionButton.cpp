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
#include <QDebug>

namespace GVA {

  ActionButton::ActionButton(ChromeSnippet * snippet, QGraphicsItem* parent)
    : NativeChromeItem(snippet, parent),
      m_internalAction(NULL),
      m_triggerOn(QEvent::GraphicsSceneMouseRelease),
      m_active(false)
  {


  }

  void ActionButton::paint( QPainter * painter, const QStyleOptionGraphicsItem * opt, QWidget * widget )
  {
    Q_UNUSED(opt)
    Q_UNUSED(widget)

    painter->save();
    QAction * action = defaultAction();
    QIcon::Mode mode = QIcon::Normal;

    if (action) {
        if (m_active || action->isChecked() ) {
            mode = QIcon::Selected;
        }
        else if (!action->isEnabled()) {
            mode = QIcon::Disabled;
        }
        else {
            mode = QIcon::Active;
        }
    }
    m_icon.paint(painter, boundingRect().toRect(), Qt::AlignCenter, mode, QIcon::On);
    painter->restore();
    NativeChromeItem::paint(painter, opt, widget);
  }

  void ActionButton::mousePressEvent( QGraphicsSceneMouseEvent * ev )
  {
    QAction * action = defaultAction();
    if (action ) {
      if (m_triggerOn == ev->type()){
        if (ev->button() == Qt::LeftButton) {
            if (action->isEnabled()){
              action->trigger();
              emit activated();
            }
        }
      }

    }
    emit mouseEvent(ev->type() );
  }

  void ActionButton::mouseReleaseEvent( QGraphicsSceneMouseEvent * ev )
  {
    QAction * action = defaultAction();
    //qDebug() << "ActionButton::mouseReleaseEvent " << m_snippet->elementId();
    if (m_triggerOn == ev->type()){
      if (ev->button() == Qt::LeftButton) {
        if (action && action->isEnabled()){
          action->trigger();
          emit activated();
        }
      }
    }
    emit mouseEvent(ev->type() );
  }

  void ActionButton::contextMenuEvent( QGraphicsSceneContextMenuEvent * ev )
  {
    Q_UNUSED(ev)
    emit contextMenuEvent();
  }

  //Action buttons only have one action at a time, so whenever we add an action, we remove any previously set action
  //NB: The action is typically one of the available actions on a view (via ControllableView.getContext()).
  //ActionButtonSnippet provides the scriptable method connectAction() to create native connections to view actions

  void ActionButton::setAction ( QAction * action, QEvent::Type triggerOn )
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
    m_triggerOn = triggerOn;


    // Save the action as the internal action and set its properties
    m_internalAction = action;
    m_internalAction->setCheckable(true);

    update();
  }

  void ActionButton::disconnectAction () {
    setAction(NULL);
  }

  void ActionButton::setEnabled(bool enabled)
  {
    m_internalAction->setEnabled(enabled);
  }

  void ActionButton::setChecked(bool checked)
  {
    m_internalAction->setChecked(checked);
  }
  
  void ActionButton::setActive(bool active)
  {
    m_active = active;
    update();
  }

  void ActionButton::setInputEvent(QEvent::Type event)
  {
    m_triggerOn = event;
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
      update();
    }
  }


}//end of name space
