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


#include "ContentToolbarChromeItem.h"
#include "GWebContentView.h"
#include "mostvisitedsnippet.h"

#include <QTimeLine>
#include <QDebug>

#define TOOLBAR_MARGIN 4
#define TOOBAR_ANIMATION_DURATION 200
#define TOOBAR_INACTIVITY_DURATION 5000

namespace GVA {

  ToolbarFadeAnimator::ToolbarFadeAnimator(): m_timeLine(NULL) 
  {
  }


  ToolbarFadeAnimator::~ToolbarFadeAnimator() {

    if (m_timeLine)
      delete m_timeLine;
  }

  void ToolbarFadeAnimator::start(bool visible) {
    //qDebug() << __PRETTY_FUNCTION__ << visible;
    if (!m_timeLine) {
      m_timeLine = new QTimeLine(TOOBAR_ANIMATION_DURATION);
      connect(m_timeLine, SIGNAL(valueChanged(qreal)), this, SLOT(valueChange(qreal)));
      connect(m_timeLine, SIGNAL(finished()), this, SIGNAL(finished()));
    }
    else {
      m_timeLine->stop();
    }
    if (!visible) {
      m_timeLine->setDirection(QTimeLine::Forward);

    }
    else {
      m_timeLine->setDirection(QTimeLine::Backward);

    }
    m_timeLine->start();

  }

  void ToolbarFadeAnimator::stop() {

    //qDebug() << __PRETTY_FUNCTION__ ;
    m_timeLine->stop();
  }

  void ToolbarFadeAnimator::valueChange(qreal step) {
    qreal value = step - 0.25;
    value =  (value > 0)? value: 0.0;
    emit  updateVisibility(value);
  }

  ContentToolbarChromeItem::ContentToolbarChromeItem(QGraphicsItem* parent)
      : ToolbarChromeItem(parent), 
      m_background(NULL),
      m_middleSnippet(NULL),
      m_bgopacity(0.75),
      m_state(CONTENT_TOOLBAR_STATE_FULL),
      m_inactiveTimerState(CONTENT_TOOLBAR_INACTIVITY_TIMER_NONE)
  {

    m_inactivityTimer = new QTimer(this);
    connect(m_inactivityTimer, SIGNAL(timeout()), this, SLOT(onInactivityTimer()));

    m_animator = new ToolbarFadeAnimator();
    connect(m_animator, SIGNAL(updateVisibility(qreal)), this, SLOT(onUpdateVisibility(qreal)));
    connect(m_animator, SIGNAL(finished()), this, SLOT(onAnimFinished()));

    setFlags(QGraphicsItem::ItemDoesntPropagateOpacityToChildren);
   
  }

  ContentToolbarChromeItem::~ContentToolbarChromeItem()
  {
    if (m_background ) 
        delete m_background;
    delete m_inactivityTimer;
    delete m_animator;

  }

  void ContentToolbarChromeItem::resizeEvent(QGraphicsSceneResizeEvent * ev)
  {
    //qDebug() << __PRETTY_FUNCTION__ << boundingRect();
    ToolbarChromeItem::resizeEvent(ev);
    addFullBackground();
 
  }

