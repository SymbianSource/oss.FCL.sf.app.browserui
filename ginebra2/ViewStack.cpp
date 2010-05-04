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


#include "Utilities.h"
#include <QtCore/QUrl>
#include "ViewStack.h"
#include "ViewController.h"
#include "ChromeWidget.h"
#include "webpagecontroller.h"
#include "HistoryFlowView.h"
#include "BookmarksTreeView.h"
#include "BookMarksHistoryView.h"
#include "WindowFlowView.h"

namespace GVA {

static const QString KBookmarkHistoryViewName = "BookmarkHistoryView";
static const QString KBookmarkTreeViewName = "BookmarkTreeView";
static const QString KGoAnywhereViewName = "GoAnywhereView";
static const QString KWebViewName = "WebView";
static const QString KWindowViewName = "WindowView";
static const QString KHistoryViewName = "HistoryView";
static const QString KSettingsViewName = "SettingsView";


ViewStack* ViewStack::getSingleton()
{
    static ViewStack* singleton = 0;
    if(!singleton)
    {
        singleton = new ViewStack;
        singleton->setObjectName("ViewStack");
    } // if(! singleton)

    assert(singleton);
    return singleton;
}

void ViewStack::fromWindowView(const QString &to)
{
    emit (deActivateWindowView());

    //m_chromeView->setViewofCurrentPage();

    WRT::WindowFlowView* windowView = static_cast<WRT::WindowFlowView *>(m_viewController->view(KWindowViewName));

    disconnect(windowView, SIGNAL(ok(WrtBrowserContainer*)), this, SLOT(goBackFromWindowView()));
    disconnect(windowView, SIGNAL(addPageComplete()), this, SLOT(goBackFromWindowView()));
    disconnect(windowView, SIGNAL(centerIndexChanged(int)), this, SIGNAL(pageChanged(int)));

    m_viewController->showContent(to);
}

void ViewStack::fromSettingsView(const QString &to)  // goto WebView
{
    emit (deActivateSettingsView());    
    emit (currentViewChanged());
    m_viewController->showContent(to);
}

void ViewStack::fromGoAnywhereView(const QString &to)
{
    WRT::HistoryFlowView* historyView = static_cast<WRT::HistoryFlowView*>(m_viewController->view(KHistoryViewName));
    WRT::BookmarksTreeView* bookmarkTreeView = static_cast<WRT::BookmarksTreeView *>(m_viewController->view(KBookmarkTreeViewName));
    WRT::BookmarksHistoryView* bookmarkHistoryView = static_cast<WRT::BookmarksHistoryView *>(m_viewController->view(KBookmarkHistoryViewName));

    disconnect(historyView, SIGNAL(ok(int)), this, SLOT(loadHistoryItem(int)));
    disconnect(bookmarkTreeView, SIGNAL(openUrl(const QUrl &)), this, SLOT(goBackFromGoAnywhereView()));
    disconnect(bookmarkHistoryView, SIGNAL(openUrl(const QUrl &)), this, SLOT(goBackFromGoAnywhereView()));

    m_viewController->showContent(to);

    disconnect(bookmarkTreeView, SIGNAL(activated()), this, SIGNAL(activateBookmark()));
    disconnect(bookmarkTreeView, SIGNAL(deactivated()), this, SIGNAL(deActivateBookmark()));
    disconnect(historyView, SIGNAL(activated()), this, SIGNAL(activateHistory()));
    disconnect(historyView, SIGNAL(deactivated()), this, SIGNAL(deActivateHistory()));
    disconnect(bookmarkHistoryView, SIGNAL(activated()), this, SIGNAL(activateBookMarkHistory()));
    disconnect(bookmarkHistoryView, SIGNAL(deactivated()), this, SIGNAL(deActivateBookMarkHistory()));
}

void ViewStack::fromWebView(const QString &to)
{
    m_viewController->showContent(to);
    emit (deActivateWebView());
}


void ViewStack::fromBookmarkTreeView(const QString &to)
{
    //m_viewController->showContent(to);
    //WRT::BookmarksTreeView* bookmarkTreeView = static_cast<WRT::BookmarksTreeView *>(m_viewController->view(KBookmarkTreeViewName));

    //disconnect(bookmarkTreeView, SIGNAL(openUrl(const QUrl &)), this, SLOT(goBackFromBookmarkView()));
    m_viewController->viewChanged();
    
    emit(deActivateBookmark());
}


void ViewStack::fromBookmarkHistoryView(const QString &to)
{
    //m_viewController->showContent(to);
    //WRT::BookmarksHistoryView* bookmarkHistoryView = static_cast<WRT::BookmarksHistoryView *>(m_viewController->view(KBookmarkHistoryViewName));

    //disconnect(bookmarkHistoryView, SIGNAL(openUrl(const QUrl &)), this, SLOT(goBackFromRecentUrlView()));
    m_viewController->viewChanged();

    emit(deActivateBookMarkHistory());
}

void ViewStack::toGoAnywhereView()
{
    emit (activateHistory()); // by default go to history view

    /* Connect ALL of goAnywhere's signals in one shot */

    WRT::HistoryFlowView* historyView = static_cast<WRT::HistoryFlowView*>(m_viewController->view(KHistoryViewName));
    WRT::BookmarksTreeView* bookmarkTreeView = static_cast<WRT::BookmarksTreeView *>(m_viewController->view(KBookmarkTreeViewName));
    WRT::BookmarksHistoryView* bookmarkHistoryView = static_cast<WRT::BookmarksHistoryView *>(m_viewController->view(KBookmarkHistoryViewName));

    safe_connect(historyView, SIGNAL(ok(int)), this, SLOT(loadHistoryItem(int)))
    safe_connect(bookmarkTreeView, SIGNAL(openUrl(const QUrl &)), this, SLOT(goBackFromGoAnywhereView()))
    safe_connect(bookmarkHistoryView, SIGNAL(openUrl(const QUrl &)), this, SLOT(goBackFromGoAnywhereView()));
    safe_connect(bookmarkTreeView, SIGNAL(activated()), this, SIGNAL(activateBookmark()));
    safe_connect(bookmarkTreeView, SIGNAL(deactivated()), this, SIGNAL(deActivateBookmark()));
    safe_connect(historyView, SIGNAL(activated()), this, SIGNAL(activateHistory()));
    safe_connect(historyView, SIGNAL(deactivated()), this, SIGNAL(deActivateHistory()));
    safe_connect(bookmarkHistoryView, SIGNAL(activated()), this, SIGNAL(activateBookMarkHistory()));
    safe_connect(bookmarkHistoryView, SIGNAL(deactivated()), this, SIGNAL(deActivateBookMarkHistory()));
}

void  ViewStack::initWindowView()
{
    /*
    int width = m_chromeView->size().width();
    int height = m_chromeView->size().height();
    // FIXME: the snippet id is hardcode
    ChromeSnippet* visibleSnippet = m_chromeView->getChromeWidget()->getSnippet("StatusBarChromeId");
    if (visibleSnippet)
        height -= (int) (visibleSnippet->rect().height()); 

    visibleSnippet = m_chromeView->getChromeWidget()->getSnippet("WebViewToolbarId");
    if (visibleSnippet)
        height -= (int) (visibleSnippet->rect().height()); 

    if (m_chromeView->displayMode() ==  ChromeView::DisplayModePortrait) {
        visibleSnippet = m_chromeView->getChromeWidget()->getSnippet("WindowCountBarId");
        if (visibleSnippet)
            height -= (int) (visibleSnippet->rect().height()); 
    }
    QSize windowViewSize(100,100);
    WRT::WindowFlowView* windowView = static_cast<WRT::WindowFlowView *>(m_viewController->view(KWindowViewName));
    windowView->setSize(windowViewSize);
*/
}


void ViewStack::toWindowView()
{
    emit(activateWindowView());

    WRT::WindowFlowView* windowView = static_cast<WRT::WindowFlowView *>(m_viewController->view(KWindowViewName));

    safe_connect(windowView, SIGNAL(ok(WrtBrowserContainer*)), this, SLOT(goBackFromWindowView()));
    safe_connect(windowView, SIGNAL(addPageComplete()), this, SLOT(goBackFromWindowView()));
    safe_connect(windowView, SIGNAL(centerIndexChanged(int)), this, SIGNAL(pageChanged(int)));
}

void  ViewStack::initSettingsView()
{
}

void ViewStack::toSettingsView()
{
    emit(activateSettingsView());    
    emit(currentViewChanged());
}

void ViewStack::toWebView()
{
    emit (activateWebView());
}

void ViewStack::toBookmarkHistoryView()
{
    emit(activateBookMarkHistory());

    m_viewController->viewChanged();
    //WRT::BookmarksHistoryView* bookmarkHistoryView = static_cast<WRT::BookmarksHistoryView *>(m_viewController->view(KBookmarkHistoryViewName));

    //safe_connect(bookmarkHistoryView, SIGNAL(openUrl(const QUrl &)), this, SLOT(goBackFromRecentUrlView()));
}

void ViewStack::toBookmarkView()
{
    emit(activateBookmark());
    
    m_viewController->viewChanged();

    //WRT::BookmarksTreeView* bookmarkTreeView = static_cast<WRT::BookmarksTreeView *>(m_viewController->view(KBookmarkTreeViewName));

    //safe_connect(bookmarkTreeView, SIGNAL(openUrl(const QUrl &)), this, SLOT(goBackFromBookmarkView()));
}


void ViewStack::switchView(const QString &to, const QString &from) {
    qDebug() << "ViewStack::switchView: " << to << " " << from;

    if (to == from) {
        return;
    }
    if (!m_viewController) {
        return;
    }

    if (from == KWindowViewName) {
        fromWindowView(to);
    }
    else if (from == KGoAnywhereViewName) {
        fromGoAnywhereView(to);
    }
    else if (from == KWebViewName) {
       if (to == KWindowViewName)
           initWindowView(); // to fix the size issue of windows view
       fromWebView(to);
    }
    else if (from == KBookmarkHistoryViewName) {
        fromBookmarkHistoryView(to);
    }
    else if (from == KBookmarkTreeViewName) {
        fromBookmarkTreeView(to);
    }
    else if (from == KSettingsViewName) {
        fromSettingsView(to);
    }

    if (to == KWindowViewName) {
        toWindowView();
    }
    else if (to == KGoAnywhereViewName) {
        toGoAnywhereView();
    }
    else if (to == KWebViewName) {
        toWebView();
    }
    else if (to == KBookmarkHistoryViewName) {
        toBookmarkHistoryView();
    }
    else if (to == KBookmarkTreeViewName) {
        toBookmarkView();
    }
    else if (to == KSettingsViewName) {
        toSettingsView();
    }
}


void ViewStack::loadHistoryItem(int item) {

    if (!m_viewController) {
        return;
    }
    WRT::HistoryFlowView* historyView = static_cast<WRT::HistoryFlowView*>(m_viewController->view(KHistoryViewName));
    //var myIndex = window.viewManager.historyView.currentHistIndex;
    int myIndex = historyView->currentIndex();

    if (myIndex != item ) {
        //window.chrome.alert("loadFromHistory");
        WebPageController::getSingleton()->currentSetFromHistory(item);
        safe_connect(m_viewController,SIGNAL(loadProgess(const int)), this, SLOT(showContentView(const int)));
    }
    else {
        //window.chrome.alert("Just go back");
        goBackFromGoAnywhereView();
    }
}

void ViewStack::goBackFromGoAnywhereView() {
    switchView(KWebViewName, KGoAnywhereViewName);
}


void ViewStack::goBackFromWindowView() {
    switchView(KWebViewName, KWindowViewName);
}

void ViewStack::goBackFromRecentUrlView() {
    switchView(KWebViewName, KBookmarkHistoryViewName);
}

void ViewStack::goBackFromBookmarkView() {
    switchView(KWebViewName, KBookmarkTreeViewName);
}

void ViewStack::goBackFromSettingsView() {
    switchView(KWebViewName, KSettingsViewName);
}

void ViewStack::showContentView(int progress) {

    if (!m_viewController) {
        return;
    }

    if (progress >= 30 && m_viewController->currentView()->type() == KGoAnywhereViewName) {
        //goBackToWebView();
        goBackFromGoAnywhereView();
        disconnect(m_viewController,SIGNAL(loadProgess(const int)), this, SLOT(showContentView(const int)));
    }
    else if ( m_viewController->currentView()->type() == KWebViewName ) {
        disconnect(m_viewController,SIGNAL(loadProgess(const int)), this, SLOT(showContentView(const int)));
    }
}

}  // end of namespace

