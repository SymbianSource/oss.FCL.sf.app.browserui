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


#include "chromesnippet.h"
#include "chromesnippetjsobject.h"
#include "chromewidget.h"
#include "graphicsitemanimation.h"
#include "scriptobjects.h"

#include <QGraphicsLinearLayout>
#include <QGraphicsSceneContextMenuEvent>
#include <QImage>
#include <QPainter>
#include "qwebframe.h"
#include "qwebpage.h"
#include "chromeview.h"

bool ChromeSnippet::contextMenuFlag = false;

ChromeSnippet::ChromeSnippet(QGraphicsItem * parent, ChromeWidget * owner, QObject *jsParent, const QString &docElementId)
  : QGraphicsRectItem(parent),
    m_owner(owner),
    m_docElementId(docElementId),
    m_layout(0),
    m_visibilityAnimator(0),
    m_attentionAnimator(0),
    m_dragging(false),
    m_draggable(false),
    m_anchor("AnchorNone"),
    m_anchorOffset(0),
    m_hidesContent(false),
    m_isHiding(false),
    m_jsObject(new ChromeSnippetJSObject(jsParent, *this, docElementId))
{
	  // Prevent hover events from passing through to the underlying widget.
    setAcceptHoverEvents(true);
}

ChromeSnippet::~ChromeSnippet()
{
  //qDebug() << "ChromeSnippet::~ChromeSnippet: " << m_docElementId;
  if(m_visibilityAnimator)
    delete m_visibilityAnimator;
  if(m_attentionAnimator)
    delete m_attentionAnimator;
  if(m_jsObject)
    delete m_jsObject;
}

void ChromeSnippet::setOwnerArea(const QRectF& ownerArea)
{
  m_ownerArea = ownerArea; 
}

QRectF& ChromeSnippet::ownerArea(){
  return m_ownerArea;
}

void ChromeSnippet::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  Q_UNUSED(option)
  Q_UNUSED(widget)

  //qDebug() << "ChromeSnippet::paint: " << m_docElementId << " " << m_ownerArea;
  //If this snippet has no children, draw the snippet. Otherwise let the children draw themselves.
  // NB: Make this behavior configurable?
  if(childItems().isEmpty()){
#ifdef Q_OS_SYMBIAN
  painter->drawPixmap(QPoint(0, 0), *m_owner->buffer(), m_ownerArea);
#else
  painter->drawImage(QPoint(0, 0), *m_owner->buffer(), m_ownerArea);
#endif
  }
}

void ChromeSnippet::updateChildGeometries()
{
  //NB: layouts should be usable here, but the behavior
  //is weird. Investigage further.
  /* delete m_layout;
  m_layout = new QGraphicsLinearLayout();
  QList<QGraphicsItem*> children = childItems();
  for(int i = 0; i < children.size(); i++){
    ChromeSnippet * child = dynamic_cast<ChromeSnippet*>(children[i]);
    m_layout->addItem(child);
  }
  setLayout(m_layout);
  */
  // For now just lay out in a row according to the html layout
  
        
  QList<QGraphicsItem*> children = childItems();
  for(int i = 0; i < children.size(); i++){
    ChromeSnippet * child = dynamic_cast<ChromeSnippet*>(children[i]);  
  	child->setPos(child->ownerArea().x(), 0);
  }
  
  if((docElementId() == "ContextMenuBgId") || (docElementId() == "ContextMenuId")) // hide context menu
  {
  	hide();  			
  }
  
  	  
}
bool ChromeSnippet::getContextMenuFlag()
{
	return contextMenuFlag;
}

