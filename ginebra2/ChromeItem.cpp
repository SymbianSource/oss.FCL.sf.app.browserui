
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
* This is the base class for all chrome items and extends QGraphicsWidget.
*
*/
#include "ChromeItem.h"
#include "ChromeEffect.h"
#include <QtGui>
#include "qstmgestureevent.h"


namespace GVA {
using namespace qstmGesture;

  ChromeItem::ChromeItem(ChromeSnippet * snippet, QGraphicsItem* parent)
    : QGraphicsWidget(parent),
  m_snippet(snippet)
  {
    grabGesture(QStm_Gesture::assignedType());
    setObjectName("ChromeItem");
    installEventFilter(this);
  }

ChromeItem::~ChromeItem() {

}

void ChromeItem::setSnippet(ChromeSnippet * s) {
    m_snippet = s;
}

void ChromeItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    QGraphicsWidget::paint(painter, option, widget);
    if(m_snippet && !m_snippet->enabled()) {
        painter->save();
        painter->setOpacity(ChromeEffect::disabledOpacity);
        painter->fillRect(option->exposedRect, ChromeEffect::disabledColor);
        painter->restore();
    }
}
  
  
bool ChromeItem::handleGesture(QEvent* event)
{
    //return QStm_GestureEventFilter::instance()->event(event);
	return false;
}

bool ChromeItem::eventFilter(QObject* receiver, QEvent* event)
{
    //if (receiver == this) {
    //    return QStm_GestureEventFilter::instance()->eventFilter(receiver, event);
    //}
    return false;
}

  bool ChromeItem::event(QEvent* event)
  {
      bool ret = false;
      if (event->type() == QEvent::Gesture) {
          ret = handleGesture(event);
      }
      if (!ret) {
          ret = QGraphicsWidget::event(event);
      }
      return ret;
  }
  
} // end of namespace GVA
