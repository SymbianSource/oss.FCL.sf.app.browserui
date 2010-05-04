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


#include "ActionButtonSnippet.h"
#include "controllableviewimpl.h"

namespace GVA {

  ActionButtonSnippet::ActionButtonSnippet( const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element ) 
    : ChromeSnippet( elementId, chrome, widget, element )
  {
    connect(static_cast<ActionButton*>(m_widget), SIGNAL(activated()), this, SIGNAL(activated()));
    connect(static_cast<ActionButton*>(m_widget), SIGNAL(contextMenuEvent()), this, SIGNAL(contextMenuEvent()));
  }
  
  void ActionButtonSnippet::setIcon( const QString & icon )
  {
    static_cast<ActionButton*>(m_widget)->addIcon(icon);
  }
 
  void ActionButtonSnippet::setDisabledIcon( const QString & icon )
  {
    static_cast<ActionButton*>(m_widget)->addIcon(icon, QIcon::Disabled);

  }

  void ActionButtonSnippet::setSelectedIcon( const QString & icon )
  {
    static_cast<ActionButton*>(m_widget)->addIcon(icon, QIcon::Selected);
  }

  void ActionButtonSnippet::setActiveIcon( const QString & icon )
  {
    static_cast<ActionButton*>(m_widget)->addIcon(icon, QIcon::Active);
  }

  // Scriptable method to directly connect an action button to a view action

  void ActionButtonSnippet::connectAction( const QString & action, const QString & view, const QString & inputEvent )
  {
    ControllableViewBase *viewBase = m_chrome->getView( view );
    if(viewBase){
      QAction * viewAction = viewBase->getAction(action);
      if(viewAction)
	static_cast<ActionButton*>(m_widget)->setAction(viewAction, 
						      (inputEvent == "Down") ? QEvent::GraphicsSceneMousePress : QEvent::GraphicsSceneMouseRelease);
      return;
    }
  }  

  //NB: setEnabled and setLatched only affect button behavior when no action
  //is currently set. These methods are intended to be used when the button
  //is controlled by javascript. When an action has been set, button behavior and
  //rendering is instead controlled by the action.

  void ActionButtonSnippet::setEnabled( bool enabled )
  {
    static_cast<ActionButton*>(m_widget)->setEnabled(enabled);
  }

  void ActionButtonSnippet::setLatched( bool latched )
  {
    static_cast<ActionButton*>(m_widget)->setChecked(latched);
  }

  void ActionButtonSnippet::setInputEvent( const QString & inputEvent )
  {
    static_cast<ActionButton*>(m_widget)->setInputEvent((inputEvent=="Down") ? QEvent::GraphicsSceneMousePress : QEvent::GraphicsSceneMouseRelease);
  }


}
