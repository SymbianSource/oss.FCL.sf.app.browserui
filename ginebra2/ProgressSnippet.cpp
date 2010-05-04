/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "ProgressSnippet.h"
#include "GWebContentView.h"
#include "ContentViewDelegate.h"
#include "ChromeSnippet.h"

#include <QDebug>

namespace GVA {

  ProgressSnippet::ProgressSnippet(ChromeSnippet * snippet, QGraphicsItem* parent)
    : NativeChromeItem(snippet, parent),
      m_progress(0),
      m_webView(0)
  {
    //Set indicator color from element css
    QString cssColor = m_snippet->element().styleProperty("color", QWebElement::ComputedStyle);
    CSSToQColor(cssColor, m_color);
  }

  ProgressSnippet:: ~ProgressSnippet()
  {

  }

  void ProgressSnippet::onProgress(int progress)
  {
    //QApplication::processEvents(QEventLoop::ExcludeSocketNotifiers);
    qreal p = ((qreal)progress)/100;
    qDebug() << "ProgressSnippet::onProgress: " << p;
    if(m_progress != p){
      m_progress = p;
      update();
    }
  }

  void ProgressSnippet::onStart() {
    onProgress(0);
  }

  void ProgressSnippet::onFinished(bool ok) {
    Q_UNUSED(ok)
    onProgress(0);
  }

  void ProgressSnippet::paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget)
  {
    Q_UNUSED(opt)
    //NB: Would be nice to do this a bit sooner. How about adding viewAdded signal to ChromeWidget? 
    if(!m_webView){
      m_webView  = dynamic_cast<GWebContentView*> (m_snippet->chrome()->getView("WebView"));
      if(m_webView){
        connect(m_webView, SIGNAL(loadStarted()), this, SLOT(onStart()));
	    connect(m_webView, SIGNAL(loadProgress(int)), this, SLOT(onProgress(int)));
	    connect(m_webView, SIGNAL(loadFinished(bool)), this, SLOT(onFinished(bool)));
      }
      else
	qDebug() << "No web view found";
    }
    
    //painter->fillRect(QRectF(0,0, geometry().width(), geometry().height()), Qt::blue);
    qreal minWidth = geometry().width()/10;
    painter->fillRect(QRectF(0,0, minWidth + (geometry().width() - minWidth)*m_progress, geometry().height()), m_color);
  }

} // end of namespace GVA


