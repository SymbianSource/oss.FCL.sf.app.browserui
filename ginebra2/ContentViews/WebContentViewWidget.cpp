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

#include "WebContentViewWidget.h"

#include "ScrollableWebContentView.h"
#include "webpagecontroller.h"
#include "WebView.h"
#include "wrtbrowsercontainer.h"
#include "webpagedata.h"

#ifdef Q_WS_MAEMO_5
#include "ContentViewContextMenu.h"
#endif
#include "GWebContentView.h"

#include <QGraphicsLinearLayout>
#include <QStyleOptionGraphicsItem>
#include <QWebFrame>
#include "QWebHistoryItem"

namespace GVA {

WebContentViewWidget::WebContentViewWidget(QObject* parent, GWebContentView* view, QWebPage* page): 
	m_webContentView(view)
{
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, true);
    setParent(parent);

    m_webView = new WebView();
    m_webViewport = new ScrollableWebContentView(m_webView, this);

    if (page)
        setPage(page);

    //updatePreferredContentSize();

    //FIX ME : Should we have to delete layout??
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical, this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0.);
    layout->addItem(m_webViewport);

    connect(m_webViewport
            , SIGNAL(updateZoomActions(bool,bool))
            , this
            , SIGNAL(updateZoomActions(bool,bool)));
    #ifdef Q_WS_MAEMO_5
    connect(m_webViewport
            , SIGNAL(contextEventObject(QWebHitTestResult*, QPointF))
            , this
            , SLOT(onContextEventObject(QWebHitTestResult*, QPointF)));
    #else
    connect(m_webViewport
            , SIGNAL(contextEventObject(QWebHitTestResult*, QPointF))
            , this
            , SIGNAL(contextEventObject(QWebHitTestResult*, QPointF)));
    #endif
    connect(m_webViewport
            , SIGNAL(viewScrolled(QPoint&, QPoint&))
            , this
            , SIGNAL(viewScrolled(QPoint&, QPoint&)));

    connect(m_webViewport
            , SIGNAL(contentViewMouseEvent(QEvent::Type))
            , this
            , SIGNAL(mouseEvent(QEvent::Type)));

    //To speed up painting.
    setFlag(QGraphicsItem::ItemHasNoContents, true);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
}

WebContentViewWidget::~WebContentViewWidget()
{
    delete m_webView;
    delete m_webViewport;
}


bool WebContentViewWidget::event(QEvent * e) 
{
    if (e->type() == QEvent::Gesture) {
        return  m_webViewport->event(e);
    }
    return QGraphicsWidget::event(e);
}

void WebContentViewWidget::resizeEvent(QGraphicsSceneResizeEvent* event)
{
    QGraphicsWidget::resizeEvent(event);

    // WTF? setGeometry(QRectF(pos(), size()));
    setPreferredSize(size());
    QSizeF vpSize = m_webViewport->size();
    if(vpSize.width() > size().width()) vpSize.setWidth(size().height());
    if(vpSize.height() > size().height()) vpSize.setHeight(size().height());

    QPointF vpPos = m_webViewport->pos();
    if(vpPos.x() + vpSize.width() > geometry().right())
        vpPos.setX(geometry().right() - vpSize.width());
    if(vpPos.y() + vpSize.height() > geometry().bottom())
        vpPos.setY(geometry().bottom() - vpSize.height());

    QRectF vpGeom(vpPos,vpSize);
    if(vpGeom != m_webViewport->geometry()) {
        // Should we center it here?
        m_webViewport->setGeometry(vpGeom);
    }
}

#ifdef Q_WS_MAEMO_5
void WebContentViewWidget::onContextEventObject(QWebHitTestResult* hitTest, QPointF position) {
    if(m_webContentView->currentPageIsSuperPage()) {
        // Let the superpage handle the event.
        ::WebViewEventContext *context = new ::WebViewEventContext(view()->type(), *hitTest);
        m_webContentView->currentSuperPage()->onContextEvent(context);
    }
    else {
        ContentViewContextMenu menu(hitTest, 0);
        menu.exec(position.toPoint());
    }
}
#endif

QWebPage* WebContentViewWidget::page()
{
    return m_webView->page();
}

