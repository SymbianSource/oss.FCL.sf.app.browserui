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

#include "wrtbrowsercontainer.h"
#include "HistoryFlowView.h"

#include "WindowFlowView.h"
#include "webpagecontroller.h"
#include "actionjsobject.h"
#include "bedrockprovisioning.h"
#include "chromejsobject.h"
#include "chromesnippet.h"
#include "chromeview.h"
#include "chromewidget.h"
#include "controllableview.h"
#include "BookmarksManager.h"
#include "utilities.h"
#include "viewcontroller.h"
#include "webcontentview.h"
#include "webnavigation.h"
#include "viewstack.h"
#include "ZoomMetaData.h"
#include "LocaleDelegate.h"
#include "devicedelegate.h"
#include <assert.h>

#include <QtGui>
#include "qwebview.h"
#include "qwebframe.h"
#include "qwebhistory.h"

#include <QProcessEnvironment>

#ifdef USE_DOWNLOAD_MANAGER
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include "downloadcontroller.h"
#endif

#ifdef USE_STATEMACHINE
Q_SCRIPT_DECLARE_QMETAOBJECT(ChromeView, QWidget *)
#endif

static QBrush s_backgroundBrush(QColor(0,0,0));

ChromeView::ChromeView(QWidget * parent = 0)
: QGraphicsView(parent)
{
	init(ChromeView::getChromePath());
}

ChromeView::ChromeView(const QString chromeUrl, QWidget * parent = 0)
: QGraphicsView(parent)
{
	init(chromeUrl);
}

void ChromeView::init(const QString chromeUrl)
{
    m_graphicsScene = new QGraphicsScene;  // probably should be created by owner...
    m_chromeWidget = 0;
    m_viewController = 0;
    m_js = 0;
    m_mainWidget = 0;
    m_displayMode = DisplayModePortrait;
    m_chromeUrl = chromeUrl;
	m_splashScreen = 0;
    m_navigation = 0;
    m_downloadController = 0;

  qDebug() << "ChromeView::ChromeView: " << chromeUrl;
  setBackgroundBrush(s_backgroundBrush);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  setScene(m_graphicsScene);

  showSplashScreen();
  
  m_mainWidget = new QGraphicsWidget();
  m_graphicsScene->addItem(m_mainWidget);

  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  
  // Create the "chrome" javascript object.
  m_js = new ChromeJSObject(this);

  // Create the chrome overlay.
  m_chromeWidget = new ChromeWidget(this, m_mainWidget, "snippets");
  QTimer::singleShot(0, this, SLOT(connectAll()));
  m_localeDelegate = new LocaleDelegate(this);
  m_deviceDelegate = new DeviceDelegate();
  
  ActionJSObject::initClass();
  
  initViewController();
  
  setObjectName("ChromeView");
  setStyleSheet("QGraphicsView#ChromeView {margin:0; border: 0; padding:0;}");

  safe_connect(m_chromeWidget, SIGNAL(delegateLink(const QUrl&)), WebPageController::getSingleton(), SLOT(currentLoad(const QUrl&)));
  safe_connect(WebPageController::getSingleton(), SIGNAL(pageCreated(WRT::WrtBrowserContainer*)), this, SLOT(pageCreated(WRT::WrtBrowserContainer*)));
  safe_connect(WebPageController::getSingleton(), SIGNAL(pageChanged(WRT::WrtBrowserContainer*, WRT::WrtBrowserContainer*)), this, SLOT(pageChanged(WRT::WrtBrowserContainer*, WRT::WrtBrowserContainer*)));
}

