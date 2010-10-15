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
#include "UiUtil.h"
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
#ifdef BROWSER_LAYOUT_TENONE
#include "secureuicontroller.h"
#endif


#include <QWebHistoryItem>

namespace GVA {

#define GO_BUTTON_ICON ":/urlsearch/go_btn.png"
#define STOP_BUTTON_ICON ":/urlsearch/stop_btn.png"
#define SPECIFIC_BUTTON_ICON ":/urlsearch/com.svg"
#define SPECIFIC_BUTTON_STRING ".com"
#define BETWEEN_ENTRY_AND_BUTTON_SPACE 4

#ifdef BROWSER_LAYOUT_TENONE
#define URL_TITLE_IND_BUTTON_ICON ":/urlsearch/url_title_ind_btn.png"
#define URL_TITLE_IND_SECURE_BUTTON_ICON ":/urlsearch/url_title_ind_btn_secure.png"
#endif

static const QString KBookmarkHistoryViewName = "BookmarkHistoryView";
static const QString KBookmarkTreeViewName = "BookmarkTreeView";
static const QString KWindowViewName = "WindowView";
static const QString KSettingsViewName = "SettingsView";

//#define VBORDER_GRADIENT_START "#292929"
//#define VBORDER_GRADIENT_STOP "#535354"
//#define VBORDER_GRADIENT_END "#020202"

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
 
    QString cssPadding = we.styleProperty("padding-top", QWebElement::ComputedStyle);
    m_padding = cssPadding.remove("px").toInt();

    QString cssBorder = we.styleProperty("border-top-width", QWebElement::ComputedStyle);
    m_border = cssBorder.remove("px").toInt();

    // Create the view port widget
    m_viewPort = new QGraphicsWidget(this);
    m_viewPort->setFlags(QGraphicsItem::ItemClipsChildrenToShape);
    
    createEditor();
    createIcons();
    
    // Update state as soon as chrome completes loading.
    safe_connect(m_chrome, SIGNAL(chromeComplete()),
            this, SLOT(onChromeComplete()));

#ifndef BROWSER_LAYOUT_TENONE
    // Monitor resize events.
    safe_connect(m_chrome->renderer(), SIGNAL(chromeResized()),
            this, SLOT(resize())); 
#endif

 /*   safe_connect(ViewStack::getSingleton(), SIGNAL(currentViewChanged()),
            this, SLOT(viewChanged()));*/
}

GUrlSearchItem::~GUrlSearchItem()
{

}


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
    
    changeLayout(m_urlSearchBtn->isVisible());
}

//TODO: Shouldn't have to explicitly set the viewport sizes here
void GUrlSearchItem::changeLayout(const bool isSearch)
{
    
#ifdef BROWSER_LAYOUT_TENONE
    qreal searchBtnWidth = (isSearch) ? m_buttonIconWidth : 0;
    qreal titleIndBtnWidth = (m_urlTitleIndBtn->isVisible() || m_urlTitleIndSecureBtn->isVisible()) ? m_buttonIconWidth : 0;
    
    m_urlTitleIndBtn->setGeometry(0,
        (m_viewPortHeight - m_buttonIconHeight)/2,
        titleIndBtnWidth,
        m_buttonIconHeight);
    
    m_urlTitleIndSecureBtn->setGeometry(0,
        (m_viewPortHeight - m_buttonIconHeight)/2,
        titleIndBtnWidth,
        m_buttonIconHeight);
    
    // NOTE: removed padding for the x coordinate since too much separation was introduced
    m_urlSearchBtn->setGeometry(
        m_viewPortWidth - searchBtnWidth,
        (m_viewPortHeight - m_buttonIconHeight)/2,
        searchBtnWidth,
        m_buttonIconHeight);

    m_urlSearchEditor->setRightTextMargin(m_buttonIconWidth);
    m_urlSearchEditor->setGeometry(titleIndBtnWidth,
            0,
            m_viewPortWidth - titleIndBtnWidth,
            m_viewPortHeight);
    
#else
    qreal searchBtnWidth = m_buttonIconWidth;

    m_urlSearchBtn->setGeometry(
        m_viewPortWidth - searchBtnWidth - m_padding/2,
        (m_viewPortHeight - m_buttonIconHeight)/2,
        searchBtnWidth,
        m_buttonIconHeight);
    
    m_urlSearchEditor->setGeometry(0,
            0,
            m_viewPortWidth,
            m_viewPortHeight);
    
#endif
    
}

