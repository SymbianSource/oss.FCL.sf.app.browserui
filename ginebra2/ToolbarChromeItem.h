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

#ifndef __GINEBRA_TOOLBARCHROMEITEM_H
#define __GINEBRA_TOOLBARCHROMEITEM_H

#include <QtGui>
#include "ChromeSnippet.h"
#include "NativeChromeItem.h"

namespace GVA {

#ifndef  Q_WS_MAEMO_5
  class ToolbarChromeItem : public ChromeItem
  {
    Q_OBJECT
  public:
    ToolbarChromeItem(ChromeSnippet* snippet, QGraphicsItem* parent = 0);
    virtual ~ToolbarChromeItem();
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget);
    virtual void setSnippet(ChromeSnippet* snippet);
    QPen pen() { return m_pen;}
#ifdef BROWSER_LAYOUT_TENONE
    void paintCornerBackgrounds(QPainter *painter);
    void addCornerBackground();
    void removeCornerBackground() { m_cornerBackgroundPixmap = QPixmap("");}
#else
    QLinearGradient grad() { return m_grad;}
    QPainterPath* partialbg() { return m_partialbg;}
    void removePartialbg() { delete m_partialbg; m_partialbg = NULL;}
    void addPartialbg() ;
#endif
    qreal opacity() { return m_opacity;}
    void setOpacity(qreal opacity) {m_opacity = opacity;}

  protected:
    virtual void resizeEvent(QGraphicsSceneResizeEvent * ev);

  private:
    void setProperties();

#ifdef BROWSER_LAYOUT_TENONE
    QPixmap m_cornerBackgroundPixmap;
#else
    QPainterPath* m_partialbg;
    QLinearGradient m_grad;
#endif

    QPen m_pen;
    qreal m_opacity;
    QColor m_borderColor;
    int m_padding;
    int m_borderWidth;

  };
#else  
  class ToolbarChromeItem : public ChromeItem
  {
    Q_OBJECT
  public:
    ToolbarChromeItem(ChromeSnippet* snippet, QGraphicsItem* parent = 0);
    virtual ~ToolbarChromeItem();
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget);
    virtual void setSnippet(ChromeSnippet* snippet);
    QPen pen() { return m_pen;}
    qreal opacity() { return m_opacity;}
    void setOpacity(qreal opacity) {m_opacity = opacity;}
    void paintCornerBackgrounds(QPainter *painter);
    void addCornerBackground();

  protected:
    virtual void resizeEvent(QGraphicsSceneResizeEvent * ev);

  private:
    void setProperties();

    QPen m_pen;
    QPixmap m_cornerBackgroundPixmap;
    qreal m_opacity;
    QColor m_borderColor;
    int m_padding;
    int m_borderWidth;

  };
#endif
} // end of namespace GVA

#endif // __GINEBRA_TOOLBARCHROMEITEM_H
