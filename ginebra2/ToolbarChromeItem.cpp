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

#include "ToolbarChromeItem.h"
#include "NativeChromeItem.h"
#include "GWebContentView.h"
#include "WebChromeContainerSnippet.h"
#include "ChromeEffect.h"

#include <QDebug>

#if !defined(Q_WS_MAEMO_5) && !defined(BROWSER_LAYOUT_TENONE)
#define TOOLBAR_GRADIENT_START "#2E3B57"
#define TOOLBAR_GRADIENT_END "#44587D"
#endif

#if defined(Q_WS_MAEMO_5) || defined(BROWSER_LAYOUT_TENONE)
#define TOOLBAR_BG_OPACITY 1
#else
#define TOOLBAR_BG_OPACITY 0.75
#endif
#define TOOLBAR_LEFTCORNER_ITEM 0

namespace GVA {

  ToolbarChromeItem::ToolbarChromeItem(ChromeSnippet* snippet, QGraphicsItem* parent)
    : ChromeItem(NULL, parent),
#if defined(Q_WS_MAEMO_5) || defined(BROWSER_LAYOUT_TENONE)
      m_cornerBackgroundPixmap(""),
#else
      m_partialbg(NULL),
#endif
      m_opacity(TOOLBAR_BG_OPACITY)
  {
      setSnippet(snippet);
  }

  ToolbarChromeItem::~ToolbarChromeItem() {

#if !defined(Q_WS_MAEMO_5) && !defined(BROWSER_LAYOUT_TENONE)
    if (m_partialbg) {
      delete m_partialbg;
    }
#endif
  }

  void ToolbarChromeItem::resizeEvent(QGraphicsSceneResizeEvent * ev) {

    Q_UNUSED(ev)

#if !defined(Q_WS_MAEMO_5) && !defined(BROWSER_LAYOUT_TENONE)
    addPartialbg();
#endif
  }

  void ToolbarChromeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget) {

    Q_UNUSED(opt)
    Q_UNUSED(widget)

    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);

    painter->setPen(m_pen);
    painter->setOpacity(m_opacity);

    //qDebug() << __PRETTY_FUNCTION__ << boundingRect();

#if defined(Q_WS_MAEMO_5) || defined(BROWSER_LAYOUT_TENONE)
    paintCornerBackgrounds(painter);
#else
    painter->fillPath(*m_partialbg, QBrush(m_grad));
    painter->drawPath(*m_partialbg);
#endif

    // restore painter
    painter->restore();

    if(!isEnabled()) {
        // Disabled, apply whitewash.
        //ChromeEffect::paintDisabledRect(painter, opt->exposedRect);
    }
  }

  void ToolbarChromeItem::setProperties() {

    m_pen.setWidth(m_borderWidth);
    m_pen.setBrush(QBrush(m_borderColor));

#if !defined(Q_WS_MAEMO_5) && !defined(BROWSER_LAYOUT_TENONE)
    m_grad.setColorAt(0, TOOLBAR_GRADIENT_START);
    m_grad.setColorAt(1, TOOLBAR_GRADIENT_END);
#endif
  }

#if defined(Q_WS_MAEMO_5) || defined(BROWSER_LAYOUT_TENONE)
  void ToolbarChromeItem::paintCornerBackgrounds(QPainter *painter) {

    WebChromeContainerSnippet * s = static_cast<WebChromeContainerSnippet*>(m_snippet);
    if(!s->layout()->itemAt(TOOLBAR_LEFTCORNER_ITEM))
        return;
    QRectF rc = s->layout()->itemAt(TOOLBAR_LEFTCORNER_ITEM)->geometry();

    // Left corner background
    painter->drawPixmap(0, 0, m_cornerBackgroundPixmap);

    // Right Corner background
    qreal x = boundingRect().width()- m_cornerBackgroundPixmap.width();

    painter->drawPixmap(x, 0, m_cornerBackgroundPixmap);
  }

  void ToolbarChromeItem::addCornerBackground(){
      
      m_cornerBackgroundPixmap = QPixmap(":/toolbar/circle_bg.png");

  }
#else
  void ToolbarChromeItem::addPartialbg() {

    WebChromeContainerSnippet * s = static_cast<WebChromeContainerSnippet*>(m_snippet);
    if(!s->layout()->itemAt(TOOLBAR_LEFTCORNER_ITEM))
        return;
    QRectF rc = s->layout()->itemAt(TOOLBAR_LEFTCORNER_ITEM)->geometry();
    if (m_partialbg) {
      delete m_partialbg;
    }

    m_partialbg = new QPainterPath();
    qreal width =  rc.width()-m_padding;

    // Add left corner bg
    m_partialbg->addEllipse(1, 1, width, width);

    // Right Corner background
    qreal x = boundingRect().width()- rc.width() + 1;
    QRectF r(x, 1, width, width);

    m_partialbg->addEllipse(r);
  }
#endif

  void ToolbarChromeItem::setSnippet(ChromeSnippet* snippet) {

    ChromeItem::setSnippet(snippet);
    QString cssVal = m_snippet->element().styleProperty("border-top-color",
        QWebElement::ComputedStyle);
    NativeChromeItem::CSSToQColor(cssVal, m_borderColor);

    cssVal = m_snippet->element().styleProperty("padding-top", QWebElement::ComputedStyle);
    m_padding = cssVal.remove("px").toInt();

    cssVal = m_snippet->element().styleProperty("border-top-width", QWebElement::ComputedStyle);
    m_borderWidth = cssVal.remove("px").toInt();
    setProperties();
  }

} // end of namespace GVA


