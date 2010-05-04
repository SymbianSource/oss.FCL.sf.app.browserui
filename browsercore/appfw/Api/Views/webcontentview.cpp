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


#include <QDebug>
#include "qwebframe.h"
#include <QGraphicsSceneResizeEvent>
#include <QGraphicsView>
#include <QGraphicsWebView>
#include <QNetworkReply>
#include <QPainter>
#include <QSettings>
#include <QWebPage>
#include <QWebHistory>
#include <qwebelement.h>
#include <QGraphicsSceneContextMenuEvent>
#include <QTimer>
#include <qevent.h>

#include "browserpagefactory.h"
#include "webcontentview.h"
#include "controllableviewjsobject.h"
#include "scriptobjects.h"
#include "WebViewEventContext.h"

#define safe_connect(src, srcSig, target, targetSlot) \
    { int res = connect(src, srcSig, target, targetSlot); assert(res); }

// ----------------------------------------------------------

const QString KViewPortWidthTag("width");
const QString KViewPortHeightTag("height");
const QString KViewPortInitialScaleTag("initial-scale");
const QString KViewPortMinScaleTag("minimum-scale");
const QString KViewPortMaxScaleTag("maximum-scale");
const QString KViewPortUserScalableTag("user-scalable");
const QString KViewPortDeviceWidthTag("device-width");
const QString KViewPortDeviceHeightTag("device-height");


const int KDefaultViewportWidth = 980;
const int KDefaultPortraitScaleWidth = 540;
const int KMinViewportWidth = 200;
const int KMaxViewportWidth = 10000;
const int KMinViewportHeight = 200;
const int KMaxViewportHeight = 10000;
const int KMaxPageZoom = 10;
const qreal KDefaultMinScale = 0.25;
const qreal KDefaultMaxScale = 10.00;
const QPoint KFocussPoint(5, 50);
const qreal KZoomInStep = 1.05;
const qreal KZoomOutStep = 0.95238;
const int checkerSize = 16;
const unsigned checkerColor1 = 0xff555555;
const unsigned checkerColor2 = 0xffaaaaaa;

WebContentView::WebContentView(QWebPage* pg,QWidget *parent)
  : m_networkMgr(0)
  ,m_timer(NULL)

//, m_flickCharm(0)
{
    qDebug() << "WebContentView::WebContentView";
    m_widget = new WebContentWidget(parent,this,pg);
    setZoomActions();

    m_jsObject = new WebContentViewJSObject(this, 0);

    m_networkMgr = webView()->page()->networkAccessManager();

    webView()->page()->currentFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    webView()->page()->currentFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    
 
    connectAll();
}

void WebContentView::connectAll() {

    safe_connect(widget(), SIGNAL(contextEvent(WebViewEventContext *)), m_jsObject, SLOT(onContextEvent(WebViewEventContext *)));
}

WebContentView::~WebContentView() {
    disconnect(m_jsObject);
    disconnect(webView());

    delete m_actionZoomIn;
    delete m_actionZoomOut;

    delete m_widget;
}

QVariant WebContentView::getContentWindowObject() {
    try {
        return webView()->page()->mainFrame()->evaluateJavaScript("window");
    }
    catch(...) {
        qDebug() << "WebContentView::getContentWindowObject: caught expection";
        return QVariant();
    }
}

void WebContentView::setZoomActions(){

    // Create zoomIn and zoomOut actions */
    m_actionZoomIn = new QAction("zoomIn", this);
    m_actionZoomIn->setObjectName("zoomIn");

    m_actionZoomOut = new QAction("zoomOut", this);
    m_actionZoomOut->setObjectName("zoomOut");
    /* Disable zoomOut action initially as we are the the minimum scale */
    /* Revisit this to determine whether we can use the change signal to 
     * set the zoomOut button image initially as well
     */
    m_actionZoomOut->setEnabled(false);

    connect(m_actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(m_actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));


}


void WebContentView::bitmapZoomStop()
{
    if(m_timer) {
        m_timer->stop();
        disconnect(m_timer,SIGNAL(timeout()));
        delete m_timer;
        m_timer = NULL;	
	}	
    qreal zoomFactor = m_value * webView()->zoomFactor();
    ( (zoomFactor+0.001) >= webView()->maximumScale() )? webView()->setZoomFactor(webView()->maximumScale()):webView()->setZoomFactor(zoomFactor);
    webView()->bitmapZoomCleanup();
}

