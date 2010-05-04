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


#include "GWebContentView.h"
#include "GWebContentViewWidget.h"
#include "GWebContentViewJSObject.h"
#include "ChromeWidget.h"
#include "WebViewEventContext.h"
#include "browserpagefactory.h"
#include "wrtbrowsercontainer.h"
#include "webpagecontroller.h"
#ifndef NO_QSTM_GESTURE
#include "qstmgestureevent.h"
#endif

#include <QGraphicsWebView>
#include <QWebHistory>
#include <QWebPage>
#include <QWebFrame>
#include <QTimeLine>
#include <QDebug>


#define safe_connect(src, srcSig, target, targetSlot) \
  { int res = connect(src, srcSig, target, targetSlot); assert(res); }

namespace GVA {


  // ----------------------------------------------------------

  const qreal KZoomInStep = 1.05;
  const qreal KZoomOutStep = 0.95238;
  
  GWebContentView::GWebContentView(ChromeWidget *chrome, QObject * parent, const QString &objectName)
    : ControllableViewBase(parent),
      m_networkMgr(0),
      m_chrome(chrome),
      m_timeLine(0),
      m_zoomIn(false),
      m_backEnabled(false),
      m_forwardEnabled(false),
      m_currentSuperPage(m_superPages.begin()),
      m_currentPageIsSuperPage(false),
      m_timer(NULL)
  {
      setObjectName(objectName);
      qDebug() << "GWebContentView::GWebContentView: this=" << this;
      QWebPage* page = BrowserPageFactory::openBrowserPage();

      setZoomActions();
      m_widget = new GWebContentViewWidget(parent, this, page);
#ifndef __gva_no_chrome__
      m_jsObject = new GWebContentViewJSObject(this, m_chrome->page()->mainFrame(), objectName);
#endif
      m_networkMgr = webWidget()->page()->networkAccessManager();

      webWidget()->page()->currentFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
      webWidget()->page()->currentFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

#ifdef NO_QSTM_GESTURE
      m_touchNavigation = new GWebTouchNavigation(page, m_widget);
      m_touchNavigation->setChromeWidget(m_chrome);
      connect(m_touchNavigation, SIGNAL(mouseEvent(QEvent::Type)) , this, SIGNAL(contentViewMouseEvent(QEvent::Type)));
#endif
      WRT::WrtBrowserContainer * wrtPage = static_cast<WRT::WrtBrowserContainer *>(page);
      if(wrtPage) {
          wrtPage->setWebWidget(webWidget());
          pageCreated(wrtPage);
      }

      connectAll();
  }

