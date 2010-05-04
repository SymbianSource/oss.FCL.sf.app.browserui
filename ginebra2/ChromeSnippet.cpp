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


#include "ChromeSnippet.h"
#include "VisibilityAnimator.h"
#include "GraphicsItemAnimation.h"
#include "ScriptObjects.h"
#include "ChromeRenderer.h"
#include "WebChromeItem.h"
#include <QWebElementCollection>

#include <QtGui>

namespace GVA {

  ChromeSnippet::ChromeSnippet(const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element) 
    : m_elementId(elementId), 
      m_element(element),
      m_parentId(QString()),
      m_chrome(chrome),
      m_widget(widget),
      m_initiallyVisible(false), // NB: not needed?
      m_visible(m_initiallyVisible),
      m_hidesContent(false),
      m_anchor(anchorNone),
      m_anchorOffset(0),
      m_vAnimator(0),
      m_effect(0),
      m_hiding(0),
      m_dontshowFlag (false)
  {
    setObjectName(m_elementId); //This will be the name under which the snippet is visible in snippets js object
    if(m_widget)
      m_widget->hide();
    //When chrome is resized child snippets may need to be repositioned
    QObject::connect(m_chrome->renderer(), SIGNAL(chromeResized()), this, SLOT(positionChildren()));
    QObject::connect(m_chrome, SIGNAL(chromeComplete()), this, SLOT(onChromeComplete()));

    // widget->setFlags(QGraphicsItem::ItemIsFocusable);
  }

  ChromeSnippet::~ChromeSnippet()
  {
    //NB: Fix QObject parent relationships so we don't have to explicitly delete all these

    delete m_effect;
    // Snippet has taken ownership of m_widget, so delete it.
    // delete m_widget;
    delete m_vAnimator;
  }
 
  //This method slot is not actually hooked up in the base ChromeSnippet class.
  //Instead, specializations of this class can hook it up to their underlying
  //widget implementations as appropriate. Currently this hooked up in WebChromeSnippet
  //to the webkit context menu.

  void ChromeSnippet::onContextMenuEvent(QGraphicsSceneContextMenuEvent * ev)
  {
    qDebug() << "ChromeSnippet::contextMenuEvent: " << ev->pos();
    emit contextMenuEvent(ev->pos().x(), ev->pos().y());
  }
 

  //NB: Would be architecturally cleaner to do this translation in ChromeDOM
  //so snippets don't have to know anything about dom attribute string values

  void ChromeSnippet::setAnchor(const QString& anchor, bool update)
  {
  
    if(anchor == "AnchorTop")
      setAnchor(anchorTop);
    else if (anchor == "AnchorBottom")
      setAnchor(anchorBottom);
    else if (anchor == "AnchorCenter")
      setAnchor(anchorCenter);
    else if (anchor == "AnchorLeft")
      setAnchor(anchorLeft);
    else if (anchor == "AnchorRight")
      setAnchor(anchorRight);
    else if (anchor == "AnchorTopLeft")
      setAnchor(anchorTopLeft);
    else if (anchor == "AnchorTopRight")
      setAnchor(anchorTopRight);
    else if (anchor == "AnchorBottomLeft")
      setAnchor(anchorBottomLeft);
    else if (anchor == "AnchorBottomRight")
      setAnchor(anchorBottomRight);
    else
      setAnchor(anchorNone);
    if(update)
      m_chrome->anchorSnippet(this);
  }

  void ChromeSnippet::setAnchorOffset(int offset, bool update)
  {
    int delta = offset - m_anchorOffset;
    m_anchorOffset = offset;
    
    if(update){
      if(m_widget->isVisible())
	m_chrome->adjustAnchorOffset(this, delta);
      m_chrome->anchorSnippet(this);
    }
  }

  QString ChromeSnippet::getAnchorString() {
    switch(anchor()) {
    case anchorTop:
      return "AnchorTop";
    case anchorBottom:
      return "AnchorBottom";
    case anchorCenter:
      return "AnchorCenter";
    case anchorLeft:
      return "AnchorLeft";
    case anchorRight:
      return "AnchorRight";
    case anchorTopLeft:
      return "AnchorTopLeft";
    case anchorTopRight:
      return "AnchorTopRight";
    case anchorBottomLeft:
      return "AnchorBottomLeft";
    case anchorBottomRight:
      return "AnchorBottomRight";
    default:
      return QString::null;
    }
  }


