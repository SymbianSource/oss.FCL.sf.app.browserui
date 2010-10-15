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

#ifndef WebContentViewWidget_h
#define WebContentViewWidget_h

#include <QGraphicsWidget>
#include "webpagedata.h"

class QGraphicsWebView;
class QWebPage;
class QWebHitTestResult;
class QWebFrame;
class QWebHistoryItem;

namespace GVA {

class ScrollableWebContentView;
class WebView;
class GWebContentView;

class WebContentViewWidget : public QGraphicsWidget {
    Q_OBJECT
    Q_PROPERTY(QWebPage* page READ page WRITE setPage)

public:
    WebContentViewWidget(QObject* parent, GWebContentView* view, QWebPage* page = 0);
    ~WebContentViewWidget();

    void resizeEvent(QGraphicsSceneResizeEvent* event);

    QWebPage* page();
    void setPage(QWebPage* page);

    QGraphicsWebView* webView() const;
    QGraphicsWidget* viewPort() const;
    GWebContentView* view() const { return m_webContentView; }
    void setPageZoom(bool zoomIn);

    WebPageData pageDataFromViewportInfo();
    void setPageDataToViewportInfo(const WebPageData&);
    WebPageData defaultZoomData();
    void showPage(bool isSuperPage);
    void updatePreferredContentSize();
    void setGesturesEnabled(bool value);
    bool gesturesEnabled();
    bool event(QEvent * e);
    
Q_SIGNALS:
    void updateZoomActions(bool enableZoomIn, bool enableZoomOut);
    
    void contextEventObject(QWebHitTestResult* eventTarget, QPointF position);
    
    void viewScrolled(QPoint& scrollPos,QPoint& delta);
    void mouseEvent(QEvent::Type type);

#ifdef Q_WS_MAEMO_5
protected slots:
    void onContextEventObject(QWebHitTestResult* hitTest, QPointF position);
#endif

public slots:
    void restoreViewportFromHistory(QWebFrame*);
    void saveViewportToHistory(QWebFrame*, QWebHistoryItem*);

private:
    GWebContentView* m_webContentView;
    WebView* m_webView;
    ScrollableWebContentView* m_webViewport;
};

}// namespace GVA

#endif //WebContentViewWidget_h