void WebContentView::zoomP()
{
    if((m_value * webView()->zoomFactor()) >  webView()->maximumScale()) {
	    if(m_timer && m_timer->isActive())
		    bitmapZoomStop();
    }else {
        if(m_timer->isSingleShot()) {
            m_timer->setSingleShot(false);
            m_timer->start(1);
        }
	    webView()->setBitmapZoom(m_value * webView()->zoomFactor());
	    m_value *= KZoomInStep;
    }
}

void WebContentView::zoomN()
{
    if((m_value * webView()->zoomFactor()) <  webView()->minimumScale()){
	    if(m_timer && m_timer->isActive())
		    bitmapZoomStop();	
    }else {
        if(m_timer->isSingleShot()) {
            m_timer->setSingleShot(false);
            m_timer->start(1);
        }
        webView()->setBitmapZoom(m_value * webView()->zoomFactor());
	    m_value *= KZoomOutStep;
    }
}

void WebContentView::zoomIn(qreal deltaPercent)	
{	
    if(webView() && webView()->isUserScalable()) {
        if(m_timer && m_timer->isActive()) {
            if(!m_timer->isSingleShot())
                m_value /= KZoomInStep;
	        bitmapZoomStop();
	        return;
        }else if(!m_timer)
            m_timer = new QTimer(this);
  
        m_value = KZoomInStep;
	
        if( (m_value * webView()->zoomFactor()) <  webView()->maximumScale()) {
  	        webView()->createPageSnapShot();
		    bool ret = connect(m_timer,SIGNAL(timeout()),this,SLOT(zoomP()));
		    zoomP();
            m_timer->setSingleShot(true);
		    m_timer->start(500);
        }else {
            delete m_timer;
	        m_timer = NULL;
	        webView()->setZoomFactor(m_value * webView()->zoomFactor());
        }
    }   
}

void WebContentView::zoomOut(qreal deltaPercent)	
{
    if(webView() && webView()->isUserScalable()) {
        if(m_timer && m_timer->isActive()) {
            if(!m_timer->isSingleShot())
		        m_value /= KZoomOutStep;
		    bitmapZoomStop();
	        return;
        }else if(!m_timer)
		    m_timer = new QTimer(this);
 
        m_value = KZoomOutStep;

        if( (m_value * webView()->zoomFactor()) >  webView()->minimumScale()) {
            webView()->createPageSnapShot();
            bool ret = connect(m_timer,SIGNAL(timeout()),this,SLOT(zoomN()));
            zoomN();
            m_timer->setSingleShot(true);
            m_timer->start(500);
        }else {
	        delete m_timer;
	        m_timer = NULL;
  	        webView()->setZoomFactor(m_value * webView()->zoomFactor());
        }
    }
}
void WebContentView::deactivateZoomActions()
{
	m_actionZoomOut->setEnabled(false);
	m_actionZoomIn->setEnabled(false);
}

void WebContentView::changeZoomAction(qreal zoom){
    
    if(!(webView()->isUserScalable() ) ){
        deactivateZoomActions();
    }
    else {

        if (zoom <=   webView()->minimumScale() ) {
           m_actionZoomOut->setEnabled(false); 
        }
        else { 
           m_actionZoomOut->setEnabled(true); 
        }

        if (zoom >=  webView()->maximumScale()  ){
           m_actionZoomIn->setEnabled(false); 
        }
        else { 
           m_actionZoomIn->setEnabled(true); 
        }
    }
}

void WebContentView::setZoomFactor(qreal factor){
  if(webView())
      webView()->setZoomFactor(factor);
}

qreal WebContentView::getZoomFactor() const {
  return webViewConst() ? webViewConst()->zoomFactor() : 0.0;
}




void WebContentView::activate() {
    WebContentViewBase::activate();
}

void WebContentView::deactivate() {
    WebContentViewBase::deactivate();
}

static void appendAction(QWebPage* page, QList<QAction*> &list, enum QWebPage::WebAction webAction, const QString &name) {
    QAction *action = page->action(webAction);
    if(action) {
        action->setObjectName(name);
        list.append(action);
    }
}

