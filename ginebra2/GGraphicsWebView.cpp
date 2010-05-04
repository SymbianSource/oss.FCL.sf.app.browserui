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


#include "GGraphicsWebView.h"
namespace GVA {

  GGraphicsWebView::GGraphicsWebView(QGraphicsItem * parent)
    : QGraphicsWebView(parent),
      m_loading(false)
  {
    connect(this, SIGNAL(loadStarted()), this, SLOT(onLoadStarted()));
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
  }

  void GGraphicsWebView::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
  {
    //    qDebug() << "d:" << painter->device()->depth() << " w:" << painter->device()->width() << " h:" << painter->device()->height();
    if(m_loading && m_loadTime.elapsed() < 7000) {
      painter->save();
      painter->setBrush(Qt::transparent);
      painter->setPen(Qt::NoPen);
      painter->drawRect(option->exposedRect.toRect());
      painter->restore();
    } else {
      QGraphicsWebView::paint(painter, option, widget);
    }
  }

  void GGraphicsWebView::onLoadStarted()
  {
    m_loading = true;
    m_loadTime.start();
  }

  void GGraphicsWebView::onLoadFinished(bool ok)
  {
    m_loading = false;
    update();
  }

}
