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


#ifndef GWebContentView_H
#define GWebContentView_H

#include <QObject>
#include <QVariant>
#include <QGraphicsWidget>
#include <qwebelement.h>
#include "qgraphicswebview.h"
#include "controllableviewimpl.h"
#include "messageboxproxy.h"
#include "ZoomMetaData.h"
#include "GWebPage.h"
#include "ContentViewDelegate.h"
#include "GWebTouchNavigation.h"

class WebViewEventContext;
class QContextMenuEvent;
class QWebPage;
class QWebFrame;
class QTimeLine;
class GWebContentViewWidget;
namespace WRT {
    class WrtBrowserContainer;
}

namespace GVA {

  class GWebPage;
  class ChromeWidget;
  class ContentViewDelegate;

  class GWebContentView : public ControllableViewBase
  {
      Q_OBJECT
  public:
      GWebContentView(ChromeWidget *chrome, QObject * parent = 0, const QString &objectName = QString::null);
      virtual ~GWebContentView();

      static QString Type() { return "webView"; }
      virtual QString type() const { return Type(); }

// do we need both of these?
      QGraphicsWidget* widget() const;
      GWebContentViewWidget *webWidget() const;

      // Returns the DOM 'window' object of the page.
      QVariant getContentWindowObject();

      virtual void connectAll();
      virtual void activate();
      virtual void deactivate();

      virtual void triggerAction(const QString & action);

      virtual QString title() const;

      QUrl url();
      QWebPage *currentPage();

      qreal getZoomFactor() const;

      static ControllableView* createNew(QWidget *parent);

      /*!
        Return the list of public QActions most relevant to the view's current context
        (most approptiate for contextual menus, etc.
      */
      virtual QList<QAction*> getContext();
      virtual QAction * getAction(const QString & action);

      QWebPage* wrtPage();
      void scrollViewBy(int dx, int dy);
      void scrollViewTo(int x, int y);

      void  changeZoomAction(qreal zoom);
      void deactivateZoomActions();

      // Super page methods.
      GWebPage * createSuperPage(const QString &name);
      void destroySuperPage(const QString &name);
      QObjectList getSuperPages();
      void setCurrentSuperPage(const QString &name);
      GWebPage * currentSuperPage() {return m_currentSuperPage.value();}
      void showSuperPage(const QString &name);
      GWebPage * superPage(const QString &name);
      bool isSuperPage(const QString &name);
      bool currentPageIsSuperPage() const;
      void bitmapZoomStop();
      virtual void show() {
          qDebug() << "GWebContentView::show: " << widget();
          widget()->show();
      }

      virtual void hide() {
          qDebug() << "GWebContentView::hide: " << widget();
          widget()->hide();
      }

	  bool gesturesEnabled() const { return m_touchNavigation->enabled(); }
      void setGesturesEnabled(bool value) { m_touchNavigation->setEnabled(value); }

  signals:
      void ContextChanged();
      void iconChanged();
      void loadFinished(bool ok);
      void loadProgress(int progress);
      void loadStarted();
      void statusBarMessage(const QString & text);
      void titleChanged(const QString & title);
      void urlChanged(const QString & url);
      void secureConnection(bool secure);
      void backEnabled(bool enabled);
      void forwardEnabled(bool enabled);
      void startingPanGesture(int);
      void contentViewMouseEvent(QEvent::Type type);

  public slots:
      void loadUrlToCurrentPage(const QString & url);
      void zoomIn(qreal factor = 0.1);
      void zoomOut(qreal factor = 0.1);
      void setZoomFactor(qreal factor);
      void showMessageBox(WRT::MessageBoxProxy* data);
      void zoomP();
      void zoomN();
      void stop();
      void back();
      void forward();
      void reload();
      void zoomBy(qreal delta) { zoomIn(delta); }
      void zoom(bool in);
      void toggleZoom();
      void stopZoom();
      void scrollBy(int deltaX, int deltaY) { scrollViewBy(deltaX, deltaY); }
      int scrollX();
      int scrollY();
      int contentWidth();
      int contentHeight();

      // Show the current normal web page, ie. not a super page.
      void showNormalPage();

      void dump();

  private slots:
    void updateZoom(qreal delta);
    void onLoadStarted();
    void onLoadFinished(bool ok);
    void onUrlChanged(const QUrl& url);

    // Called by the page controller when it creates a page.
    void pageCreated(WRT::WrtBrowserContainer * pg);
    // Called by the page controller when changes a page.
    void pageChanged(WRT::WrtBrowserContainer * , WRT::WrtBrowserContainer *);

  protected:
    GWebContentViewWidget *webWidgetConst() const { return m_widget; }
    ChromeWidget *chrome() { return m_chrome; }
    void updateWebPage(WRT::WrtBrowserContainer * pg);
    void changeContentViewZoomInfo(WRT::WrtBrowserContainer* newPage);
    
  protected:
    GWebContentViewWidget *m_widget;
    QNetworkAccessManager *m_networkMgr; //Owned

  private:
    void setZoomActions();

    ChromeWidget *m_chrome;  // not owned
    QAction * m_actionZoomIn;
    QAction * m_actionZoomOut;
    QTimeLine * m_timeLine;
    bool m_zoomIn;

    GWebTouchNavigation* m_touchNavigation;
    bool m_backEnabled;
    bool m_forwardEnabled;

    ChromeWidget *m_chromeWidget;  // not owned
    typedef QMap<QString, GWebPage *> PageMap;
    PageMap m_superPages;
    PageMap::iterator m_currentSuperPage;
    bool m_currentPageIsSuperPage;
    QTimer *m_timer;
    qreal m_value;
    bool m_gesturesEnabled;
  };

}

#endif // GWebContentView_H