/*!
  Return the list of public QActions most relevant to the view's current context.
  @return  List of public actions
*/
QList<QAction *> WebContentView::getContext()
{
    // Get some of the actions from the page (there are many more available) and build a list
    // list of them.  

    QList<QAction*> actions;

    /* Add zoomIn and zoomOut actions created earlier*/
    actions.append(m_actionZoomIn);
    actions.append(m_actionZoomOut);
     
    return actions;
}

void WebContentView::scrollViewBy(int dx, int dy)
{
    wrtPage()->mainFrame()->scroll(dx, dy);
}

void WebContentView::scrollViewTo(int x, int y)
{
    wrtPage()->mainFrame()->setScrollPosition(QPoint(x, y));
}


void WebContentView::showMessageBox(WRT::MessageBoxProxy* proxy)
{
/*
    QMessageBox msgBox(this);
    msgBox.setText(proxy->m_text);
    msgBox.setInformativeText(proxy->m_informativeText);
    msgBox.setDetailedText(proxy->m_detailedText);
    msgBox.setStandardButtons(proxy->m_buttons);
    msgBox.setDefaultButton(proxy->m_defaultButton);
    msgBox.setIcon(proxy->m_icon);
    int ret = msgBox.exec();
    */
    QString displayText = proxy->m_text + QLatin1String("\n") + QLatin1String("\n")+ proxy->m_detailedText + QLatin1String("\n") + QLatin1String("\n") + proxy->m_informativeText;
    int ret = QMessageBox::warning(0/* TODO: find appropriate widget if required or just remove this widget()*/, 
                                   proxy->m_text, displayText, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    proxy->onMessageBoxResponse(ret);
}

ControllableView* WebContentView::createNew(QWidget *parent) 
{ 
    QWebPage* page = BrowserPageFactory::openBrowserPage();
    return new WebContentView(page, parent);
}


// ---------------------------------------------------------------------------
// WebContentViewJSObject
// ---------------------------------------------------------------------------

void WebContentViewJSObject::statusBarMessage( const QString & text ){
    emit onStatusBarMessage(text);
}

void WebContentViewJSObject::statusBarVisibilityChangeRequested(bool visible){
    emit onStatusBarVisibilityChangeRequested(visible);
}

void WebContentViewJSObject::onContextEvent(WebViewEventContext *context){
    QWebFrame *chrome = chromeFrame();
    if(chrome) {
        chrome->addToJavaScriptWindowObject(context->objectName(), context, QScriptEngine::ScriptOwnership);
    }
    emit contextEvent(context);
}

// ---------------------------------------------------------------------------
// WebContentWidget
// ---------------------------------------------------------------------------
void WebContentWidget::updateViewport()
{
    if (page() && size() != page()->viewportSize()) {
        page()->setViewportSize(size().toSize());
    }
    setViewportSize();
}

void WebContentWidget::setBlockElement(QWebElement pt)
{
	m_BlockElement = pt;
}

QImage WebContentWidget::getPageSnapshot()
{
    QImage img(size().toSize(), QImage::Format_RGB32);
    QPainter painter(&img);
    QWebFrame *frame = page()->mainFrame();

    painter.fillRect(0, 0, size().width(), size().height(), QColor(255, 255, 255));
//    QTransform transform;
//    transform.scale(d->m_pageZoomFactor, d->m_pageZoomFactor);
//    painter.translate(-transform.map(frame->scrollPosition()));

    QRegion clipRegion(QRect(QPoint(0,0),size().toSize()));
//    QTransform invert = transform.inverted();
//    clipRegion = invert.map(clipRegion);
//    clipRegion.translate(frame->scrollPosition());

//    painter.scale(d->m_pageZoomFactor, d->m_pageZoomFactor);
//    d->m_webPage->mainFrame()->renderContents(&painter, clipRegion);
    frame->render(&painter, clipRegion);

    return img;
}

void WebContentWidget::updateViewportSize(QGraphicsSceneResizeEvent* e)
{
    //if there is change in mode (like landscape, potraite relayout the content)
    if (e->newSize().width() == e->oldSize().width())
        return;
	m_isResize = true;
    setViewportSize();
	m_isResize = false;
}

void WebContentWidget::resizeEvent(QGraphicsSceneResizeEvent* e)
{
    // set the fixed text layout size for text wrapping
    if (page()) {
#if defined CWRTINTERNALWEBKIT
        p->m_webPage->settings()->setMaximumTextColumnWidth(e->newSize().width() - 6);
#endif
    }

	m_previousViewPortwidth = page()->viewportSize().width();
	
    const QSize &s = e->newSize().toSize();
    if (page() && s != page()->viewportSize()) {
		if(m_BlockElement.isNull()) {
			QPoint pos = QPoint(0,0);
			QWebFrame* frame = page()->frameAt(pos);
			frame = (frame) ? frame : page()->currentFrame();
			QWebHitTestResult htr = frame->hitTestContent(pos);
			m_BlockInFocus = htr.element();

			if(m_BlockInFocus.tagName() != "IMG")
				m_BlockInFocus = htr.enclosingBlockElement();

			QPoint position = m_BlockInFocus.geometry().topLeft() - page()->currentFrame()->scrollPosition();
			m_Ratiox = (qreal) position.x() / m_BlockInFocus.geometry().width();
			m_Ratioy = (qreal) position.y() / m_BlockInFocus.geometry().height();
		}
        page()->setViewportSize(s);
    }

    updateViewportSize(e);
}

void WebContentWidget::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) 
{
    QPoint p = event->pos().toPoint();
    QWebHitTestResult hitTest = page()->currentFrame()->hitTestContent(p); 

    WebViewEventContext *context = 
        new WebViewEventContext(view()->type(), hitTest);
    
    emit contextEvent(context);
    event->accept();
}