void WebContentViewWidget::setPage(QWebPage* page)
{
    QWebPage* oldPage = this->page(); 
    if(oldPage) {
        disconnect(this->page()->mainFrame(), SIGNAL(initialLayoutCompleted()), m_webViewport, SLOT(reset()));
        disconnect(this->page(), SIGNAL(restoreFrameStateRequested(QWebFrame*)), this, SLOT(restoreViewportFromHistory(QWebFrame*)));
        disconnect(this->page(), SIGNAL(saveFrameStateRequested(QWebFrame*,QWebHistoryItem*)), this, SLOT(saveViewportToHistory(QWebFrame*,QWebHistoryItem*)));
        disconnect(this->page()->mainFrame(), SIGNAL(contentsSizeChanged(const QSize &)), m_webViewport, SLOT(contentsSizeChanged(const QSize&)));
        disconnect(this->page(), SIGNAL(loadStarted()), m_webViewport, SLOT(pageLoadStarted()));
        disconnect(this->page(), SIGNAL(loadProgress(int)), m_webViewport, SLOT(pageLoadProgress(int)));
        disconnect(this->page(), SIGNAL(loadFinished(bool)), m_webViewport, SLOT(pageLoadFinished(bool)));
    }
    
    m_webViewport->setPage(page);

    connect(this->page()->mainFrame(), SIGNAL(initialLayoutCompleted()), m_webViewport, SLOT(reset()));
    connect(this->page(), SIGNAL(restoreFrameStateRequested(QWebFrame*)), this, SLOT(restoreViewportFromHistory(QWebFrame*)));
    connect(this->page(), SIGNAL(saveFrameStateRequested(QWebFrame*,QWebHistoryItem*)), this, SLOT(saveViewportToHistory(QWebFrame*,QWebHistoryItem*)));
    connect(this->page()->mainFrame(), SIGNAL(contentsSizeChanged(const QSize &)), m_webViewport, SLOT(contentsSizeChanged(const QSize&)));
    connect(this->page(), SIGNAL(loadStarted()), m_webViewport, SLOT(pageLoadStarted()));
    connect(this->page(), SIGNAL(loadProgress(int)), m_webViewport, SLOT(pageLoadProgress(int)));
    connect(this->page(), SIGNAL(loadFinished(bool)), m_webViewport, SLOT(pageLoadFinished(bool)));
}

QGraphicsWebView* WebContentViewWidget::webView() const
{
    return m_webView;
}

QGraphicsWidget* WebContentViewWidget::viewPort() const
{
    return m_webViewport;
}

WebPageData WebContentViewWidget::pageDataFromViewportInfo()
{
    return m_webViewport->pageDataFromViewportInfo();
}

void WebContentViewWidget::setPageDataToViewportInfo(const WebPageData& data)
{
    m_webViewport->setPageDataToViewportInfo(data);
}

WebPageData WebContentViewWidget::defaultZoomData()
{
    return m_webViewport->defaultZoomData();
}

void WebContentViewWidget::setPageZoom(bool zoomIn)
{
    Q_ASSERT(m_webViewport);
    m_webViewport->toggleZoom(zoomIn);
}

void WebContentViewWidget::showPage(bool isSuperPage)
{
    if (!isSuperPage) {
        WRT::WrtBrowserContainer* wbc = WebPageController::getSingleton()->currentPage();
        setPage((QWebPage*)wbc);  // static_cast here gives compiler error
    } else { 
        //Its a super page
        m_webViewport->setSuperPage();
    }
}

void WebContentViewWidget::updatePreferredContentSize()
{
    m_webViewport->updatePreferredContentSize();
}


void WebContentViewWidget::setGesturesEnabled(bool value)
{
    m_webViewport->setGesturesEnabled(value);
}
    
bool WebContentViewWidget::gesturesEnabled()
{
    return m_webViewport->gesturesEnabled();
}

void WebContentViewWidget::restoreViewportFromHistory(QWebFrame* frame)
{
    WRT::WrtBrowserContainer* wbc = WebPageController::getSingleton()->currentPage();
    if(wbc->pageZoomMetaData()->isValid() && wbc->mainFrame() == frame) {
        WebPageData* d = wbc->pageZoomMetaData();
        m_webViewport->setPageDataToViewportInfo(*d);
    }
}

void WebContentViewWidget::saveViewportToHistory(QWebFrame* frame, QWebHistoryItem* item)
{
    WRT::WrtBrowserContainer* wbc = WebPageController::getSingleton()->currentPage();
    if(wbc->mainFrame() == frame) {
        WebPageData d = m_webViewport->pageDataFromViewportInfo();
        if(d.isValid())
            item->setUserData(QVariant::fromValue(d));
    }
}

} // namespace GVA
