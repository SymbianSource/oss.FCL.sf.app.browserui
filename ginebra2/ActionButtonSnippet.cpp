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

#include "ActionButtonSnippet.h"
#include "controllableviewimpl.h"

namespace GVA {

  ActionButtonSnippet::ActionButtonSnippet( const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element )
    : ChromeSnippet( elementId, chrome, widget, element )
  {

  }

  QAction * ActionButtonSnippet::getDefaultAction()
  {
    return (static_cast<ActionButton*>(m_widget)->defaultAction());
  }

  void ActionButtonSnippet::setDefaultAction( QAction * action, QEvent::Type triggerOn )
  {
    static_cast<ActionButton*>(m_widget)->setAction(action,triggerOn);
  }

  QIcon  ActionButtonSnippet::icon( )
  {
    return static_cast<ActionButton*>(m_widget)->icon();
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

    if (viewBase){
      QAction * viewAction = viewBase->getAction(action);
      if (viewAction)
        static_cast<ActionButton*>(m_widget)->setAction(viewAction,
          (inputEvent == "Down") ? QEvent::GraphicsSceneMousePress : QEvent::GraphicsSceneMouseRelease);
      return;
    }
  }

  bool ActionButtonSnippet::isChecked( )
  {
    return (static_cast<ActionButton*>(m_widget)->isChecked());
  }

  void ActionButtonSnippet::setEnabled( bool enabled )
  {
    static_cast<ActionButton*>(m_widget)->setEnabled(enabled);
  }

  void ActionButtonSnippet::setLatched( bool latched )
  {
    static_cast<ActionButton*>(m_widget)->setChecked(latched);
  }

  void ActionButtonSnippet::setActive( bool enabled )
  {
    static_cast<ActionButton*>(m_widget)->setActive(enabled);
  }

  void ActionButtonSnippet::setInputEvent( const QString & inputEvent )
  {
    static_cast<ActionButton*>(m_widget)->setInputEvent((inputEvent=="Down") ? QEvent::GraphicsSceneMousePress : QEvent::GraphicsSceneMouseRelease);
  }

  void ActionButtonSnippet::updateButtonState(bool state) {
      if (state ) {
          static_cast<ActionButton*>(m_widget)->onShown();
      }
      else {
          static_cast<ActionButton*>(m_widget)->onHidden();
      }
  }

}