void WebContentWidget::setZoomFactor(qreal zoom)
{
    this->setFocus();
    if (!m_userScalable)
        return;

    setPageZoomFactor(zoom);
}

void WebContentWidget::setPageZoomFactor(qreal zoom)
{

    //qDebug() << __func__ << "Zoom " << zoom << "Max : " << m_maximumScale << "Min: " << m_minimumScale;

    if (zoom < m_minimumScale)
        zoom = m_minimumScale;
    else if (zoom > m_maximumScale)
        zoom = m_maximumScale;

 
	QPoint pos = QPoint(0,0);
	
	if(!m_isResize) {
		QWebFrame* frame = page()->frameAt(pos);
		frame = (frame) ? frame : page()->currentFrame();
		QWebHitTestResult htr = frame->hitTestContent(pos);
		m_BlockInFocus = htr.element();

		if(m_BlockInFocus.tagName() != "IMG")
			m_BlockInFocus = htr.enclosingBlockElement();

		QPoint position = m_BlockInFocus.geometry().topLeft() - page()->currentFrame()->scrollPosition();
		m_Ratiox = (qreal) position.x() / m_BlockInFocus.geometry().width();
		m_Ratioy = (qreal) position.y() / m_BlockInFocus.geometry().height();
	}
    
    if( m_dirtyZoomFactor != zoom ) {
        m_dirtyZoomFactor = zoom;
    }

    QGraphicsWebView::setZoomFactor( zoom );
   
	if(!m_BlockElement.isNull() && m_isResize) {
		QPoint imageFocusPoint;
		QPoint m_focusedBlockPt = QPoint(m_BlockElement.geometry().topLeft()) - page()->mainFrame()->scrollPosition(); 
		if(m_BlockElement.tagName() != "IMG" && (m_BlockElement.styleProperty(QString("background-image"),QWebElement::InlineStyle) == "")) 
			page()->mainFrame()->scroll(m_focusedBlockPt.x() - KFocussPoint.x() , m_focusedBlockPt.y() - KFocussPoint.y());                                    
		else {
			if((page()->viewportSize().width() - m_BlockElement.geometry().width()) > 0)
				imageFocusPoint.setX((page()->viewportSize().width() - m_BlockElement.geometry().width())/2);
			else
				imageFocusPoint.setX(0);

			if((page()->viewportSize().height() - m_BlockElement.geometry().height()) > 0)
				imageFocusPoint.setY((page()->viewportSize().height() - m_BlockElement.geometry().height())/2);
			else
				imageFocusPoint.setY(0);

			page()->mainFrame()->scroll(m_focusedBlockPt.x() - imageFocusPoint.x() , 
										m_focusedBlockPt.y() - imageFocusPoint.y());
			}
			m_focusedBlockPt = QPoint(m_BlockElement.geometry().topLeft()) - page()->mainFrame()->scrollPosition();
			emit BlockFocusChanged(m_focusedBlockPt);
	} else {
		QPoint m_focusedBlockPt = QPoint(m_BlockInFocus.geometry().topLeft()) - page()->mainFrame()->scrollPosition(); 
		page()->currentFrame()->scroll(m_focusedBlockPt.x() - (m_Ratiox * m_BlockInFocus.geometry().width()),
									m_focusedBlockPt.y() - (m_Ratioy * m_BlockInFocus.geometry().height()));
		m_BlockElement = QWebElement();
	}
		
    m_webContentView->changeZoomAction(zoom);
    
}

