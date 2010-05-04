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


#ifndef CHROMEVIEW_H
#define CHROMEVIEW_H
#include <QtGui>
#include "wrtbrowsercontainer.h"
#include "bedrockprovisioning.h"

class ChromeJSObject;
class ChromeSnippet;
class ChromeWidget;
class GraphicsItemRotater;
class QNetworkAccessManager;
class QResizeEvent;
class QWebView;
class ControllableView;
class ViewController;

class LocaleDelegate;
class DownloadController;
class DeviceDelegate;
namespace WRT {
    class WebNavigation;
};

using namespace WRT;

/**
 * \brief Provides the main view of the browser.
 * 
 * The ChromeView class provides the main view of the browser including the chrome
 * and the content view.
 */
class ChromeView : public QGraphicsView
{
    Q_OBJECT
  public:
    ChromeView(const QString chromeUrl, QWidget *parent);
    ChromeView(QWidget *parent);
    ~ChromeView();

    void show(const QString& id, int x , int y );
    void show(const QString& id);
    void hide(const QString& id);
    void setAnchor(const QString& id, const QString& anchor);
    void toggleVisibility(const QString& id);
    void setLocation(const QString& id, int x, int y);
    void toggleAttention(const QString& id);
    void setVisibilityAnimator(const QString& elementId, const QString & animatorName);
    void setAttentionAnimator(const QString& elementId, const QString & animatorName);
    void flipToCurrentView(const QString& fromId);
    void flipFromCurrentView(const QString& toId);
    void updateViewPort();
    void setViewPort(QRect viewPort);
    ControllableView *currentContentView();
    ControllableView *contentView(const QString & type);
    ChromeWidget * getChromeWidget();
    QGraphicsScene *getScene() {return m_graphicsScene;}
    void setBackgroundColor(const QColor &color);
    void resizeScrollArea(QResizeEvent *e);
	QRect contentViewGeometry() const;

    typedef enum {DisplayModePortrait = 0, DisplayModeLandscape } DisplayMode;
    DisplayMode displayMode() const { return m_displayMode; }
  	QString getDisplayMode() {return displayMode() == ChromeView::DisplayModePortrait ? "portrait" : "landscape";}

    void updateChildGeometries(const QRect &rect);
    void setViewofCurrentPage();
    static QString getChromePath();

    virtual bool viewportEvent(QEvent* event);
    
  public slots:
    void loadChrome(const QString &url);
    QString chromeUrl() const { return m_chromeUrl; }
    void setChromeUrl(const QString &url) { m_chromeUrl = url; }
    void reloadChrome();
    void loadStarted();
    void loadFinished(bool ok);
    void setContentView();
    void contentViewChanging();
    void pageCreated(WRT::WrtBrowserContainer * pg);
    void pageChanged(WRT::WrtBrowserContainer * , WRT::WrtBrowserContainer *);

    void saveToHistory(bool);
    void loadContentView();
    void loadUrltoCurrentPage(const QUrl & url);
    static QString getChromeBaseDir();
         

  signals:
    void requestChrome(const QString &url);
    void chromeLoadFinished();
    void viewPortResize(int x, int y, int w, int h);

  protected slots:
    void injectJSObjects();
    void chromeLoaded();
    void connectContentView();
    void connectAll();
    void updateContentGeometry(const QRect &rect);

  protected:    
    void init(const QString chromeUrl);
    void initViewController();
    void resizeEvent(QResizeEvent *e);
    void injectJSObject(QObject *object);
    void updateDisplayMode();
    void setDisplayMode(DisplayMode mode);
    void updateSceneSize(const QSize &size);
    void displayModeChangeStart();

    /// Show a splashscreen while chrome is loading.
		void showSplashScreen();
    void updateWebPage(WRT::WrtBrowserContainer* pg);

    void changeContentViewZoomInfo(WRT::WrtBrowserContainer* newPage);
 
  protected:
    QGraphicsScene *m_graphicsScene; //Owned
    ChromeWidget *m_chromeWidget; //Owned
    ViewController *m_viewController; // Owned
    ChromeJSObject* m_js; //Owned
    QGraphicsWidget *m_mainWidget; //Ownership passed to scene
    DisplayMode m_displayMode;
    QString m_chromeUrl;
	QLabel *m_splashScreen;  // Owned
    WebNavigation *m_navigation;
    LocaleDelegate *m_localeDelegate; // Owned
	DownloadController *m_downloadController; // Owned
    DeviceDelegate *m_deviceDelegate; // Owned
};

class GiWidget : public QMainWindow
{
	Q_OBJECT
	public:
	 void resizeEvent(QResizeEvent *e){cw->resize(e->size());}
	 void setChromeView(ChromeView *p){cw = p;}	
	private: 
	 ChromeView *cw;
};

#endif // CHROMEVIEW_H