void GUrlSearchItem::onContextEvent(bool isContentSelected)
{
    // dismiss suggest snippet
    PageSnippet * suggestSnippet = qobject_cast<PageSnippet*>(m_chrome->getSnippet("SuggestsChromeId"));
    if (suggestSnippet) {
        QString cmd = "searchSuggests.hideSuggests();";
        suggestSnippet->evaluateJavaScript(cmd);
    }
    emit contextEvent(isContentSelected);
}

#ifdef BROWSER_LAYOUT_TENONE
void GUrlSearchItem::layoutToEditMode(const bool editMode)
{
    m_urlSearchBtn->setVisible(editMode);
    m_urlSearchEditor->changeEditorMode(editMode);
    changeLayout(editMode);
}
#endif

void GUrlSearchItem::urlSearchActivatedByEnterKey()
{
    m_urlSearchEditor->removeFocus();
    urlSearchActivated();
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
#ifndef BROWSER_LAYOUT_TENONE
    NativeChromeItem::paint(painter, option, widget);
#endif

}

#ifdef BROWSER_LAYOUT_TENONE
void GUrlSearchItem::changeToUrl(QPointF& pos)
{
    ViewController * viewController = m_chrome->viewController();
    ControllableViewBase* curView = viewController->currentView();

    if( curView && curView->type() == "webView" ) {
        emit changeEditMode(true);
        layoutToEditMode(true);
        m_urlSearchEditor->grabFocus();
        tapped(pos);
        m_urlSearchEditor->openVKB();
    }
}

void GUrlSearchItem::changeToTitle()
{
    emit changeEditMode(false);
    layoutToEditMode(false);
}

#endif

void GUrlSearchItem::onContentMouseEvent(QEvent::Type type )
{

    //qDebug() << "UrlSearchItem::onContentMouseEvent" << type;
    // We are dismissing the VKB on mouse release so that if the mouse press was on a link
    // we will not close VKB that might cause a shift in geometry which can potentially prevent
    // the link from being selected
    if ((WebPageController::getSingleton()->editMode()) && (type == QEvent::GraphicsSceneMouseRelease)) {
        // Edit canceled, so update LoadController state
        changeLoadState(false);
    
        m_urlSearchEditor->closeVKB();

#ifdef BROWSER_LAYOUT_TENONE
        if (! WebPageController::getSingleton()->isPageLoading())
            changeToTitle();
        // Set the text to the url of page
        setUrlText(urlToBeDisplayed());
#endif
    }
    
}

