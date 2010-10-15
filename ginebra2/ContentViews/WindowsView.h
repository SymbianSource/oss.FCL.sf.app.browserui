/*
 * WindowsView.h
 *
 *  Created on: Jun 11, 2010
 *      Author: lewontin
 */

#ifndef WINDOWSVIEW_H
#define WINDOWSVIEW_H_

#include "controllableviewimpl.h"
#include <QMenu>

namespace WRT {
  class WrtBrowserContainer;
}

namespace GVA {

class BrowserWindow;
class ChromeWidget;
class GWebContentView;

class WindowsView : public ControllableViewBase
{
  Q_OBJECT
      
  public:
    WindowsView(ChromeWidget * chrome, QObject * parent = 0, const QString &objectName = QString::null);
    virtual ~WindowsView();
    //Reimplement ControllableViewBase methods
    static QString Type() { return "windowView"; }
    virtual QString type() const { return Type(); }
    QGraphicsWidget* widget() const { return 0; }
    virtual QList<QAction*> getContext(){ return QList<QAction*>(); }
    virtual void show(){;}
    virtual void hide(){;}
    virtual void activate(BrowserWindow * window);
    virtual void deactivate(BrowserWindow * window);
    //WindowView methods
    BrowserWindow * addWindow(WRT::WrtBrowserContainer * page);
    BrowserWindow * currentWindow(){ return m_current; }
    void addPage();
    void handlePageEvents(bool handle);
    void setMenu(QMenu * menu);
    void setMenuEnabled(bool enabled);
    bool isMenuEnabled() const;
    GWebContentView * contentView();
  public slots:
    void onPageCreated(WRT::WrtBrowserContainer * page);
    void onPageDeleted(WRT::WrtBrowserContainer * page);
    void onWindowAction(); //Handle menu add window action
  private:
    QMap<WRT::WrtBrowserContainer*, BrowserWindow*>  m_windows;
    BrowserWindow * m_current;
    ChromeWidget * m_chrome;
    QMenu * m_menu; //Shared menu, not owned
};
}
#endif /* WINDOWSVIEW_H_ */
