#include "WindowsView.h"
#include "BrowserWindow.h"
#include "GWebContentView.h"
#include "ViewStack.h"

//For page created signal
#include "wrtbrowsercontainer.h"
#include "webpagecontroller.h"

//For shared chrome object
#include "ChromeWidget.h"
#include "ChromeView.h"

#include <QDebug>

namespace GVA {

  WindowsView::WindowsView(ChromeWidget * chrome, QObject * parent, const QString & objectName)
    : ControllableViewBase(parent),
      m_current(0),
      m_chrome(chrome),
      m_menu(0)
{
  setObjectName(objectName);
}

WindowsView::~WindowsView()
{
  foreach (QMainWindow * window, m_windows){
    delete window;
  }   
}

void WindowsView::activate(BrowserWindow * window)
{

  ViewStack * viewStack = ViewStack::getSingleton();
  GWebContentView* webView = contentView();
  if(webView) {
    if(contentView()->currentPageIsSuperPage()){
      webView->showNormalPage();
      webView->setGesturesEnabled(true);
      window->setView("WebView"); 
      viewStack->switchView(window->view(), viewStack->getViewController()->currentViewName());
    }
    else {
      viewStack->getViewController()->currentView()->deactivate();
    }
    window->setTitle(viewStack->getViewController()->currentView()->title());
  }
  // Let the chrome take focus, or whatever it wants to do.
  m_chrome->activate();
}

void WindowsView::deactivate(BrowserWindow * window)
{
  ViewStack * viewStack = ViewStack::getSingleton();
  /*
  if(m_current != window){
    m_current = window;
    viewStack->switchView(window->view(), viewStack->getViewController()->currentViewName());
  } 
  else */
  //#ifndef BEDROCK_TILED_BACKING_STORE
  if(viewStack->getViewController()->currentView())
    viewStack->getViewController()->currentView()->activate();
  //#endif
}


void WindowsView::setMenu(QMenu * menu)
{
  m_menu = menu;
  if(m_menu){
    m_menu->addAction("New Window", this, SLOT(onWindowAction()));
  }
}

void WindowsView::setMenuEnabled(bool enabled)
{
  m_menu->setEnabled(enabled);
  m_current->setMenuEnabled(enabled);
}

bool WindowsView::isMenuEnabled() const {
    return m_menu->isEnabled();
}

void WindowsView::handlePageEvents(bool handleEvents){

  if(handleEvents){
    connect(WebPageController::getSingleton(), SIGNAL(pageCreated(WRT::WrtBrowserContainer *)),
	  this, SLOT(onPageCreated(WRT::WrtBrowserContainer *)));
  }else{
    disconnect(WebPageController::getSingleton(), SIGNAL(pageCreated(WRT::WrtBrowserContainer *)),
	  this, SLOT(onPageCreated(WRT::WrtBrowserContainer *)));
  }
}

BrowserWindow * WindowsView::addWindow(WRT::WrtBrowserContainer * page)
{
  BrowserWindow * window = new BrowserWindow(m_chrome, this);
  window->setPage(page);
  window->menuBar()->addMenu(m_menu);
  m_windows[page] = window;
  m_current = window;
  m_current->show();
  deactivate(window);
  return window;
}

GWebContentView * WindowsView::contentView()
{
  return static_cast<GWebContentView*>(ViewStack::getSingleton()->getViewController()->view("WebView"));
}

void WindowsView::onWindowAction() 
{
  //Tell the page controller to create a new QWebPage. 
  //The controller emits pageCreated when this happens
  //The onPageCreated slot actually creates the window
  WebPageController::getSingleton()->openPage();
}
  
void WindowsView::onPageCreated(WRT::WrtBrowserContainer * page)
{
  addWindow(page);
  //WebPageController::getSingleton()->loadFromHistory();
}

void WindowsView::onPageDeleted(WRT::WrtBrowserContainer * page)
{
  assert(m_windows.contains(page));
  delete m_windows.take(page);
}

};
