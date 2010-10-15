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

#ifndef WebView_h
#define WebView_h

#include <QGraphicsWebView>

#ifdef OWN_BACKING_STORE
#include "TiledWebView.h"
#endif // OWN_BACKING_STORE

class QWebPage;

namespace GVA {

#ifdef OWN_BACKING_STORE
typedef TiledWebView WebViewParent;
#else
typedef QGraphicsWebView WebViewParent;
#endif // OWN_BACKING_STORE

class WebView :public WebViewParent {
    Q_OBJECT
    Q_PROPERTY(QWebPage* page READ page WRITE setPage)
public:
    WebView();
    ~WebView();

    QWebPage* page()const;
    void setPage(QWebPage* page);
    bool event(QEvent * e);
    bool eventFilter(QObject* o, QEvent* e);
    bool sceneEvent(QEvent* event);
    
protected:
    QWebPage* createWebPage();

private:
    QWebPage* m_webPage;

};//WebView
}//namespace GVA

#endif //WebView_h
