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


#include "WebChromeContainerSnippet.h"
#include "WebChromeItem.h"
#include "ChromeWidget.h"
#include "ChromeRenderer.h"
#include "ChromeSnippet.h"

#include <QDebug>

namespace GVA {

  WebChromeContainerSnippet::WebChromeContainerSnippet(const QString & elementId, ChromeWidget * chrome, const QRectF& ownerArea, const QWebElement & element, QGraphicsWidget* gwidget)
    : ChromeSnippet(elementId, chrome, gwidget, element),
      m_ownerArea(ownerArea),
      m_layoutHeight(0)
  {
    QGraphicsWidget * item = static_cast<QGraphicsWidget*> (widget());
    //NB: maybe size should be fixed only in one direction?
    item->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed)); 
    //NB: add a method for setting owner area
    //item->setPreferredSize(m_ownerArea.width(), m_ownerArea.height());
    //Also resize in case item is not part of anchor layout
    //item->resize(item->preferredSize());
 
   //NB: Linear layout efaults to horizontal: handle vertical layouts too.   
    m_layout = new QGraphicsLinearLayout();
    m_layout->setContentsMargins(0,0,0,0);
    m_layout->setSpacing(0);
    
    //Add a stretch element at the beginning.
    m_layout->addStretch();
    item->setLayout(m_layout);
    //When chrome is resized owner areas for snippets may change
    QObject::connect(m_chrome->renderer(), SIGNAL(chromeResized()), this, SLOT(updateOwnerArea()));
  }

  WebChromeContainerSnippet::~WebChromeContainerSnippet()
  {
    // delete m_layout;
  }

  void WebChromeContainerSnippet::positionChildren()
  {
    ; //Do nothing since the layout positions children automatically.
  }

  void WebChromeContainerSnippet::addChild(ChromeSnippet * child)
  {
    //Prevent layout from stretching the child widgets. NB: Revisit this to make configurable from chrome?
    child->widget()->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    //child->widget()->setParentItem(this->widget()); // Shouldn't be needed, right?
    m_layout->addItem(child->widget());
    //Add a stretch after each element so the layout looks like this: |stretch|item|stretch|item . . . . stretch|item|
    m_layout->addStretch();
   
    // If child is not a container itself, connect to its mouseEvent
    WebChromeContainerSnippet * s = qobject_cast <WebChromeContainerSnippet * >(child);
    if (!s ) {
      //qDebug() << __PRETTY_FUNCTION__ << s  << child->elementId();
      // Connect to mouse events of snippets
      WebChromeItem * item = static_cast<WebChromeItem*> (child->widget());
      connect(item, SIGNAL(mouseEvent(QEvent::Type)), this, SIGNAL(snippetMouseEvent(QEvent::Type)));
    } 
    qreal childHeight = child->widget()->size().height();
    if(childHeight > m_layoutHeight){
      m_layoutHeight = childHeight;
      updateOwnerArea();
    }
  }

  void WebChromeContainerSnippet:: updateOwnerArea()
  {
    QGraphicsWidget * item = static_cast<QGraphicsWidget*> (widget());
    //Resize the item
    m_ownerArea =  m_chrome->getSnippetRect(m_elementId);
    item->setPreferredSize(m_ownerArea.width(), m_layoutHeight);
    //Also resize in case item is not part of anchor layout
    item->resize(item->preferredSize());
  }

  void WebChromeContainerSnippet::setLayoutHeight(int height){
    if(m_layoutHeight != height){
      m_layoutHeight = height;
      updateOwnerArea();
    }
  }

} // endof namespace GVA
