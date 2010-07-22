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

#include "UrlSearchSnippet.h"
#include "Utilities.h"

#include "ChromeRenderer.h"
#include "ChromeLayout.h"
#include "ChromeWidget.h"
#include "PageSnippet.h"
#include "ViewController.h"
#include "ViewStack.h"
#include "WebChromeSnippet.h"
#include "LoadController.h"
#include "webpagecontroller.h"
#include "GWebContentView.h"
#include "WindowFlowView.h"

#include <QWebHistoryItem>

namespace GVA {

#define GO_BUTTON_ICON ":/chrome/bedrockchrome/urlsearch.snippet/icons/go_btn.png"
#define STOP_BUTTON_ICON ":/chrome/bedrockchrome/urlsearch.snippet/icons/stop_btn.png"
#define REFRESH_BUTTON_ICON ":/chrome/bedrockchrome/urlsearch.snippet/icons/refresh_btn.png"
#define BETWEEN_ENTRY_AND_BUTTON_SPACE 4

GUrlSearchItem::GUrlSearchItem(ChromeSnippet * snippet, ChromeWidget * chrome, QGraphicsItem * parent)
: NativeChromeItem(snippet, parent)
, m_chrome(chrome)
, m_viewPortWidth(0.0)
, m_viewPortHeight(0.0)
, m_pendingClearCalls(0)
, m_backFromNewWinTrans(false)
, m_justFocusIn(false)
{
    // Extract style information from element CSS.

    // For border-related properties, we constrain all values (top, left, etc.)
    // to be the same.  These can be set using the css shorthand (e.g. padding:10px),
    // but the computed css style will be for the four primitive values (padding-top,
    // padding-left) etc, which will all be equal.  Hence we just use one of the
    // computed primitive values (top) to represent the common value.

    QWebElement we = m_snippet->element();

    QColor textColor;
    NativeChromeItem::CSSToQColor(
            we.styleProperty("color", QWebElement::ComputedStyle),
            textColor);

    QColor backgroundColor;
    NativeChromeItem::CSSToQColor(
            we.styleProperty("background-color", QWebElement::ComputedStyle),
            backgroundColor); // FIXME text edit background color doesn't work

    QColor progressColor;
    NativeChromeItem::CSSToQColor(
            we.styleProperty("border-bottom-color", QWebElement::ComputedStyle),
            progressColor); //FIXME text-underline-color causes the crash

    NativeChromeItem::CSSToQColor(
            we.styleProperty("border-top-color", QWebElement::ComputedStyle),
            m_borderColor);

    QString cssPadding = we.styleProperty("padding-top", QWebElement::ComputedStyle);
    m_padding = cssPadding.remove("px").toInt();

    QString cssBorder = we.styleProperty("border-top-width", QWebElement::ComputedStyle);
    m_border = cssBorder.remove("px").toInt();

    // Create the view port widget
    m_viewPort = new QGraphicsWidget(this);
    m_viewPort->setFlags(QGraphicsItem::ItemClipsChildrenToShape);

    // Create the url search editor
    m_urlSearchEditor = new GProgressEditor(snippet, chrome, m_viewPort);
    m_urlSearchEditor->setTextColor(textColor);
    m_urlSearchEditor->setBackgroundColor(backgroundColor);
    m_urlSearchEditor->setProgressColor(progressColor);
    m_urlSearchEditor->setBorderColor(m_borderColor);
    m_urlSearchEditor->setPadding(0.1); // draw the Rounded Rect
    m_urlSearchEditor->setInputMethodHints(Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    safe_connect(m_urlSearchEditor, SIGNAL(textMayChanged()), this, SLOT(updateLoadStateAndSuggest()));
    safe_connect(m_urlSearchEditor, SIGNAL(activated()),this, SLOT(urlSearchActivatedByEnterKey()));
    safe_connect(m_urlSearchEditor, SIGNAL(focusChanged(bool)),this, SLOT(focusChanged(bool)));
    safe_connect(m_urlSearchEditor, SIGNAL(tapped(QPointF&)),this, SLOT(tapped(QPointF&)));

    // Create the url search button
    m_urlSearchBtn = new ActionButton(snippet, m_viewPort);
    QAction* urlSearchBtnAction = new QAction(this);
    m_urlSearchBtn->setAction(urlSearchBtnAction); // FIXME: should use diff QActions

    m_urlSearchBtn->setActiveOnPress(false);
    safe_connect(urlSearchBtnAction, SIGNAL(triggered()), this, SLOT(urlSearchActivated()));

    // Get the icon size
    QIcon btnIcon(GO_BUTTON_ICON);
    QSize defaultSize(50, 50);
    QSize actualSize = btnIcon.actualSize(defaultSize);
    m_iconWidth = actualSize.width();
    m_iconHeight = actualSize.height();
    // Set the right text margin to accomodate the icon inside the editor
    m_urlSearchEditor->setRightTextMargin(m_iconWidth + BETWEEN_ENTRY_AND_BUTTON_SPACE);

    // Update state as soon as chrome completes loading.
    safe_connect(m_chrome, SIGNAL(chromeComplete()),
            this, SLOT(onChromeComplete()));

    // Monitor resize events.
    safe_connect(m_chrome->renderer(), SIGNAL(chromeResized()),
            this, SLOT(resize()));

    WebPageController * pageController = WebPageController::getSingleton();

    safe_connect(pageController, SIGNAL(pageUrlChanged(const QString)),
            m_urlSearchEditor, SLOT(setText(const QString &)))

    safe_connect(pageController, SIGNAL(pageLoadStarted()),
            this, SLOT(setStarted()));

    safe_connect(pageController, SIGNAL(pageLoadProgress(const int)),
            this, SLOT(setProgress(int)));

    safe_connect(pageController, SIGNAL(pageLoadFinished(bool)),
            this, SLOT(setFinished(bool)));

    safe_connect(pageController, SIGNAL(pageCreated(WRT::WrtBrowserContainer*)),
            this, SLOT(setPageCreated()));

    safe_connect(pageController, SIGNAL(pageChanged(WRT::WrtBrowserContainer*, WRT::WrtBrowserContainer*)),
            this, SLOT(setPageChanged()));

    // Monitor view changes.

    ViewController * viewController = chrome->viewController();

    safe_connect(viewController, SIGNAL(currentViewChanged()),
            this, SLOT(viewChanged()));

 /*   safe_connect(ViewStack::getSingleton(), SIGNAL(currentViewChanged()),
            this, SLOT(viewChanged()));*/
}

GUrlSearchItem::~GUrlSearchItem()
{
}

//TODO: Shouldn't have to explicitly set the viewport sizes here

void GUrlSearchItem::resizeEvent(QGraphicsSceneResizeEvent * event)
{
    QSizeF size = event->newSize();

    m_viewPortWidth  = size.width()  - m_padding * 2;
    m_viewPortHeight = size.height() - m_padding * 2;
    
    m_viewPort->setGeometry(
            m_padding,
            m_padding,
            m_viewPortWidth,
            m_viewPortHeight);

    qreal w = m_iconWidth;
    qreal h = m_iconHeight;

    m_urlSearchBtn->setGeometry(
	    m_viewPortWidth - w - m_padding/2,
	    (m_viewPortHeight - h)/2,
	    w,
	    h);

    m_urlSearchEditor->setGeometry(0,
            0,
            m_viewPortWidth,
            m_viewPortHeight);

}

void GUrlSearchItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    if (m_padding > 0 || m_border > 0) {
        QPainterPath border;
        border.addRect(boundingRect());
        border.addRoundedRect(
                m_padding,
                m_padding,
                m_viewPortWidth,
                m_viewPortHeight,
                4,
                4);

        if (m_padding > 0) {
            painter->fillPath(border, m_borderColor);
        }

        if (m_border > 0) {
            QPen pen;
            pen.setWidth(m_border);
            pen.setBrush(m_borderColor);
            painter->setPen(pen);
            painter->drawPath(border);
        }
    }

    painter->restore();
    NativeChromeItem::paint(painter, option, widget);
}

void GUrlSearchItem::onChromeComplete()
{
    setStarted();

    WRT::WindowFlowView* windowView = static_cast<WRT::WindowFlowView *>(m_chrome->viewController()->view("WindowView"));
    safe_connect(windowView, SIGNAL(newWindowTransitionComplete()), this, SLOT(onNewWindowTransitionComplete()));

    PageSnippet * suggestSnippet = qobject_cast<PageSnippet*>(m_chrome->getSnippet("SuggestsChromeId"));

    // instantiate items needed to display suggest page snippet
    if (suggestSnippet) {
        suggestSnippet->instantiate();
    }
}

void GUrlSearchItem::setStarted()
{
    WebPageController * pageController = WebPageController::getSingleton();
    ViewController * viewController = m_chrome->viewController();

    m_urlSearchEditor->setText(pageController->currentRequestedUrl());
    ControllableViewBase* curView = viewController->currentView();
    if (curView && curView->type() == "webView") {
        GWebContentView * gView = qobject_cast<GWebContentView*> (curView);
        bool isSuperPage = gView ? gView->currentPageIsSuperPage() : false;
        if(!isSuperPage)
	  m_chrome->layout()->slideView(100);
    }
    // Strictly speaking we should set progress to 0.
    // But set it higher to give immediate visual feedback
    // that something is happening.

    int progress = 0;

    if (pageController->isPageLoading()) {
        progress = 5;
    }

    m_urlSearchEditor->setProgress(progress);
    updateUrlSearchBtn();
}

void GUrlSearchItem::setProgress(int percent)
{
    m_urlSearchEditor->setProgress(percent);
}

// Wait a half-second before actually clearing the progress bar.
//
// We have to be careful of the following two use cases:
//
// 1. Another page starts loading between the call to setFinished()
//    and the scheduled call to clearProgress().
//
//    We don't want to clear the progress bar if another page is
//    loading.  WebPageController::isPageLoading() can tell us
//    if that is the case.
//
// 2. Another page finishes loading between the call to setFinished()
//    and the scheduled call to clearProgress().  The sequence here is:
//
//      setFinished(ok) // for URL #1
//      setFinished(ok) // for URL #2
//      clearProgress() // for URL #1
//      clearProgress() // for URL #2
//
//    We don't want to clear the progress bar in the first call to
//    clearProgress() because we want the progress bar to retain its
//    appearance for the full timeout period.  We manage this by
//    tracking the number of pending calls to clearProgress() and
//    only clearing the progress bar when that number becomes 0.
void GUrlSearchItem::setFinished(bool ok)
{
    WebPageController * pageController = WebPageController::getSingleton();
    // If the load was finished normally and not due to user stopping it,
    // simulate progress completion
    if (!pageController->loadCanceled())
        m_urlSearchEditor->setProgress(100);

    if (ok)
        m_urlSearchEditor->setText(formattedUrl());

    m_urlSearchEditor->removeFocus();

    ViewController * viewController = m_chrome->viewController();
    ControllableViewBase* curView = viewController->currentView();
    if (curView && curView->type() == "webView" && pageController->contentsYPos() > 0)
      m_chrome->layout()->slideView(-100);

    ++m_pendingClearCalls;

    QTimer::singleShot(500, this, SLOT(clearProgress()));
}

void GUrlSearchItem::setPageCreated()
{
    // remove slideview(100) since the new transition for the code-driven window
    //m_chrome->layout()->slideView(100);
}

void GUrlSearchItem::setPageChanged()
{
    m_urlSearchEditor->setText(formattedUrl());
    updateUrlSearchBtn();

    WebPageController * pageController = WebPageController::getSingleton();
    int progress = pageController->loadProgressValue();
    if (progress == 100)
        m_urlSearchEditor->removeFocus();
}

void GUrlSearchItem::clearProgress()
{
    --m_pendingClearCalls;

    if (m_pendingClearCalls > 0) {
        return;
    }

    WebPageController * pageController = WebPageController::getSingleton();
    if (pageController->isPageLoading()) {
        return;
    }
    m_urlSearchEditor->setProgress(0);
    updateUrlSearchBtn();
}

void GUrlSearchItem::viewChanged()
{
    ViewController * viewController = m_chrome->viewController();
    WebPageController * pageController = WebPageController::getSingleton();

    ControllableViewBase* curView = viewController->currentView();
    GWebContentView * gView = qobject_cast<GWebContentView*> (curView);
    bool isSuperPage = gView ? gView->currentPageIsSuperPage() : false;

    // view changes to web content view
    if (curView && curView->type() == "webView" && !isSuperPage) {
        int progress = pageController->loadProgressValue();
        if (progress >= 100)
            progress = 0;
        m_urlSearchEditor->setProgress(progress);
        updateUrlSearchBtn();

        // place focus in urlsearch bar when returning from adding a new window in windows view
        if (pageController->loadText() == "") {
            if (m_backFromNewWinTrans ) {
                m_backFromNewWinTrans = false;
                WebPageController * pageController = WebPageController::getSingleton();
                m_urlSearchEditor->setText(pageController->currentRequestedUrl());
            }
            else {
                m_urlSearchEditor->grabFocus();
            }
        }
        if (!isSuperPage  && (pageController->contentsYPos() <= 0 || pageController->isPageLoading())){
	  m_chrome->layout()->slideView(100);
        } else {
	  m_chrome->layout()->slideView(-100);
        }
        m_backFromNewWinTrans = false;
    } else {
         pageController->urlTextChanged(m_urlSearchEditor->text());
         // Remove progress bar
         // incorrect values are not seen before we can update when we come back
         m_urlSearchEditor->setProgress(0);
         m_chrome->layout()->slideView(-100);
    }
}

void GUrlSearchItem::urlSearchActivatedByEnterKey()
{
    m_urlSearchEditor->removeFocus();
    urlSearchActivated();
}

void GUrlSearchItem::urlSearchActivated()
{
    WebPageController * pageController = WebPageController::getSingleton();
    switch (pageController->loadState()) {
        case WRT::LoadController::GotoModeLoading:
            pageController->currentStop();
            ++m_pendingClearCalls;
            QTimer::singleShot(500, this, SLOT(clearProgress()));
            break;
        case WRT::LoadController::GotoModeEditing:
            loadToMainWindow();
            break;
        case WRT::LoadController::GotoModeReloadable:
            if (pageController->currentDocUrl() == m_urlSearchEditor->text())
                pageController->currentReload();
            else
                loadToMainWindow();
            break;
        default:
            qDebug() << "Incorrect state";
            break;
    }
    updateUrlSearchBtn();
}

void GUrlSearchItem::updateUrlSearchBtn()
{
    WebPageController * pageController = WebPageController::getSingleton();
    switch (pageController->loadState()) {
        case WRT::LoadController::GotoModeLoading:
            m_urlSearchBtn->addIcon(STOP_BUTTON_ICON);
            break;
        case WRT::LoadController::GotoModeEditing:
            m_urlSearchBtn->addIcon(GO_BUTTON_ICON);
            break;
        case WRT::LoadController::GotoModeReloadable:
            m_urlSearchBtn->addIcon(REFRESH_BUTTON_ICON);
            break;
        default:
            qDebug() << "Incorrect state";
            break;
    }
    m_urlSearchBtn->update();
    
    // notify suggest object of changes in load state
    PageSnippet * suggestSnippet = qobject_cast<PageSnippet*>(m_chrome->getSnippet("SuggestsChromeId"));
    if (suggestSnippet) {
        QString cmd = "searchSuggests.updateLoadState();";
        suggestSnippet->evaluateJavaScript(cmd);
    }
}

void GUrlSearchItem::loadToMainWindow()
{
    QString url = m_urlSearchEditor->text();
    WebPageController * pageController = WebPageController::getSingleton();
    QString gotourl = pageController->guessUrlFromString(url);
    m_urlSearchEditor->setText(gotourl);
    pageController->currentLoad(gotourl);
    pageController->urlTextChanged(gotourl);
}

void GUrlSearchItem::updateLoadState()
{
    WebPageController * pageController = WebPageController::getSingleton();
    if (pageController->loadState() == WRT::LoadController::GotoModeReloadable) {
        pageController->setLoadState(WRT::LoadController::GotoModeEditing);
        updateUrlSearchBtn();
    }
}

void GUrlSearchItem::updateLoadStateAndSuggest()
{
    updateLoadState();
    PageSnippet * suggestSnippet = qobject_cast<PageSnippet*>(m_chrome->getSnippet("SuggestsChromeId"));
    if (suggestSnippet) {
        QString cmd = "searchSuggests.updateUserInput();";
        suggestSnippet->evaluateJavaScript(cmd);
    }
}

void GUrlSearchItem::tapped(QPointF& pos)
{
    bool hitText = m_urlSearchEditor->tappedOnText(pos.x());
    if (!m_justFocusIn && !hitText)
        m_urlSearchEditor->unselect();

    if (m_justFocusIn) {
        m_justFocusIn = false;
        if (hitText && !m_urlSearchEditor->hasSelection())
            m_urlSearchEditor->selectAll();
    }
}

void GUrlSearchItem::focusChanged(bool focusIn)
{
    if (focusIn)
        m_justFocusIn = true;
    else {
        m_justFocusIn = false;
        m_urlSearchEditor->unselect();
        m_urlSearchEditor->shiftToLeftEnd();
        
        // Suggestion snippet needs to know about this event.
        PageSnippet * suggestSnippet = qobject_cast<PageSnippet*>(m_chrome->getSnippet("SuggestsChromeId"));
        if (suggestSnippet) {
            QString cmd = "searchSuggests.urlSearchLostFocus();";
            suggestSnippet->evaluateJavaScript(cmd);
        }
    }
}

void GUrlSearchItem::resize()
{
    QWebElement we = m_snippet->element();
    QRectF g = we.geometry();
    qreal newWidth  = g.width();
    qreal newHeight = g.height();
    QGraphicsWidget::resize(newWidth, newHeight);
}

void GUrlSearchItem::onNewWindowTransitionComplete()
{
    m_backFromNewWinTrans = true;
}

QString GUrlSearchItem::formattedUrl() const
{
    WebPageController * pageController = WebPageController::getSingleton();
    QString url = pageController->loadText();
    // for first load of the windows restored from last session
    if (url.isEmpty()&& pageController->currentDocUrl().isEmpty()) {
        QWebHistoryItem item = pageController->currentPage()->history()->currentItem();
        url = item.url().toString();
    }
    return url.replace(" ","+");
}

GUrlSearchSnippet::GUrlSearchSnippet(const QString & elementId, ChromeWidget * chrome,
                         QGraphicsWidget * widget, const QWebElement & element)
  : ChromeSnippet(elementId, chrome, widget, element)
{
}

GUrlSearchSnippet * GUrlSearchSnippet::instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element)
{
    GUrlSearchSnippet* that = new GUrlSearchSnippet(elementId, chrome, 0, element);
    that->setChromeWidget( new GUrlSearchItem( that, chrome ) );
    return that;
}

inline GUrlSearchItem* GUrlSearchSnippet::urlSearchItem()
{
    return static_cast<GUrlSearchItem *>(widget());
}

inline GUrlSearchItem const * GUrlSearchSnippet::constUrlSearchItem() const
{
    return static_cast<GUrlSearchItem const *>(constWidget());
}

QString GUrlSearchSnippet::url() const
{
    return constUrlSearchItem()->url();
}

void GUrlSearchSnippet::setUrl(const QString &url)
{
    urlSearchItem()->setUrl(url);
}

} // namespace GVA
