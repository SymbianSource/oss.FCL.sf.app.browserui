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


#include "ToolbarChromeItem.h"
#include "GWebContentView.h"
#include "WebChromeContainerSnippet.h"

#include <QDebug>

#define TOOLBAR_MARGIN 4
#define TOOLBAR_BORDER_WIDTH 3
#define TOOLBAR_BORDER_COLOR "#2A3447"
#define TOOLBAR_GRADIENT_START "#2E3B57"
#define TOOLBAR_GRADIENT_END "#44587D"
#define TOOLBAR_RIGHTCORNER_ITEM 2
#define TOOLBAR_LEFTCORNER_ITEM 0

namespace GVA {

  ToolbarChromeItem::ToolbarChromeItem(QGraphicsItem* parent)
    : QGraphicsWidget(parent),
      m_snippet(NULL),
      m_partialbg(NULL),
      m_opacity(0.75)
  {

    setProperties();


  }

  ToolbarChromeItem::~ToolbarChromeItem()
  {
    if (m_partialbg) {
      delete m_partialbg;
    }
  }

  void ToolbarChromeItem::resizeEvent(QGraphicsSceneResizeEvent * ev)
  {
    Q_UNUSED(ev)

    addPartialbg();

  }

  void ToolbarChromeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget)
  {
    Q_UNUSED(opt)
    Q_UNUSED(widget)

    painter->save();
 
    painter->setRenderHint(QPainter::Antialiasing);
 
    painter->setPen(m_pen);
    painter->setOpacity(m_opacity);
 

//    qDebug() << __PRETTY_FUNCTION__ << boundingRect();
    painter->fillPath(*m_partialbg, QBrush(m_grad));
    painter->drawPath(*m_partialbg);
   
    // restore painter
    painter->restore(); 
 
  }
  void ToolbarChromeItem::setSnippet(WebChromeContainerSnippet* snippet) {

    //qDebug() << __func__ << snippet;
    m_snippet = snippet;
  }

  void ToolbarChromeItem::setProperties() {

    m_pen.setWidth(TOOLBAR_BORDER_WIDTH);
    m_pen.setBrush(QBrush(TOOLBAR_BORDER_COLOR));
   
    m_grad.setColorAt(0, TOOLBAR_GRADIENT_START);
    m_grad.setColorAt(1, TOOLBAR_GRADIENT_END);
        
  }

  void ToolbarChromeItem::addPartialbg() {

    WebChromeContainerSnippet * s = static_cast<WebChromeContainerSnippet*>(m_snippet);
    QRectF rc = s->layout()->itemAt(TOOLBAR_LEFTCORNER_ITEM)->geometry();
    if (m_partialbg) {
      delete m_partialbg;
    }

    m_partialbg = new QPainterPath();
    int width =  rc.width()-TOOLBAR_MARGIN;
   
    // Add left corner bg
    m_partialbg->addEllipse(1, 1, width, width);

    // Right Corner background
    int x = boundingRect().width()- rc.width() + 1;
    QRectF r(x, 1, width, width);

    m_partialbg->addEllipse(r);
  }


} // end of namespace GVA


