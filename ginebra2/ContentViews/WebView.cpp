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
#include "WebView.h"

#include "browserpagefactory.h"
#include "wrtbrowsercontainer.h"

#include <QWebFrame>
#include <QWebPage>
#include "qstmgestureevent.h"
#include "qstmfilelogger.h"

namespace GVA {

WebView::WebView()
    : WebViewParent()
    , m_webPage(0)
{
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setResizesToContents(true);
    setObjectName("WebView");
    setAcceptHoverEvents(false);   
    installEventFilter(this);
}

WebView::~WebView()
{}

QWebPage* WebView::page() const
{
    return m_webPage;
}

void WebView::setPage(QWebPage* page)
{
    if (m_webPage == page)
        return;

    if (m_webPage) {
        disconnect(m_webPage->mainFrame(), 0, this, 0);
        m_webPage->setView(0);
    }

    m_webPage = page;

    if (!m_webPage)
        m_webPage = createWebPage();

    WebViewParent::setPage(m_webPage);

    setGeometry(QRectF(pos(), m_webPage->mainFrame()->contentsSize()));

    emit titleChanged(title());
    emit urlChanged(url());
}

QWebPage* WebView::createWebPage()
{
    return reinterpret_cast<QWebPage*>(BrowserPageFactory::openBrowserPage());
}


bool WebView::sceneEvent(QEvent* event)
{
    if (!WebViewParent::eventFilter(this, event)) {
        return WebViewParent::sceneEvent(event);
    }
    return false;
}

bool WebView::eventFilter(QObject* o, QEvent* e)
{
    return WebViewParent::eventFilter(o, e);
}


bool WebView::event(QEvent * e) 
{
    if (e->type() == QEvent::Gesture) {
          QStm_Gesture* gesture = getQStmGesture(e);
          if (gesture) {
              QGraphicsObject* go = this->parentItem()->toGraphicsObject();
              return go->event(e);
          }
    }
    return WebViewParent::event(e);
}

}//namespace GVA
