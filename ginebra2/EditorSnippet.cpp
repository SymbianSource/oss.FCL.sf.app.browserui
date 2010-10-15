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

#include "EditorSnippet.h"

namespace GVA {

  EditorSnippet::EditorSnippet( const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element )
    : ChromeSnippet( elementId, chrome, widget, element )
  {
    connectAll();
  }

  void EditorSnippet::setChromeWidget(QGraphicsWidget * widget){
    ChromeSnippet::setChromeWidget(widget);
    connectAll();
  }

  EditorSnippet * EditorSnippet::instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element)
  {
      EditorSnippet* that = new EditorSnippet(elementId, chrome, 0, element);
      TextEditItem * textEditItem = new TextEditItem(that, chrome);
     that->setChromeWidget(textEditItem);
     connect(textEditItem, SIGNAL(contextEvent(bool)), that, SLOT(sendContextMenuEvent(bool)));
	  
#ifdef BROWSER_LAYOUT_TENONE
      QFont textFont;
      //textFont = QFont(QApplication::font());
      textFont = QFont("Series 60 Sans");
      //textFont.setPointSize(7);
      that->setTextFont(textFont);
#endif
	  
      return that;
  }
  
  void EditorSnippet::connectAll(){
    if(m_widget){
      GTextEditor * editor = static_cast<TextEditItem*>(m_widget)->editor();
      connect(editor, SIGNAL(textMayChanged()), this, SIGNAL(textChanged()));
      connect(editor, SIGNAL(activated()), this, SIGNAL(activated()));
      connect(editor, SIGNAL(focusChanged(bool)), this, SLOT(onFocusChanged(bool)));
    }
  }

  TextEditItem * EditorSnippet::textEditItem()
  {
    return qobject_cast<TextEditItem*>(m_widget);
  }

  void EditorSnippet::sendContextMenuEvent(bool isContentSelected)
  {
      emit contextEvent(isContentSelected, elementId());
  }

  void EditorSnippet::onFocusChanged(bool in)
  {
    if(in)
      emit gainedFocus();
    else
      emit lostFocus();
  }

#ifdef BROWSER_LAYOUT_TENONE
  void EditorSnippet::setTextFont( QFont & font )
  {
    textEditItem()->setTextFont(font);
  }
#endif

  void EditorSnippet::setText( const QString & text )
  {
    textEditItem()->setText(text);
  }

  QString EditorSnippet::text()
  {
    return textEditItem()->text();
  }

  void EditorSnippet::setCursorPosition(int pos)
  {
    textEditItem()->setCursorPosition(pos);
  }

  int EditorSnippet::charCount()
  {
    return textEditItem()->characterCount();
  }

  void EditorSnippet::selectAll()
  {
    return textEditItem()->selectAll();
  }

  void EditorSnippet::unselect()
  {
    return textEditItem()->unselect();
  }
  
  int EditorSnippet::getTextOptions()
  {
    return (int) textEditItem()->getTextOptions(); 
  }
  
  void EditorSnippet::setTextOptions(int flag)
  {
    return textEditItem()->setTextOptions(flag);
  }

  void EditorSnippet::setMaxTextLength(int length)
  {
    return textEditItem()->setMaxTextLength(length);
  }

  void EditorSnippet::cut()
  {
    textEditItem()->cut();
  }

  void EditorSnippet::copy()
  {
    textEditItem()->copy();
  }
 
  void EditorSnippet::paste()
  {
    textEditItem()->paste();
  }

  void EditorSnippet::setContextMenuStatus(bool on)
  { 
    textEditItem()->setContextMenuStatus(on);
  }
}
