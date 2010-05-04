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



#ifndef GWebContentViewWidget_H_
#define GWebContentViewWidget_H_

#include <QObject>
#include <QWebElement>
#include <QImage>
#include <QGraphicsWebView>
#include <QWebFrame>
#include <QTime>

#include "WebViewEventContext.h"
#include "ZoomMetaData.h"

class QGraphicsSceneContextMenuEvent;

namespace GVA {
#ifndef NO_QSTM_GESTURE
class WebTouchNavigation;
#endif

class GWebContentView;

class GWebContentViewWidget : public QGraphicsWebView
{
    Q_OBJECT
public:

    GWebContentViewWidget(QObject* parent, GWebContentView* view, QWebPage* pg);
    virtual ~GWebContentViewWidget();

    GWebContentView* view() const { return m_webContentView; }

    /*!
     * \brief Returns zoom factor value
     */
    qreal zoomFactor() { return m_wrtPage ? m_wrtPage->mainFrame()->zoomFactor() : 0; }

    /*!
     * \brief Returns dirty zoom factor value
     */
    qreal dirtyZoomFactor() { return m_dirtyZoomFactor; }

    /*!
     * \brief Set zoom factor value
     * \param zoom
     */
    void setZoomFactor(qreal zoom);

    /*!
     * \brief Set dirty zoom factor value
     * \param zoom
     */
    void setDirtyZoomFactor(qreal zoom);

    void setTextSizeMultiplier(qreal factor);

    QImage getPageSnapshot();

    void bitmapZoomCleanup();
    void createPageSnapShot();

    void resizeEvent(QGraphicsSceneResizeEvent* e);
    virtual bool event(QEvent * e);

    QWebPage* page() const;
    void setPage(QWebPage* pg);
    QWebPage* wrtPage() { return m_wrtPage; }
    void showNormalPage();

    void createPagePixmap();
    void setBitmapZoom(qreal zoom);
    void deletePagePixmap();
    void setPageCenterZoomFactor(qreal zoom);

    void initializeViewportParams();
    void parseViewPortParam(const QString &propertyName, const QString &propertyValue);
    bool isUserScalable();
    qreal minimumScale();
    qreal maximumScale();
    qreal initialScale();
    void updateViewport();
    void setBlockElement(QWebElement pt);
    QPointF mapToGlobal(const QPointF& p);
    QPointF mapFromGlobal(const QPointF& p);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) ;
    void setPageZoomFactor(qreal zoom);

    ZoomMetaData pageZoomMetaData();
    void setPageZoomMetaData(ZoomMetaData params);
    ZoomMetaData defaultZoomData();
    void setCheckeredPixmap();
public slots:
    void setViewportSize();

    void onInitLayout();

signals:
    void contextEvent(::WebViewEventContext *context);
    void BlockFocusChanged(QPoint element);

    void pageZoomMetaDataChange(QWebFrame* frame, ZoomMetaData data);protected:
    void paintEvent(QPaintEvent *event);

    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

public:
    void onLoadStarted();
    void onLoadFinished();

private:
    void updateViewportSize(QGraphicsSceneResizeEvent *e);

    GVA::GWebContentView* m_webContentView;
    qreal       m_dirtyZoomFactor;
    QPixmap    *m_frozenPixmap;
    int         m_freezeCount;
    QWebPage   *m_wrtPage;
    bool        m_bitmapZoom;
    QPixmap    *m_pagePixmap;
    qreal       m_aspectRation;
    qreal       m_viewportWidth;
    qreal       m_viewportHeight;
    bool        m_userScalable;
    qreal       m_minimumScale;
    qreal       m_maximumScale;
    qreal       m_initialScale;
    bool        m_inferWidthHeight;
    bool        m_isResize;
    qreal       m_currentinitialScale;
    QWebElement m_BlockElement;
    QWebElement m_BlockInFocus;
    int         m_previousViewPortwidth;
    qreal       m_Ratiox;
    qreal       m_Ratioy;
    
    
    QImage*     m_bitmapImage;
    qreal       m_bitmapZoomFactor;
    QPixmap*    m_checkeredBoxPixmap;
    
    bool        m_inLoading;
    QTime       m_loadingTime;
#ifndef NO_QSTM_GESTURE
    WebTouchNavigation* m_touchNavigation;
#endif
};

}

#endif /* GWebContentViewWidget_H_ */
