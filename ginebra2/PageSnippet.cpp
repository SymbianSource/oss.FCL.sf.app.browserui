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

#include "PageSnippet.h"
#include "PageItem.h"
#include "Utilities.h"
#include "ObjectCharm.h"

#include <QGraphicsWebView>
#include <QVariant>

namespace GVA {

// ----------------------------
// Inlines
// ----------------------------

inline PageItem *PageSnippet::pageItem() {
    return static_cast<PageItem *>(widget());
}

inline PageItem const *PageSnippet::constPageItem() const {
    return static_cast<PageItem const *>(constWidget());
}

// ----------------------------
// PageSnippet
// ----------------------------

PageSnippet::PageSnippet(const QString & elementId, ChromeWidget * chrome,
                         QGraphicsWidget * widget, const QWebElement & element)
  : ChromeSnippet(elementId, chrome, widget, element)
{
}

void PageSnippet::setWidget(QGraphicsWidget * widget) {
    ChromeSnippet::setWidget(widget);
    ExternalEventCharm *charm = new ExternalEventCharm(widget);  // auto deleted
    safe_connect(charm, SIGNAL(externalMouseEvent(int, const QString &, const QString &)),
                 this, SIGNAL(externalMouseEvent(int, const QString &, const QString &)));
}

void PageSnippet::setVisible(bool visiblity, bool animate) {
    qDebug() << "PageSnippet::setVisible: " << visiblity;
    ChromeSnippet::setVisible(visiblity, animate);
}

void PageSnippet::onLoadFinished(bool ok) {  // slot
    qDebug() << "PageSnippet::onLoadFinished: " << ok;
}

QString PageSnippet::url() const {
    return constPageItem()->url();
}

void PageSnippet::setUrl(const QString &url) {
    qDebug() << "PageSnippet::setUrl: " << url;
    pageItem()->setUrl(url);
}

void PageSnippet::setHtml(const QString &value) {
    pageItem()->setHtml(value);
}

QString PageSnippet::html() const {
    return constPageItem()->html();
}

void PageSnippet::setGeometry(int x, int y, int width, int height) {
    widget()->setGeometry(x, y, width, height);
}

void PageSnippet::setSize(int width, int height) {
    pageItem()->setSize(QSizeF(width, height));
}

QVariant PageSnippet::evaluateJavaScript(const QString &expression) {
    return pageItem()->evaluateJavaScript(expression);
}

void PageSnippet::instantiate() {
    pageItem()->instantiate();
}

void PageSnippet::cleanUp() {
    pageItem()->cleanUpOnTimer();
}

}
