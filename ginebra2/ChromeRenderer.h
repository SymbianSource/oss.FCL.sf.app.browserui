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


#ifndef __GINEBRA_CHROMERENDERER_H__
#define __GINEBRA_CHROMERENDERER_H__

#include <QtGui>
#include <QGraphicsWebView>
class QFocusEvent;
namespace GVA {

  class WebChromeItem;

  class ChromeRenderer : public QGraphicsWebView
  {
    Q_OBJECT
  public:
    ChromeRenderer(QWebPage * chromePage, QGraphicsItem * parent = 0);
    virtual ~ChromeRenderer();
    //QPixmap * pageBits() {return m_pageBits;}
    void addRenderItem(WebChromeItem * item) {m_renderList.append(item);}
    void clearRenderList() {m_renderList.clear();}
  public slots:
    void repaintRequested(const QRect& dirtyRect);
    void onContentsSizeChanged(const QSize & size);
    void onInitialLayoutCompleted();
  signals:
    void chromeRepainted(const QRectF& rect = QRectF());
    void chromeResized();
#ifdef Q_OS_SYMBIAN //Ginebra 1 hack for symbian fep key handler
    void symbianCarriageReturn();
#endif
  protected:
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    virtual void resizeEvent(QGraphicsSceneResizeEvent * ev);
    virtual void keyPressEvent( QKeyEvent * ev );
    void focusInEvent(QFocusEvent* event);
  private:
    QList<WebChromeItem*> m_renderList;
  };

} // end of namespace GVA

#endif // __GINEBRA_CHROMERENDERER_H__