void WebContentWidget::setDirtyZoomFactor(qreal zoom)
{
    if( m_dirtyZoomFactor == zoom )
        return;

    m_dirtyZoomFactor = zoom;

    update();
}


void WebContentWidget::setCheckeredPixmap()
{
    delete m_checkeredBoxPixmap;
    m_checkeredBoxPixmap = NULL;
    int checkerPixmapSizeX = size().toSize().width();
    int checkerPixmapSizeY = size().toSize().height() + 50;
    m_checkeredBoxPixmap = new QPixmap(size().width(), size().height() + 50);
    QPainter painter(m_checkeredBoxPixmap);
   
    for (int y = 0; y < checkerPixmapSizeY; y += checkerSize / 2) {
        bool alternate = y % checkerSize;
        for (int x = 0; x < checkerPixmapSizeX; x += checkerSize / 2) {
            QColor color(alternate ? checkerColor1 : checkerColor2);
            painter.fillRect(x, y, checkerSize / 2, checkerSize / 2, color);
            alternate = !alternate;
        }
    }
}

void WebContentWidget::createPageSnapShot()
{
    bitmapZoomCleanup();
    QRegion clipRegion;
    QWebFrame *frame = page()->mainFrame();
    m_bitmapImage = new QImage(size().width() ,size().height(),QImage::Format_RGB32);
    clipRegion = QRect(QPoint(0,0),size().toSize());
    QPainter painterImage(m_bitmapImage);
    painterImage.fillRect(0, 0, size().width(), size().height(), QColor(255, 255, 255));
    frame->render(&painterImage,clipRegion);
}

void WebContentWidget::bitmapZoomCleanup()
{
    m_bitmapZoom = false;
    if(m_bitmapImage) {
	    delete m_bitmapImage;
		m_bitmapImage = NULL;
	}
}

void WebContentWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) 
{
    //if(!m_active) return;
    if(m_freezeCount > 0) {
        // Frozen, paint the snapshot.
        painter->drawPixmap(0, 0, *m_frozenPixmap);
    }else {
        if (m_bitmapZoom) {
            qreal czf = 1;
            qreal zoomF = zoomFactor();

            if( m_bitmapZoomFactor != zoomF )
                czf = m_bitmapZoomFactor/zoomF;
				
	        painter->save();
	       
            if(czf < 1) 
		        painter->drawPixmap(QPoint(0,0), *m_checkeredBoxPixmap);
	        
	        painter->drawImage(QRectF(0,0,size().width() * czf,size().height() * czf), *m_bitmapImage);
	        painter->restore();
        } else if( zoomFactor() == m_dirtyZoomFactor ) {
            // Cannot use normal QGraphicsWebView paint because have to fill background with white for the phone build
            // QGraphicsWebView::paintEvent( event);
            QWebFrame* frame = page()->mainFrame();
            painter->fillRect(0, 0, size().width(), size().height(), QColor(255, 255, 255));
            QGraphicsWebView::paint(painter, option, widget);
        } else {
            qreal czf = m_dirtyZoomFactor / zoomFactor();

            QWebFrame* frame = page()->mainFrame();

            painter->save();
            painter->fillRect(0, 0, size().width(), size().height(), QColor(255, 255, 255));
            QTransform transform;
            transform.scale(czf, czf);
            //painter.translate(-transform.map(frame->scrollPosition()));

            QRegion clipRegion = geometry().toRect();
        
            if(option && !option->exposedRect.isEmpty())
                clipRegion.intersect( option->exposedRect.toRect());
        
            QTransform invert = transform.inverted();
            clipRegion = invert.map(clipRegion);
            //clipRegion.translate(frame->scrollPosition());

            painter->scale(czf, czf);
            //p->m_webPage->mainFrame()->renderContents(&painter, clipRegion);
            frame->render(painter, clipRegion);
            painter->restore();   
        }
    }
}
 
