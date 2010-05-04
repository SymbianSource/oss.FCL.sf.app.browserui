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


#ifndef __GINEBRA_TEXTEDITITEM_H__
#define __GINEBRA_TEXTEDITITEM_H__

#include <QtGui>
#include "NativeChromeItem.h"

class QGraphicsTextItem;

namespace GVA {

  class EditorWidget : public QGraphicsTextItem
  {
   Q_OBJECT
  public:
    EditorWidget(QGraphicsItem* parent);
    virtual ~EditorWidget(){;}
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem * option, QWidget* widget);
    void setText(const QString& text, bool html=false);
  signals:
    void cursorXChanged(qreal newx, qreal oldx);
  protected: 
    virtual void keyPressEvent(QKeyEvent *event);
  private:
    qreal cursorX();
    QTextLine m_textLine;
  };

  class TextEditItem : public NativeChromeItem
  {
    Q_OBJECT
  public:
    TextEditItem(ChromeSnippet * snippet, QGraphicsItem* parent = 0);
    virtual ~TextEditItem();
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem * option, QWidget* widget);
    virtual bool eventFilter( QObject * obj, QEvent *ev);
  public slots:
    QString text();
    void setText(const QString & text);
  signals:
    void activated();
  protected:
    virtual void resizeEvent(QGraphicsSceneResizeEvent * ev);
  private slots:
    //void onCursorPositionChanged(const QTextCursor& cursor);
    void onCursorXChanged(qreal newx, qreal oldx);
  private:
    void internalScroll(qreal deltaX);
    EditorWidget * m_editor;
    QPainterPath m_background;
    QColor m_textColor;
    QColor m_backgroundColor;
    int m_padding;
    int m_border;
    QColor m_borderColor;
    QGraphicsDropShadowEffect * m_shadow;
    QTextOption m_textOption;
    qreal m_textWidth;
    qreal m_viewPortWidth;
    QTextCursor m_cursor;
    qreal m_scrollPos;
    QGraphicsWidget * m_viewPort;
  };

} // end of namespace GVA

#endif // __GINEBRA_PROGRESSSNIPPET_H__
