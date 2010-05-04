/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __WRTPAGEMANAGER_H__
#define __WRTPAGEMANAGER_H__

//#include <QWidget>
#include <QAction>
#include <QNetworkReply>
#include <QSslError>
#include <qwebframe.h>
#include <QIcon>
#include <QEvent>
#include "browserpagefactory.h"
#include "BWFGlobal.h"
#include "messageboxproxy.h"

#define WEBPAGE_ZOOM_RANGE_MIN 25
#define WEBPAGE_ZOOM_RANGE_MAX 300
#define WEBPAGE_ZOOM_PAGE_STEP 20

class QGraphicsWebView;

namespace WRT {
    class WrtPage;
    class WrtBrowserContainer;
}

using namespace WRT;

// -----------------------------------------------------------

class BWF_EXPORT WebPageControllerUpdateViewPortEvent : public QEvent {
  public:
    WebPageControllerUpdateViewPortEvent() : QEvent(staticType()) {}
    static QEvent::Type staticType() {
        static int s_type = QEvent::registerEventType();
        return (QEvent::Type)s_type;
    }
};

// -----------------------------------------------------------

class WebPageControllerPrivate;

class BWF_EXPORT WebPageController : public QObject, public BrowserPageFactory
{
    Q_OBJECT
public:
   /*!
    * WebPageController Constructor
    * @param parent: parent QObject 
    */  
    WebPageController(QObject* parent = 0);
    /*!
     * WebPageController Destructor
     */ 
    ~WebPageController();

public:

    QString currentDocTitle();
    Q_PROPERTY(QString currentDocTitle READ currentDocTitle)

    QString currentDocUrl() const;
    Q_PROPERTY(QString currentDocUrl READ currentDocUrl)

    QString currentPartialUrl();
    Q_PROPERTY(QUrl currentPartialUrl READ currentPartialUrl)

    QString currentRequestedUrl() const;
    Q_PROPERTY(QString currentRequestedUrl READ currentRequestedUrl)

    QVariant currentContentWindowObject();
    Q_PROPERTY(QVariant currentWindowObject READ currentContentWindowObject)
   
    int currentPageIndex();
    Q_PROPERTY(int currentPageIndex READ currentPageIndex)

    int contentsYPos();
    Q_PROPERTY(int contentsYPos READ contentsYPos)

    /* This indicates whether the current page is secure or not */
    int secureState();
    Q_PROPERTY(int secureState READ secureState)

    int loadState();
    Q_PROPERTY(int loadState READ loadState)

    QString  loadText();
    Q_PROPERTY(QString loadText READ loadText)

    int loadProgressValue();
    Q_PROPERTY(int loadProgressValue READ loadProgressValue)

    bool isPageLoading();
    Q_PROPERTY(bool isPageLoading READ isPageLoading)

    bool loadCanceled();
    Q_PROPERTY(bool loadCanceled READ loadCanceled)

    QWebPage* openPage();
    QWebPage* openPageFromHistory(int index);
    void closePage(WRT::WrtBrowserContainer*);

    WRT::WrtBrowserContainer* currentPage() const;
    void closeCurrentPage();

    QList<WRT::WrtBrowserContainer*>* allPages();
    WRT::WrtBrowserContainer * findPageByMainFrameName(const QString & name);

    QString title();
//    QWidget * widgetParent();

    void setOffline(bool offline);
    void initUASettingsAndData();
    
    QList<QAction*> getContext();

    // persistent storage related methods
    void saveHistory();
    void clearHistoryInMemory();
    void deleteHistory();    
    
    WRT::WrtBrowserContainer* restoreHistory(QWidget* parent, int index);
    WRT::WrtBrowserContainer* startupRestoreHistory(QWidget* parent, int index, WRT::WrtBrowserContainer* page);
    void saveNumberOfWindows();
    int restoreNumberOfWindows();
    int historyWindowCount();
    
    void setLastUrl(QString url);

    QIcon pageIcon();
    
    QGraphicsWebView* webView();
    void updatePageThumbnails();
    void resizeAndUpdatePageThumbnails(QSize& s);

private:
    void checkAndUpdatePageThumbnails();
    WRT::WrtBrowserContainer* openPage(QObject* parent, WRT::WrtBrowserContainer* page=0);

public: // public actions available for this view
    QAction * getActionReload();
    QAction * getActionStop();
    QAction * getActionBack();
    QAction * getActionForward();
    QAction * getActionWebInspector();
    
public slots:
    void setLoadState(int);
    int pageCount();
    QString getLastUrl();
    void deleteCookies();
    void deleteCache();
    void deleteDataFiles();
    void savePopupSettings(int);
    bool getPopupSettings();
    
    void currentReload();
    void currentStop();
    void currentBack();
    void currentForward();

    void currentLoad(const QString &url);
    void currentLoad(const QUrl & url);
    void currentSetFromHistory(int historyIndex);
    void gotoCurrentItem();
    void pageGotoCurrentItem(int index);
    void pageReload(int index);
    void setCurrentPage(WRT::WrtBrowserContainer*);
         	    
    void webInspector();

    static WebPageController* getSingleton();

    void urlTextChanged(QString );
    void loadFromHistory();
    void loadLocalFile();
    QString guessUrlFromString(const QString &s);

private slots:
    void settingChanged(const QString &key);
    void updateStatePageLoading();
    void updateStatePageLoadComplete(bool);
    void updateActions(bool pageIsLoading=false);
    void unsupportedContentArrived(QNetworkReply *);
    void createWindow(WrtBrowserContainer* page);
    void networkRequestFinished(QNetworkReply *reply);
    void updateJSActions();
    void urlChanged(const QUrl &url);
    void secureStateChange(int);

signals:
    void pageCreated( WRT::WrtBrowserContainer* newPage);
    void pageDeleted( WRT::WrtBrowserContainer* oldPage);
    void pageChanged( WRT::WrtBrowserContainer* oldPage, WRT::WrtBrowserContainer* newPage );

    void titleChanged(const QString &);
    void loadStarted();
    void loadProgress( const int progress );
    void loadFinished( const bool ok );

    void currentPageIconChanged();
    void currentPageUrlChanged( const QUrl & url);
    void partialUrlChanged(QString url);
    void unsupportedContent(QNetworkReply *);

    void networkRequestStarted(QWebFrame*, QNetworkRequest*);
    void networkRequestError(QNetworkReply *reply);
    void sslErrors(QNetworkReply *, const QList<QSslError> &);
    void showMessageBox(WRT::MessageBoxProxy*);

    void linkClicked( const QString & url );
    void selectionChanged();

    void pageScrollPositionZero();
    void pageScrollRequested(int, int, const QRect & );

    void showSecureIcon();
    void hideSecureIcon();

    // All signals for urlsearch 
    void pageLoadStarted();
    void pageLoadProgress( const int progress );
    void pageLoadFinished( const bool ok );
    void pageUrlChanged( const QString str );
    void pageIconChanged();
 
private:
    QString partialUrl(const QUrl &url);

    WebPageControllerPrivate * const d;
};
#endif // __WRTPAGEMANAGER_H__