WebContentWidget::WebContentWidget(QObject* parent, WebContentView* view,QWebPage* pg) : QGraphicsWebView(0)    
, m_webContentView(view)
, m_dirtyZoomFactor(1)
, m_frozenPixmap(0)
, m_freezeCount(0)
, m_wrtPage(0)
, m_bitmapZoom(false)
, m_pagePixmap(0)
, m_isResize(false)
, m_currentinitialScale(0)
, m_previousViewPortwidth(size().toSize().width())
, m_bitmapImage(NULL)
, m_checkeredBoxPixmap(NULL)
{
    setParent(parent);
    if( pg )
    {
        setPage(pg);
    }
	m_currentinitialScale = zoomFactor();
	connect(page()->mainFrame(), SIGNAL(initialLayoutCompleted()), this, SLOT(setViewportSize()));
}

WebContentWidget::~WebContentWidget()
{
    if(m_wrtPage) 
    {
        m_wrtPage->setView(0);
        setPage(0);
    }
    if(m_bitmapImage)
	    delete m_bitmapImage;
    if(m_checkeredBoxPixmap)
        delete m_checkeredBoxPixmap;
}

void WebContentWidget::setPage(QWebPage* pg)
{
    if(m_wrtPage) {
        m_wrtPage->setView(0);
    }
    /* Reset the webview page as well - for its internal clean up */
    QGraphicsWebView::setPage(pg);    
     
    m_wrtPage = pg;

}

void WebContentWidget::createPagePixmap()
{
    if (m_pagePixmap)
       delete m_pagePixmap;

    m_pagePixmap = new QPixmap(size().toSize());
    QStyleOptionGraphicsItem op;
    QPainter p(m_pagePixmap);
    paint(&p,&op,0);
    p.end();
}

void WebContentWidget::setBitmapZoom(qreal zoomF) {
    if (!m_userScalable || (zoomF == zoomFactor()))
        return;
    if (zoomF < m_minimumScale)
        zoomF = m_minimumScale;
    else if (zoomF > m_maximumScale)
        zoomF = m_maximumScale;

    m_bitmapZoom = true;
    m_bitmapZoomFactor = zoomF;
    update();
}

void WebContentWidget::deletePagePixmap()
{
    if (m_pagePixmap) {
        delete m_pagePixmap;
        m_pagePixmap = 0;
    }
    m_bitmapZoom = false;
}

void WebContentWidget::setPageCenterZoomFactor(qreal zoom)
{
    //calculating the center of the widget
    QPoint widgetCenter = rect().center().toPoint();
    //find the content size before applying zoom
    QSize docSizeBeforeZoom = page()->mainFrame()->contentsSize();

    qDebug()<<"setPageCenterZoomFactor() : "<<zoom;
    setZoomFactor(zoom);
    //after applying zoom calculate the document size and document center point
    QSize docSizeAfterZoom = page()->mainFrame()->contentsSize();
    QPoint docPoint = widgetCenter + page()->mainFrame()->scrollPosition();
    
    //calculate the shift in center point after applying zoom
    int dx = docSizeAfterZoom.width() * docPoint.x() / docSizeBeforeZoom.width();
    int dy = docSizeAfterZoom.height() * docPoint.y() / docSizeBeforeZoom.height();

    //move back the shifted center
    page()->mainFrame()->scroll(dx-docPoint.x(), dy-docPoint.y()); 
}

void WebContentWidget::initializeViewportParams()
{    
    m_maximumScale = KDefaultMaxScale;
    m_userScalable = true;
    m_inferWidthHeight = true;

    m_aspectRation = size().width() / size().height();
    m_viewportWidth = KDefaultViewportWidth;
    m_viewportHeight = (int)size().height();
	     
    if( size().width() < size().height())				//if Portrait 
    	m_initialScale = size().width() / KDefaultPortraitScaleWidth;
    else
    	m_initialScale = size().width() / KDefaultViewportWidth;
    m_minimumScale = m_initialScale;

}

