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


#include "ChromeRenderer.h"
#include "WebChromeItem.h"
#include <QWebFrame>
#include <QEvent>
#include <QFocusEvent>

#include <QDebug>

namespace GVA {

ChromeRenderer::ChromeRenderer(QWebPage * chromePage, QGraphicsItem * parent)
  : QGraphicsWebView(parent)/*,
    m_pageBits(0),
    m_painter(0)*/
    
{
  setPage(chromePage);
  setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  QPalette viewPalette = palette();
  viewPalette.setBrush(QPalette::Base, Qt::transparent);
  //viewPalette.setColor(QPalette::Window, Qt::transparent);
  page()->setPalette(viewPalette);
  setFocus(); //Initially grab the focus
  connect(page(), SIGNAL(repaintRequested(const QRect &)), this, SLOT(repaintRequested(const QRect &)));
  connect(page()->mainFrame(), SIGNAL(contentsSizeChanged(const QSize &)), this, SLOT(onContentsSizeChanged(const QSize &)));
  connect(page()->mainFrame(), SIGNAL(initialLayoutCompleted()), this, SLOT(onInitialLayoutCompleted()));

  //QObject::connect(page()->mainFrame(), SIGNAL(contentSizeChanged(const QSize &)), this, SIGNAL(chromeResized())); 
}

ChromeRenderer::~ChromeRenderer()
{
  //delete m_pageBits;
}

void ChromeRenderer::resizeEvent(QGraphicsSceneResizeEvent * ev)
{
  //qDebug() << "ChromeRenderer resizeEvent: " << ev->newSize();
  //QGraphicsWebView::resizeEvent(ev);
  page()->setPreferredContentsSize(ev->newSize().toSize());
  // qDebug()<< "ChromeRenderer::resizeEvent: ev->newSize(): " << ev->newSize() << " ContentSize: " << page()->mainFrame()->contentsSize(); 
  page()->setViewportSize(page()->mainFrame()->contentsSize());
  updateGeometry();
  //qDebug() << "ChromeRenderer::resizeEvent: new contents size " << page()->mainFrame()->contentsSize() << " : " << page()->mainFrame()->documentElement().geometry();
  emit chromeResized();
}

void ChromeRenderer::keyPressEvent( QKeyEvent * ev )
{ 
    //  qDebug() << "ChromeRenderer::keyPressEvent" << ev->type();
  QGraphicsWebView::keyPressEvent(ev);
#ifdef Q_OS_SYMBIAN //Ginebra 1 hack for symbian fep key handler
  if(ev->key() == Qt::Key_Select || ev->key() == Qt::Key_Return || ev->key() == Qt::Key_Enter) {
    emit symbianCarriageReturn();
  }
  if(ev->key() == Qt::Key_Left || ev->key() == Qt::Key_Right || ev->key() == Qt::Key_Down || ev->key() == Qt::Key_Up)
    ev->accept();
#endif
}

void ChromeRenderer::focusInEvent(QFocusEvent * event)
{
  if (event->reason() != Qt::PopupFocusReason) // to fix the special char issue on VKB
    QGraphicsWebView::focusInEvent(event);
}

void ChromeRenderer::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  //qDebug() << "ChromeRenderer::paint";
}

void ChromeRenderer::repaintRequested(const QRect& dirtyRect)
{
  //qDebug() << "ChromeRenderer repaintRequested: " << dirtyRect;
  WebChromeItem * item;
  foreach(item, m_renderList){
    if(item->ownerArea().intersects(dirtyRect) && !item->isPainting()){
      //  qDebug() << "ChromeRenderer::repaintRequested: " << item->element().attribute("id") <<  " isPainting: " << item->isPainting();
      item->update();
    }
  }
  //emit chromeRepainted(QRectF(dirtyRect));
}

void ChromeRenderer::onContentsSizeChanged(const QSize & size)
{
  ;//qDebug() << "ChromeRenderer::onContentsSizeChanged: " << size;
}

void ChromeRenderer::onInitialLayoutCompleted(){
  ;//qDebug() << "ChromeRenderer::onInitialLayoutCompleted";
}


} // end of namespace GVA
