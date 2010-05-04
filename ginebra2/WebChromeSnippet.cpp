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
      return new PopupWebChromeItem(ownerArea, chrome, element);
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