  void ChromeSnippet::setVisible(bool visibility, bool animate)
  {
    if(m_visible == visibility)
      return;
    m_visible = visibility;
    if(m_visible) {

      //Visibility animators might leave the snippet 
      //in some visual state where it cannot be properly shown (e.g. transparent) 
      //on a previous hide. If no animation is wanted, we need to ask the 
      //animator to put the snippet back into a showable state before we show it.

      if(m_vAnimator && !animate)
	  m_vAnimator->setVisible(m_visible, animate);
      m_widget->show();
      if(m_vAnimator && animate)
      	m_vAnimator->setVisible(m_visible, animate);
      m_chrome->snippetShown(this); //NB: handle this via shown signal
      m_hiding = false;
      emit shown();
    }
    else{
      m_chrome->snippetHiding(this); //NB: handle this via hiding signal
      m_hiding = true;
      emit hidden();
      if(m_vAnimator)
	m_vAnimator->setVisible(m_visible, animate);
      else
	m_widget->hide();
    }
  }

  void ChromeSnippet::visibilityFinished(bool visible)
  {
    if(!visible)
      m_widget->hide();
  }

  void ChromeSnippet::toggleVisibility(bool animate)
  {
    setVisible(!m_visible, animate);
  }

  void ChromeSnippet::setOpacity(qreal opacity)
  {
    m_widget->setOpacity(opacity);
  }


  qreal ChromeSnippet::opacity()
  {
    return m_widget->opacity();
  }

  //NB: Move effects to a separate class factory

  void ChromeSnippet::setEffect(const QString & effect)
  {
    if(effect=="Shadow"){
      m_effect = new QGraphicsDropShadowEffect();
      static_cast<QGraphicsDropShadowEffect*>(m_effect)->setOffset(3.0,3.0);
      static_cast<QGraphicsDropShadowEffect*>(m_effect)->setBlurRadius(3.0);
    }
    else if (effect=="Blur"){
      m_effect = new QGraphicsBlurEffect();
      static_cast<QGraphicsBlurEffect*>(m_effect)->setBlurRadius(3.0);
    }
    else if (effect=="Glow"){
      m_effect = new QGraphicsDropShadowEffect();
      static_cast<QGraphicsDropShadowEffect*>(m_effect)->setOffset(1.0,1.0);
      static_cast<QGraphicsDropShadowEffect*>(m_effect)->setBlurRadius(5.0);
      static_cast<QGraphicsDropShadowEffect*>(m_effect)->setColor(Qt::green);
    }
    else 
      return;
    m_widget->setGraphicsEffect(m_effect);
  }

  void ChromeSnippet::enableEffect(bool enable)
  {
    if(m_effect)
      m_effect->setEnabled(enable);
  }

  void ChromeSnippet::toggleEffect()
  {
    if(m_effect)
      m_effect->setEnabled(!m_effect->isEnabled());
  }

  void ChromeSnippet::moveBy(int dx, int dy)
  {
    m_widget->moveBy(dx,dy);
  }  

  QObject *ChromeSnippet::getGeometry() const {
    
    ScriptRectF *r = new ScriptRectF(m_widget->mapToScene(m_widget->rect()).boundingRect());
    // m_owner->chromePage()->mainFrame()->addToJavaScriptWindowObject("rectf", r, QScriptEngine::ScriptOwnership);
    return r;
  }

  QObject *ChromeSnippet::childGeometry(const QString id) const {
    QWebElement child = m_element.findFirst("#" + id);
    if(child.isNull())
      return 0;
    QRect childGeo = child.geometry();
    QRect parentGeo = m_element.geometry();
    //Return geometry relative to parent
    return new ScriptRectF(QRect(childGeo.x()-parentGeo.x(), childGeo.y()-parentGeo.y(), childGeo.width(), childGeo.height()));
  }

  void ChromeSnippet::onChromeComplete() {

    positionChildren();
    //If this snippet is linked to another (through 'LinkedTo' attribute, add as a link to that snippet 
    QString linkedTo =  m_element.attribute("data-GinebraItemLinkedTo", "none" );
    if (linkedTo != "none") {

       ChromeSnippet * s = m_chrome->getSnippet(linkedTo);
       if (s) {
         s->addLink(this);
         WebChromeItem * item = static_cast<WebChromeItem*> (widget());
         connect(item, SIGNAL(mouseEvent(QEvent::Type)), this, SIGNAL(snippetMouseEvent(QEvent::Type)));

      }
    }

  }

