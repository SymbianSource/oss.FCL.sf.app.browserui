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


#ifndef _GINEBRA_CHROME_SNIPPET_H_
#define _GINEBRA_CHROME_SNIPPET_H_

#include <QtGui>
#include "visibilityanimator.h"
#include "attentionanimator.h"

class QTimer;
class QGraphicsItemAnimation;
class QGraphicsLinearLayout;
class ChromeWidget;
class ChromeSnippetJSObject;
class QGraphicsSceneContextMenuEvent;
/**
 * \brief Encapsulates an individual piece of the browser chrome.
 * 
 * The ChromeSnippet class encapsulates an individual piece of the browser chrome.
 * The contents of the snippet are taken from individual DOM elements (typically DIVs)
 * in the chrome HTML page by ChromeWidget and can be accessed from javascript in that 
 * page through the properties, slots and signals provided by this class.
 * 
 * \sa ChromeWidget 
 */
class ChromeSnippet: public QGraphicsRectItem 
{
    
  friend class VisibilityAnimator;
  friend class AttentionAnimator;

public:
  ChromeSnippet(QGraphicsItem * parent, ChromeWidget * owner, QObject *jsParent, const QString &docElementId);
  ~ChromeSnippet();
  void setOwnerArea(const QRectF& ownerArea);
  QRectF& ownerArea();
  void updateChildGeometries();
  void setDocElementId(const QString &id) { m_docElementId = id; }
  void setDocElementName(const QString &name) { m_docElementName = name; }
  void setVisibilityAnimator(VisibilityAnimator * animator) {delete m_visibilityAnimator; m_visibilityAnimator = animator; }
  void setAttentionAnimator(AttentionAnimator * animator) { delete m_visibilityAnimator; m_attentionAnimator = animator; }
  bool isHiding() { return m_isHiding; }
  void setHiding(bool value) { m_isHiding = value; }
  
public:
  void toggleVisibility();
  void show(bool useAnimation = true);
  void hide(bool useAnimation = true);
  void toggleAttention();
  
  /// Starts an animation timer.  Returns an animation object that can rotate, move or fade the snippet.
  /// 
  /// \sa GraphicsItemAnimation
  QObject *animate(int duration);
  
  QString docElementId() const { return m_docElementId; }
  
  void setPosition(int x, int y) { setPos(x, y); }
  QString  getDisplayMode();
  //Snippet auto-layout methods
  QString anchor() { return m_anchor; }
  void setAnchor(const QString& anchor) { m_anchor = anchor; }
  int anchorOffset() { return m_anchorOffset; }
  void setAnchorOffset(int anchorOffset) { m_anchorOffset = anchorOffset; }
  bool hidesContent(){ return m_hidesContent;}
  void setHidesContent(bool hidesContent) { m_hidesContent = hidesContent; }
  
  /// Set the animation that is triggered when the snippet is shown or hidden.
  void setVisibilityAnimator(const QString &animatorName); 
  
  /// Set the animation that is triggered when the snippet wants attention.
  void setAttentionAnimator(const QString &animatorName);    
  
  void dump() const;
  
  /// The snippet's screen geometry.
  QObject *getGeometry() const;
  QRect geometry() const { return rect().toRect(); }

  /// The position of the snippet's top-left corner.
  QObject *getPosition() const;
  
  void resize(const QSizeF &size) { resize(size.width(), size.height()); }
  void resize(qreal width, qreal height);

  /// True if the snippet can be dragged with the mouse.  Defaults to false.
  bool draggable() const { return m_draggable; }
  void setDraggable(bool value) { m_draggable = value; }

  /// Repaint the snippet
  void repaint();
  
  QObject *getJSObject();
  static bool getContextMenuFlag();
  
protected:
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
  void mousePressEvent(QGraphicsSceneMouseEvent  *ev);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent  *ev);
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent  *ev);
  void mouseMoveEvent(QGraphicsSceneMouseEvent  *ev);
  void keyPressEvent ( QKeyEvent * event );
  void keyReleaseEvent ( QKeyEvent * event );
  void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
  void passMouseEventToPage(QEvent::Type type, QGraphicsSceneMouseEvent *ev);
  virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);

 private:
  ChromeWidget* m_owner;
  QRectF m_ownerArea;
  QString m_docElementId;
  QString m_docElementName;
  QGraphicsLinearLayout * m_layout;
  VisibilityAnimator *m_visibilityAnimator; //Take ownership
  AttentionAnimator *m_attentionAnimator; //Take ownership
  bool m_dragging;
  bool m_draggable;
  QString m_anchor;
  int m_anchorOffset;
  bool m_hidesContent;
  bool m_isHiding;
 QPointer<ChromeSnippetJSObject> m_jsObject;
  static bool contextMenuFlag;

};


#endif