void GUrlSearchItem::onChromeComplete()
{
    WebPageController * pageController = WebPageController::getSingleton();

    safe_connect(pageController, SIGNAL(pageUrlChanged(const QString)),
            this, SLOT(setUrlText(const QString &)))

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

    safe_connect(pageController, SIGNAL(pageLoadFailed()),
            this, SLOT(setPageFailed()));
#ifdef BROWSER_LAYOUT_TENONE
    safe_connect(pageController, SIGNAL(titleChanged(const QString&)), this, SLOT(onTitleChange(const QString&)));
    
    // Check for secure URL state change to change between the url and lock indicator icons in the url title indicator button
    safe_connect(pageController, SIGNAL(showSecureIcon(bool)),
            this, SLOT(showSecureIcon(bool)));
#endif
			
    // Monitor view changes.

    ViewController * viewController = m_chrome->viewController();
    
    safe_connect(viewController, SIGNAL(currentViewChanged(ControllableViewBase *)),
            this, SLOT(viewChanged(ControllableViewBase *)));

    GWebContentView* webView = static_cast<GWebContentView*> (m_chrome->getView("WebView"));
    safe_connect(webView, SIGNAL(contentViewMouseEvent(QEvent::Type )), this, SLOT(onContentMouseEvent(QEvent::Type )));


    setStarted();
#ifndef Q_WS_MAEMO_5
    WRT::WindowFlowView* windowView = static_cast<WRT::WindowFlowView *>(m_chrome->viewController()->view("WindowView"));
    safe_connect(windowView, SIGNAL(newWindowTransitionComplete()), this, SLOT(onNewWindowTransitionComplete()));
#endif
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

#ifdef BROWSER_LAYOUT_TENONE
    showSecureIcon(false);
    emit changeEditMode(false);
    layoutToEditMode(true);
#endif

    QString url = pageController->currentRequestedUrl();
    if(!url.contains(KBOOKMARKURLFILESLASH)){
       if(url.contains(KBOOKMARKURLFILE))
          url.replace(QString(KBOOKMARKURLFILE), QString(KBOOKMARKURLFILESLASH));
    }
    setUrlText(url);
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
    ViewController * viewController = m_chrome->viewController();
    ControllableViewBase* curView = viewController->currentView();
    if ( curView && curView->type() == "webView" && m_urlSearchEditor )
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
    ViewController * viewController = m_chrome->viewController();
    ControllableViewBase* curView = viewController->currentView();
    //qDebug() << "GUrlSearchItem::setFinished" << pageController->loadState();
    
    if (pageController->loadState() != WRT::LoadController::GotoModeEditing) {
        m_urlSearchEditor->removeFocus();
        
        if ( ok && curView && curView->type() == "webView" ){
            setUrlText(formattedUrl());
#ifdef BROWSER_LAYOUT_TENONE
            m_urlSearchEditor->setTitle(currentTitle());
#endif
        }
        if (curView && curView->type() == "webView" && pageController->contentsYPos() > 0)
          m_chrome->layout()->slideView(-100);
    }
    
    // If the load was finished normally and not due to user stopping it,
    // simulate progress completion
    if ( !pageController->loadCanceled() && curView && curView->type() == "webView" )
        m_urlSearchEditor->setProgress(100);

    ++m_pendingClearCalls;

    QTimer::singleShot(500, this, SLOT(clearProgress()));
}

void GUrlSearchItem::setPageFailed()
{
    WebPageController * pageController = WebPageController::getSingleton();
    LoadController * loadController = pageController->currentPage()->loadController();
    if( loadController->loadCanceled() && !loadController->pointOfNoReturn() )
    {     
        setUrlText(loadController->urlText());
    }
    else
    {
        setUrlText(formattedUrl());
    }
    #ifdef BROWSER_LAYOUT_TENONE
     m_urlSearchEditor->setTitle(currentTitle());
    #endif
}

void GUrlSearchItem::setPageCreated()
{
    // remove slideview(100) since the new transition for the code-driven window
    //m_chrome->layout()->slideView(100);
    #ifdef Q_WS_MAEMO_5
     setProgress(0); // no progress bar in new window shall be left from previous windows
    #endif
}

void GUrlSearchItem::setPageChanged()
{
    setUrlText(formattedUrl());
    updateUrlSearchBtn();
    WebPageController * pageController = WebPageController::getSingleton();
#ifdef BROWSER_LAYOUT_TENONE
    
    ViewController * viewController = m_chrome->viewController();
    ControllableViewBase* curView = viewController->currentView();

    QString title;
    if (curView && curView->type() == KWindowViewName  ) {
        title = getWindowsViewTitle();
    }
    else {
        title = currentTitle();
    }

    m_urlSearchEditor->setTitle(title);
#endif


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
    
#ifdef BROWSER_LAYOUT_TENONE
    if ( !pageController->editMode() ) {
        changeToTitle();
    }
#endif
}


