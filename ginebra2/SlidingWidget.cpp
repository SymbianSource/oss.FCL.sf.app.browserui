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


#include "SlidingWidget.h"
#include "ChromeSnippet.h"

#include <QDebug>

//NB: Note that the current implementation only supports one snippet child

namespace GVA {

  SlidingWidget::SlidingWidget(QGraphicsItem* parent)
    : QGraphicsWidget(parent),
      m_top(0),
      m_window(0),
      m_bottom(0),
      m_slideMax(0),
      m_slidePos(0)
  {
    //Clip child item so that it can "slide" out of view
    setFlags(QGraphicsItem::ItemClipsChildrenToShape);
    //setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
    m_layout = new QGraphicsAnchorLayout();
    m_layout->setContentsMargins(0,0,0,0);
    m_layout->setSpacing(0);
    setLayout(m_layout);
  }

  SlidingWidget:: ~SlidingWidget()
  {
  }

  void SlidingWidget::resizeEvent(QGraphicsSceneResizeEvent *ev)
  {
    qDebug() << "SlidingWidget::resizeEvent: " << ev->newSize();
    if(m_window)
      m_window->resize(ev->newSize());
      //m_window->resize(ev->newSize().width(), m_window->size().height());
    QGraphicsWidget::resizeEvent(ev);
  }

  qreal SlidingWidget::slide(qreal delta)
  {

    qreal newPos = m_slidePos + delta;

    //qDebug() << "SlidingWidget::slide: delta: " << delta << " m_slidePos: " << m_slidePos << " newPos: " << newPos;

    if(newPos < 0) {
      if(m_slidePos == 0)
	return 0;
      delta = -m_slidePos;
      m_slidePos = 0;
    }

    else if(newPos > m_slideMax){
      if(m_slidePos == m_slideMax)
	return 0;
      delta = m_slideMax - m_slidePos;
      m_slidePos = m_slideMax;
    }

    else m_slidePos = newPos;

    //qDebug() << "calculated delta: " << delta << " new m_slidePos: " << m_slidePos;

    QTransform transform;
    transform.translate(0,delta);
    //m_window->setTransform(transform, true);
    //m_top->setTransform(transform, true);
    m_top->setPos(0, m_top->pos().y() + delta);
    m_window->setPos(0, m_window->pos().y() + delta);
    return delta;
  }

  void SlidingWidget::setWindow(QGraphicsWidget * window)
  {
    if(m_window)
      m_window->setParentItem(0);
    m_window = window;
    m_window->setParentItem(this);
    m_window->resize(size());
    if(m_top) {
    	slide(0);
    	QGraphicsWidget* item=m_top;
    	attachItem(item);
    }
    else
      m_window->setPos(0,0);
    //m_layout->addAnchors(m_window, m_layout, Qt::Horizontal);
  }

  void SlidingWidget::attachItem(QGraphicsWidget * item)
  {
    if(m_top)
      detachItem(m_top);
    m_top = item;
    m_top->setParentItem(this);
    m_top->setPos(0,0);
    m_slideMax =  m_top->size().height();
    m_slidePos = m_slideMax;
    if(m_window)
      m_window->setPos(0, m_top->size().height());
  }


  void SlidingWidget::detachItem(QGraphicsWidget * item)
  {
    if(m_top != item)
      return;
    m_top->setParentItem(0);
    m_top = 0;
    if(m_window)
      m_window->setPos(0,0);
  }


} // end of namespace GVA