  GWebContentView::~GWebContentView() {
      disconnect(m_jsObject);
      disconnect(webWidget());
      delete m_touchNavigation;
      delete m_actionZoomIn;
      delete m_actionZoomOut;
      delete m_widget;
      foreach(GWebPage *page, m_superPages) {
          delete page;
      }
  }

void GWebContentView::bitmapZoomStop()
{
    if (m_timer) {
        m_timer->stop();
        disconnect(m_timer,SIGNAL(timeout()));
        delete m_timer;
        m_timer = NULL;	
    }
    qreal zoomFactor = m_value * webWidget()->zoomFactor();
    ((zoomFactor+0.001) >= webWidget()->maximumScale() )? webWidget()->setZoomFactor(webWidget()->maximumScale()):webWidget()->setZoomFactor(zoomFactor);
    webWidget()->bitmapZoomCleanup();
}

void GWebContentView::zoomP()
{
    if ((m_value * webWidget()->zoomFactor()) >  webWidget()->maximumScale()) {
        if (m_timer && m_timer->isActive())
            bitmapZoomStop();
    }else {
        if (m_timer->isSingleShot()) {
            m_timer->setSingleShot(false);
            m_timer->start(1);
        }
        webWidget()->setBitmapZoom(m_value * webWidget()->zoomFactor());
        m_value *= KZoomInStep;
    }
}

void GWebContentView::zoomN()
{
    if ((m_value * webWidget()->zoomFactor()) <  webWidget()->minimumScale()){
        if (m_timer && m_timer->isActive())
            bitmapZoomStop();	
    }else {
        if (m_timer->isSingleShot()) {
            m_timer->setSingleShot(false);
            m_timer->start(1);
        }
        webWidget()->setBitmapZoom(m_value * webWidget()->zoomFactor());
        m_value *= KZoomOutStep;
    }
}

void GWebContentView::zoomIn(qreal deltaPercent)	
{
    Q_UNUSED(deltaPercent)
    if (webWidget() && webWidget()->isUserScalable()) {
        if (m_timer && m_timer->isActive()) {
            if (!m_timer->isSingleShot())
                m_value /= KZoomInStep;
            bitmapZoomStop();
	        return;
        }else if (!m_timer)
            m_timer = new QTimer(this);
  
        m_value = KZoomInStep;
	
        if ((m_value * webWidget()->zoomFactor()) <  webWidget()->maximumScale()) {
            webWidget()->createPageSnapShot();
            connect(m_timer,SIGNAL(timeout()),this,SLOT(zoomP()));
            zoomP();
            m_timer->setSingleShot(true);
            m_timer->start(500);
        }else {
            delete m_timer;
            m_timer = NULL;
            webWidget()->setZoomFactor(m_value * webWidget()->zoomFactor());
        }
    }   
}

void GWebContentView::zoomOut(qreal deltaPercent)	
{
    Q_UNUSED(deltaPercent)
    if (webWidget() && webWidget()->isUserScalable()) {
        if (m_timer && m_timer->isActive()) {
            if (!m_timer->isSingleShot())
                m_value /= KZoomOutStep;
            bitmapZoomStop();
            return;
        }else if (!m_timer)
            m_timer = new QTimer(this);
 
        m_value = KZoomOutStep;

        if ((m_value * webWidget()->zoomFactor()) >  webWidget()->minimumScale()) {
            webWidget()->createPageSnapShot();
            connect(m_timer,SIGNAL(timeout()),this,SLOT(zoomN()));
            zoomN();
            m_timer->setSingleShot(true);
            m_timer->start(500);
        }else {
            delete m_timer;
            m_timer = NULL;
            webWidget()->setZoomFactor(m_value * webWidget()->zoomFactor());
        }
    }
}