void GUrlSearchItem::viewChanged(ControllableViewBase *newView)
{
    ViewController * viewController = m_chrome->viewController();
    WebPageController * pageController = WebPageController::getSingleton(); 

    ControllableViewBase* curView = viewController->currentView();
    GWebContentView * gView = qobject_cast<GWebContentView*> (curView);
    bool isSuperPage = gView ? gView->currentPageIsSuperPage() : false;

#ifdef BROWSER_LAYOUT_TENONE
    QColor bgColor;
    QColor textColor;
    QColor titleColor;
#endif
    
    // view changes to web content view
    if (curView && curView->type() == "webView" && !isSuperPage) {
        
#ifdef BROWSER_LAYOUT_TENONE

        bgColor = Qt::transparent;
        textColor = Qt::black;
        titleColor = Qt::white;

        // when going to a web page view, set the correct icon for the url title indicator button state
        if (pageController->secureState() == 0){
            showSecureIcon(false);
        } else {
            showSecureIcon(true);
        }

        if (pageController->isPageLoading()) {

            m_urlSearchEditor->changeEditorMode(true);
            m_urlSearchBtn->show();
#endif
            int progress = pageController->loadProgressValue();
            if (progress >= 100)
                progress = 0;
            m_urlSearchEditor->setProgress(progress);
            updateUrlSearchBtn();

            // place focus in urlsearch bar when returning from adding a new window in windows view
            if (pageController->loadText() == "") {
                if (m_backFromNewWinTrans ) {
                    m_backFromNewWinTrans = false;
                    setUrlText(pageController->currentRequestedUrl());
                }
                else {
                    m_urlSearchEditor->grabFocus();
                }
            }
#ifdef BROWSER_LAYOUT_TENONE
        }
        else {
            m_urlSearchEditor->setTitle(currentTitle());
        }
#endif
        if (!isSuperPage  && (pageController->contentsYPos() <= 0 || pageController->isPageLoading())){
            m_chrome->layout()->slideView(100);
        } else {
            m_chrome->layout()->slideView(-100);
        }
        m_backFromNewWinTrans = false;
    } else {

#ifdef BROWSER_LAYOUT_TENONE
     bgColor = Qt::transparent;
     textColor = Qt::black;
     titleColor = Qt::white;

     // when going to a view other than a web page, set the non-secure icon for the url title indicator button
     hideIndicatorButton();
#endif
     
     pageController->urlTextChanged(m_urlSearchEditor->text());
     // Remove progress bar
     // incorrect values are not seen before we can update when we come back
     m_urlSearchEditor->setProgress(0);

#ifdef BROWSER_LAYOUT_TENONE
     QString key = curView->objectName();
     if (key == KWindowViewName) {
         key = getWindowsViewTitle();
     }
     else if (key == KSettingsViewName) {
         key = qtTrId("txt_browser_settings_settings");
     }
     else if (key == KBookmarkHistoryViewName) {
         key = qtTrId("txt_browser_history_history");
     }
     else if (key == KBookmarkTreeViewName) {
         key = qtTrId("txt_browser_bookmarks_bookmarks");
     }
     changeToTitle();
     m_urlSearchEditor->setTitle(key);
#else
     m_chrome->layout()->slideView(-100);
#endif 
    }
    
#ifdef BROWSER_LAYOUT_TENONE
    m_urlSearchEditor->setBackgroundColor(bgColor);
    m_urlSearchEditor->setTextColor(textColor);
    // reset the layout for secure icon
    changeLayout(m_urlSearchBtn->isVisible());
#endif
}

#ifdef BROWSER_LAYOUT_TENONE
void GUrlSearchItem::onTitleChange(const QString& text)
{
    ViewController * viewController = m_chrome->viewController();
    ControllableViewBase* curView = viewController->currentView();
    if ( curView && curView->type() == "webView" ){
         m_urlSearchEditor->setTitle(text);
    }
}

QString GUrlSearchItem::getWindowsViewTitle() {
    QString title =  currentTitle();
    if (title.isEmpty()) {
        title = qtTrId("txt_browser_windows_windows");
        title += ": ";
        title += qtTrId("txt_browser_windows_new_window");
    }
    else {
        title.prepend(": ");
        title.prepend(qtTrId("txt_browser_windows_windows"));
            
    }
    return title;
}

void GUrlSearchItem::showSecureIcon(bool show)
{
    m_urlTitleIndBtn->setVisible(!show);
    m_urlTitleIndSecureBtn->setVisible(show);
}

void GUrlSearchItem::hideIndicatorButton()
{
    m_urlTitleIndBtn->hide();
    m_urlTitleIndSecureBtn->hide();
}

#endif

