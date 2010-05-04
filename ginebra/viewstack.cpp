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


#include "utilities.h"
#include <QtCore/QUrl>
#include "viewstack.h"
#include "viewcontroller.h"
#include "chromeview.h"
#include "chromewidget.h"
#include "chromesnippet.h"
#include "webpagecontroller.h"
#include "HistoryFlowView.h"
#include "WindowFlowView.h"


ViewStack* ViewStack::getSingleton()
{
    static ViewStack* singleton = 0;
    if(!singleton)
    {
        singleton = new ViewStack;
        singleton->setObjectName("viewStack");
    } // if(! singleton)

    assert(singleton);
    return singleton;
}

void ViewStack::fromWindowView(const QString &to)
{
    emit (deActivateWindowView());

    m_chromeView->setViewofCurrentPage();

    WRT::WindowFlowView* windowView = static_cast<WRT::WindowFlowView *>(m_viewController->getView("WindowView"));

    disconnect(windowView, SIGNAL(ok(WrtBrowserContainer*)), this, SLOT(goBackFromWindowView()));
    disconnect(windowView, SIGNAL(addPageComplete()), this, SLOT(goBackFromWindowView()));
    disconnect(windowView, SIGNAL(centerIndexChanged(int)), this, SIGNAL(pageChanged(int)));

    m_viewController->showContent(to);
}

void ViewStack::fromGoAnywhereView(const QString &to)
{
    WRT::HistoryFlowView* historyView = static_cast<WRT::HistoryFlowView*>(m_viewController->getView("historyView"));

    disconnect(historyView, SIGNAL(ok(int)), this, SLOT(loadHistoryItem(int)));

    m_viewController->showContent(to);

    disconnect(historyView, SIGNAL(activated()), this, SIGNAL(activateHistory()));
    disconnect(historyView, SIGNAL(deactivated()), this, SIGNAL(deActivateHistory()));
}

void ViewStack::fromWebView(const QString &to)
{
    m_viewController->showContent(to);
    emit (deActivateWebView());
}


void ViewStack::fromBookmarkTreeView(const QString &to)
{
    m_viewController->showContent(to);
    emit(deActivateBookmark());
}


void ViewStack::fromBookmarkHistoryView(const QString &to)
{
    m_viewController->showContent(to);
    emit(deActivateBookMarkHistory());
}

void ViewStack::toGoAnywhereView()
{
    emit (activateHistory()); // by default go to history view

    /* Connect ALL of goAnywhere's signals in one shot */

    WRT::HistoryFlowView* historyView = static_cast<WRT::HistoryFlowView*>(m_viewController->getView("historyView"));

    safe_connect(historyView, SIGNAL(ok(int)), this, SLOT(loadHistoryItem(int)))
    safe_connect(historyView, SIGNAL(activated()), this, SIGNAL(activateHistory()));
    safe_connect(historyView, SIGNAL(deactivated()), this, SIGNAL(deActivateHistory()));
}

void  ViewStack::initWindowView()
{
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

    QSize windowViewSize(width, height);
    WRT::WindowFlowView* windowView = static_cast<WRT::WindowFlowView *>(m_viewController->getView("WindowView"));
    windowView->setSize(windowViewSize);
}

void ViewStack::toWindowView()
{
    emit(activateWindowView());

    WRT::WindowFlowView* windowView = static_cast<WRT::WindowFlowView *>(m_viewController->getView("WindowView"));

    safe_connect(windowView, SIGNAL(ok(WrtBrowserContainer*)), this, SLOT(goBackFromWindowView()));
    safe_connect(windowView, SIGNAL(addPageComplete()), this, SLOT(goBackFromWindowView()));
    safe_connect(windowView, SIGNAL(centerIndexChanged(int)), this, SIGNAL(pageChanged(int)));
}

void ViewStack::toWebView()
{
    emit (activateWebView());
}

void ViewStack::toBookmarkHistoryView()
{
    emit(activateBookMarkHistory());

}

void ViewStack::toBookmarkView()
{
    emit(activateBookmark());
}


void ViewStack::switchView(const QString &to, const QString &from) {

    if (to == from) {
        return;
    }
    if (!m_viewController) {
        return;
    }

    if (from == "WindowView") {
        fromWindowView(to);
    }
    else if (from == "goAnywhereView") {
        fromGoAnywhereView(to);
    }
    else if (from == "webView") {
       if (to == "WindowView")
           initWindowView(); // to fix the size issue of windows view
       fromWebView(to);
    }
    else if (from == "bookmarkHistoryView") {
        fromBookmarkHistoryView(to);
    }
    else if (from == "bookmarkTreeView") {
        fromBookmarkTreeView(to);
    }

    if (to == "WindowView") {
        toWindowView();
    }
    else if (to == "goAnywhereView") {
        toGoAnywhereView();
    }
    else if (to == "webView") {
        toWebView();
    }
    else if (to == "bookmarkHistoryView") {
        toBookmarkHistoryView();
    }
    else if (to == "bookmarkTreeView") {
        toBookmarkView();
    }
}


void ViewStack::loadHistoryItem(int item) {

    if (!m_viewController) {
        return;
    }
    WRT::HistoryFlowView* historyView = static_cast<WRT::HistoryFlowView*>(m_viewController->getView("historyView"));
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
    switchView("webView", "goAnywhereView");
}


void ViewStack::goBackFromWindowView() {
    switchView("webView", "WindowView");
}

void ViewStack::goBackFromRecentUrlView() {
    switchView("webView", "bookmarkHistoryView");
}

void ViewStack::goBackFromBookmarkView() {
    switchView("webView", "bookmarkTreeView");
}

void ViewStack::showContentView(int progress) {

    if (!m_viewController) {
        return;
    }

    if (progress >= 30 && m_viewController->currentView()->type() == "goAnywhereView") {
        //goBackToWebView();
        goBackFromGoAnywhereView();
        disconnect(m_viewController,SIGNAL(loadProgess(const int)), this, SLOT(showContentView(const int)));
    }
    else if ( m_viewController->currentView()->type() == "webView" ) {
        disconnect(m_viewController,SIGNAL(loadProgess(const int)), this, SLOT(showContentView(const int)));
    }
}

