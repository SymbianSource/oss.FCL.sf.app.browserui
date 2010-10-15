/*
 * BrowserWindow.h
 *
 */

#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H_

#include <QMainWindow>
#include "wrtbrowsercontainer.h"

class QGraphicsWidget;
class QGraphicsScene;
class QGraphicsItem;

class ControllableViewBase;

namespace GVA {

class ChromeLayout;
class ChromeWidget;
class ChromeView;
class WindowsView;
class ViewController;

class BrowserWindow : public QMainWindow
{      

 Q_OBJECT

 enum {
   blocked = 1,
   active = 2,
   menuEnabled = 4
 };

 public:
  BrowserWindow(ChromeWidget * chrome, WindowsView * windows, QWidget * parent = 0, Qt::WindowFlags flags = 0 );
  virtual ~BrowserWindow() { ; }
  void setPage(WRT::WrtBrowserContainer * page);
  void closeWindow();
  void toggleDecoration(); 
  void showDecoration();
  void hideDecoration();
  const QString & view() { return m_viewName; }
  void setView(const QString & view) { m_viewName = view; }
  void setMenuEnabled(bool enabled);
  void setTitle(const QString &title);
 protected slots:
  void onViewChanged(ControllableViewBase *newView);
  void onPageTitleChanged(const QString &title);
  void toggleNormalFullScreen();
 protected:
  ViewController *viewController();
  virtual bool event(QEvent * event);
  virtual void changeEvent(QEvent * event);
  void connectPageTitleChanged();
  void disconnectPageTitleChanged();
  void fixupWindowTitle();
  void slideViewIfRequired(int value); // hide or show UrlSearchBar if required
  void handleWindowActivate();
  void handleWindowDeactivate();
  
  #ifdef Q_WS_MAEMO_5
  void grabZoomKeys(bool grab);
  virtual void keyPressEvent(QKeyEvent* event);
	#endif
	
 private:
  WRT::WrtBrowserContainer * m_page; //Not owned
  WRT::WrtBrowserContainer * m_specialPage;
  bool m_decorated;
  ChromeWidget * m_chrome; //Shared, not owned
  ChromeView * m_view;
  QGraphicsScene * m_scene;
  QGraphicsItem * m_snapshot;
  WindowsView * m_windows;
  int m_state;
  QString m_viewName;  
  static bool m_preventDisconnectPageTitleChanged;
  static bool m_preventConnectPageTitleChanged;
  bool m_slidedUp;
  bool m_changingMode;
};
}
#endif /* BROWSERWINDOW_H_ */