void GUrlSearchItem::urlSearchActivated()
{
    WebPageController * pageController = WebPageController::getSingleton();
    switch (pageController->loadState()) {
        case WRT::LoadController::GotoModeLoading:
            pageController->currentStop();
            ++m_pendingClearCalls;
            QTimer::singleShot(500, this, SLOT(clearProgress()));
#ifdef BROWSER_LAYOUT_TENONE
            layoutToEditMode(false);
#endif
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
            qDebug() << "GUrlSearchItem::urlSearchActivated() Incorrect state";
            break;
    }
    updateUrlSearchBtn();
}

#ifdef BROWSER_LAYOUT_TENONE
void GUrlSearchItem::urlTitleIndBtnActivated()
{
    // When the url title indicator button is pressed, focus into the url text area
    QPointF pos(0, 0);
    changeToUrl(pos);
}
#endif

void GUrlSearchItem::updateUrlSearchBtn()
{
    
    WebPageController * pageController = WebPageController::getSingleton();
    switch (pageController->loadState()) {
        case WRT::LoadController::GotoModeLoading:
            m_urlSearchBtn->addIcon(STOP_BUTTON_ICON);
            break;
        case WRT::LoadController::GotoModeEditinLoading:
        case WRT::LoadController::GotoModeEditing:
            m_urlSearchBtn->addIcon(GO_BUTTON_ICON);
            break;
        case WRT::LoadController::GotoModeReloadable:
            m_urlSearchBtn->addIcon(GO_BUTTON_ICON);
            break;
        default:
            qDebug() << "GUrlSearchItem::updateUrlSearchBtn Incorrect state";
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

void GUrlSearchItem::createEditor()
{
    QColor textColor;
    QColor backgroundColor;
    QColor progressColor;
    
    QWebElement we = m_snippet->element();
    NativeChromeItem::CSSToQColor(
            we.styleProperty("border-bottom-color", QWebElement::ComputedStyle),
            progressColor);
    
#ifdef BROWSER_LAYOUT_TENONE
    QFont titleFont;
    QColor titleColor;
    textColor = QColor(Qt::black);
    titleColor = QColor(Qt::white);
    m_borderColor = QColor(Qt::transparent);
    backgroundColor = QColor(Qt::transparent);
    
    titleFont = QFont(QApplication::font());
    titleFont.setPointSize(9);
    titleFont.setWeight(QFont::Bold);
#endif
#ifndef BROWSER_LAYOUT_TENONE
    NativeChromeItem::CSSToQColor(
            we.styleProperty("color", QWebElement::ComputedStyle),
            textColor);

    NativeChromeItem::CSSToQColor(
            we.styleProperty("background-color", QWebElement::ComputedStyle),
            backgroundColor);

    NativeChromeItem::CSSToQColor(
            we.styleProperty("border-top-color", QWebElement::ComputedStyle),
            m_borderColor);
#endif
    
    // Create the url search editor
    m_urlSearchEditor = new GProgressEditor(m_snippet, m_chrome, m_viewPort);
    m_urlSearchEditor->setObjectName("UrlEditor");
    m_urlSearchEditor->setTextColor(textColor);
 #ifdef BROWSER_LAYOUT_TENONE
    m_urlSearchEditor->setTitleFont(titleFont);
    m_urlSearchEditor->setTitleColor(titleColor);
 #endif
    m_urlSearchEditor->setBackgroundColor(backgroundColor);
    m_urlSearchEditor->setProgressColor(progressColor);
    m_urlSearchEditor->setBorderColor(m_borderColor);
    m_urlSearchEditor->setPadding(0.1); // draw the Rounded Rect
    m_urlSearchEditor->setInputMethodHints(Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    m_urlSearchEditor->setSpecificButton(SPECIFIC_BUTTON_STRING, SPECIFIC_BUTTON_ICON);
    safe_connect(m_urlSearchEditor, SIGNAL(contentsChange(int, int, int)), 
        this, SLOT(updateLoadStateAndSuggest(int, int, int)));
    safe_connect(m_urlSearchEditor, SIGNAL(activated()),this, SLOT(urlSearchActivatedByEnterKey()));
    safe_connect(m_urlSearchEditor, SIGNAL(focusChanged(bool)),this, SLOT(focusChanged(bool)));
#ifndef BROWSER_LAYOUT_TENONE
    safe_connect(m_urlSearchEditor, SIGNAL(tapped(QPointF&)),this, SLOT(tapped(QPointF&)));
#endif    
    safe_connect(m_urlSearchEditor, SIGNAL(contextEvent(bool)), this, SIGNAL(contextEvent(bool)));
#ifdef BROWSER_LAYOUT_TENONE
    safe_connect(m_urlSearchEditor, SIGNAL(titleMouseEvent(QPointF&)),this, SLOT(changeToUrl(QPointF&)));
#endif
}

void GUrlSearchItem::createIcons()
{
    // Create the url search button
    m_urlSearchBtn = new ActionButton(m_snippet, "UrlSearchButton", m_viewPort);
    QAction* urlSearchBtnAction = new QAction(this);
    m_urlSearchBtn->setAction(urlSearchBtnAction); // FIXME: should use diff QActions
    m_urlSearchBtn->setActiveOnPress(false);
    safe_connect(urlSearchBtnAction, SIGNAL(triggered()), this, SLOT(urlSearchActivated()));
    
#ifdef BROWSER_LAYOUT_TENONE
    // Create the url title indicator button (NOTE: QAction created but not hooked up)
    m_urlTitleIndBtn = new ActionButton(m_snippet, "UrlSearchIndButton", m_viewPort);
    QAction* urlTitleIndBtnAction = new QAction(this);
    m_urlTitleIndBtn->setAction(urlTitleIndBtnAction);
    m_urlTitleIndBtn->addIcon(URL_TITLE_IND_BUTTON_ICON);
    m_urlTitleIndBtn->setActiveOnPress(false);
    safe_connect(urlTitleIndBtnAction, SIGNAL(triggered()), this, SLOT(urlTitleIndBtnActivated()));
    
    m_urlTitleIndSecureBtn = new ActionButton(m_snippet, "UrlSearchSecureButton", m_viewPort);
    QAction* urlTitleIndSecureBtnAction = new QAction(this);
    m_urlTitleIndSecureBtn->setAction(urlTitleIndSecureBtnAction);
    m_urlTitleIndSecureBtn->addIcon(URL_TITLE_IND_SECURE_BUTTON_ICON);
    m_urlTitleIndSecureBtn->setActiveOnPress(false);
    safe_connect(urlTitleIndSecureBtnAction, SIGNAL(triggered()), this, SLOT(urlTitleIndBtnActivated()));

#endif
    
    QIcon btnIcon(GO_BUTTON_ICON);
    QSize actualSize = btnIcon.availableSizes()[0];
    m_buttonIconWidth = actualSize.width();
    m_buttonIconHeight = actualSize.height();

#ifndef BROWSER_LAYOUT_TENONE
    m_urlSearchEditor->setRightTextMargin(m_buttonIconWidth + BETWEEN_ENTRY_AND_BUTTON_SPACE);

#endif

}

void GUrlSearchItem::loadToMainWindow()
{
    QString url = m_urlSearchEditor->text();
    if(!url.contains(KBOOKMARKURLFILESLASH)){
       if(url.contains(KBOOKMARKURLFILE))
          url.replace(QString(KBOOKMARKURLFILE), QString(KBOOKMARKURLFILESLASH));
    }
    WebPageController * pageController = WebPageController::getSingleton();
    QString gotourl = pageController->guessUrlFromString(url);
    setUrlText(gotourl);
    pageController->currentLoad(gotourl);
    pageController->urlTextChanged(gotourl);
#ifdef BROWSER_LAYOUT_TENONE
    m_urlSearchEditor->closeVKB();
#endif
}

void GUrlSearchItem::updateLoadStateAndSuggest(int /*position*/, int charsRemoved, int charsAdded)
{
    WebPageController * pageController = WebPageController::getSingleton();
    if (url().contains('\n'))
    {
        urlSearchActivatedByEnterKey();
        return;
    }
    
    // will get contentsChanged() signal on programmatic changes and sometimes 
    // position changes but we are only interested in user input
    if ((charsRemoved || charsAdded) && pageController->editMode()) {
        PageSnippet * suggestSnippet = qobject_cast<PageSnippet*>(m_chrome->getSnippet("SuggestsChromeId"));
        if (suggestSnippet) {
            QString cmd = "searchSuggests.updateUserInput();";
            suggestSnippet->evaluateJavaScript(cmd);
        }
    }

}


void GUrlSearchItem::changeLoadState(bool editing) {

    WebPageController * pageController = WebPageController::getSingleton();
    if (pageController->editMode() != editing)  {
        pageController->setEditMode(editing);
        updateUrlSearchBtn();
    }
}
void GUrlSearchItem::tapped(QPointF& pos)
{
    changeLoadState(true);
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
    // Suggestion snippet needs to know about this event.
    PageSnippet * suggestSnippet = qobject_cast<PageSnippet*>(m_chrome->getSnippet("SuggestsChromeId"));
    if (suggestSnippet) {
        QString cmd("searchSuggests.urlSearchFocusChanged(");
        cmd += focusIn ? "true);" : "false);";
        suggestSnippet->evaluateJavaScript(cmd);
    }

    if (focusIn) {
        m_justFocusIn = true;
    }
    else {
        m_justFocusIn = false;
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


void GUrlSearchItem::setUrlText(const QString & str) 
{
	ViewController * viewController = m_chrome->viewController();
	ControllableViewBase* curView = viewController->currentView();
	if ( curView && curView->type() == "webView" ) {
        QUrl url(str);
	    m_urlSearchEditor->setText(WRT::UiUtil::encodeQueryItems(url, true).toString());
	}
}

QString GUrlSearchItem::urlToBeDisplayed() 
{

    WebPageController * pageController = WebPageController::getSingleton();
    QString url = pageController->currentDocUrl();
    if (url.isEmpty() ) {
        url = pageController->currentRequestedUrl();
    }
    //qDebug() << "GUrlSearchItem::urlToBeDisplayed" << url;
    return url;
}

QString GUrlSearchItem::currentTitle() 
{

    WebPageController * pageController = WebPageController::getSingleton();
    QString title  = pageController->currentDocTitle();
    if (title.isEmpty() ){ 
        title = pageController->currentPartialUrl();
        if (title.isEmpty() ) {
             // New Window, so set title as such
            title = qtTrId("txt_browser_windows_new_window");
        }
    }
    return title;
}


// GUrlSearchSnippet class


GUrlSearchSnippet::GUrlSearchSnippet(const QString & elementId, ChromeWidget * chrome,
                         QGraphicsWidget * widget, const QWebElement & element)
  : ChromeSnippet(elementId, chrome, widget, element)
{
}

GUrlSearchSnippet * GUrlSearchSnippet::instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element)
{
    GUrlSearchSnippet* that = new GUrlSearchSnippet(elementId, chrome, 0, element);
    GUrlSearchItem* urlSearchItem = new GUrlSearchItem(that, chrome);
    safe_connect(urlSearchItem, SIGNAL(contextEvent(bool)), that, SLOT(sendContextMenuEvent(bool)));
    that->setChromeWidget(urlSearchItem);
    return that;
}

void GUrlSearchSnippet::sendContextMenuEvent(bool isContentSelected)
{
    emit contextEvent(isContentSelected, elementId());
}

GUrlSearchItem* GUrlSearchSnippet::urlSearchItem()
{
    return qobject_cast<GUrlSearchItem *>(widget());
}

GUrlSearchItem const * GUrlSearchSnippet::constUrlSearchItem() const
{
    return qobject_cast<GUrlSearchItem const *>(constWidget());
}

QString GUrlSearchSnippet::url() const
{
    return constUrlSearchItem()->url();
}

void GUrlSearchSnippet::setUrl(const QString &url)
{
    urlSearchItem()->setUrl(url);
}

void GUrlSearchSnippet::cut()
{
    urlSearchItem()->cut();
}

void GUrlSearchSnippet::copy()
{
    urlSearchItem()->copy();
}

void GUrlSearchSnippet::paste()
{
    urlSearchItem()->paste();
}

} // namespace GVA