void ChromeView::initViewController() {
 // Create content view manager/factory.
  m_viewController = new ViewController(0, m_graphicsScene);
  ViewStack::getSingleton()->setViewController(m_viewController);
  ViewStack::getSingleton()->setChromeView(this);

  safe_connect(m_viewController, SIGNAL(currentViewChanging()), this, SLOT(contentViewChanging()));
  safe_connect(m_viewController, SIGNAL(currentViewChanged()), this, SLOT(setContentView()));
  
  // Setup view class factory.
  m_viewController->subscribe(WebContentView::Type(), WebContentView::createNew);
  m_viewController->subscribe(WRT::WindowFlowView::Type(), WRT::WindowFlowView::createNew);
  m_viewController->subscribe(WRT::HistoryFlowView::Type(), WRT::HistoryFlowView::createNew);

  
  // Create window view
  m_viewController->createView(WRT::WindowFlowView::Type());

  /*
  // Create the goAnywhereview view.
  WRT::GoAnywhereView *goAnywhereView = 
  static_cast<WRT::GoAnywhereView *>(m_viewController->createView(WRT::GoAnywhereView::Type()));
  */
  // Create the history view.
  WRT::HistoryFlowView *historyView = 
          static_cast<WRT::HistoryFlowView *>(m_viewController->createView(WRT::HistoryFlowView::Type()));
 

   historyView->setChromeFrame(m_chromeWidget->chromePage()->mainFrame());
    // Create the web view.
  WebContentView *webContentView = 
      static_cast<WebContentView *>(m_viewController->createView(WebContentView::Type()));
  webContentView->hide();

  m_viewController->setCurrent(webContentView);
  WRT::WrtBrowserContainer* pg = WebPageController::getSingleton()->currentPage();
  pg->setWebWidget(webContentView->widget());
  m_navigation = new WebNavigation(pg,webContentView->widget());
  
#ifdef USE_DOWNLOAD_MANAGER
  QWebPage * wrtPage = webContentView->wrtPage();
  QNetworkAccessManager * accessManager = wrtPage->networkAccessManager();
  QNetworkProxy proxy = accessManager->proxy();

  m_downloadController = new DownloadController("Bedrock Browser", proxy);
  if (!m_downloadController->handlePage(wrtPage)) {
      qDebug() << "DownloadController::handlePage failed for" << wrtPage;
  }

  connect(m_downloadController, SIGNAL(downloadCreated(Download*)),
          m_js, SLOT(downloadCreated(Download*)));

  connect(m_downloadController, SIGNAL(downloadStarted(Download*)),
          m_js, SLOT(downloadStarted(Download*)));

  connect(m_downloadController, SIGNAL(downloadProgress(Download*)),
          m_js, SLOT(downloadProgress(Download*)));

  connect(m_downloadController, SIGNAL(downloadFinished(Download*)),
          m_js, SLOT(downloadFinished(Download*)));

  connect(m_downloadController, SIGNAL(downloadPaused(Download*, const QString &)),
          m_js, SLOT(downloadPaused(Download*, const QString &)));

  connect(m_downloadController, SIGNAL(downloadCancelled(Download*, const QString &)),
          m_js, SLOT(downloadCancelled(Download*, const QString &)));

  connect(m_downloadController, SIGNAL(downloadFailed(Download*, const QString &)),
          m_js, SLOT(downloadFailed(Download*, const QString &)));

  connect(m_downloadController, SIGNAL(downloadNetworkLoss(Download*, const QString &)),
          m_js, SLOT(downloadNetworkLoss(Download*, const QString &)));

  connect(m_downloadController, SIGNAL(downloadError(Download*, const QString &)),
          m_js, SLOT(downloadError(Download*, const QString &)));
#endif
}


bool ChromeView::viewportEvent(QEvent* event)
{
    return QGraphicsView::viewportEvent(event);
}

void ChromeView::contentViewChanging(){
  //Remove the current content view
  if(m_viewController && m_viewController->currentView()) {
      m_viewController->currentView()->widget()->disconnect(this);
  }
}

void ChromeView::saveToHistory(bool ok)
{
    if(ok){
        WRT::WrtBrowserContainer * currPage = WebPageController::getSingleton()->currentPage();
        Q_ASSERT(currPage);
        Q_ASSERT(!currPage->mainFrame()->url().isEmpty());
        //WRT::BookmarksManager::getSingleton()->addHistory(currPage->mainFrame()->url(),
        //                                   currPage->mainFrame()->title(),
        //                                  currPage->mainFrame()->icon());

    }    
}


