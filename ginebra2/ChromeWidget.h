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
*
*/

#ifndef __GINEBRA_CHROMEWIDGET_H__
#define __GINEBRA_CHROMEWIDGET_H__

#include "BWFGlobal.h"
#include <QMap>
#include <QtGui>
//#include <QtScript>
#include "wrtbrowsercontainer.h"
#include "ViewController.h"

class QWebPage;
class QPixmap;

class MostVisitedPageStore;
class ControllableViewBase;
class QGraphicsSceneContextMenuEvent;
using namespace WRT;

namespace GVA {

  class ChromeSnippet;
  class ChromeDOM;
  class ChromeRenderer;
  class GinebraApplication;
  class ViewController;
  class Snippets;
  //  class CollapsingWidget;
  class SlidingWidget;
  class ChromeWidgetJSObject;
  class LocaleDelegate;
  class DeviceDelegate;
  class NetworkDelegate;
  class Downloads;

  enum ChromeAnchor
  {
    anchorNone,
    anchorCenter,
    anchorTop,
    anchorBottom,
    anchorLeft,
    anchorRight,
    anchorTopLeft,
    anchorTopRight,
    anchorBottomLeft,
    anchorBottomRight
  };

  enum Aspect
  {
    portrait,
    landscape
  };

  /*!
   * \brief This class is responsible for maintaining the browser chrome and
   * most of the other major components of the application.
   *
   */
  class ChromeWidget : public QGraphicsWidget
  {

  Q_OBJECT

  public:
    ChromeWidget(QGraphicsItem * parent = 0, Qt::WindowFlags wFlags = 0);
    virtual ~ChromeWidget();
    void setChromeBaseDirectory(const QString dir);
    void setChromeFile(const QString file);
    void addView(ControllableViewBase * controllableView);
    QGraphicsScene * getScene() { return m_scene; } //NB: change name to scene() !!
    void setScene(QGraphicsScene *scene);
    ChromeRenderer * renderer() { return m_renderer; }
    ChromeDOM * dom() { return m_dom; }
    QRect getSnippetRect(const QString &docElementId);
    ChromeSnippet * getSnippet(const QString & docElementId, QGraphicsItem * parent = 0);
    //  QScriptValue evalWithEngineContext(const QString& program);
    ControllableViewBase * getView(const QString& name);
    void showView(const QString &name);
    QWebPage * page() { return m_page; }
    void anchorSnippet(ChromeSnippet * snippet);
    //NB: get rid of this (see comment in .cpp)!!!
    void adjustAnchorOffset(ChromeSnippet * snippet, qreal delta);
    void anchorToView(ChromeSnippet* snippet, const QString & where = "top");
    void detachFromView(ChromeSnippet* snippet, const QString & where = "top");
    void anchorTogether(ChromeSnippet* first, const QString &  secondId, qreal x = 0, qreal y = 0);
    void unAnchor(ChromeSnippet* first);
    //void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget* widget = 0);
    QGraphicsView *graphicsView() { return m_view; }
    void dump();
    QString getBaseDirectory() const { return m_baseDirectory; }
    ViewController * viewController() { return m_viewController; } // needed for UrlSearchSnippet
    QString currentView() {return m_viewController->currentView()->type() ;}
    int bottomBarHeight() { return m_bottomBarHeight;}
    void updateChromeLayout();
    void sizeChange(QSize sz){ emit prepareForSizeChange(sz) ;}
    void emitPopupShown(const QString &popupId);
    void emitPopupHidden(const QString &popupId);

  public slots:
    int width(){return (int)size().width();}
    void snippetShown(ChromeSnippet * snippet);
    void snippetHiding(ChromeSnippet * snippet);
    void loadStarted();
    void loadFinished(bool ok);
    void exportJSObjects();
    void exportJSObjectsToPage(QWebPage *page);
    void alert(const QString & msg);
    qreal slideView(qreal delta);
    qreal shrinkView(qreal delta);
    void onViewInstantiated(ControllableViewBase *view);
    void onCurrentViewChanged();
    void chromeInitialized();
    void reloadChrome();
    void loadUrlToCurrentPage(const QUrl & url);
    void pageCreated(WRT::WrtBrowserContainer * page);

  signals:
    void internalChromeComplete();
    void chromeComplete();
    void aspectChanged(int aspect);
    void prepareForGeometryChange();
    void prepareForSizeChange(QSize);
    //NB: This should be symbian ifdef'd but that would require symbian-specific chrome
    void symbianCarriageReturn();
    void popupShown(const QString &id);
    void popupHidden(const QString &id);

  public:
    Q_PROPERTY(QObjectList snippets READ getSnippets)
    QObjectList getSnippets();
    /// \brief Returns either "portrait" or "landscape".
    QString getDisplayMode() const;
    Q_PROPERTY(QString displayMode READ getDisplayMode)

    QObject* getDisplaySize() const;
    Q_PROPERTY(QObject* displaySize READ getDisplaySize)

  protected:
    virtual void resizeEvent(QGraphicsSceneResizeEvent *ev);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
    //bool eventFilter(QObject *obj, QEvent *ev);
  private:
    void addSnippet( ChromeSnippet * snippet, const QString & docElementId);
    void getInitialSnippets();
    void clearChrome();
    void addAnchors();
    void addJSObjectToWindow(QObject *object);
    void addJSObjectToPage(QObject *object, QWebPage *page);
    void updateMVGeometry();
    void addViewToLayout(ControllableViewBase * controllableView);

    //void addJSObjectToEngine(QObject *object);
    QGraphicsScene * m_scene;
    QGraphicsView *m_view;
    QGraphicsAnchorLayout *m_layout;
    QString m_baseDirectory;
    QWebPage * m_page;
    ChromeRenderer * m_renderer;
    ChromeDOM * m_dom;
    Snippets * m_snippets;
    ViewController *m_viewController;
    //QList<QObject*> m_jsObjects;
    //QScriptEngine m_engine;
    QGraphicsAnchorLayout *m_viewLayout;
    SlidingWidget *m_viewPort;
    QGraphicsWidget *m_topBar;
    QGraphicsWidget *m_bottomBar;
    QGraphicsWidget *m_leftBar;
    QGraphicsWidget *m_rightBar;
    //CollapsingWidget *m_viewBar;
    GinebraApplication *m_app;
    Aspect m_aspect;
    ChromeWidgetJSObject *m_jsObject;
    LocaleDelegate *m_localeDelegate; // Owned
    DeviceDelegate *m_deviceDelegate;
    NetworkDelegate *m_networkDelegate;
    Downloads * m_downloads; // Owned
    int m_bottomBarHeight;
  };

} // end of namespace GVA

Q_DECLARE_METATYPE(QObjectList)

#endif // __GINEBRA_CHROMEWIDGET_H__