/*!
 * Provides the default values - used when opening a new blank window
 */ 
ZoomMetaData WebContentWidget::defaultZoomData()
{    
    ZoomMetaData data;

    data.maxScale = KDefaultMaxScale;
    data.minScale =  KDefaultMinScale;
    data.userScalable = false;

    return data;
}

/*!
 * Set the viewport Size
 */ 
void WebContentWidget::setViewportSize()
{
    QWebFrame* frame = page()->mainFrame();

    initializeViewportParams();

    // TODO: INVESTIGATE: In the case of multiple windows loading pages simultaneously, it is possible
    // to be calling this slot on a signal from a frame that is not
    // the frame of the page saved here. It might be better to use 'sender' instead of
    // page->mainFrame() to get the metaData so that we use the meta data of the corresponding
    // frame
    QMap<QString, QString> metaData = frame->metaData();
    QString viewportTag = metaData.value("viewport");
    
    if (!viewportTag.isEmpty()) {
        QStringList paramList;

        if (viewportTag.contains(';')) {
            paramList = viewportTag.split(";", QString::SkipEmptyParts);
        } else {
            paramList = viewportTag.split(",", QString::SkipEmptyParts);
        }

        int paramCount = 0;
        while (paramCount < paramList.count()) { 
            QStringList subParamList = paramList[paramCount].split ('=', QString::SkipEmptyParts);
            paramCount++;
            QString viewportProperty = subParamList.front();
            QString propertyValue = subParamList.back();
            parseViewPortParam(viewportProperty.trimmed(), propertyValue.trimmed());
        }    
    }

    m_initialScale = qBound(m_minimumScale, m_initialScale, m_maximumScale);

#if QT_VERSION < 0x040600
    page()->setFixedContentsSize(QSize(m_viewportWidth, m_viewportHeight));
#else    
    page()->setPreferredContentsSize(QSize(m_viewportWidth, m_viewportHeight)); 
#endif
	qreal zoomF = 0.0;
	QString str;
	if(m_isResize &&  (m_currentinitialScale != zoomFactor())) {
		zoomF = ((qreal)(page()->viewportSize().width()-10) * zoomFactor())/(m_previousViewPortwidth-10);
		str.setNum(zoomF,'f',2);
		zoomF = str.toDouble();
		setPageZoomFactor(zoomF);
	}
	else {
		setPageZoomFactor(m_initialScale);
	}
	m_BlockInFocus = QWebElement();
	m_currentinitialScale = m_initialScale;
	
	setCheckeredPixmap();
    
	// Let the page save the data. Even though it is part of the frame, it is easier to
    // save the info in the page to avoid parsing the meta data again. 
    emit pageZoomMetaDataChange(frame, pageZoomMetaData());
}

qreal WebContentWidget::initialScale() 
{
	return 	m_initialScale;
}

