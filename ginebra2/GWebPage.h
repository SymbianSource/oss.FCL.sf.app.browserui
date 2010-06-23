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

#ifndef __GINEBRA_GWEBPAGE_H__
#define __GINEBRA_GWEBPAGE_H__

#include <QDebug>
#include <QWebPage>
#include <QWebFrame>
#include "ChromeWidget.h"
#include "WebViewEventContext.h"

namespace GVA {

// ------------------------------
// Simple wrapper class for QWebPage to allow interception of javascript errors.
class WebPageWrapper : public QWebPage {
public:
    WebPageWrapper(QObject *parent, const QString &prefix);

    // Called when javascript errors are hit in the chrome page.
    virtual void javaScriptConsoleMessage(const QString & message, int lineNumber, const QString & sourceID);

    QString m_prefix;
};

// ------------------------------

class GWebPage : public QObject {
    Q_OBJECT
public:
    GWebPage(QWebPage *page) {
        m_page = page;  // take ownership
    }

    virtual ~GWebPage() {
        delete m_page;
    }

    Q_PROPERTY(QString name READ objectName)  // JS API
    Q_PROPERTY(QString title READ getTitle)  // JS API
    QString getTitle() {
        return m_page->mainFrame()->title();
    }

    QWebPage *page() { return m_page; }
    operator QWebPage *() { return m_page; }

    void dump() {
        qDebug() << "GWebPage::dump: " << this;
        qDebug() << "   page=" << (m_page ? m_page : 0);
    }

signals:
    /// Triggered by a QContextEvent such as a long-press or right mouse button click.
    void contextEvent(::WebViewEventContext *context);

protected slots:
    void onContextEvent(::WebViewEventContext *context) { emit contextEvent(context); }

protected:
    QWebPage *m_page;  // owned

    friend class GWebContentViewWidget;
};

// ------------------------------
/*! \ingroup JavascriptAPI
* \brief A content view that has full access to the Javascript APIs.
*
* Example code to load an HTML file into a super page:
* \code
* window.views.WebView.createSuperPage("BookmarkView", true);
* window.views.WebView.BookmarkView.load("./chrome/BookmarkView.html");
* \endcode
*/
class GSuperWebPage : public GWebPage {
    Q_OBJECT
public:
    GSuperWebPage(WebPageWrapper *page, ChromeWidget *chromeWidget);

public slots:
    void load(const QString &url);

signals:
    /*!
    * Triggered by the javascript code within the superpage when it wants a context menu to be displayed by
    * the chrome's javascript.
    *
    * The normal chain of events is:
    * \li User executes a long-press (or RMB click).
    * \li Qt sends QContextMenuEvent from QWebView.
    * \li GWebContentViewWidget::contextMenuEvent is called, which passes the event to the superpage
    * (if one is currently displayed).
    * \li The superpage emits \c contextEvent().
    * \li The context event handler in the superpage's javascript determines what was clicked on
    * and emits \c showContextMenu() from the superpage.
    * \li Javascript \c showContextMenu signal handler in the chrome is called which then displays the context menu.
    */
    void showContextMenu(QVariant obj);

private slots:
    void onJavaScriptWindowObjectCleared();

private:
    ChromeWidget *m_chromeWidget;  // not owned
};

}

#endif // __GINEBRA_GWEBPAGE_H__
