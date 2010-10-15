#include "BrowserWindow.h"
#include "WindowsView.h"
#include "ChromeLayout.h"
#include "ChromeWidget.h"
#include "ChromeView.h"
#include "ChromeDOM.h" //Get rid of this dependency
#include "GWebContentView.h"
#include <QGraphicsWidget>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QVBoxLayout>
#include "webpagecontroller.h"
#include "ViewStack.h"
#ifdef Q_WS_MAEMO_5
#include <QtGui/QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif

namespace GVA {

  bool BrowserWindow::m_preventDisconnectPageTitleChanged = false; // workaraound for multiple windows crash
  bool BrowserWindow::m_preventConnectPageTitleChanged = false;    // workaround for multiple windows crash

  BrowserWindow::BrowserWindow(ChromeWidget * chrome, WindowsView * windows, QWidget * parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags),
      m_page(0),
      m_specialPage(0),
      m_decorated(false),
      m_chrome(chrome),
      m_scene(new QGraphicsScene()),
      m_snapshot(0),
      m_windows(windows),
      m_state(menuEnabled),   
      m_slidedUp(false),   
      m_viewName("WebView"),
      m_changingMode(false)
  {
    m_view = new ChromeView(m_scene, m_chrome, this);
    setCentralWidget(m_view);
    show();
    ViewController *controller = viewController();
    if(controller) {
        connect(controller, SIGNAL(currentViewChanged(ControllableViewBase *)), this, SLOT(onViewChanged(ControllableViewBase *)));
    }
    slideViewIfRequired(100); // display UrlSearchBar for every new window
  }

  void BrowserWindow::setPage(WRT::WrtBrowserContainer * page)
  { 
    m_page = page;
    connectPageTitleChanged();
  }

	// hide or show UrlSearchBar if required
	// This is a workaround for the problem that created by the fact that
	// multiple instances of BrowserWindow use same instance of m_chrome and m_chrome->layout()
	// 
  void BrowserWindow::slideViewIfRequired(int value)
  {
      if(value > 0) { // slide up to display UrlSearchBar
           m_chrome->layout()->slideView(value);
           m_slidedUp = true;
           return;
      }

      if((m_slidedUp == true) && (value < 0)) { // slide down UrlSearchBar
           m_chrome->layout()->slideView(value);
           m_slidedUp = false;
           return;
      }
  }
  
  void BrowserWindow::toggleDecoration() 
  { 
    m_decorated = !m_decorated;
    if(m_decorated) {
      showDecoration();
    } 
    else {
      hideDecoration();
    }
  }

  void BrowserWindow::showDecoration()
  {
    m_state |= active;
    if(m_page)       
      WebPageController::getSingleton()->setCurrentPage(m_page);
    m_windows->deactivate(this);
    if(m_snapshot)
      m_snapshot->hide();
    if(m_chrome){
      m_chrome->layout()->setPos(0,0);
      m_scene->addItem(m_chrome->layout());
      m_chrome->layout()->show();
      // Ensure that the layout size is in-synch with the window size.
      m_chrome->layout()->resize(size());
    }
  }

  void BrowserWindow::hideDecoration()
  {
    m_state &= ~active;

    m_windows->activate(this);
    if(m_chrome)
      m_scene->removeItem(m_chrome->layout());

    if(m_page){
      m_page->triggerAction(QWebPage::Stop);
      //QImage image = WebPageController::getSingleton()->pageThumbnail(m_page);
      //	if(image.isNull()){
      //QImage image =  m_page->pageThumbnail(1,1);
      //
      
      QImage image;
      //This is really a bit hackish: the page controller should be able
      //to figure this out;
      //if(m_windows->contentView()->currentPageIsSuperPage()){
	//m_specialPage = static_cast<WRT::WrtBrowserContainer*>(m_windows->contentView()->wrtPage());
        //m_windows->contentView()->updateWebPage(m_page);
	//image = m_windows->contentView()->pageThumbnail(1,1);
      //}
      //else
        QSize s(800,464);
        image = m_page->thumbnail(s);
      if(m_snapshot){
	m_scene->removeItem(m_snapshot);
	delete m_snapshot;
      }
      m_snapshot = new QGraphicsPixmapItem(QPixmap::fromImage(image));
      m_scene->addItem(m_snapshot);
      m_snapshot->show();
    } else {
      qDebug() << "BrowserWindow::hideDecoration: page not set";
    }
  }
  
  void BrowserWindow::closeWindow()
  {
    //Tell the page controler to close this page.
    //The controller emits pageClosed. WindowsView
    //handles this and deletes this browser window
    // Don't delete if only 1 window open..causes crash on exit
    if (WebPageController::getSingleton()->pageCount() > 1)
      WebPageController::getSingleton()->closePage(m_page);
  }

  //Handle window events
  bool BrowserWindow::event(QEvent * event) 
  {
    //qDebug() << "=====================BrowserWindow:event: " << event->type();
    switch (event->type()) {
    case QEvent::WindowBlocked:
      m_state |= blocked;
      break;
    case QEvent::WindowUnblocked:
      m_state &= ~blocked;
      break;
    case QEvent::WindowActivate: //Newly shown window or window maximized
      handleWindowActivate();
      m_chrome->windowStateChange(windowState());
      break;
    case QEvent::WindowDeactivate:
      handleWindowDeactivate();
      m_chrome->windowStateChange(windowState());
      break;
    case QEvent::Close:
      m_state |= blocked; 
      closeWindow();
      break;    
    default:
      break;
    }
    return QMainWindow::event(event);
  }