void ChromeView::loadContentView()
{
    if(m_viewController) m_viewController->showContent("webView");
    //toggleVisibility("UrlSearchChromeId");
    //toggleVisibility("HistoryViewToolbarId");
    //toggleVisibility("WebViewToolbarId");
}

void ChromeView::loadUrltoCurrentPage(const QUrl & url)
{   
    WRT::WrtBrowserContainer * activePage = WebPageController::getSingleton()->currentPage();
    
    if(activePage) {
        activePage->mainFrame()->load(url);
    }
}

void ChromeView::setContentView(){
  connectContentView();
}

void ChromeView::connectContentView(){
  ControllableView *view = m_viewController ? m_viewController->currentView() : 0;
  if(!view) return;

  QGraphicsWidget *contentWidget = view->widget();
  assert(contentWidget);

  contentWidget->setGeometry(geometry());

  safe_connect(m_chromeWidget, SIGNAL(dragStarted()), m_viewController, SLOT(freezeView()));
  safe_connect(m_chromeWidget, SIGNAL(dragFinished()), m_viewController, SLOT(unfreezeView()));
  
  contentWidget->setFocus();
  updateGeometry();
  parentWidget()->updateGeometry();
}

void ChromeView::connectAll(){
  //Connect content signals
  connectContentView();
  //NB: Revisit this to avoid signal spaghetti
  //When the chrome widget finishes loading and initialization of snippets, JS emits loadComplete
  safe_connect(m_chromeWidget, SIGNAL(loadComplete()), m_js, SIGNAL(loadComplete()));
  safe_connect(m_chromeWidget, SIGNAL(loadComplete()), this, SLOT(chromeLoaded()));
  //When the widget resizes the view port, invoke update of the current view geometry. NB: this signal could also be handled by
  //the viewstack directly which would, for example, allow it to resize all views, not just the current view.
  safe_connect(m_chromeWidget, SIGNAL(viewPortResize(QRect)), this, SLOT(updateContentGeometry(QRect)));
  //Also, JS emits its version of this signal
  safe_connect(this, SIGNAL(viewPortResize(int, int, int, int)), m_js, SIGNAL(viewPortResize(int, int, int, int)));
  //Create the JS API
  safe_connect(m_chromeWidget->chromePage()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(injectJSObjects()));
  
  safe_connect(WebPageController::getSingleton(), SIGNAL(loadFinished(bool)), this, 
                  SLOT(saveToHistory(bool)));
          
  //Load the chrome contents from the default location
  reloadChrome();
}

ChromeView::~ChromeView()
{
  delete m_navigation;
  delete m_chromeWidget;
  delete m_js;
  if(m_splashScreen) delete m_splashScreen;

  //Deleting the scene will delete m_mainWidget.
  //Deleting m_mainWidget will also delete any graphics items that the chromeWidget has
  //added to the m_mainWidget.

  //Content views are not owned by the chrome view; this allows the application to manage
  //its content windows independently of the chrome.

  if(m_viewController) {
      delete m_viewController;
      ViewStack::getSingleton()->setViewController(NULL);
  }
  delete m_graphicsScene;

#ifdef USE_DOWNLOAD_MANAGER
  if (m_downloadController) {
      delete m_downloadController;
  }
#endif

  delete m_localeDelegate;
  delete m_deviceDelegate;
}

ChromeWidget * ChromeView::getChromeWidget(){
  return m_chromeWidget;
}

void ChromeView::updateViewPort(){
  assert (m_chromeWidget);
  m_chromeWidget->updateViewPort();
}

void ChromeView::setViewPort(QRect viewPort){
  assert (m_chromeWidget);
  m_chromeWidget->setViewPort(viewPort);
}

void ChromeView::loadChrome(const QString &url){
  qDebug() << "ChromeView::loadChrome: "  << url;
  m_chromeUrl = url;
  if(m_chromeWidget) {
      m_chromeWidget->setChromeUrl(url);
  }
}

