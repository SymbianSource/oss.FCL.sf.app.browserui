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


#include <QWebElement>
#include <QWebPage>
#include <QWebFrame>
#include <QList>
#include <QKeyEvent>
#include <QDebug>
#include <QGraphicsSceneContextMenuEvent>

#include "ChromeWidgetJSObject.h"
#include "ChromeRenderer.h"
#include "ChromeDOM.h"
#include "Snippets.h"
#include "ChromeSnippet.h"
#include "ChromeWidget.h"
#include "WebChromeContainerSnippet.h"
#include "Application.h"
#include "AppContentView.h"
#include "BlueChromeSnippet.h"
#include "GreenChromeSnippet.h"
#include "ViewController.h"
#include "ViewStack.h"
//#include "CollapsingWidget.h"
#include "SlidingWidget.h"
#include "ProgressSnippet.h"
#include "GWebPage.h"
#include "webpagecontroller.h"
//#include "ViewStack.h"
#include "BookmarksManager.h"
#include "ScriptObjects.h"
#include "LocaleDelegate.h"
#include "DeviceDelegate.h"

#ifdef USE_DOWNLOAD_MANAGER
#include "Downloads.h"
#endif

#include "wrtbrowsercontainer.h"
#include "webpagecontroller.h"

namespace GVA {

// -----------------------------