 void BrowserWindow::changeEvent(QEvent * event) {
    switch (event->type()) {
      case QEvent::WindowStateChange:
        m_chrome->windowStateChange(windowState());
        break;
      default:
        break;
    }
    QMainWindow::changeEvent(event);
  }

  void BrowserWindow::handleWindowActivate() {
      if(m_changingMode) {
          // Ignore if in the process of changing from fullscreen to normal.  We get a deactivate followed by
          // an activate call when this is done.  Clear the flag for the next time activate or deactivate is
          // called for some other reason.
          m_changingMode = false;
          return;
      }
#ifdef Q_WS_MAEMO_5
      grabZoomKeys(true);
#endif
      showDecoration();
      if(m_chrome) {
          connect(m_chrome, SIGNAL(requestToggleNormalFullScreen()), this, SLOT(toggleNormalFullScreen()));
      }
  }

  void BrowserWindow::handleWindowDeactivate() {
      if(m_changingMode) {
          // Ignore if in the process of changing from fullscreen to normal.
          return;
      }
#ifdef Q_WS_MAEMO_5
      grabZoomKeys(false);
#endif
      if(!((m_state & blocked) == blocked))
        hideDecoration();
      slideViewIfRequired(-100); // hide UrlSearchBar if required
      if(m_chrome) {
          disconnect(m_chrome, SIGNAL(requestToggleNormalFullScreen()), this, SLOT(toggleNormalFullScreen()));
      }
  }

  void BrowserWindow::setMenuEnabled(bool enabled)
  {
    if(enabled)
      m_state |= menuEnabled;
    else
      m_state &= ~menuEnabled;

    fixupWindowTitle();
  }

  void BrowserWindow::setTitle(const QString &title)
  {
    if(title.isEmpty()) {
        setWindowTitle(QApplication::applicationName());
    }
    else {
        setWindowTitle(title);
    }

    fixupWindowTitle();
  }

  /// Hack to hide the menu bar arrow when the menu is disabled.
  void BrowserWindow::fixupWindowTitle() {
      QString title = windowTitle();
      title = title.trimmed();
      if(!m_windows->isMenuEnabled()) {
          // The menu is disabled, add some spaces to the title to push the down arrow out of view.
          title += QString(60, ' ');
      }
      setWindowTitle(title);
  }

  void BrowserWindow::onPageTitleChanged(const QString &title)
  {
      setTitle(title);
  }

  void BrowserWindow::onViewChanged(ControllableViewBase *newView) {
      Q_ASSERT(newView);

      if(m_windows->contentView()->currentPageIsSuperPage()) {
          if(!m_preventDisconnectPageTitleChanged)
            disconnectPageTitleChanged();
          m_preventDisconnectPageTitleChanged = true;
          m_preventConnectPageTitleChanged = false;
      }
      else {
          if(!m_preventConnectPageTitleChanged)
            connectPageTitleChanged();
          m_preventDisconnectPageTitleChanged = false;
          m_preventConnectPageTitleChanged = true;
      }
      if(m_state & active) {
          setTitle(newView->title());
      }
  }

  ViewController *BrowserWindow::viewController() {
    ViewStack *viewStack = ViewStack::getSingleton();
    if(viewStack) {
        return viewStack->getViewController();
    }

    return 0;
  }

  void BrowserWindow::connectPageTitleChanged() {
      connect(m_page->mainFrame(), SIGNAL(titleChanged(const QString &)),
              this, SLOT(onPageTitleChanged(const QString &)));
  }

  void BrowserWindow::disconnectPageTitleChanged() {
      disconnect(m_page->mainFrame(), SIGNAL(titleChanged(const QString &)),
              this, SLOT(onPageTitleChanged(const QString &)));
  }
  
  
   void BrowserWindow::toggleNormalFullScreen() {
      m_changingMode = true;

      // Block paint updates while switching modes.  This avoids ugly flicker in toolbar.
      setUpdatesEnabled(false);

      if(isFullScreen())
          showNormal();
      else {
          // Show full screen unless it's showing a super page: bookmarks, histroy etc. should always
          // be in normal mode.
          if(!m_windows->contentView()->currentPageIsSuperPage()) {
              showFullScreen();
          }
      }
      // Un-block updates.
      setUpdatesEnabled(true);
  }
  
  #ifdef Q_WS_MAEMO_5
  // Tell the system we want to handle volume key events (or not).
  void BrowserWindow::grabZoomKeys(bool grab)
  {
      if (!winId()) {
          qWarning("Can't grab keys unless we have a window id");
          return;
      }

      unsigned long val = (grab) ? 1 : 0;
      Atom atom = XInternAtom(QX11Info::display(), "_HILDON_ZOOM_KEY_ATOM", False);
      if (!atom) {
          qWarning("Unable to obtain _HILDON_ZOOM_KEY_ATOM. This example will only work "
                   "on a Maemo 5 device!");
          return;
      }

      XChangeProperty (QX11Info::display(),
              winId(),
              atom,
              XA_INTEGER,
              32,
              PropModeReplace,
              reinterpret_cast<unsigned char *>(&val),
              1);
  }

  void BrowserWindow::keyPressEvent(QKeyEvent* event)
  {
      switch (event->key()) {
      case Qt::Key_F7:
          {
              // Handle "volume down" key by triggering zoom-in.
              ControllableViewBase *view = m_chrome->getView("WebView");
              if(view) {
                  view->triggerAction("ZoomIn");
              }
              event->accept();
          }
          break;

      case Qt::Key_F8:
          {
              // Handle "volume up" key by triggering zoom-out.
              ControllableViewBase *view = m_chrome->getView("WebView");
              if(view) {
                  view->triggerAction("ZoomOut");
              }
              event->accept();
          }
          break;
      }
      QWidget::keyPressEvent(event);
  }
#endif
};