void ChromeView::reloadChrome(){
  loadChrome(m_chromeUrl);
}

void ChromeView::show(const QString& id, int x, int y){
  assert(m_chromeWidget);
  m_chromeWidget->show(id, x, y);
}

void ChromeView::show(const QString& id){
  assert(m_chromeWidget);
  m_chromeWidget->show(id);
}


void ChromeView::hide(const QString& id){
  assert(m_chromeWidget);
  m_chromeWidget->hide(id);
}


void ChromeView::toggleVisibility(const QString& id){
  assert(m_chromeWidget);
  m_chromeWidget->toggleVisibility(id);
}

void ChromeView::setLocation(const QString& id, int x, int y){
  assert(m_chromeWidget);
  m_chromeWidget->setLocation(id, x, y);
}

void ChromeView::setAnchor(const QString& id, const QString& anchor){
  assert(m_chromeWidget);
  m_chromeWidget->setAnchor(id, anchor);
}

void ChromeView::toggleAttention(const QString& id){
  assert(m_chromeWidget);
  m_chromeWidget->toggleAttention(id);
}

void ChromeView::setVisibilityAnimator(const QString& elementId, const QString & animatorName){
  assert(m_chromeWidget);
  m_chromeWidget->setVisibilityAnimator(elementId, animatorName);
}

void ChromeView::setAttentionAnimator(const QString& elementId, const QString & animatorName){
  assert(m_chromeWidget);
  m_chromeWidget->setAttentionAnimator(elementId, animatorName);
}

//Animated "flips" between the current view an a chrome snippet
// TO DO: needs updating or removal.
void ChromeView::flipFromCurrentView(const QString& toId){
    assert(m_chromeWidget);

  // TBD
  
  ChromeSnippet *snippet = m_chromeWidget->getSnippet(toId);
  if(snippet)
        snippet->show(true);
}

void ChromeView::flipToCurrentView(const QString& fromId){
    Q_UNUSED(fromId)
	
	// TBD 
	
    if(m_viewController && m_viewController->currentView())
        m_viewController->currentView()->show();
}

ControllableView *ChromeView::currentContentView() {
    return m_viewController ? m_viewController->currentView() : 0;
}

ControllableView *ChromeView::contentView(const QString & type) {
    return m_viewController ? m_viewController->getView(type) : 0;
}

void ChromeView::loadStarted(){  // slot
  //qDebug() << "ChromeView::loadStarted: ";
  //m_viewController->currentView()->setCursor(Qt::BusyCursor);
}

void ChromeView::loadFinished(bool ok){  // slot
  Q_UNUSED(ok)
  //qDebug() << "ChromeView::loadFinished: " << m_viewController->currentView()->url();
  //m_viewController->currentView()->setCursor(Qt::ArrowCursor);
}

void ChromeView::updateSceneSize(const QSize &size) {
    QRectF sceneRect = m_graphicsScene->sceneRect();
    qDebug() << "ChromeView::updateSceneSize: sceneRect=" << sceneRect << " new size=" << size;
    m_graphicsScene->setSceneRect(sceneRect.x(), sceneRect.y(), size.width(), size.height());    
}

void ChromeView::resizeEvent(QResizeEvent *e){
  qDebug() << "ChromeView::resizeEvent: " << e->size();
  QRect rect(QPoint(0,0), e->size());
  
  displayModeChangeStart();

  updateChildGeometries(rect);
  updateDisplayMode();
  updateSceneSize(rect.size());

	if(m_splashScreen) {
			m_splashScreen->setGeometry(rect);
	}
}

void ChromeView::resizeScrollArea(QResizeEvent *e){
  qDebug() << "ChromeView::resizeScrollArea: " << e;
  QRect rect(QPoint(0,0), e->size());
  
  updateChildGeometries(rect);
  updateDisplayMode();
  
  QGraphicsView::setGeometry(rect);
  updateSceneSize(rect.size());
}