  void ContentToolbarChromeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget)
  {
    Q_UNUSED(opt)
    Q_UNUSED(widget)

//    qDebug() << __PRETTY_FUNCTION__ << m_state;
    painter->save();
 
    painter->setRenderHint(QPainter::Antialiasing);
 
    painter->setPen(pen());
    painter->setOpacity(m_bgopacity);
 
    switch (m_state) {
        case CONTENT_TOOLBAR_STATE_PARTIAL:
          ToolbarChromeItem::paint(painter, opt, widget);
          break;
        case CONTENT_TOOLBAR_STATE_ANIM_TO_FULL:
        case CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL:
          ToolbarChromeItem::paint(painter, opt, widget);
        case CONTENT_TOOLBAR_STATE_FULL:
          // fill path with color
          painter->fillPath(*m_background,QBrush(grad()));
          painter->drawPath(*m_background);
          break;
        default:
          qDebug() << "ContentToolbarChromeItem::paint invalid state" ;
          break;
    }
    // restore painter
    painter->restore(); 
 
  }


  void ContentToolbarChromeItem::setSnippet(WebChromeContainerSnippet* snippet) {

    //qDebug() << __func__ << snippet;
    ToolbarChromeItem::setSnippet(snippet);

    connect(snippet->chrome(), SIGNAL(chromeComplete()), this, SLOT(onChromeComplete()));
    connect(snippet, SIGNAL(snippetMouseEvent(QEvent::Type)), this, SLOT(onSnippetMouseEvent(QEvent::Type)));
    
  }

  void ContentToolbarChromeItem::onWebViewMouseEvents(QEvent::Type type) {

//    qDebug() << __PRETTY_FUNCTION__ << type;
    switch (type ) {
      case QEvent::GraphicsSceneMousePress:
      case QEvent::GraphicsSceneMouseDoubleClick:
        handleMousePress();
        break;
      case QEvent::GraphicsSceneMouseRelease:
        handleMouseRelease();
        break;
      default:
        break;

    }
   
  }

  void ContentToolbarChromeItem::onSnippetMouseEvent( QEvent::Type type) {

    
//    qDebug() << __PRETTY_FUNCTION__ << type;
    // Handle the snippet mouse events when we are in webview
    if ( getSnippet()->chrome()->currentView() == "webView") {

      if (type == QEvent::MouseButtonPress || type ==  QEvent::GraphicsSceneMousePress) {
  
        m_inactiveTimerState = CONTENT_TOOLBAR_INACTIVITY_TIMER_NONE;
        switch (m_state) {
          case CONTENT_TOOLBAR_STATE_PARTIAL:
            changeState(CONTENT_TOOLBAR_STATE_ANIM_TO_FULL);
            break;
          case CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL:
            // Stop animation and change to full immediately
            m_animator->stop();
            changeState(CONTENT_TOOLBAR_STATE_FULL, false);
            break;
          case CONTENT_TOOLBAR_STATE_ANIM_TO_FULL:
            // Do nothing here - will reset inactivity timer when
            // animation completes
            break;
          case CONTENT_TOOLBAR_STATE_FULL:
            resetInactivityTimer();
            break;
          default:
            break;
  
        }
      }
      else if ( type == QEvent::MouseButtonRelease || type ==  QEvent::GraphicsSceneMouseRelease){
        m_inactiveTimerState = CONTENT_TOOLBAR_INACTIVITY_TIMER_ALLOWED;
        if (m_state == CONTENT_TOOLBAR_STATE_FULL ){
          resetInactivityTimer();
        }
      }
    }
  }

  void ContentToolbarChromeItem::onChromeComplete() {

    GWebContentView* webView  = static_cast<GWebContentView*> (getSnippet()->chrome()->getView("WebView"));
    //qDebug() << __PRETTY_FUNCTION__ << webView;
    if(webView){
      connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
      connect(webView, SIGNAL(loadStarted()), this, SLOT(onLoadStarted()));
      connect(webView, SIGNAL(contentViewMouseEvent(QEvent::Type)) , this, SLOT(onWebViewMouseEvents(QEvent::Type)) );
    }
    // Get the middle snippet here for now
    m_middleSnippet = getSnippet()->chrome()->getSnippet("ButtonContainer");
    connect(m_middleSnippet, SIGNAL(snippetMouseEvent(QEvent::Type)), this, SLOT(onSnippetMouseEvent(QEvent::Type)));
    QList <ChromeSnippet *> links = getSnippet()->links();
    for (int i=0; i < links.count() ; i++) {

      connect(links.at(i), SIGNAL(snippetMouseEvent(QEvent::Type)), this, SLOT(onSnippetMouseEvent(QEvent::Type)));

    }

    // Connect to shown and hidden signals to reset the inactivity timer
    connect(getSnippet() , SIGNAL(shown()), this, SLOT(onSnippetHide()));
    connect(getSnippet() , SIGNAL(hidden()), this, SLOT(onSnippetShow()));
  }

  void ContentToolbarChromeItem::onSnippetShow() {

    // This is signal is received when the snippet is about to be hidden. So stop the timer
    //qDebug() << __PRETTY_FUNCTION__; 
    if (m_inactivityTimer->isActive() ) 
        m_inactivityTimer->stop();
  }

  void ContentToolbarChromeItem::onSnippetHide() {

    //qDebug() << __PRETTY_FUNCTION__; 
    changeState(CONTENT_TOOLBAR_STATE_FULL, false);
  }

  void ContentToolbarChromeItem::onMVCloseComplete() {

//    qDebug() << __PRETTY_FUNCTION__; 
    ChromeSnippet * mv = getSnippet()->chrome()->getSnippet("MostVisitedViewId");
    disconnect(mv, SIGNAL(mostVisitedSnippetCloseComplete()) , this, SLOT(onMVCloseComplete()));
    // MostVisitedSnippet animation complete, so let's do toolbar animation
    if (m_state == CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL) 
      changeState(CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL, true);
 
  }
  
  void ContentToolbarChromeItem::onLoadFinished(bool ok) {

    Q_UNUSED(ok);
    //qDebug() << __PRETTY_FUNCTION__ << m_state;
    
    // We should be in STATE_FULL when load completes as we would have 
    // changed to  that state on starting a load
    //assert(m_state == CONTENT_TOOLBAR_STATE_FULL)
    m_inactiveTimerState = CONTENT_TOOLBAR_INACTIVITY_TIMER_ALLOWED;
    resetInactivityTimer();

  }

  void ContentToolbarChromeItem::onLoadStarted() {

    //qDebug() << __PRETTY_FUNCTION__<< m_state; ;
    m_inactiveTimerState = CONTENT_TOOLBAR_INACTIVITY_TIMER_NONE;

    switch (m_state) {
      case CONTENT_TOOLBAR_STATE_PARTIAL:
        changeState(CONTENT_TOOLBAR_STATE_FULL, false);
        break;
      case CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL:
      case CONTENT_TOOLBAR_STATE_ANIM_TO_FULL:
        // Stop animation and change to full immediately
        m_animator->stop();
        changeState(CONTENT_TOOLBAR_STATE_FULL, false);
        break;
      case CONTENT_TOOLBAR_STATE_FULL:
        resetInactivityTimer();  
        break;
      default:
        break;

    }
    //qDebug() << __PRETTY_FUNCTION__<< m_state; ;

  }

  void ContentToolbarChromeItem::onInactivityTimer() {

    //assert(m_state == CONTENT_TOOLBAR_STATE_PARTIAL);
    //qDebug() << __PRETTY_FUNCTION__ << m_state;
    if (mvSnippetVisible()) {
      changeState(CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL);
      
    }
    else {
      changeState(CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL, true);
    }
  }

  void ContentToolbarChromeItem::onUpdateVisibility(qreal step) {

    qreal value = 0.76 - step;
    //qDebug() << __PRETTY_FUNCTION__ << step << value << m_bgopacity << opacity();
    if (m_bgopacity != step ) {
      m_bgopacity = step; 
      getSnippet()->setOpacity(value);
      m_middleSnippet->widget()->setOpacity(step);
      update();
    }
  }

  void ContentToolbarChromeItem::onAnimFinished() {

    //qDebug() << __PRETTY_FUNCTION__ << m_state; 
    switch (m_state) {
        case CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL:
          changeState(CONTENT_TOOLBAR_STATE_PARTIAL);
          break;
        case CONTENT_TOOLBAR_STATE_ANIM_TO_FULL:
          changeState(CONTENT_TOOLBAR_STATE_FULL, true);
          break;
        default:
          break;
  
    }
    //qDebug() << __PRETTY_FUNCTION__ << m_state; 
    
  }
  void ContentToolbarChromeItem::addFullBackground() {

    //qDebug() << __PRETTY_FUNCTION__ ;
    qreal roundness((boundingRect().height() -TOOLBAR_MARGIN)/2);
    QRectF r(1, 1, boundingRect().width()-TOOLBAR_MARGIN, boundingRect().height()-TOOLBAR_MARGIN);

    if (m_background ) {
        delete m_background;
        m_background = NULL;
    }
    m_background = new QPainterPath();
    m_background->addRoundedRect(r, roundness, roundness);
  }

  void ContentToolbarChromeItem::resetInactivityTimer() {

//    qDebug()  << __PRETTY_FUNCTION__ << m_inactiveTimerState;
    if (m_inactivityTimer->isActive() )
        m_inactivityTimer->stop();
    if (m_inactiveTimerState == CONTENT_TOOLBAR_INACTIVITY_TIMER_ALLOWED ) { 
      m_inactivityTimer->start(TOOBAR_INACTIVITY_DURATION);
    }
  }

  void ContentToolbarChromeItem::stateEnterFull(bool animate) {
    
    //qDebug() <<__PRETTY_FUNCTION__ ;
    
    resetInactivityTimer();
    // Show the middle snippet and reset the opacity if we are here directly with no aniamtion
    if (!animate) {
      m_bgopacity = 0.75; 
      m_middleSnippet->show();
    }

    m_state = CONTENT_TOOLBAR_STATE_FULL;
    m_middleSnippet->widget()->setOpacity(1.0);
    // TODO: specify the rect to be updated to avoid full repaint
    update();
  }

  void ContentToolbarChromeItem::stateEnterPartial(bool animate) {

    //qDebug() <<__PRETTY_FUNCTION__ ;
    Q_UNUSED(animate);

    // Explicity hide the linked snippets so that toggle button javascript gets the right
    // signals that it is expecting
    QList <ChromeSnippet *> links = getSnippet()->links();
    for (int i=0; i < links.count() ; i++) {
      //qDebug() << __PRETTY_FUNCTION__ << links.at(i)->elementId();
      links.at(i)->hide();
    }

    m_middleSnippet->hide();
    m_state = CONTENT_TOOLBAR_STATE_PARTIAL;
    
  }

  void ContentToolbarChromeItem::stateEnterAnimToFull(bool animate) {

    //qDebug() <<__PRETTY_FUNCTION__ ;
    Q_UNUSED(animate);
    m_state = CONTENT_TOOLBAR_STATE_ANIM_TO_FULL;
    m_inactivityTimer->stop();
    m_middleSnippet->show();
    m_animator->start(false);
    
  }

  void ContentToolbarChromeItem::stateEnterAnimToPartial(bool animate) {

    //qDebug() <<__PRETTY_FUNCTION__ << animate ;
    m_inactivityTimer->stop();
    m_state = CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL;
 
    if (animate ) {
      m_animator->start(true);
    }
    else {

      MostVisitedSnippet * mv = static_cast<MostVisitedSnippet *>(getSnippet()->chrome()->getSnippet("MostVisitedViewId")); 
      connect(mv, SIGNAL(mostVisitedSnippetCloseComplete()) , this, SLOT(onMVCloseComplete()));
      mv->close();
    }
    
  }

  void ContentToolbarChromeItem::changeState( ContentToolbarState state, bool animate){

    onStateEntry(state, animate);
  }

  void ContentToolbarChromeItem::onStateEntry(ContentToolbarState state, bool animate){

   //qDebug() << __PRETTY_FUNCTION__ ;
   switch (state) {
        case CONTENT_TOOLBAR_STATE_PARTIAL:
          stateEnterPartial(animate);
          break;
        case CONTENT_TOOLBAR_STATE_ANIM_TO_FULL:
          stateEnterAnimToFull(animate);
          break;
        case CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL:
          stateEnterAnimToPartial(animate);
          break;
        case CONTENT_TOOLBAR_STATE_FULL:
          stateEnterFull(animate);
          break;
        default:
          qDebug() << "ContentToolbarChromeItem::onStateEntry -  invalid state" ;
          break;
    }
  }

  void ContentToolbarChromeItem::onStateExit(ContentToolbarState state){

   //qDebug() << __PRETTY_FUNCTION__ ;
   switch (state) {
        case CONTENT_TOOLBAR_STATE_PARTIAL:
          break;
        case CONTENT_TOOLBAR_STATE_ANIM_TO_FULL:
          break;
        case CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL:
          break;
        case CONTENT_TOOLBAR_STATE_FULL:
          break;
        default:
          qDebug() << "ContentToolbarChromeItem::onStateExit - invalid state" ;
          break;
    }
  }

  void ContentToolbarChromeItem::handleMousePress() {

//    qDebug() << __PRETTY_FUNCTION__ << m_state;
    m_inactiveTimerState = CONTENT_TOOLBAR_INACTIVITY_TIMER_NONE;
    switch (m_state) {
      case CONTENT_TOOLBAR_STATE_PARTIAL:
        changeState(CONTENT_TOOLBAR_STATE_ANIM_TO_FULL);
        break;
      case CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL:
        // Stop animation and change to full immediately
        m_animator->stop();
        changeState(CONTENT_TOOLBAR_STATE_FULL, false);
        break;
      case CONTENT_TOOLBAR_STATE_ANIM_TO_FULL:
        // Do nothing here - will reset inactivity timer when
        // animation completes based on timer state then
        break;
      case CONTENT_TOOLBAR_STATE_FULL:
        resetInactivityTimer();
        break;
      default:
          qDebug() << "ContentToolbarChromeItem::handleMousePress invalid state" ;
        break;
 
    }

    
  }

  void ContentToolbarChromeItem::handleMouseRelease() {

//    qDebug() << __PRETTY_FUNCTION__ << m_state;
    /* If in STATE_FULL, restart inactivity timer. In other states:
     * STATE_PARTIAL, STATE_ANIM_TO_PARTIAL - not possible
     * STATE_ANIM_TO_FULL - timer will be restarted on animation completion
     */
    
    m_inactiveTimerState = CONTENT_TOOLBAR_INACTIVITY_TIMER_ALLOWED;
    if (m_state == CONTENT_TOOLBAR_STATE_FULL ){
      resetInactivityTimer();
    }
  }

  bool ContentToolbarChromeItem::mvSnippetVisible() {

    ChromeSnippet * mv = getSnippet()->chrome()->getSnippet("MostVisitedViewId"); 
    bool result = false;
   
    if (mv && mv->isVisible() ) {
      result = true;
    }
    return result;
  }

