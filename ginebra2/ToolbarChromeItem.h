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


#ifndef __GINEBRA_TOOLBARCHROMEITEM_H
#define __GINEBRA_TOOLBARCHROMEITEM_H

#include <QtGui>
#include "WebChromeContainerSnippet.h"

namespace GVA {


  class ToolbarChromeItem : public QGraphicsWidget
  {
    Q_OBJECT
  public:
    ToolbarChromeItem(QGraphicsItem* parent = 0);
    virtual ~ToolbarChromeItem();
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget);
    virtual void setSnippet(WebChromeContainerSnippet * snippet);
    QPen pen() { return m_pen;}
    QLinearGradient grad() { return m_grad;}
    qreal opacity() { return m_opacity;}
    void setOpacity(qreal opacity) {m_opacity = opacity;}
    WebChromeContainerSnippet * getSnippet() { return m_snippet;}
    QPainterPath* partialbg() { return m_partialbg;}
    void removePartialbg() { delete m_partialbg; m_partialbg = NULL;}
    void addPartialbg() ;

  protected:
    virtual void resizeEvent(QGraphicsSceneResizeEvent * ev);
  
  private:
    void setProperties();

    WebChromeContainerSnippet * m_snippet;
    QPainterPath* m_partialbg;
    QPen m_pen;
    QLinearGradient m_grad;
    qreal m_opacity;

  };

} // end of namespace GVA

#endif // __GINEBRA_TOOLBARCHROMEITEM_H
