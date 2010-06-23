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
* This class extends WebChromeContainerSnippet class to hold the
* content view toolbar buttons
* Note: Showing/Hiding of the sub-chrome snippets in response to
* the button selection is not entirely done here as we have a
* a hybrid html-native design.
* MostVisited: Show/Hide is handled in handleMVButton.
* Menu: Show is handled in Javascript. Hide is handled here.
* Zoom: Show/Hide is handled here
* Note: Most Visited is native while Menu and Zoom are html based. All three of them need to 
* be dismissed if the user taps anywhere else on the screen. Since mv is native, it handles
* all mouse events and dismisses itself. Zoom/Menu are created as PopupChromeItem which receives
* the mouse events in its event filter and emits a signal if the user taps anywhere on the screen
* other than itself. This signal is handled here.
*
*/

#include "ContentToolbarSnippet.h"
#include "ContentToolbarChromeItem.h"
#include "mostvisitedsnippet.h"
#include "Toolbar.h"
#include "ViewStack.h"
#include "webpagecontroller.h"

#include <QDebug>


namespace GVA {


  ContentToolbarSnippet::ContentToolbarSnippet(const QString& elementId, ChromeWidget * chrome, const QRectF& ownerArea, const QWebElement & element, QGraphicsWidget * widget)
                       : ToolbarSnippet(elementId, chrome, ownerArea, element, widget),
                       m_middleSnippet(NULL),
                       m_subChromeSnippet(NULL)
  {

      m_type = TOOLBAR_WEB_VIEW;
      m_subChromeInactiveTimer = new QTimer(this);
      connect(m_subChromeInactiveTimer, SIGNAL(timeout()), this, SLOT(onInactivityTimer()));

  }

  ContentToolbarSnippet::~ContentToolbarSnippet()
  {
      delete m_subChromeInactiveTimer;
  }

  void ContentToolbarSnippet::updateSize(QSize size) {
      //qDebug() << "ContentToolbarSnippet::updateSize" << size.width() ;
      setWidth(size.width());
      ToolbarSnippet::updateSize(size);

  }

  void ContentToolbarSnippet::updateOwnerArea() {


      //qDebug() << "ContentToolbarSnippet::updateOwnerArea" << m_chrome->width() ;
      setWidth(m_chrome->width());
      ToolbarSnippet::updateOwnerArea();

      //qDebug()  << "------------Relayout "<< elementId() << hidesContent();
      // If hidesContent is true, it means that the snippet is tied to the chrome's layout. Hence, we
      // should invalidate and activate the layout here so that the layout and recalculate all
      // edges (we don't want to wait for the asynchronous layout request to be handled as
      // that would cause the this snippet to be painted in incorrect position before the layoutRequest
      // is handled
      if (hidesContent() ) {
          chrome()->layout()->invalidate();
          chrome()->layout()->activate();
      }
  }

  void ContentToolbarSnippet::setWidth(int width ){

      QGraphicsLinearLayout* l = layout();

      // We are using the width of the first element as the first and last elements are the same width
      if (l && l->itemAt(0)) {

          qreal snippetWidth =  width -(2 * l->itemAt(0)->geometry().width());
          m_middleSnippet->setLayoutWidth(snippetWidth, true);

      }

  }

  void ContentToolbarSnippet::childAdded(ChromeSnippet* child) {

      addActionInfo(child->elementId());
  }