void ChromeView::updateDisplayMode() {
  if(width() > height())
    setDisplayMode(DisplayModeLandscape);
  else
    setDisplayMode(DisplayModePortrait);
}
 
void ChromeView::setDisplayMode(DisplayMode mode) {
  qDebug() << "ChromeView::setDisplayMode: " << mode;
  if(mode != m_displayMode){
    m_displayMode = mode;
    m_js->displayModeChanged(m_displayMode);
    QString newMode = ( (mode == DisplayModeLandscape) ? "Landscape" : "Portrait" );
    m_viewController->currentView()->displayModeChanged(newMode);
  }
}

void ChromeView::displayModeChangeStart() {
  DisplayMode mode = DisplayModePortrait;

  if(width() > height())
  {
      mode = DisplayModeLandscape;
  }
  if (mode != m_displayMode) {
     m_js->displayModeChangeStart(mode);
  }
}

void ChromeView::updateChildGeometries(const QRect &rect){
  m_mainWidget->setGeometry(rect);
  updateContentGeometry(rect);
  if(m_chromeWidget)
     m_chromeWidget->setGeometry(rect);
}

void ChromeView::updateContentGeometry(const QRect &rect) {
  if(m_viewController && m_viewController->currentView())
      m_viewController->currentView()->widget()->setGeometry(rect);
  emit viewPortResize(rect.x(), rect.y(), rect.width(), rect.height());
}


QRect ChromeView::contentViewGeometry() const {
  if(m_viewController && m_viewController->currentView())
      return m_viewController->currentView()->widget()->geometry().toRect();
  return QRect();
}

void ChromeView::setBackgroundColor(const QColor &color){
    s_backgroundBrush.setColor(color);
}

void ChromeView::injectJSObjects() {   // slot
  //qDebug() << "ChromeView::injectJSObjects";
  injectJSObject(m_chromeWidget->jsObject());
  injectJSObject(m_js);
  injectJSObject(m_viewController);
  injectJSObject(WebPageController::getSingleton());
  injectJSObject(WRT::BookmarksManager::getSingleton());
  injectJSObject(ViewStack::getSingleton());
  injectJSObject(m_localeDelegate);
  injectJSObject(m_deviceDelegate);
  if(m_viewController) 
      m_viewController->setChromeFrame(m_chromeWidget->chromePage()->mainFrame());
}

void ChromeView::injectJSObject(QObject *object) {
    if(object) {
        m_chromeWidget->chromePage()->mainFrame()->addToJavaScriptWindowObject(object->objectName(), object);
    }
}

void ChromeView::chromeLoaded() {  // slot
  if(m_splashScreen) {
    // Remove splashscreen now that the chrome is ready.
    delete m_splashScreen;
    m_splashScreen = 0;
  }  
  
//  foreach(QGraphicsItem *child, m_mainWidget->childItems()){
//      qDebug() << "      m_mainWidget child: " << child ;
//  }
//  
//  foreach(QObject *objChild, children()){
//      qDebug() << "      ChromeView child: " << objChild;
//  }
//  
//  foreach(QGraphicsItem *item, m_graphicsScene->items()){
//      qDebug() << "      graphics items: " << item;
//  }
}