  ChromeWidget::ChromeWidget(QGraphicsItem * parent, Qt::WindowFlags wFlags) 
    : QGraphicsWidget(parent, wFlags),
      m_renderer(0),
      m_dom(0),
      m_viewController(new ViewController()),
      m_topBar(0),
      m_bottomBar(0),
      m_leftBar(0),
      m_rightBar(0),
      m_aspect(portrait),
      m_jsObject(new ChromeWidgetJSObject(0, this)),
      m_localeDelegate(new LocaleDelegate(this)),
      m_deviceDelegate(new DeviceDelegate()),
      m_downloads(0)
  {
    m_scene = new QGraphicsScene();
    //Keep key events not otherwise consumed from going to the scene
    //installEventFilter(this);
    //This is the root of the scene hierarchy
    m_scene->addItem(this);
    m_layout = new QGraphicsAnchorLayout();
    m_layout->setContentsMargins(0,0,0,0);
    m_layout->setSpacing(0);
    setLayout(m_layout);
    addAnchors();
    m_viewPort = new SlidingWidget(this);
    //m_viewBar = new CollapsingWidget(m_viewPort);
#ifndef __gva_no_chrome__
    m_snippets = new Snippets(this, this);
#endif
    m_layout->addAnchor(m_viewPort, Qt::AnchorTop, m_topBar, Qt::AnchorBottom);
    m_layout->addAnchor(m_viewPort, Qt::AnchorBottom, m_bottomBar, Qt::AnchorTop);
    m_layout->addAnchor(m_viewPort, Qt::AnchorLeft, m_leftBar, Qt::AnchorRight);
    m_layout->addAnchor(m_viewPort, Qt::AnchorRight, m_rightBar, Qt::AnchorLeft);
#ifndef __gva_no_chrome__
    m_page =  static_cast<QWebPage *>(new GVA::WebPageWrapper(this, "Chrome Javascript error"));
    m_page->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    m_page->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
#endif
    m_viewController->setObjectName("views");

    connect(m_viewController, SIGNAL(currentViewChanged()), this, SLOT(onCurrentViewChanged()));
#ifndef __gva_no_chrome__

    m_jsObject->setObjectName("chrome");
    // Pass some signals from this object to the Javascript object.
    QObject::connect(this, SIGNAL(chromeComplete()), m_jsObject, SIGNAL(chromeComplete()));
    QObject::connect(this, SIGNAL(aspectChanged(int)), m_jsObject, SIGNAL(aspectChanged(int)));
    QObject::connect(this, SIGNAL(prepareForGeometryChange()), m_jsObject, SIGNAL(prepareForGeometryChange()));
    QObject::connect(this, SIGNAL(symbianCarriageReturn()), m_jsObject, SIGNAL(symbianCarriageReturn()));

    //addJSObjectToEngine(this);
    m_app = new GinebraApplication();
    //addJSObjectToEngine(m_app);

#ifdef USE_DOWNLOAD_MANAGER
    m_downloads = new Downloads();
    QObject::connect(
            WebPageController::getSingleton(), SIGNAL(pageCreated(WRT::WrtBrowserContainer*)),
            this, SLOT(pageCreated(WRT::WrtBrowserContainer*)));
#endif
    
    QObject::connect(m_page, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
    QObject::connect(m_page, SIGNAL(loadStarted()), this, SLOT(loadStarted()));
    QObject::connect(m_page->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(exportJSObjects()));

#endif
    
    ViewStack::getSingleton()->setViewController(m_viewController);
    // TO DO: need a better home for this.
    qMetaTypeId<QObjectList>();
    qRegisterMetaType<QObjectList>("QObjectList");
  }

  void ChromeWidget::loadUrlToCurrentPage(const QUrl & url)
  {
    WRT::WrtBrowserContainer * activePage = WebPageController::getSingleton()->currentPage();

    if (activePage) {
      activePage->mainFrame()->load(url.toString());
    }
  }

  void ChromeWidget::pageCreated(WRT::WrtBrowserContainer * page)
  {
#ifdef USE_DOWNLOAD_MANAGER
    if (m_downloads) {
        m_downloads->handlePage(page);
    }
#else
    Q_UNUSED(page)
#endif
  }

  ChromeWidget::~ChromeWidget()
  {
    // clearChrome(); // crashes on exit
    delete m_viewController;
    delete m_bottomBar;
    delete m_topBar;
    delete m_leftBar;
    delete m_rightBar;
    delete m_viewPort;
    // delete m_viewLayout; // crashes on exit
    delete m_renderer;
    delete m_dom;
    delete m_page;
    // delete m_scene;  // crashes on exit
    delete m_snippets;
    // delete m_layout; // crashes on exit
    delete m_localeDelegate;
    delete m_deviceDelegate;
#ifdef USE_DOWNLOAD_MANAGER
    delete m_downloads;
#endif
  }
 

  //Eat key events not otherwise consumed.
  /*bool ChromeWidget::eventFilter(QObject * obj, QEvent * ev)
  {
   if(ev->type() == QEvent::KeyPress){
      return true;
    }
    return QObject::eventFilter(obj,ev);
 
  }*/

  void ChromeWidget::resizeEvent(QGraphicsSceneResizeEvent *ev)
  {
#ifndef __gva_no_chrome__
    if(m_dom && m_renderer) {
      emit prepareForGeometryChange();
      m_renderer->setGeometry(QRectF(-1200,-1200, ev->newSize().width(), m_dom->height()));
    }
    int aspect = m_aspect;
    m_aspect = (ev->newSize().width() > ev->newSize().height())?landscape:portrait;
    if(m_aspect != aspect) {
      QString mode = (m_aspect == landscape ? "Landscape" : "Portrait");
      ControllableViewBase* cview = m_viewController->currentView();
      if (cview)
        cview->displayModeChanged(mode);
      emit aspectChanged(m_aspect);
    }
#endif
    QGraphicsWidget::resizeEvent(ev);    
    
  }

  /*
  //For layout debugging
  void ChromeWidget::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget* widget){
    painter->fillRect(geometry(), Qt::red);
  }
  */

  void ChromeWidget::setChromeBaseDirectory(const QString dir) {
    m_baseDirectory = dir;
  }

  void ChromeWidget::setChromeFile(const QString filePath)
  {
#ifndef __gva_no_chrome__
    qDebug() << "ChromeWidget::setChromeFile: " << m_baseDirectory << filePath;
    m_page->mainFrame()->load(QUrl(m_baseDirectory + filePath));
#else
    Q_UNUSED(filePath)
#endif
  }  

  void ChromeWidget::reloadChrome()
  {
    clearChrome();
    m_page->triggerAction(QWebPage::Reload);
  }

  void ChromeWidget::addViewToLayout(ControllableViewBase * controllableView){
    //qDebug() << "ChromeWidget::addViewToLayout: " << controllableView->widget();
    if(controllableView->widget()) {
      m_viewPort->setWindow(controllableView->widget());
      controllableView->widget()->setFocusPolicy(Qt::ClickFocus); //NB: Is this needed? Does it break anything?
      controllableView->widget()->setZValue(-1.0); //Make sure we are behind any center anchored snippets
    }
  }

  void ChromeWidget::addView(ControllableViewBase * controllableView) {
    //qDebug() << "ChromeWidget::addView: " << controllableView->widget();
    m_viewController->addView(controllableView);

    /*
    if(controllableView->widget()) {
      addViewToLayout(controllableView);
    }
    else {
      qWarning() << "ChromeWidget::addView: view has no widget. " << controllableView;
      connect(controllableView, SIGNAL(instantiated(ControllableViewBase *)), this, SLOT(onViewInstantiated(ControllableViewBase *)));
    }
//    m_jsObjects.append(controllableView); // Add to list of exported objects
#ifndef __gva_no_chrome__
    addJSObjectToWindow(controllableView); // Export to DOM
#endif
    //addJSObjectToEngine(viewDelegate); // Export to script engine
    */
  }

  void ChromeWidget::anchorToView(ChromeSnippet* snippet, const QString& where)
  {
    Q_UNUSED(where)
      //m_viewBar->addItem(snippet);
    m_viewPort->attachItem(snippet->widget());
  }
 
  void ChromeWidget::detachFromView(ChromeSnippet* snippet, const QString& where)
  {
    Q_UNUSED(where)
      //  m_viewBar->removeItem(snippet);
    m_viewPort->detachItem(snippet->widget());
  }

  void ChromeWidget::anchorTogether(ChromeSnippet* first, const QString& secondId, qreal x, qreal y)
  { 
    ChromeSnippet* second = getSnippet(secondId);
    if (second){
      //qDebug() << "Anchoring: " << first->objectName() << " to: " << second->objectName();
      first->widget()->setParentItem(second->widget());
      first->widget()->setPos(x,y);
    }
  }

  void ChromeWidget::unAnchor(ChromeSnippet* snippet)
  {
    snippet->widget()->setParentItem(0);
    snippet->widget()->setParentItem(this);
  }

  qreal ChromeWidget::slideView(qreal delta)
  {
    //return m_viewBar->collapse(delta);
    return m_viewPort->slide(delta);
  }

  ControllableViewBase * ChromeWidget::getView(const QString& view)
  {
    return m_viewController->view(view);
  }  

  void ChromeWidget::showView(const QString &name) {
    qDebug() << "ChromeWidget::showView: " << name;
    m_viewController->showView(name);
  }

  void ChromeWidget::onCurrentViewChanged() {
    addViewToLayout(m_viewController->currentView());
  }

  // Clean up all existing snippets;
  
  void ChromeWidget::clearChrome() 
  {
    m_snippets->clear();
  }

  void ChromeWidget::loadStarted() // slot
  {
    clearChrome();
  }

  void ChromeWidget::loadFinished(bool ok)  // slot
  {
    //qDebug() << "ChromeWidget::loadFinished";
    if(!ok)
      {
      qDebug() << "ChromeWidget::loadFinished: error";
      return;
      }
    //NB: do we really need to instantiate a new renderer?
    if(m_renderer)
      delete m_renderer;
    // qDebug() << "Instantiate renderer";
    m_renderer = new ChromeRenderer(m_page, this);
    // qDebug() << "Resize the renderer 1";
    m_renderer->resize(size());
    m_renderer->setPos(-1200, -1200);
    m_renderer->setZValue(-3);
#ifdef Q_OS_SYMBIAN
    connect(m_renderer, SIGNAL(symbianCarriageReturn()), this, SIGNAL(symbianCarriageReturn()));
#endif
    if(m_dom) 
      delete m_dom; // NB: This may need some further investigation
    m_dom = new ChromeDOM(m_page, this);
    getInitialSnippets();
    //Set the final renderer size to match the chrome
    m_renderer->resize(size().width(), m_dom->height());
    //qDebug() << m_dom->getCacheableScript();

    // Let internal objects know that the chrome is complete.
    emit internalChromeComplete();
    // Now let the javascript world know that it is complete.
    emit chromeComplete();
  }

  void ChromeWidget::chromeInitialized()
  {
    //NB: Don't want to implement this, but just in case
  }

  void ChromeWidget::exportJSObjects()
  {
    exportJSObjectsToPage(m_page);
  }

  void ChromeWidget::exportJSObjectsToPage(QWebPage *page) {
    addJSObjectToPage(m_jsObject, page);
    addJSObjectToPage(m_snippets, page);
    addJSObjectToPage(m_app, page);
    addJSObjectToPage(m_viewController, page);
    addJSObjectToPage(WebPageController::getSingleton(), page);
    addJSObjectToPage(WRT::BookmarksManager::getSingleton(), page);
    addJSObjectToPage(ViewStack::getSingleton(), page);
    addJSObjectToPage(m_localeDelegate, page);
    addJSObjectToPage(m_deviceDelegate, page);
    // Dynamically added objects
    foreach(QObject * jsObj, m_jsObjects) {
      addJSObjectToPage(jsObj, page);
    }
#ifdef USE_DOWNLOAD_MANAGER
    if (m_downloads != 0) {
        addJSObjectToPage(m_downloads, page);
    }
#endif
  }

  void ChromeWidget::getInitialSnippets()
  {
    //TODO: get the list of containers form m_dom (via new method to be added).
    QList <QWebElement> initialSnippets = m_dom->getInitialElements();
    foreach(QWebElement element, initialSnippets) {
      ChromeSnippet * s = getSnippet(element.attribute("id"));
      if(s->initiallyVisible())
        s->setVisible(true);
    }
  }

  void ChromeWidget::addAnchors(){
    if(!m_bottomBar){
      m_bottomBar = new QGraphicsWidget(this);
      m_bottomBar->setPreferredHeight(0);
      m_bottomBar->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
      m_layout->addAnchor(m_bottomBar, Qt::AnchorBottom, m_layout, Qt::AnchorBottom);
      m_layout->addAnchors(m_bottomBar, m_layout, Qt::Horizontal);
    }
    if(!m_topBar){
      m_topBar = new QGraphicsWidget(this);
      m_topBar->setPreferredHeight(0);
      m_topBar->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
      m_layout->addAnchor(m_topBar, Qt::AnchorTop, m_layout, Qt::AnchorTop);
      m_layout->addAnchors(m_topBar, m_layout, Qt::Horizontal);
    }
    if(!m_leftBar){
      m_leftBar = new QGraphicsWidget(this);
      m_leftBar->setPreferredWidth(0);
      m_leftBar->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
      m_layout->addAnchor(m_leftBar, Qt::AnchorLeft, m_layout, Qt::AnchorLeft);
      m_layout->addAnchors(m_leftBar, m_layout, Qt::Vertical);
    }
    if(!m_rightBar){
      m_rightBar = new QGraphicsWidget(this);
      m_rightBar->setPreferredWidth(0);
      m_rightBar->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
      m_layout->addAnchor(m_rightBar, Qt::AnchorRight, m_layout, Qt::AnchorRight);
      m_layout->addAnchors(m_rightBar, m_layout, Qt::Vertical);
    }
  }
  
  //Check to see if snippet has an anchor and lay it out accordingly.
  //Otherwise snippets determine their own positions from html

  void ChromeWidget::anchorSnippet(ChromeSnippet * snippet)
  {
    if(snippet->anchor() == anchorBottom){
      if(snippet->hidesContent()){
	m_layout->addAnchor(snippet->widget(), Qt::AnchorBottom, m_bottomBar, Qt::AnchorTop);
	m_layout->addAnchor(snippet->widget(), Qt::AnchorHorizontalCenter, m_bottomBar, Qt::AnchorHorizontalCenter);
      } else {
	snippet->widget()->setParentItem(m_bottomBar);
	snippet->widget()->setY(-snippet->anchorOffset());
      }
    }
    else if (snippet->anchor() == anchorTop){
      if(snippet->hidesContent()){
	m_layout->addAnchor(snippet->widget(),Qt::AnchorTop, m_topBar, Qt::AnchorBottom);
	m_layout->addAnchor(snippet->widget(), Qt::AnchorHorizontalCenter, m_topBar, Qt::AnchorHorizontalCenter);
      }
      else {
	snippet->widget()->setParentItem(m_topBar);
	snippet->widget()->setY(snippet->anchorOffset());
      }
    }
    else if (snippet->anchor() == anchorLeft){
      if(snippet->hidesContent())
	m_layout->addAnchor(snippet->widget(),Qt::AnchorLeft, m_leftBar, Qt::AnchorRight);
      else {
	snippet->widget()->setParentItem(m_leftBar);
	snippet->widget()->setX(snippet->anchorOffset());
      }
    }
    else if (snippet->anchor() == anchorRight){
      if(snippet->hidesContent())
	m_layout->addAnchor(snippet->widget(),Qt::AnchorRight, m_rightBar, Qt::AnchorLeft);
      else {
	snippet->widget()->setParentItem(m_rightBar);
	snippet->widget()->setX(-snippet->anchorOffset());
      }
    }
    else if (snippet->anchor() == anchorCenter) {
      snippet->widget()->setParentItem(m_viewPort);
      snippet->widget()->setZValue(0.0);
      QGraphicsAnchorLayout * vl = static_cast<QGraphicsAnchorLayout*>(m_viewPort->layout());
      vl->addAnchor(snippet->widget(), Qt::AnchorVerticalCenter, vl, Qt::AnchorVerticalCenter);
      vl->addAnchor(snippet->widget(), Qt::AnchorHorizontalCenter, vl, Qt::AnchorHorizontalCenter);
    }
    else if (snippet->anchor() == anchorTopLeft){
      qDebug() << "Setting top left anchor";
      m_layout->addCornerAnchors(snippet->widget(), Qt::TopLeftCorner, m_layout, Qt::TopLeftCorner);
    }
    else if (snippet->anchor() == anchorTopRight)
      m_layout->addCornerAnchors(snippet->widget(), Qt::TopRightCorner, m_layout, Qt::TopRightCorner);
    else if (snippet->anchor() == anchorBottomLeft)
      m_layout->addCornerAnchors(snippet->widget(), Qt::BottomLeftCorner, m_layout, Qt::BottomLeftCorner);
    else if (snippet->anchor() == anchorBottomRight)
      m_layout->addCornerAnchors(snippet->widget(), Qt::BottomRightCorner, m_layout, Qt::BottomRightCorner);
  }

  void ChromeWidget:: addSnippet(ChromeSnippet * snippet, const QString & docElementId )
  {
    m_snippets->addSnippet(snippet, docElementId);
    snippet->widget()->setParentItem(this);
    
    if(!snippet->parentId().isNull()){
      ChromeSnippet * container = getSnippet(snippet->parentId());
      if(container)
	container->addChild(snippet);
    }
    
    anchorSnippet(snippet);
  }

  //NB: This really shouldn't be necessary: anchor bars should be implemented as an expanding
  //widget class !!!!!!

  void ChromeWidget::adjustAnchorOffset(ChromeSnippet * snippet, qreal delta)
  {
    if(snippet->anchor() == anchorBottom)
      m_bottomBar->setPreferredHeight(m_bottomBar->preferredHeight() + delta);
    else if(snippet->anchor() == anchorTop)
      m_topBar->setPreferredHeight(m_topBar->preferredHeight() + delta);
    else if(snippet->anchor() == anchorLeft){
      m_leftBar->setPreferredWidth(m_leftBar->preferredWidth() + delta);
    }
    else if(snippet->anchor() == anchorRight){
      m_rightBar->setPreferredWidth(m_rightBar->preferredWidth() + delta);
    }
  }

  // Call after a snippet changes to visible state
  // (NB: does not check to see whether snippet has been
  // correctly added to chrome (by calling addSnippet).
  // At a minimum should assert that the anchor "bars"
  // are not null.)

  void ChromeWidget::snippetShown(ChromeSnippet * snippet)
  {
    if(snippet->hidesContent())
      return;
    if(snippet->anchor() == anchorBottom)
      m_bottomBar->setPreferredHeight(m_bottomBar->preferredHeight() + snippet->widget()->preferredHeight());
    else if(snippet->anchor() == anchorTop)
      m_topBar->setPreferredHeight(m_topBar->preferredHeight() + snippet->widget()->preferredHeight());
    else if(snippet->anchor() == anchorLeft){
      m_leftBar->setPreferredWidth(m_leftBar->preferredWidth() + snippet->widget()->preferredWidth());
    }
    else if(snippet->anchor() == anchorRight){
      m_rightBar->setPreferredWidth(m_rightBar->preferredWidth() + snippet->widget()->preferredWidth());
    }
  }

  // Call before a snippet changes to invisible state
  // (NB: does not check to see whether snippet has been
  // correctly added to chrome (by calling addSnippet).
  // At a minimum should assert that the anchor "bars"
  // are not null.)

  void ChromeWidget::snippetHiding(ChromeSnippet * snippet)
  {
    if(snippet->hidesContent())
      return;
    if(snippet->anchor() == anchorBottom)
      m_bottomBar->setPreferredHeight(m_bottomBar->preferredHeight() - snippet->widget()->preferredHeight());
    else if(snippet->anchor() == anchorTop)
      m_topBar->setPreferredHeight(m_topBar->preferredHeight() - snippet->widget()->preferredHeight());
    else if(snippet->anchor() == anchorLeft){
      m_leftBar->setPreferredWidth(m_leftBar->preferredWidth() - snippet->widget()->preferredWidth());
    }
    else if(snippet->anchor() == anchorRight){
      m_rightBar->setPreferredWidth(m_rightBar->preferredWidth() - snippet->widget()->preferredWidth());
    }
  }

  ChromeSnippet *ChromeWidget::getSnippet(const QString & docElementId, QGraphicsItem * parent) {
 
    ChromeSnippet *result = m_snippets->getSnippet(docElementId);
    if(!result){
      result = m_dom->getSnippet(docElementId, parent);
      if(result) {
	result->setParent(m_snippets); // Exports to "Snippets" JS object
        addSnippet(result, docElementId);
      }
      else{
	qDebug() << "Snippet not found: " << docElementId;
	return 0;
      }
    }else{
      //qDebug() << "Found existing snippet: " << docElementId;
    }
    
    return result;
  }

  QRect ChromeWidget::getSnippetRect(const QString &docElementId)
  {
    return m_dom->getElementRect(docElementId);
  }

  void ChromeWidget::addJSObjectToWindow(QObject *object) 
  {
    m_page->mainFrame()->addToJavaScriptWindowObject(object->objectName(), object);
  }

  void ChromeWidget::addJSObjectToPage(QObject *object, QWebPage *page)
  {
    page->mainFrame()->addToJavaScriptWindowObject(object->objectName(), object);
  }

  QObjectList ChromeWidget::getSnippets() {
    return m_snippets->getList();
  }

  void ChromeWidget::alert(const QString & msg) {
    // To do: open a dialog box showing msg.
    qDebug() << msg;
  }

  QString ChromeWidget::getDisplayMode() const {
    return (m_aspect==portrait)?"portrait":"landscape";
  }

  void ChromeWidget::onViewInstantiated(ControllableViewBase *view) {   // slot
    qDebug() << "ChromeWidget::onViewInstantiated: "; // << view;
    addViewToLayout(view);
  }

  QObject*  ChromeWidget::getDisplaySize() const 
  {
    ScriptSize * sz = new ScriptSize(size().toSize());
    m_page->mainFrame()->addToJavaScriptWindowObject("size", sz, QScriptEngine::ScriptOwnership);
    return sz;

  }
  
void ChromeWidget::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) 
{
    event->accept();
}
  
  /*
  void ChromeWidget::addJSObjectToEngine(QObject *object)
  {
    QScriptValue val = m_engine.newQObject(object);
    m_engine.globalObject().setProperty(object->objectName(), val);
  }

  QScriptValue ChromeWidget::evalWithEngineContext(const QString& program)
  {
    return m_engine.evaluate(program);
  }
  */

  void ChromeWidget::dump() {
    qDebug() << "---------------------";
    qDebug() << "ChromeWidget::dump";
    m_snippets->dump();
    m_viewController->dump();
    //WebPageController::getSingleton()->dump();
    qDebug() << "---------------------";
  }
} // endof namespace GVA
