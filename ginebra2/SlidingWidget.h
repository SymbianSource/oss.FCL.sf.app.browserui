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


#ifndef __gva_slidingwidget_h__
#define __gva_slidingwidget_h__

#include <QtGui>

namespace GVA {

  class ChromeSnippet;

  class SlidingWidget : public QGraphicsWidget
  {
  public:
    SlidingWidget(QGraphicsItem* parent = 0);
    virtual ~SlidingWidget();
    void setWindow(QGraphicsWidget* window);
    qreal slide(qreal delta);
    void attachItem(QGraphicsWidget * item);
    void detachItem(QGraphicsWidget * item);
  protected:
    virtual void resizeEvent(QGraphicsSceneResizeEvent *ev);
  private:
    QGraphicsWidget *m_top;
    QGraphicsWidget *m_window;
    QGraphicsWidget *m_bottom;
    QGraphicsAnchorLayout *m_layout;
    qreal m_slidePos;
    qreal m_slideMax;
  };

} // end of namespace GVA

#endif