/*
  //Not using this method now - potential performance hit 
  void ContentToolbarChromeItem::changeState( ContentToolbarState state, bool animate){

    if ( m_states[m_state].exitFunc ) {
      
      (*this.*(m_states[m_state].exitFunc))();
    }
    if (  m_states[state].enterFunc ) {
      (*this.*(m_states[state].enterFunc))(animate);
      
    }

  }
 
  void ContentToolbarChromeItem::initStates() {


    m_states[CONTENT_TOOLBAR_STATE_FULL].enterFunc = &ContentToolbarChromeItem::stateEnterFull;
    m_states[CONTENT_TOOLBAR_STATE_FULL].exitFunc = NULL;

    m_states[CONTENT_TOOLBAR_STATE_PARTIAL].enterFunc = &ContentToolbarChromeItem::stateEnterpartial;
    m_states[CONTENT_TOOLBAR_STATE_PARTIAL].exitFunc = NULL;

    m_states[CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL].enterFunc = &ContentToolbarChromeItem::stateEnterAnimToPartial;
    m_states[CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL].exitFunc = NULL;

    m_states[CONTENT_TOOLBAR_STATE_ANIM_TO_FULL].enterFunc = &ContentToolbarChromeItem::stateEnterAnimToFull;
    m_states[CONTENT_TOOLBAR_STATE_ANIM_TO_FULL].exitFunc = NULL;
  }
*/
} // end of namespace GVA