  void ContentToolbarSnippet::addActionInfo(QString id) {

      ToolbarActions_t* t = new ToolbarActions_t();
      if (id == "BackButtonSnippet" ) {
          t->actionId = CONTENT_VIEW_ACTION_BACK;
          t->actionName = CONTENT_TOTOLBAR_BACK;
          t->activeImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_back.png";
          t->disabledImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_back_disabled.png";
          t->selectedImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_back_pressed.png";
      }
      else if (id  == "ZoomButtonSnippet" ) {
          t->actionId = CONTENT_VIEW_ACTION_ZOOM;
          t->actionName = CONTENT_TOTOLBAR_ZOOM;
          t->activeImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_zoom.png";
          t->disabledImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_zoom_disabled.png";
          t->selectedImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_zoom_pressed.png";
      }
      else if (id == "MenuButtonSnippet" ) {
          t->actionId = CONTENT_VIEW_ACTION_MENU;
          t->actionName = CONTENT_TOTOLBAR_MENU;
          t->activeImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_menu.png";
          t->disabledImg = "";
          t->selectedImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_menu_pressed.png";
      }
      else if (id == "MostVisitedButtonSnippet" ) {
          t->actionId = CONTENT_VIEW_ACTION_MOSTVISITED;
          t->actionName = CONTENT_TOTOLBAR_MOSTVISITED;
          t->activeImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_mostvisited.png";
          t->disabledImg = "";
          t->selectedImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_mostvisited_pressed.png";
      }
      else if (id == "ToggleTBButtonSnippet" ) {
          t->actionId = CONTENT_VIEW_ACTION_TOGGLETB;
          t->actionName = CONTENT_TOTOLBAR_TOGGLETB;
          t->activeImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_toggleTB.png";
          t->disabledImg = "";
          t->selectedImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_toggleTB_pressed.png";
      }
      t->id = id;
      m_actionInfo.append(t);
  }

  void ContentToolbarSnippet::addChild(ChromeSnippet* child) {

      WebChromeContainerSnippet * s =  dynamic_cast<WebChromeContainerSnippet* >(child);
      if (s) {
          connect(child, SIGNAL(childAdded(ChromeSnippet*)), this, SLOT(childAdded(ChromeSnippet*)));
          m_middleSnippet = s;
          // Set the snippet width
          setWidth(m_chrome->width());
      }
      else  {
          addActionInfo(child->elementId());
      }

      WebChromeContainerSnippet::addChild(child);

  }

  void ContentToolbarSnippet::onChromeComplete() {

      ChromeItem * item =  static_cast<ChromeItem* >(widget());
      manageChildren(item);

      // Connect back to initialLayoutCompleted signal
      ActionButtonSnippet * button  =  getActionButtonSnippet(CONTENT_VIEW_ACTION_BACK);
      connect(WebPageController::getSingleton(), SIGNAL(initialLayoutCompleted()), button->widget(), SLOT(onHidden()));

      connect(m_chrome, SIGNAL(aspectChanged(int)) , this, SLOT(onAspectChanged()));

  }

  void ContentToolbarSnippet::manageChildren(ChromeItem * it) {


      /*!
       * Content toolbar needs to listen to mouse events of each of the toolbar
       * buttons as well as the snippets that pop-up on selecting these
       * buttons. Some of these buttons are not direct children of content toolbar
       * but placed within another container. Also, the pop-ups are not directly
       * linked to content toolbar. Here, all these snippets are determined to connect to the
       * widget's mosueEvent signal
       */
      QList<QGraphicsItem *> items = it->childItems();
      ContentToolbarChromeItem * w = static_cast<ContentToolbarChromeItem*>(widget());
      //qDebug() << " ------------------- Num of children " << items.count();
      for (int i = 0; i < items.count() ; i++) {

          // Can't use qobject_cast since QGraphicsWdiget is not a QObject
          ChromeItem * item =  dynamic_cast<ChromeItem* >(items.at(i));
          if (item ) {

              WebChromeContainerSnippet * s =  dynamic_cast<WebChromeContainerSnippet* >(item->snippet());
              // If it is a container, need to connect to it's child items, so recurse
              if (s ) {
                manageChildren(item);
              }
              else { // Individual item
                  //qDebug() << "Item: " << item->snippet()->elementId();
                  if (w->autoHideToolbar()) {
                      connect(item, SIGNAL(mouseEvent(QEvent::Type)), w, SLOT(onSnippetMouseEvent(QEvent::Type)));
                  }
                  // Connect to the mouse event to show selected image on key press
                  //connect(item, SIGNAL(mouseEvent( QEvent::Type )), this, SLOT(onMouseEvent(QEvent::Type)));
                  setAction(item->snippet());


                  ChromeSnippet * link = item->snippet()->linkedSnippet();

                  if (link) {
                      ChromeItem* it = dynamic_cast <ChromeItem * > (link->widget());
                      if (it ) {
                          connect(it, SIGNAL(mouseEvent(QEvent::Type)), this, SLOT(onSnippetMouseEvent(QEvent::Type)));
                          // Save the linked children to be used later for hiding
                          w->addLinkedChild(link);

                          // In case it has more children, recurse
                          manageChildren(it);
                    }
                }
            }
        }
    }

  }