void ChromeSnippet::mousePressEvent(QGraphicsSceneMouseEvent * ev)
{
  // qDebug() << "ChromeSnippet::mousePressEvent: " << scene()->focusItem();
  // qDebug() << "ChromeSnippet::mousePressEvent: " << docElementId() << m_ownerArea << ev->pos();
  
  
  QRect qr = m_owner->getDocElementRect("contextMenuButton");  
  ChromeSnippet *cmsn = m_owner->getSnippet("ContextMenuId");	
   
  qreal qx = ev->pos().x();
  qreal qy = ev->pos().y();
  bool cmVisibility = cmsn->isVisible();

  if(!cmVisibility)
  	contextMenuFlag = false;
  else  if(qx >= qr.left() && qx <= qr.right() && (docElementId() == "WebViewToolbarId"))
  {
  		contextMenuFlag = true;  		
  		return;
  }
  
  
  contextMenuFlag = false;
  
  passMouseEventToPage(QEvent::MouseButtonPress, ev);
  m_owner->chromePage()->view()->setFocus();
  
  if(docElementId() == "ContextMenuBgId")
  {
  	ChromeSnippet *sn = m_owner->getSnippet("ContextMenuId");	
  	if(sn)
  		sn->hide();
  	hide();
  }
  else if(docElementId() != "ContextMenuId")
  {
  	ChromeSnippet *sn = m_owner->getSnippet("ContextMenuId");	
  	if(sn)
  		sn->hide();
  	ChromeSnippet *sn2 = m_owner->getSnippet("ContextMenuBgId");	
  	if(sn2)
  		sn2->hide();
  	
  }
 
  ev->accept();

  //FIXME hide the Suggests snippet or not
  ChromeSnippet* ssn = m_owner->getSnippet("SuggestsId");
  if (ssn && ssn->isVisible()) {
      if (docElementId() == "SuggestsId") {
          if (m_ownerArea.height() < ev->pos().y())
              hide();
      }
      else
          ssn->hide();
  }
}

void ChromeSnippet::mouseReleaseEvent(QGraphicsSceneMouseEvent * ev)
{
	
  if(m_dragging) {
    emit m_jsObject->dragFinished();
    m_dragging = false;
  }
  passMouseEventToPage(QEvent::MouseButtonRelease, ev);
}


void ChromeSnippet::mouseMoveEvent(QGraphicsSceneMouseEvent * ev)
{
  if(m_draggable) {
      if(!m_dragging) {
          m_dragging = true;
          emit m_jsObject->dragStarted();
      }
      moveBy((ev->pos()-ev->lastPos()).x(),(ev->pos()-ev->lastPos()).y());
  }
  else {
      passMouseEventToPage(QEvent::MouseMove, ev);
  }
}


void ChromeSnippet::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * ev)
{
  passMouseEventToPage(QEvent::MouseButtonDblClick, ev);
  if(m_owner && m_owner->chromePage())
      m_owner->chromePage()->view()->setFocus();
}

void ChromeSnippet::passMouseEventToPage(QEvent::Type type, QGraphicsSceneMouseEvent *ev) {
  ev->accept();
  if(m_owner && m_owner->chromePage()) {
      // Translate the mouse press to the owner's coords and forward to owner page
      QMouseEvent shiftedEv( type, QPoint(int(m_ownerArea.x()), int(m_ownerArea.y()))+ev->pos().toPoint(),
                             ev->button(), ev->buttons(), ev->modifiers() );
    
      //qDebug() << "ChromeSnippet::passMouseEventToPage: " << shiftedEv.pos() << " " << &shiftedEv;
      m_owner->chromePage()->event(&shiftedEv);
  }
}

void ChromeSnippet::hoverMoveEvent(QGraphicsSceneHoverEvent *event ) {
  // Translate to a mouse move event.
  QMouseEvent shiftedEv( QEvent::MouseMove, QPoint(int(m_ownerArea.x()), int(m_ownerArea.y()))+event->pos().toPoint(),
                         Qt::NoButton, Qt::NoButton, Qt::NoModifier);
  if(m_owner && m_owner->chromePage())
        m_owner->chromePage()->event(&shiftedEv);
}

void ChromeSnippet::keyPressEvent ( QKeyEvent * event ) {
  event->accept();
  QApplication::sendEvent(m_owner->chromePage()->view(), event);
}

void ChromeSnippet::keyReleaseEvent ( QKeyEvent * event ) {
  event->accept();
  QApplication::sendEvent(m_owner->chromePage()->view(), event);
}

