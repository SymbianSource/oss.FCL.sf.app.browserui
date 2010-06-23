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

#include "WebChromeSnippet.h"
#include "WebChromeItem.h"
#include "PopupWebChromeItem.h"
#include "ChromeRenderer.h"
#include "ChromeWidget.h"
#include "ChromeDOM.h"
#include <QWebElement>
#include <QDebug>

namespace GVA {

  static WebChromeItem * newChromeItem(
      ChromeWidget * chrome,
      const QRectF& ownerArea,
      const QWebElement & element)
  {
    QString type = element.attribute("data-GinebraItemType", "normal");

    if (type == "popup") {
      QString modal = element.attribute("data-GinebraPopupModal", "true");
      return new PopupWebChromeItem(ownerArea, chrome, element, 0, modal == "true");
    }

    return new WebChromeItem(ownerArea, chrome, element);
  }

  WebChromeSnippet::WebChromeSnippet(
          const QString & elementId,
          ChromeWidget * chrome,
          const QRectF& ownerArea,
          const QWebElement & element)
    : ChromeSnippet(elementId, chrome, newChromeItem(chrome, ownerArea, element), element)

  {
    WebChromeItem * item = static_cast<WebChromeItem*> (widget());
    item->init(this);
  }

  WebChromeSnippet::~WebChromeSnippet()
  {
  }

  WebChromeItem * WebChromeSnippet::item()
  {
    return static_cast<WebChromeItem*> (widget());
  }

  void WebChromeSnippet:: grabFocus()
  {
    //qDebug() << "WebChromeSnippet::grabFocus";
    WebChromeItem * item = static_cast<WebChromeItem*> (widget());
    item->grabFocus();
  }

  void WebChromeSnippet:: updateOwnerArea()
  {

    WebChromeItem * item = static_cast<WebChromeItem*> (widget());
    //Setting owner area also resets the item's size and preferred size
    item->setOwnerArea(m_chrome->getSnippetRect(m_elementId));
    //qDebug() << WebChromeSnippet::updateOwnerArea: id: " << m_elementId << " element rect: " << item->ownerArea();
    //NB: Should move this to WebChromeItem::setOwnerArea()?
    item->setCachedHandlers(m_chrome->dom()->getCachedHandlers(m_elementId, item->ownerArea()));

  }

} // endof namespace GVA
