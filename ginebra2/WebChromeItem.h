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


#ifndef __GINEBRA_WEBCHROMEITEM_H__
#define __GINEBRA_WEBCHROMEITEM_H__

#include <QtGui>
#include <QWebElement>
#include "CachedHandler.h"

namespace GVA {

  class ChromeWidget;
  class ChromeRenderer;
  class WebChromeSnippet;

  class WebChromeItem : public QGraphicsWidget
  {
    Q_OBJECT
  public:
    WebChromeItem(const QRectF& ownerArea, ChromeWidget* chrome, const QWebElement & element, QGraphicsItem* parent=0);
    virtual ~WebChromeItem();
    virtual void init(WebChromeSnippet * snippet);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget);
    QRectF ownerArea() {return m_ownerArea;}\
    void setOwnerArea(const QRectF& ownerArea);
    void setCachedHandlers(QList<CachedHandler> handlers) {m_handlers = handlers;}
    QGraphicsScene * scene();
    ChromeRenderer * renderer();
    void grabFocus();
    bool event(QEvent* event);
    ChromeWidget * chrome() { return m_chrome; }
    QWebElement element() { return m_element; }
    inline bool isPainting() { return m_painting; }
  public slots:
    void repaintFromChrome(const QRectF& rect = QRectF());
    //inline void setCacheMode(bool mode) {m_cacheMode = mode;};
  signals:
    void contextMenu(QGraphicsSceneContextMenuEvent * ev);
    void mouseEvent(QEvent::Type type);
  protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent * ev);
    void mousePressEvent(QGraphicsSceneMouseEvent * ev);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * ev);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent  *ev);
    void mouseMoveEvent(QGraphicsSceneMouseEvent  *ev);
    //void keyPressEvent ( QKeyEvent * event );
    //void keyReleaseEvent ( QKeyEvent * event );
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void forwardMouseEvent(QEvent::Type type, QGraphicsSceneMouseEvent *ev);

    //virtual bool event(QEvent * ev);
  private:
    void cachedHandlerEvent(QGraphicsSceneMouseEvent * ev);
    QRectF m_ownerArea;
    QPixmap * m_pageBits;
    ChromeWidget* m_chrome;
    QWebElement m_element;
    //ChromeRenderer * m_renderer;
    QList<CachedHandler> m_handlers;
    //bool m_cacheMode;
    bool m_painting;
  };

} // end of namespace GVA

#endif // __GINEBRA_REDCHROMESNIPPET_H__