void WebContentWidget::parseViewPortParam(const QString &propertyName, const QString &propertyValue)
{
    if (propertyName == KViewPortWidthTag) {
	    if (propertyValue == KViewPortDeviceWidthTag) {
            m_viewportWidth = (int)size().width();
		    m_viewportHeight = m_viewportWidth * m_aspectRation;
	    }
        else if(propertyValue == KViewPortDeviceHeightTag) {
            m_viewportWidth = (int)size().height();
		    m_viewportHeight = m_viewportWidth * m_aspectRation;        
        }
        else {
		    m_viewportWidth = propertyValue.toInt();

            if (m_viewportWidth < KMinViewportWidth)
			    m_viewportWidth = KMinViewportWidth;
		    else if (m_viewportWidth > KMaxViewportWidth)
			    m_viewportWidth = KMaxViewportWidth;

            m_viewportHeight = m_viewportWidth * m_aspectRation;
	    }
        m_initialScale = size().width() / m_viewportWidth;
        if (m_initialScale < KDefaultMinScale || m_initialScale > KDefaultMaxScale)
            m_initialScale = KDefaultMinScale;
        m_minimumScale = m_initialScale;
        m_inferWidthHeight = false;
    }
    else if (propertyName == KViewPortHeightTag) {
	    if (propertyValue == KViewPortDeviceWidthTag) {
            m_viewportHeight = (int)size().width();
		    m_viewportWidth = m_viewportHeight * m_aspectRation;
	    }
        else if (propertyValue == KViewPortDeviceHeightTag) {
            m_viewportHeight = (int)size().height();
		    m_viewportWidth = m_viewportHeight * m_aspectRation;        
        }
        else {
		    m_viewportHeight = propertyValue.toInt();

            if (m_viewportHeight < KMinViewportHeight)
			    m_viewportHeight = KMinViewportHeight;
		    else if (m_viewportHeight > KMaxViewportHeight)
			    m_viewportHeight = KMaxViewportHeight;

            m_viewportWidth = m_viewportHeight * m_aspectRation;
	    }
        m_initialScale = size().height() / m_viewportHeight;
        if (m_initialScale < KDefaultMinScale || m_initialScale > KDefaultMaxScale)
            m_initialScale = KDefaultMinScale;
        m_minimumScale = m_initialScale;
        m_inferWidthHeight = false;
    }
    else if (propertyName == KViewPortInitialScaleTag) {
        m_initialScale = propertyValue.toDouble();
        if (m_inferWidthHeight) {
            m_viewportWidth = (int)size().width();
            m_viewportHeight = m_viewportWidth * m_aspectRation;
        }
    }
    else if (propertyName == KViewPortMinScaleTag) {
        m_minimumScale = propertyValue.toDouble();
        if (m_minimumScale < 0 
            || m_minimumScale > KMaxPageZoom
            || m_minimumScale > m_maximumScale)
            m_minimumScale = KDefaultMinScale;
    }
    else if (propertyName == KViewPortMaxScaleTag) {
        m_maximumScale = propertyValue.toDouble();
        if (m_maximumScale < 0 
            || m_maximumScale > KMaxPageZoom 
            || m_maximumScale < m_minimumScale)

            m_maximumScale = KDefaultMaxScale;
    }
    else if (propertyName == KViewPortUserScalableTag) {
        if (propertyValue =="no" || propertyValue =="0")
		{
			m_userScalable = false;
			view()->deactivateZoomActions();
		}
        else
            m_userScalable = true;
    }
}


bool WebContentWidget::isUserScalable()
{
    return m_userScalable;
}

qreal WebContentWidget::minimumScale()
{
    return m_minimumScale;
}

qreal WebContentWidget::maximumScale()
{
    return m_maximumScale;
}

ZoomMetaData WebContentWidget::pageZoomMetaData() {

    ZoomMetaData data;

    data.minScale = m_minimumScale;
    data.maxScale = m_maximumScale;
    data.userScalable = m_userScalable;

    return data;
}

void WebContentWidget::setPageZoomMetaData(ZoomMetaData data) {

    m_minimumScale = data.minScale ;
    m_maximumScale = data.maxScale ;
    m_userScalable = data.userScalable;
}

QWebPage* WebContentWidget::page() const
{
    if (!m_wrtPage) {
        WebContentWidget* that = const_cast<WebContentWidget*>(this);
        that->setPage(BrowserPageFactory::openBrowserPage());
    }
    return m_wrtPage;
}

QPointF WebContentWidget::mapToGlobal(const QPointF& p)
{
    QList<QGraphicsView*> gvList = scene()->views();
    QList<QGraphicsView*>::iterator it;
    for(it = gvList.begin(); it != gvList.end(); it++) 
        {
            if (static_cast<QGraphicsView*>(*it)->hasFocus()) 
                {
                    QWidget* viewport = static_cast<QGraphicsView*>(*it)->viewport();
                    return viewport->mapToGlobal(mapToScene(p).toPoint());
                }
        }

    return QPoint(0, 0);
}

QPointF WebContentWidget::mapFromGlobal(const QPointF& p)
{
    QList<QGraphicsView*> gvList = scene()->views();
    QList<QGraphicsView*>::iterator it;
    for(it = gvList.begin(); it != gvList.end(); it++) 
        {
            if (static_cast<QGraphicsView*>(*it)->hasFocus()) 
                {
                    QWidget* viewport = static_cast<QGraphicsView*>(*it)->viewport();
                    return mapFromScene(viewport->mapFromGlobal(p.toPoint()));
                }
        }

    return QPoint(0, 0);
}

void WebContentWidget::setTextSizeMultiplier(qreal factor)
{
    page()->mainFrame()->setTextSizeMultiplier(factor);
}
             