  void ContentToolbarSnippet::setAction(ChromeSnippet * s) {

      //qDebug() << "setAction: " << s->elementId();
      ToolbarSnippet::setAction(s);
      ActionButtonSnippet * button  = static_cast<ActionButtonSnippet*> (s);
      int index = getIndex(s);

      if (index != -1) {
          QAction * action =  button->getDefaultAction();
          ToolbarActions_t * t = m_actionInfo.at(index);

          switch (t->actionId) {

              case CONTENT_VIEW_ACTION_ZOOM:
                  addZoomButton(action, button);
                  break;
              case CONTENT_VIEW_ACTION_MENU:
                  addMenuButton(action, button);
                  break;
              case CONTENT_VIEW_ACTION_MOSTVISITED:
                  addMostVisitedButton(action, button);
                  break;
              case CONTENT_VIEW_ACTION_TOGGLETB:
                  addToggleTBButton(action, button);
                  break;
              default:
                  break;
          }
      }

  }

  void ContentToolbarSnippet::addZoomButton(QAction * action, ActionButtonSnippet* button) {

      assert(action);
      assert(button);
      connect(action,  SIGNAL(triggered()), this, SLOT(handleZoomButton()));


      // Connect to hide and show signals of the linked snippet
      connectHideShowSignals(button);
      connect( button->linkedSnippet(),  SIGNAL(externalMouseEvent(int, const QString , const QString )), this, SLOT(onExternalMouse(int, const QString , const QString)));
     
  }

  void ContentToolbarSnippet::addMenuButton(QAction * action, ActionButtonSnippet* button) {

      assert(action);
      assert(button);

      connect(action,  SIGNAL(triggered()), this, SLOT(handleMenuButton()));

      // Connect to hide and show signals of the linked snippet
      connectHideShowSignals(button);
      connect( button->linkedSnippet(),  SIGNAL(externalMouseEvent(int, const QString , const QString )), this, SLOT(onExternalMouse(int, const QString , const QString)));
      
  }

  void ContentToolbarSnippet::addMostVisitedButton(QAction * action, ActionButtonSnippet* button) {

      assert(action);
      assert(button);

      connect(action,  SIGNAL(triggered()), this, SLOT(handleMVButton()));

      // Connect to hide and show signals of the linked snippet
      connectHideShowSignals(button);
  }

  void ContentToolbarSnippet::addToggleTBButton(QAction * action, ActionButtonSnippet* button) {

      assert(action);
      assert(button);

      connect(action,  SIGNAL(triggered()), this, SLOT(handleToggleTBButton()));
  }


  void ContentToolbarSnippet::handleZoomButton() {

      ActionButtonSnippet * button  = getActionButtonSnippet(CONTENT_VIEW_ACTION_ZOOM);
      ChromeSnippet * zoomSnippet = button->linkedSnippet();

      //qDebug() << "ContentToolbarSnippet::handleZoomButton() : Show flag: "  << zoomSnippet->getDontShowFlag() << "Checked " << button->isChecked();

      // Zoom bar is dismissed if the user taps anywhere on the screen. We get externalMouseEvent 
      // if the user presses the button also. In addition to that the action is triggered and the
      // handler is called. In this case, we don't need to do anything, so we use the DontShowFlag
      // to determine if there is any action needed.
      if (zoomSnippet->getDontShowFlag() ){
          // Set selected state to false as it gets enabled in ActionButton mousePressEvent handling
          button->setLatched(false);
          zoomSnippet->setDontShowFlag(false);
      }
      else {
          hideOtherPopups(button->elementId());
          zoomSnippet->show();          
      }

  }

