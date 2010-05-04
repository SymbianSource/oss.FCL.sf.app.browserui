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


#include <QtGui>
#include <QTimeLine>

#include "ScrollZoomWidget.h"
#include "GWebContentView.h"
#include "ChromeSnippet.h"

#include <QDebug>

namespace GVA {

  ScrollZoomWidget::ScrollZoomWidget(ChromeSnippet* snippet, QGraphicsItem* parent)
    : NativeChromeItem(snippet, parent),
      m_zoomIn(true),
      m_scrolling(false),
      m_xScale(1),
      m_yScale(1),
      m_deltaX(0),
      m_deltaY(0),
      m_timeLine(0),
      m_webView(0),
      m_effect(0)
  {
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
    setOpacity(0.50);
    // m_effect = new QGraphicsDropShadowEffect();
    //static_cast<QGraphicsDropShadowEffect*>(m_effect)->setOffset(4.0,4.0);
    //static_cast<QGraphicsDropShadowEffect*>(m_effect)->setBlurRadius(2.0);
    //setGraphicsEffect(m_effect);
    m_buttonImage = QImage(":/chrome/demochrome/Scroll.png");
  }

  ScrollZoomWidget:: ~ScrollZoomWidget()
  {
    delete m_timeLine;
  }

 
  QVariant ScrollZoomWidget::itemChange(GraphicsItemChange change, const QVariant & value)
  {
    qreal deltaX = 0;
    qreal deltaY = 0;
    qreal scrollY = 0;

    if(m_webView && (change == ItemPositionChange)) {
      QPointF newPos = value.toPointF();
      if((m_centerX != 0) && (m_centerY != 0)){
	deltaX = newPos.x() - m_centerX;
	deltaY = newPos.y() - m_centerY;
      }
      if(m_scrolling)
	scrollY = deltaY*m_yScale/2;
      if(m_webView->scrollY() == 0){
	qreal slide = m_snippet->chrome()->slideView(scrollY);	  
        scrollY-=slide;
      }
      m_webView->scrollBy(-deltaX*m_yScale/2, -scrollY);
    }
    return QGraphicsItem::itemChange(change, value);
  }
 
  void ScrollZoomWidget::onLoadFinished(bool ok)
  {
    if(m_webView->contentWidth() > parentWidget()->size().width())
      m_xScale = m_webView->contentWidth()/parentWidget()->size().width();
    if(m_webView->contentHeight() > parentWidget()->size().height())
      m_yScale = m_webView->contentHeight()/parentWidget()->size().height();
  }

   void ScrollZoomWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget)
  {
    painter->save();
    if(m_scrolling) {
      setOpacity(0.0);
      //painter->setBrush(Qt::transparent);
      //painter->scale(2.0, 2.0);
      //painter->setPen(QPen(Qt::transparent, 4.0));
      //painter->drawEllipse(QRectF(0,0, geometry().width(), geometry().height()));
    }
    else {
      setOpacity(0.60);
      //painter->setBrush(Qt::blue);
      //painter->setPen(QPen(Qt::blue, 4.0));     
      //painter->drawEllipse(QRectF(0,0, geometry().width(), geometry().height()));
    }
    painter->drawImage(QPointF(0,0),m_buttonImage);
    painter->restore();
  }

  void ScrollZoomWidget::mousePressEvent(QGraphicsSceneMouseEvent * ev)
  {

    if(!m_webView){
      m_webView  = static_cast<GWebContentView*> (m_snippet->chrome()->getView("WebView"));
      if(m_webView) 
	connect(m_webView, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
      else
	return;
    }
    m_centerX = pos().x();
    m_centerY = pos().y();
    onLoadFinished(true);
    m_scrolling = true;
    //zoom();
    //scroll();
    QGraphicsWidget::mouseReleaseEvent(ev);
  }

  void ScrollZoomWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent * ev)
  {
    m_scrolling = false;
    setPos(m_centerX, m_centerY);
    setOpacity(0.6);
    update();
    //m_timeLine->stop();
    QGraphicsWidget::mouseReleaseEvent(ev);
  }

  void ScrollZoomWidget::updateZoom(qreal delta){
    if(!m_webView) {
      m_webView  = static_cast<GWebContentView*> (m_snippet->chrome()->getView("WebView"));
      if(m_webView) 
	connect(m_webView, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
    }
    if(!m_webView)
      return;
    if(m_zoomIn)
      m_webView->zoomBy(0.1);
    else
      m_webView->zoomBy(-0.1);
  }


  void ScrollZoomWidget::updateScroll(qreal delta){
    if(!m_webView) {
      m_webView  = static_cast<GWebContentView*> (m_snippet->chrome()->getView("WebView"));
      if(m_webView) 
	connect(m_webView, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
    }
    if(!m_webView)
      return;
    m_webView->scrollBy(m_deltaX*2,m_deltaY*2);
  }

 
  void ScrollZoomWidget::zoom()
  {
    m_zoomIn = !m_zoomIn;
    if(!m_timeLine) {
      m_timeLine = new QTimeLine();
      connect(m_timeLine, SIGNAL(valueChanged(qreal)),
	      this, SLOT(updateZoom(qreal)));
    }
    else {
      m_timeLine->stop();
    }
    m_timeLine->start();
  }
 

  void ScrollZoomWidget::scroll()
  { 
    m_zoomIn = !m_zoomIn;
    if(!m_timeLine) {
      m_timeLine = new QTimeLine();
      m_timeLine->setLoopCount(0);
      connect(m_timeLine, SIGNAL(valueChanged(qreal)),
	      this, SLOT(updateScroll(qreal)));
    }
    else {
      m_timeLine->stop();
    }
    m_timeLine->start();
  }

} // end of namespace GVA