void ChromeSnippet::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) 
{
    event->accept();
}

void ChromeSnippet::setVisibilityAnimator(const QString& animatorName)
{
  setVisibilityAnimator(VisibilityAnimator::create(animatorName, this));
}

void ChromeSnippet::setAttentionAnimator(const QString& animatorName)
{
  setAttentionAnimator(AttentionAnimator::create(animatorName, this));
}

void ChromeSnippet::toggleVisibility()                    
{
  m_isHiding = (m_isHiding)?false:true;

  if(m_isHiding)
    emit m_jsObject->onHide();
  if(m_visibilityAnimator){
    emit m_jsObject->dragStarted();
    m_visibilityAnimator->toggleVisibility();
    emit m_jsObject->dragFinished();
  }
  else {
    (m_isHiding)?QGraphicsRectItem::hide():QGraphicsRectItem::show();
  }
  if(!m_isHiding)
      emit m_jsObject->onShow();
}

void ChromeSnippet::show(bool useAnimation){
  if(m_isHiding){
    if(useAnimation)
      toggleVisibility();
    else{
      QGraphicsRectItem::show();
      m_isHiding = false;
      emit m_jsObject->onShow();
    }
  }
}

void ChromeSnippet::hide(bool useAnimation){
  if(!m_isHiding){
    if(useAnimation)
      toggleVisibility();
    else{
      QGraphicsItem::hide();
      m_isHiding = true;
      emit m_jsObject->onHide();
    }
  }
}

void ChromeSnippet::toggleAttention()                    
{
  qDebug() << "ChromeSnippet::toggleAttention";
  if(m_attentionAnimator){
    m_attentionAnimator->toggleActive();
  }
}

QObject *ChromeSnippet::animate(int duration) {
  GraphicsItemAnimation *an = new GraphicsItemAnimation(this, duration);
  m_owner->chromePage()->mainFrame()->addToJavaScriptWindowObject("animation", an, QScriptEngine::ScriptOwnership);
  return an;
}

QObject *ChromeSnippet::getGeometry() const {
    ScriptRectF *r = new ScriptRectF(rect());
    m_owner->chromePage()->mainFrame()->addToJavaScriptWindowObject("rectf", r, QScriptEngine::ScriptOwnership);
    return r;
}

QObject *ChromeSnippet::getPosition() const {
    ScriptPoint *result = new ScriptPoint(pos().toPoint());
    m_owner->chromePage()->mainFrame()->addToJavaScriptWindowObject("point", result, QScriptEngine::ScriptOwnership);
    return result;
}

void ChromeSnippet::resize(qreal width, qreal height) {
    QRectF r = rect();
    r.setWidth(width);
    r.setHeight(height);
    setRect(r);
}

void ChromeSnippet::repaint() {
    //qDebug() << __func__ << "Area: " <<  m_ownerArea  << "Id " <<  m_docElementId;  
  m_owner->repaintRequested(m_ownerArea.toRect());
}

void ChromeSnippet::dump() const{
  qDebug() << "ChromeSnippet::dump: id=" << m_docElementId << " name=" << m_docElementName;
  qDebug() << "                     this=" << this;
  qDebug() << "                     pos()=" << pos();
  qDebug() << "                     rect()=" << rect();
  qDebug() << "                     m_ownerArea=" << m_ownerArea;
  qDebug() << "                     visible=" << isVisible();
  qDebug() << "                     zValue=" << zValue();
  if(parentObject())
      qDebug() << "                     parent=" << parentObject();
  if(parentWidget())
      qDebug() << "                     parentItem=" << parentWidget();
}

QObject *ChromeSnippet::getJSObject() { 
    return static_cast<QObject*>(m_jsObject); 
}

QString  ChromeSnippet::getDisplayMode()
{
	return m_owner->getDisplayMode();
	
	
}
/*
bool ChromeSnippet::sceneEventFilter ( QGraphicsItem * watched, QEvent * event ) {
    qDebug() << "ChromeSnippet::sceneEventFilter: " << event;
    return QGraphicsItem::sceneEventFilter(watched, event);
}
*/