  void ContentToolbarSnippet::handleMenuButton() {

      ActionButtonSnippet * button  = getActionButtonSnippet(CONTENT_VIEW_ACTION_MENU);
      ChromeSnippet * menuSnippet = button->linkedSnippet();

      //qDebug() << "ContentToolbarSnippet::handleMenuButton() : Show flag: "  << button->linkedSnippet()->getDontShowFlag()  << "Checked: " << button->isChecked();
      if (menuSnippet  ) {
              //qDebug() << "ContentToolbarSnippet::handleMenuButton() : Dont show flag " << menuSnippet->getDontShowFlag();
          if (menuSnippet->getDontShowFlag() ){
              // Set selected state to false as it gets enabled in ActionButton mousePressEvent handling
              button->setLatched(false);
              menuSnippet->setDontShowFlag(false);
          }
          else {
              // Hide other pop-ups if any. Showing the menu is handled in
              // javascript
              emit menuButtonSelected();
              hideOtherPopups(button->elementId());
          }
      }

  }

  void ContentToolbarSnippet::handleMVButton() {

      //qDebug() << "ContentToolbarSnippet::handleMVButton() ";
      ActionButtonSnippet * button  = getActionButtonSnippet(CONTENT_VIEW_ACTION_MOSTVISITED);
      hideOtherPopups(button->elementId());
      button->linkedSnippet()->toggleVisibility();
  }

 void ContentToolbarSnippet::handleToggleTBButton() {
     ActionButtonSnippet * button  = getActionButtonSnippet(CONTENT_VIEW_ACTION_TOGGLETB);
     ContentToolbarChromeItem * w = static_cast<ContentToolbarChromeItem*>(widget());
     
     w->toggleMiddleSnippet();
     button->updateButtonState(false);
  }


  void ContentToolbarSnippet::hideOtherPopups(QString id) {

  
    for (int i = 0; i < m_actionInfo.size() ; i++ ) {

        ToolbarActions_t * t = m_actionInfo.at(i);
        if (t->id != id ) {
            ActionButtonSnippet * buttonSnippet  = static_cast<ActionButtonSnippet*> ( m_chrome->getSnippet(t->id));
            ChromeSnippet * linkedSnippet = buttonSnippet->linkedSnippet();

            // If there is another sub-chrome visible, hide it
            if (linkedSnippet ) {
                if ( linkedSnippet->isVisible()) {
                    linkedSnippet->hide();
                }
                else {
                 	  // When the user selects the menu button, menu is displayed after certain timeout
                    // and it is possible that the user can select another button. So cancel it if the 
                    // button is 'checked' if we get another action trigger
                    
                    //qDebug() << "ContentToolbarSnippet::hideOtherPopups: "  << id << "Button: " << t->id << buttonSnippet->isChecked();

                    if ( (buttonSnippet->elementId() == "MenuButtonSnippet" ) && (buttonSnippet->isChecked() )) {
                        emit menuButtonCanceled();
                        buttonSnippet->updateButtonState(false);
                    }
                }
            } 
        }

    }
  

  }

  void ContentToolbarSnippet::connectHideShowSignals(ActionButtonSnippet * button) {

      // Connect to hide and show signals of the linked snippet
      ChromeSnippet * snippet = button->linkedSnippet();
      ActionButton * actionButton = static_cast<ActionButton*>(button->widget());
      connect(snippet, SIGNAL(hidden()), actionButton, SLOT(onHidden()));
      connect(snippet, SIGNAL(shown()), actionButton, SLOT(onShown()));

      connect(snippet, SIGNAL(hidden()), this, SLOT(onHidden()));
      connect(snippet, SIGNAL(shown()), this, SLOT(onShown()));

  }

  void ContentToolbarSnippet::onAspectChanged( ) {

      // Hide any pop-up that is visible
      hideOtherPopups(QString());
  }

  void ContentToolbarSnippet::onSnippetMouseEvent( QEvent::Type type) {

      //qDebug() << "ContentToolbarSnippet::onSnippetMouseEvent : Type" << type;
      if (type == QEvent::GraphicsSceneMousePress || type == QEvent::MouseButtonPress ) {
          resetTimer(false);
      }
      else if (type == QEvent::GraphicsSceneMouseRelease  || type == QEvent::MouseButtonRelease ) {
          resetTimer();
      }
  }

