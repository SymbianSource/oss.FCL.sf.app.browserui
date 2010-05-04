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


#ifndef WEBCONTENTVIEW_H
#define WEBCONTENTVIEW_H

#include <QObject>
#include <QVariant>
#include "qgraphicswebview.h"
#include "controllableviewimpl.h"
#include "messageboxproxy.h"
#include <qwebelement.h>
#include "ZoomMetaData.h"

//class FlickCharm;
class WebContentView;
class QContextMenuEvent;
class WebViewEventContext;
class QGraphicsSceneContextMenuEvent;
class QWebPage;
class QWebFrame;
namespace WRT {
class MessageBoxProxy;
}

class BWF_EXPORT WebContentWidget : public QGraphicsWebView
{
    Q_OBJECT
public:

    WebContentWidget(QObject* parent,WebContentView* view,QWebPage* pg);
    virtual ~WebContentWidget();

    inline WebContentView* view() const { return m_webContentView; }

    /*!
     * \brief Returns zoom factor value
     */
    qreal zoomFactor() { return m_wrtPage->mainFrame()->zoomFactor(); }

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

    QWebPage* page() const;
    void setPage(QWebPage* pg);
    QWebPage* wrtPage() { return m_wrtPage; }
    
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

    void setCheckeredPixmap();

    ZoomMetaData pageZoomMetaData();
    void setPageZoomMetaData(ZoomMetaData params); 
    ZoomMetaData defaultZoomData();

public slots:
    void setViewportSize();
    
signals:
    void contextEvent(WebViewEventContext *context);
	void BlockFocusChanged(QPoint element);

    void pageZoomMetaDataChange(QWebFrame* frame, ZoomMetaData data);protected:
    void paintEvent(QPaintEvent *event);

protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
    
private:
    void updateViewportSize(QGraphicsSceneResizeEvent* e);

    WebContentView* m_webContentView;
    qreal		m_dirtyZoomFactor;
    QPixmap*    m_frozenPixmap;
    int         m_freezeCount;
    QWebPage* 	m_wrtPage;
    QPixmap*    m_pagePixmap;
    bool        m_bitmapZoom;
    qreal       m_aspectRation;
    int         m_viewportWidth;
    int         m_viewportHeight;
    bool        m_userScalable;
    qreal       m_minimumScale;
    qreal       m_maximumScale;
    qreal       m_initialScale;
    bool        m_inferWidthHeight;
    bool        m_isResize;
    qreal       m_currentinitialScale;
    QWebElement	m_BlockElement;
    QWebElement	m_BlockInFocus;
    int         m_previousViewPortwidth;
    qreal       m_Ratiox;
    qreal       m_Ratioy;
    QImage*     m_bitmapImage;
    qreal       m_bitmapZoomFactor;
    QPixmap*    m_checkeredBoxPixmap;
};

typedef ControllableViewBase WebContentViewBase;
class BWF_EXPORT WebContentView : public WebContentViewBase 
{
    Q_OBJECT
public:
    WebContentView(QWebPage* pg,QWidget *parent = 0);
    virtual ~WebContentView();

    static QString Type() { return "webView"; }
    virtual QString type() const { return Type(); }

    QGraphicsWidget* widget() const { return static_cast<QGraphicsWidget*>(m_widget); }

    // Returns the DOM 'window' object of the page.
    QVariant getContentWindowObject();

    virtual void connectAll();
    virtual void activate();
    virtual void deactivate();

    QString title() const { return webView()->title(); }
    QUrl url() { return webView()->url(); }

    qreal getZoomFactor() const;

    WebContentWidget *webView() const { return m_widget; }

    static ControllableView* createNew(QWidget *parent);

    /*! connect objects titleChanged(const QString& title) signal
     * !param receiver connect receiver object 
     * !param method object slot to connect 
     */
    virtual void connectTitleChanged(const QObject* receiver, const char* method)
        { connect( this,SIGNAL(titleChanged),receiver,method ); }


    /*! connect objects ContextChanged() signal
     * !param receiver connect receiver object 
     * !param method object slot to connect 
     */
    virtual void connectContextChanged(const QObject* receiver, const char* method)
        { connect( this,SIGNAL(ContextChanged),receiver,method ); }

    /*! 
      Return the list of public QActions most relevant to the view's current context
      (most approptiate for contextual menus, etc.
    */
    virtual QList<QAction*> getContext();

    QWebPage* wrtPage() { return m_widget->wrtPage(); }
    void scrollViewBy(int dx, int dy);
    void scrollViewTo(int x, int y);

    void  changeZoomAction(qreal zoom);
    void deactivateZoomActions();

    void bitmapZoomStop();
signals:
    void titleChanged(const QString& title);
    void ContextChanged();

public slots:
    void zoomIn(qreal factor = 0.1);
    void zoomOut(qreal factor = 0.1);
    void setZoomFactor(qreal factor);
    void showMessageBox(WRT::MessageBoxProxy* data);
    void zoomP();
    void zoomN();
protected:
    WebContentWidget *webViewConst() const { return m_widget; }

protected:
    WebContentWidget *m_widget;
    QNetworkAccessManager *m_networkMgr; //Owned
	// NetworkAccessManager *m_networkMgr; //Owned
//    FlickCharm* m_flickCharm; //Owned

private:

    void setZoomActions();

    QAction * m_actionZoomIn;
    QAction * m_actionZoomOut;
    QTimer *m_timer;
    qreal m_value;
};

// ----------------------------------------------------------

/*!
  \brief This class provides the javascript API to WebContentViews.
  \sa WebContentView
 */
class BWF_EXPORT WebContentViewJSObject : public ControllableViewJSObject {
    Q_OBJECT
  public:
    WebContentViewJSObject(WebContentView *contentView, QWebFrame *chromeFrame)
      : ControllableViewJSObject(contentView, chromeFrame, "webView")
    {
    }

    qreal getZoomFactor() const { return webContentViewConst()->getZoomFactor(); }
    void setZoomFactor(qreal factor)  { webContentView()->setZoomFactor(factor); }
    Q_PROPERTY(qreal zoomFactor READ getZoomFactor WRITE setZoomFactor)

public slots:
    void zoomIn(qreal deltaPercent = 0.1) { webContentView()->zoomIn(deltaPercent); }
    void zoomOut(qreal deltaPercent = 0.1) { webContentView()->zoomOut(deltaPercent); }

signals:
    void onStatusBarMessage( const QString & text );
    void onStatusBarVisibilityChangeRequested(bool visible);

    // Sent when the display mode changes from landscape to protrait or vice versa.
    void onDisplayModeChanged(const QString &orientation);

    void contextEvent(QObject *context);

private slots:
    void statusBarMessage( const QString & text );
    void statusBarVisibilityChangeRequested(bool visible);
    void onContextEvent(WebViewEventContext *context);

protected:
    WebContentView *webContentView() { return static_cast<WebContentView *>(m_contentView); }
    WebContentView *webContentViewConst() const { return static_cast<WebContentView *>(m_contentView); }
};

#endif // WEBCONTENTVIEW_H