  void GWebContentView::connectAll() {
    //qDebug() << "GWebContentView::connectAll: " << widget();

#ifndef __gva_no_chrome__
    safe_connect(widget(), SIGNAL(contextEvent(::WebViewEventContext *)),
                 m_jsObject, SLOT(onContextEvent(::WebViewEventContext *)));
    QObject::connect(webWidget(), SIGNAL(titleChanged(const QString &)), m_jsObject, SIGNAL(titleChanged(const QString &)));
    QObject::connect(webWidget(), SIGNAL(loadStarted()), m_jsObject, SIGNAL(loadStarted()));
    QObject::connect(webWidget(), SIGNAL(loadProgress(int)), m_jsObject, SIGNAL(loadProgress(int)));
#endif
    QObject::connect(webWidget(), SIGNAL(loadStarted()), this, SLOT(onLoadStarted()));
    QObject::connect(webWidget(), SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
    QObject::connect(webWidget(), SIGNAL(urlChanged(const QUrl&)), this, SLOT(onUrlChanged(const QUrl&)));
#ifndef __gva_no_chrome__
    QObject::connect(this, SIGNAL(urlChanged(const QString&)), m_jsObject, SIGNAL(urlChanged(const QString&)));
    QObject::connect(this, SIGNAL(backEnabled(bool)), m_jsObject, SIGNAL(backEnabled(bool)));
    QObject::connect(this, SIGNAL(forwardEnabled(bool)), m_jsObject, SIGNAL(forwardEnabled(bool)));
    QObject::connect(this, SIGNAL(loadFinished(bool)), m_jsObject, SIGNAL(loadFinished(bool)));
    QObject::connect(this, SIGNAL(secureConnection(bool)), m_jsObject, SIGNAL(secureConnection(bool)));
    connect(m_touchNavigation, SIGNAL(startingPanGesture(int)), m_jsObject, SIGNAL(startingPanGesture(int)));
#endif
    connect(WebPageController::getSingleton(), SIGNAL(pageCreated(WRT::WrtBrowserContainer*)),
            this, SLOT(pageCreated(WRT::WrtBrowserContainer*)));
    connect(WebPageController::getSingleton(), SIGNAL(pageChanged(WRT::WrtBrowserContainer*, WRT::WrtBrowserContainer*)),
            this, SLOT(pageChanged(WRT::WrtBrowserContainer*, WRT::WrtBrowserContainer*)));    
  }

  QVariant GWebContentView::getContentWindowObject() {
    try {
        return webWidget()->page()->mainFrame()->evaluateJavaScript("window");
    }
    catch(...) {
        qDebug() << "GWebContentView::getContentWindowObject: caught expection";
        return QVariant();
    }
  }

  QGraphicsWidget* GWebContentView::widget() const {
    return static_cast<QGraphicsWidget*>(m_widget);
  }

  GWebContentViewWidget *GWebContentView::webWidget() const {
    return m_widget;
  }

  QString GWebContentView::title() const {
    return webWidget()->title();
  }

  QUrl GWebContentView::url() {
    return webWidget()->url();
  }

  QWebPage* GWebContentView::wrtPage() {
    return m_widget->wrtPage();
  }

  void GWebContentView::setZoomActions(){

      // Create zoomIn and zoomOut actions */
      m_actionZoomIn = new QAction("zoomIn", this);
      m_actionZoomIn->setObjectName("zoomIn");

      m_actionZoomOut = new QAction("zoomOut", this);
      m_actionZoomOut->setObjectName("zoomOut");
      /* Disable zoomOut action initially as we are the the minimum scale */
      /* Revisit this to determine whether we can use the change signal to
       * set the zoomOut button image initially as well
       */
      m_actionZoomOut->setEnabled(false);

      connect(m_actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
      connect(m_actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
  }

  void GWebContentView::triggerAction(const QString & action)
  {
    qDebug() << "GWebContentView::triggerAction: " << action;
    QWebPage::WebAction webAction;
    if(action=="Stop")
      webAction = QWebPage::Stop;
    else if (action=="Back")
      webAction = QWebPage::Back;
    else if(action=="Reload")
      webAction = QWebPage::Reload;
    else if (action=="Forward")
      webAction = QWebPage::Forward;
    else
      return;
    webWidget()->triggerPageAction(webAction);
  }

  void GWebContentView::deactivateZoomActions()
  {
    m_actionZoomOut->setEnabled(false);
    m_actionZoomIn->setEnabled(false);
  }
 
  void GWebContentView::changeZoomAction(qreal zoom){
    if(!webWidget()) return;
    if(!(webWidget()->isUserScalable() ) ){
        deactivateZoomActions();
    }
    else {
      if (zoom <= webWidget()->minimumScale() ) {
         m_actionZoomOut->setEnabled(false);
      }
      else {
         m_actionZoomOut->setEnabled(true);
      }

      if (zoom >= webWidget()->maximumScale()  ){
         m_actionZoomIn->setEnabled(false);
      }
      else {
         m_actionZoomIn->setEnabled(true);
      }
    }
  }

  void GWebContentView::setZoomFactor(qreal factor){
    if(webWidget())
        webWidget()->setZoomFactor(factor);
  }

  qreal GWebContentView::getZoomFactor() const {
    return webWidgetConst() ? webWidgetConst()->zoomFactor() : 0.0;
  }

  void GWebContentView::activate() {
      qDebug() << "GWebContentView::activate";
      ControllableViewBase::activate();
  }

  void GWebContentView::deactivate() {
      qDebug() << "GWebContentView::deactivate";
      ControllableViewBase::deactivate();
  }

//  static void appendAction(QWebPage* page, QList<QAction*> &list, enum QWebPage::WebAction webAction, const QString &name) {
//      QAction *action = page->action(webAction);
//      if(action) {
//          action->setObjectName(name);
//          list.append(action);
//      }
//  }

  /*!
    Return the list of public QActions most relevant to the view's current context.
    @return  List of public actions
  */
  QList<QAction *> GWebContentView::getContext()
  {
      // Get some of the actions from the page (there are many more available) and build a list
      // list of them.

      QList<QAction*> actions;

      /* Add zoomIn and zoomOut actions created earlier*/
      if(m_actionZoomIn) actions.append(m_actionZoomIn);
      if(m_actionZoomOut) actions.append(m_actionZoomOut);

      return actions;
  }


  QAction * GWebContentView::getAction(const QString & action)
  {
    if(action == "zoomIn")
      return m_actionZoomIn;
    if(action == "zoomOut")
      return m_actionZoomOut;
    QWebPage::WebAction webAction;
    if(action == "Back")
      webAction = QWebPage::Back;
    else if(action == "Forward")
      webAction = QWebPage::Forward;
    else if(action == "Stop")
      webAction = QWebPage::Stop;
    else if(action == "Reload")
      webAction = QWebPage::Reload;
    else return 0;
    return webWidget()->pageAction(webAction);
  }

  void GWebContentView::scrollViewBy(int dx, int dy)
  {
      wrtPage()->mainFrame()->scroll(dx, dy);
  }

  void GWebContentView::scrollViewTo(int x, int y)
  {
      wrtPage()->mainFrame()->setScrollPosition(QPoint(x, y));
  }


  void GWebContentView::showMessageBox(WRT::MessageBoxProxy* proxy)
  {
  /*
      QMessageBox msgBox(this);
      msgBox.setText(proxy->m_text);
      msgBox.setInformativeText(proxy->m_informativeText);
      msgBox.setDetailedText(proxy->m_detailedText);
      msgBox.setStandardButtons(proxy->m_buttons);
      msgBox.setDefaultButton(proxy->m_defaultButton);
      msgBox.setIcon(proxy->m_icon);
      int ret = msgBox.exec();
      */
      QString displayText = proxy->m_text + QLatin1String("\n") + QLatin1String("\n")+ proxy->m_detailedText + QLatin1String("\n") + QLatin1String("\n") + proxy->m_informativeText;
      int ret = QMessageBox::warning(0/* TODO: find appropriate widget if required or just remove this widget()*/,
                                     proxy->m_text, displayText, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
      proxy->onMessageBoxResponse(ret);
  }

  void GWebContentView::loadUrlToCurrentPage(const QString & url)
  {
      WRT::WrtBrowserContainer * activePage = WebPageController::getSingleton()->currentPage();

      if(activePage) {
        activePage->mainFrame()->load(url);
      }
  }

  QWebPage *GWebContentView::currentPage() {
      WRT::WrtBrowserContainer * activePage = WebPageController::getSingleton()->currentPage();
      return activePage;
  }

  int GWebContentView::contentWidth()
  {
    return currentPage()->mainFrame()->contentsSize().width();
  }

  int GWebContentView::contentHeight()
  {
    return currentPage()->mainFrame()->contentsSize().height();
  }

  void GWebContentView::stop()
  {
    webWidget()->stop();
  }

  void GWebContentView::back()
  {
    qDebug() << "GWebContentView::back";
    webWidget()->back();
  }

  void GWebContentView::forward()
  {
    webWidget()->forward();
  }

  void GWebContentView::reload()
  {
    webWidget()->reload();
  }

  int GWebContentView::scrollX()
  {
    return currentPage()->mainFrame()->scrollPosition().x();
  }

  int GWebContentView::scrollY()
  {
    return currentPage()->mainFrame()->scrollPosition().y();
  }

  void GWebContentView::zoom(bool in)
  {
    m_zoomIn = in;
    if(!m_timeLine) {
      m_timeLine = new QTimeLine();
      connect(m_timeLine, SIGNAL(valueChanged(qreal)),
        this, SLOT(updateZoom(qreal)));
    }
    else {
      m_timeLine->stop();
    }
    m_timeLine->start();
  }

  void GWebContentView::toggleZoom(){
    zoom(!m_zoomIn);
  }
  void GWebContentView::stopZoom() {
    m_timeLine->stop();
  }

  void GWebContentView::updateZoom(qreal delta){
    Q_UNUSED(delta)
    if(m_zoomIn)
      zoomBy(0.1);
    else
      zoomBy(-0.1);
  }

  void GWebContentView::onUrlChanged(const QUrl& url)
  {
    emit urlChanged(url.toString());
    //NB: Brief hack
    if(url.scheme()=="https")
      emit secureConnection(true);
    else
      emit secureConnection(false);
  }

  GWebPage *GWebContentView::createSuperPage(const QString &name) {
    qDebug() << "GWebContentView::createSuperPage: " << name;
    GWebPage *page = 0;
    PageMap::iterator it = m_superPages.find(name);
    if(it == m_superPages.end()) {
      // Doesn't exist.  Create a new one.
      page = new GSuperWebPage(0, chrome());
      page->setParent(jsObject());
      page->setObjectName(name);

      // Add it to the superpage list.
      m_superPages[name] = page;
      if(m_superPages.count() == 1) {
        // This is the only superpage, make it current.
        m_currentSuperPage = m_superPages.find(name);
      }

      //NB: Here's how to set default webview backgound color
      page->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
      page->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
      QPalette viewPalette = widget()->palette();
      viewPalette.setBrush(QPalette::Base, Qt::white);
      //viewPalette.setColor(QPalette::Window, Qt::transparent);
      page->page()->setPalette(viewPalette);
      //m_view->setPalette(viewPalette);
    }
    else {
      qDebug() << "GWebContentView::createPage: page already exists: " << name;
    }
    return page;
  }
  
  void GWebContentView::destroySuperPage(const QString &name) {
    qDebug() << "GWebContentView::destroySuperPage: " << name;
    if(!m_superPages.contains(name)) return;

    GWebPage *page = m_superPages.take(name);
    if(page) {
      delete page;
    }
  }

  void GWebContentView::setCurrentSuperPage(const QString &name) {
    PageMap::iterator it = m_superPages.find(name);
    if(it != m_superPages.end()) {
      m_currentSuperPage = it;
    }
    else {
      qDebug() << "GWebContentView::setCurrentSuperPage: not found: " << name;
    }
  }
  
  bool GWebContentView::isSuperPage(const QString &name){
      PageMap::iterator it = m_superPages.find(name);
        if(it != m_superPages.end()) {
          return true;
        }
        qDebug() << "GWebContentView::superPage: not found: " << name;
        return false;
 }
      
  
  GWebPage * GWebContentView::superPage(const QString &name) {
    PageMap::iterator it = m_superPages.find(name);
    if(it != m_superPages.end()) {
      return it.value();
    }
    qDebug() << "GWebContentView::superPage: not found: " << name;
    return 0;
  }
  
  void GWebContentView::showSuperPage(const QString &name) {
    setCurrentSuperPage(name);
    qDebug() << "GWebContentView::showSuperPage: " << currentSuperPage();
    m_currentPageIsSuperPage = true;
    webWidget()->setPage(currentSuperPage()->page());
    m_touchNavigation->setPage(webWidget()->page());
    webWidget()->page()->setPreferredContentsSize(webWidget()->size().toSize());
  }

  void GWebContentView::showNormalPage() {
    if(webWidget()) {
      webWidget()->showNormalPage();
      webWidget()->setViewportSize();
      m_touchNavigation->setPage(currentPage());
      m_currentPageIsSuperPage =false;
    }
  }
  
  QObjectList GWebContentView::getSuperPages() {
    QObjectList *result = new QObjectList;
    foreach(GVA::GWebPage *page, m_superPages) {
      result->append(page);
    }
    // TO DO: Need to add result to JS engine so it can be cleaned up.
    return *result;
  }

  bool GWebContentView::currentPageIsSuperPage() const {
    //qDebug() << "GWebContentView::currentPageIsSuperPage: " << webWidget()->page();
    return m_currentPageIsSuperPage;
  }

  void GWebContentView::onLoadStarted()
  {
    //qDebug() << "GWebContentView::onLoadStarted: " << QTime::currentTime();
#if defined(__gva_no_chrome__) || defined(NO_RESIZE_ON_LOAD)      
      m_widget->onLoadStarted();
#endif
    emit loadStarted();
  }

  void GWebContentView::onLoadFinished(bool ok)
  {
    //qDebug() << "GWebContentView::onLoadFinished: " << QTime::currentTime();
    emit loadFinished(ok);
    //NB: page manager functions really
    if(!m_backEnabled && currentPage()->history()->canGoBack()){
      m_backEnabled = true;
      emit backEnabled(true);
    }
    else if (m_backEnabled && !currentPage()->history()->canGoBack()){
      m_backEnabled = false;
      emit backEnabled(false);
    }
    if(!m_forwardEnabled && currentPage()->history()->canGoForward()){
      m_forwardEnabled = true;
      emit forwardEnabled(true);
    }
    else if (m_forwardEnabled && !currentPage()->history()->canGoForward()){
      m_forwardEnabled = false;
      emit forwardEnabled(false);
    }
    
    // Set focus to the Web View so that text boxes have the focus (BR-994)
    m_widget->setFocus();

#if defined(__gva_no_chrome__) || defined(NO_RESIZE_ON_LOAD)
    m_widget->onLoadFinished();
#endif
  }

  void GWebContentView::dump() {
    qDebug() << "-------------";
    qDebug() << "GWebContentView::dump: " << this;
    qDebug() << "\tcurrent page=: " << currentPage() << " title=" << currentPage()->mainFrame()->title();
    qDebug() << "\tcurrent superpage=: " << currentSuperPage();
    qDebug() << "\tsuperpage count=: " << m_superPages.count();
    foreach(GVA::GWebPage *page, m_superPages) {
      page->dump();
    }
    qDebug() << "GWebContentView::dump: finished";
    qDebug() << "-------------";
  }

//  ControllableView* GWebContentView::createNew(QWidget *parent)
//  {
//      QWebPage* page = BrowserPageFactory::openBrowserPage();
//      return new GWebContentView(page, parent);
//  }


  void GWebContentView::pageCreated(WRT::WrtBrowserContainer* newPage) {

      qDebug() << "GWebContentView::pageCreated";
      /* save the page snapshot before changing the current page to the new page*/
      WRT::WrtBrowserContainer * currPage = WebPageController::getSingleton()->currentPage();
      if(currPage) {
          QWebHistoryItem item = currPage->history()->currentItem();
          currPage->savePageDataToHistoryItem(currPage->mainFrame(), &item);
      }

      // When new windows are created from window view, as the content is empty, we should disable
      // zoom actions. Set the user-scalable to false and also init the other zoom params
      // so that even if we change to windows view again without loading a page we are safe.
      // In the code-driven window usecase, this will be overwritten when the page is loaded and setViewportSize is invoked
      newPage->setPageZoomMetaData(webWidget()->defaultZoomData());

      /* Set the new page as the current page */
      WebPageController::getSingleton()->setCurrentPage(newPage);

      /* Set the web widget- this one is responsible for webnavigation etc */
      newPage->setWebWidget(webWidget());

      //connect new page main frame's initialLayoutCompleted with WebContentWidget' setViewportSize SLOT
      connect(newPage->mainFrame(), SIGNAL(initialLayoutCompleted()), webWidget(), SLOT(setViewportSize()));
  }

  void GWebContentView::updateWebPage(WRT::WrtBrowserContainer* pg)
  {
      GWebContentViewWidget* w = webWidget();
      w->setPage(pg);
      if(pg)
      {
          pg->setWebWidget(w);

          // Change navigation also to the current page
          m_touchNavigation->setPage(pg);

      }
  }

  void GWebContentView::pageChanged(WRT::WrtBrowserContainer* oldPage, WRT::WrtBrowserContainer* newPage) {
    qDebug() << "GWebContentView::pageChanged";
      Q_UNUSED(oldPage)
      updateWebPage(newPage);

      // Set new page zoom info
      changeContentViewZoomInfo(newPage);
  }

  void GWebContentView::changeContentViewZoomInfo(WRT::WrtBrowserContainer* newPage){
     // Copy the new page zoom info into cv
     webWidget()->setPageZoomMetaData(newPage->pageZoomMetaData());

     if (webWidget()->isUserScalable()) {
         webWidget()->setZoomFactor(newPage->mainFrame()->zoomFactor());
     }
     else {
         // Need to call setPageZoomFactor instead of setZoomFactor because setZoomFactor
         // will not do anything if user-scalable is false. But we need to
         // ensure that the correct zoom factor is applied as there is a possibility
         // that we might have been on another page earlier
         webWidget()->setPageZoomFactor(newPage->mainFrame()->zoomFactor());
      }
  }
  


} // end of namespace GVA


