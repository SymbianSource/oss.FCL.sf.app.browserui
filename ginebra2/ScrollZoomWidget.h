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


#ifndef __gva_scrollzoomwidget_h__
#define __gva_scrollzoomwidget_h__

#include "NativeChromeItem.h"

class QTimeLine;

namespace GVA {

  class GWebContentView;

  class ScrollZoomWidget : public NativeChromeItem
  {
    Q_OBJECT
  public:
    ScrollZoomWidget(ChromeSnippet* snippet, QGraphicsItem* parent = 0);
    virtual ~ScrollZoomWidget();
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget);
    void mousePressEvent(QGraphicsSceneMouseEvent * ev);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * ev);
  public slots:
    void updateZoom(qreal delta);
    void updateScroll(qreal delta);
    void onLoadFinished(bool ok);
  protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
  private:
    void zoom();
    void scroll();
    bool m_zoomIn;
    bool m_scrolling;
    qreal m_xScale;
    qreal m_yScale;
    qreal m_deltaX;
    qreal m_deltaY;
    qreal m_centerX;
    qreal m_centerY;
    QTimeLine * m_timeLine;
    GWebContentView * m_webView;
    QGraphicsEffect * m_effect;
    QImage m_buttonImage;
  };

} // end of namespace GVA

#endif // __GINEBRA_REDCHROMESNIPPET_H__
