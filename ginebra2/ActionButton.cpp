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


#include "ActionButton.h"
#include <QDebug>

namespace GVA {

  ActionButton::ActionButton(ChromeSnippet * snippet, QGraphicsItem* parent)
    : NativeChromeItem(snippet, parent),
      m_internalAction(new QAction(this)),
      m_triggerOn(QEvent::GraphicsSceneMousePress),
      m_active(false)
  {
    m_internalAction->setCheckable(true);
    m_internalAction->setEnabled(true);
    setAction(m_internalAction);

    //Just testing . . .
    //addIcon(":/chrome/demochrome/HandButton.png");
    //addIcon(":/chrome/demochrome/Stop.png", QIcon::Disabled);
    connect(m_internalAction, SIGNAL(triggered(bool)), this, SLOT(onTriggered(bool)));
  }

  void ActionButton::paint( QPainter * painter, const QStyleOptionGraphicsItem * opt, QWidget * widget )
  {
    Q_UNUSED(opt)
    Q_UNUSED(widget)

    painter->save();
    QAction * action = defaultAction();
    QIcon::Mode mode = QIcon::Normal;
    if(m_active)
      mode = QIcon::Active;
    else if(action){
      if(action->isChecked())
	mode = QIcon::Selected;
      else if(!action->isEnabled())
	mode = QIcon::Disabled;
    }
    m_icon.paint(painter, boundingRect().toRect(), Qt::AlignCenter, mode, QIcon::On);
    painter->restore();
  }

  void ActionButton::mousePressEvent( QGraphicsSceneMouseEvent * ev )
  {
    if(m_triggerOn == ev->type()){
      if (ev->button() == Qt::LeftButton) {
	QAction * action = defaultAction();
	if(action && action->isEnabled()){
	  action->trigger();
	  emit activated();
	}
      }
      m_active = true;
    }
  }

  void ActionButton::mouseReleaseEvent( QGraphicsSceneMouseEvent * ev )
  {
    if(m_triggerOn == ev->type()){
      if (ev->button() == Qt::LeftButton) {
	QAction * action = defaultAction();
	if(action && action->isEnabled()){
	  action->trigger();
	  emit activated();
	}
      }
      m_active = false;
    }
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
    if(currentAction == action)
      return;
    if(currentAction){
      disconnect(currentAction, SIGNAL(changed()), this, SLOT(onActionChanged()));
      removeAction(currentAction);
    }
    addAction(action);
    connect(action, SIGNAL(changed()),this, SLOT(onActionChanged()));
    m_triggerOn = triggerOn;
    update();
  }

  void ActionButton::disconnectAction () {
    setAction(m_internalAction);
  }

  void ActionButton::setEnabled(bool enabled)
  {
    m_internalAction->setEnabled(enabled);
  }

  void ActionButton::setChecked(bool checked)
  {
    m_internalAction->setChecked(checked);
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
 
  // For testing only . . .
  void ActionButton::onTriggered(bool checked){
    Q_UNUSED(checked)
    qDebug() << "ActionButton::triggered";
  }

}//end of name space