  void ContentToolbarSnippet::onMouseEvent( QEvent::Type type) {

      ActionButton* button  = static_cast<ActionButton*> ( sender());
      ActionButtonSnippet * buttonSnippet =  static_cast<ActionButtonSnippet*>( button->snippet());
      ChromeSnippet * linkedSnippet = buttonSnippet->linkedSnippet();
      //qDebug() << "ContentToolbarSnippet::onMouseEvent : Button : " << buttonSnippet->elementId() << " type: " << type  << "Is Enabled " << button->defaultAction()->isEnabled();
      
      // Set the action button state to active so that we can show a selected image before the
      // action is acted upon
      if (button->defaultAction()->isEnabled() && type == QEvent::GraphicsSceneMousePress ) {

          // Set active flag if there is no linked snippet (the button acts as toggle if it
          // has a linked snippet. If there is linked snippet, set active flag if the linked snippet 
          // is not visible and dont show flag is false
          if (!linkedSnippet || (linkedSnippet && (!linkedSnippet->isVisible() &&  !linkedSnippet->getDontShowFlag()))){
              buttonSnippet->setActive(true);
          } 
      }
  }

  void ContentToolbarSnippet::onHidden() {
      
      ChromeSnippet * snippet = static_cast<ChromeSnippet *> (sender());
      //qDebug() << "ContentToolbarSnippet::onHidden" << snippet->elementId();
      if (snippet  == m_subChromeSnippet ) {

          //qDebug() << "ContentToolbarSnippet::onHidden  hide shown snippet" << snippet->elementId();
          resetTimer(false); 
          m_subChromeSnippet = NULL;
      }

  }

  void ContentToolbarSnippet::onShown() {

      // If one sub-chrome is being displayed when another is requested to be launched by the user, to avoid 
      //depending on the order of receiving hidden and shown signals for each of those, we save the snippet
      // being shown and then later when hidden signal is received, the sender is compared to the 
      // snippet for which we started the timer so that the timer is started and stopped for the
      // correct snippet. If 'shown' signal for the second is received before hidden for the second, the timer 
      // is stopped and restarted and the sub-chrome snippet is set to the new one. When 'hidden' is received
      // for the first, it is ignored as it doesn't match our saved snippet value.
      m_subChromeSnippet = static_cast<ChromeSnippet *> (sender());
      //qDebug() << "ContentToolbarSnippet::onShown" << m_subChromeSnippet->elementId();
       
      resetTimer();

  }


  void ContentToolbarSnippet::onInactivityTimer() {

      //qDebug() << "ContentToolbarSnippet::onInactivityTimer" << m_subChromeSnippet->elementId();
      if (m_subChromeSnippet->elementId() == "MostVisitedViewId" ) {
          MostVisitedSnippet * mv = static_cast<MostVisitedSnippet *>(m_subChromeSnippet);
          mv->close();
          // Stop the timer here as there is animation to complete before hide is received
          resetTimer(false);
          m_subChromeSnippet = NULL;
      }
      else {

          // Hide the snippet. onHidden slot will be called when the snippet is hidden
          // and there we will stop the timer and reset m_subChromeSnippet
          m_subChromeSnippet->hide();
      }
  }


  void ContentToolbarSnippet::resetTimer(bool start) {

      int duration =  TOOLBAR_POPUP_INACTIVITY_DURATION;
      if (m_subChromeInactiveTimer->isActive() ) {
          //qDebug() << " Stop timer for " << m_subChromeSnippet->elementId();
          m_subChromeInactiveTimer->stop();
      }

      if (start) {
          if (m_subChromeSnippet->elementId() == "ZoomBarId" ) {
              duration = TOOLBAR_ZOOMBAR_INACTIVITY_DURATION;
          }
          //qDebug() << " Start timer for " << m_subChromeSnippet->elementId();
          m_subChromeInactiveTimer->start(duration);
      }

  }
  void ContentToolbarSnippet::onExternalMouse( int type,
                                               const QString & name,
                                               const QString & description) {

      Q_UNUSED(type);
      Q_UNUSED(description);
      ChromeSnippet * snippet = static_cast<ChromeSnippet *> (sender());
      //qDebug() << "ContentToolbarSnippet::onExternalMouse"  << snippet->elementId() << name;
    
      if (name  == "QGraphicsSceneMousePressEvent" || name == "QGraphicsSceneResizeEvent" ) {
          snippet->hide();
          snippet->setDontShowFlag(true);
          QTimer::singleShot(500, snippet, SLOT(disableDontShowFlag()));
      }

      
  }


} // end of namespace GVA