void ChromeView::pageCreated(WRT::WrtBrowserContainer* newPage) {

    /* save the page snapshot before changing the current page to the new page*/
    WRT::WrtBrowserContainer * currPage = WebPageController::getSingleton()->currentPage();
    QWebHistoryItem item = currPage->history()->currentItem();
    currPage->savePageDataToHistoryItem(currPage->mainFrame(), &item);
   
    WebContentView * cv ;
    if(m_viewController) {
        cv = static_cast<WebContentView *>(m_viewController->getView("webView"));
    }
    if(cv) {
        // When new windows are created from window view, as the content is emoty, we should disable
        // zoom actions. Set the user-scalable to false and also init the other zoom params
        // so that even if we change to windows view again without loading a page we are safe.
        // In the code-driven window usecase, this will be overwritten when the page is loaded and setViewportSize is invoked
        newPage->setPageZoomMetaData(cv->webView()->defaultZoomData());


        /* Set the new page as the current page */
        WebPageController::getSingleton()->setCurrentPage(newPage);

        /* Set the web widget- this one is responsible for webnavigation etc */
        newPage->setWebWidget( cv->webView());

        //connect current page main frame's initialLayoutCompleted with WebContentWidget' setViewportSize SLOT
        connect(WebPageController::getSingleton()->currentPage()->mainFrame(), SIGNAL(initialLayoutCompleted()), cv->webView(), SLOT(setViewportSize()));

#ifdef USE_DOWNLOAD_MANAGER
        if (!m_downloadController->handlePage(newPage)) {
            qDebug() << "DownloadController::handlePage failed for" << newPage;
        }
#endif
    }

}

void ChromeView::updateWebPage(WRT::WrtBrowserContainer* pg)
{
    if(!m_viewController) return;

    WebContentView* cv = static_cast<WebContentView *>(m_viewController->getView("webView"));
    WebContentWidget* w = cv->webView();
    w->setPage(pg);
    if(pg)
    {
        // Disconnect this signal to avoid multiple connections - it is being connected in setWebWidget
        disconnect(w, SIGNAL(pageZoomMetaDataChange(QWebFrame*, ZoomMetaData)), pg, SLOT(pageZoomMetaDataChange(QWebFrame*, ZoomMetaData)));

        pg->setWebWidget(w);

        // Change navigation also to the current page
        m_navigation->setPage(pg);

    }            
}

void ChromeView::pageChanged(WRT::WrtBrowserContainer* oldPage, WRT::WrtBrowserContainer* newPage) {

    Q_UNUSED(oldPage)
    updateWebPage(newPage);

    // Set new page zoom info
    changeContentViewZoomInfo(newPage);
}


void ChromeView::changeContentViewZoomInfo(WRT::WrtBrowserContainer* newPage){

   if(m_viewController) {
       WebContentView * cv  = static_cast<WebContentView *>(m_viewController->getView("webView"));

       // Copy the new page zoom info into cv
       cv->webView()->setPageZoomMetaData(newPage->pageZoomMetaData());

       if (cv->webView()->isUserScalable()) {
           cv->webView()->setZoomFactor(newPage->mainFrame()->zoomFactor());
       }
       else {
           // Need to call setPageZoomFactor instead of setZoomFactor because setZoomFactor
           // will not do anything is user-scalable is false. But we need to
           // ensure that the correct zoom factor is applied as there is a possibility
           // that we might have been on another page earlier
           cv->webView()->setPageZoomFactor(newPage->mainFrame()->zoomFactor());

        }
    }
}

// TODO: INVESTIGATE: Is this needed anymore since we handle pageChanged now
void ChromeView::setViewofCurrentPage() {
    updateWebPage(WebPageController::getSingleton()->currentPage());
}


QString ChromeView::getChromeBaseDir() {
   QString chromeBaseDir = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("ChromeBaseDirectory");
   return chromeBaseDir;
}

QString ChromeView::getChromePath() {
   
   QString chromeFile = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("StartUpChrome");
   QString baseDir = getChromeBaseDir();
   QString chromePath = baseDir + chromeFile;
   qDebug() << chromePath;
   return chromePath;
}

void ChromeView::showSplashScreen() {

  QString splashImage = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("SplashImage");
  QString baseDir = getChromeBaseDir();
  QString imagePath =	baseDir + splashImage;
  
  if(!imagePath.isNull()) {
    m_splashScreen = new QLabel(parentWidget());
    m_splashScreen->setStyleSheet("background-color: #000");
    m_splashScreen->setPixmap(QPixmap(imagePath));
    if(m_splashScreen->pixmap()->isNull()) {
        qDebug() << "ChromeView::chromeLoaded: ERROR splashscreen creation failed. " << imagePath;
    }
    else {
        m_splashScreen->show();
    }
  }
}