  void ChromeSnippet::positionChildren()
  {
    QWebElementCollection children = m_element.findAll(".GinebraSnippet");
    foreach (QWebElement child, children) {
      ChromeSnippet * childSnippet = m_chrome->getSnippet(child.attribute("id"));
      QRect childGeo = child.geometry();
      QRect parentGeo = m_element.geometry();
      //Get child geometry relative to parent
      QRectF childGeom(childGeo.x()-parentGeo.x(), childGeo.y()-parentGeo.y(), childGeo.width(), childGeo.height());
      qDebug() << "Parent geometry: " << parentGeo << " child geometry: " << childGeo;   
      childSnippet->widget()->setGeometry(childGeom);
    }

  }

  void ChromeSnippet::addChild(ChromeSnippet * child)
  {
    qDebug() << "ChromeSnippet::addChild: " << child->elementId();
    QWebElement c = m_element.findFirst("#" + child->elementId());
    if(c.isNull())
      return;
    QRect childGeo = c.geometry();
    QRect parentGeo = m_element.geometry();
    //Get child geometry relative to parent
    QRectF childGeom(childGeo.x()-parentGeo.x(), childGeo.y()-parentGeo.y(), childGeo.width(), childGeo.height());   
    child->widget()->setGeometry(childGeom);
    child->widget()->setParentItem(widget());
  }

  QPointF ChromeSnippet::position() const {
    //qDebug() << "Relative: " << m_widget->pos() << " Absolute : " << m_widget->mapToScene(m_widget->pos());
    //return m_widget->mapToScene(m_widget->pos());
    return m_widget->pos();
  }

  QObject *ChromeSnippet::getPosition() const {
    ScriptPoint *result = new ScriptPoint(position().toPoint());
    //m_owner->chromePage()->mainFrame()->addToJavaScriptWindowObject("point", result, QScriptEngine::ScriptOwnership);
    return result;
  }

  void ChromeSnippet::setPosition(int x, int y)
  {
    //qDebug() << "Requested x: " << x << " y: " << y;
    //qDebug() << "Set position: " << m_widget->mapFromScene(QPointF(x,y));
    m_widget->setPos(QPointF(x,y));
  }  

  void ChromeSnippet::anchorToView(const QString& view, const QString& where)
  {
    m_chrome->anchorToView(this, view);
  }
  
  void ChromeSnippet::detachFromView(const QString& view, const QString& where) 
  {
    m_chrome->detachFromView(this, view);
  }

  void ChromeSnippet::anchorTo(const QString & id, int x, int y)
  {
    m_chrome->anchorTogether(this, id, x, y);
  }

  void ChromeSnippet::unAnchor()
  {
    setAnchor("AnchorNone");
    m_chrome->unAnchor(this);
  }
 
 QObject *ChromeSnippet::animate(int duration) {
    GraphicsItemAnimation *an = new GraphicsItemAnimation(this, duration);
    //m_owner->chromePage()->mainFrame()->addToJavaScriptWindowObject("animation", an, QScriptEngine::ScriptOwnership);
    return an;
  }

  void ChromeSnippet::setVisibilityAnimator(const QString& animator)
  {
    m_vAnimator = VisibilityAnimator::create(animator, this);
    connect(m_vAnimator, SIGNAL(finished(bool)), this, SLOT(visibilityFinished(bool)));
  }

  void ChromeSnippet::grabFocus()
  {
    qDebug() << "ChromeSnippet::grabFocus()";
    m_widget->setFocus();
  }

  int ChromeSnippet::zValue()
  {
    return m_widget->zValue();
  }
  
  void ChromeSnippet::setZValue(int z)
  {
    m_widget->setZValue(z);
  }

  void ChromeSnippet::setTransform(QTransform transform) 
  {
    m_widget->setTransform(transform); 
  }

  QTransform ChromeSnippet::transform()
  {
    return m_widget->transform();
  }

  void ChromeSnippet::addLink(ChromeSnippet * snippet) {

    m_links.append(snippet); 
  }

  void ChromeSnippet::dump() {
    qDebug() << "ChromeSnippet::dump:"
        << " id=" << elementId()
        << " visible=" << isVisible()
        << " zValue=" << zValue()
        << " geometry=" << m_widget->mapToScene(m_widget->rect()).boundingRect();
  }
}
