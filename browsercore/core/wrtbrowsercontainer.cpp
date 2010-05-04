/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "browserpagefactory.h"
#include "webcontentview.h"
#include "wrtpage.h"
#include "wrtbrowsercontainer_p.h"
#include "wrtbrowsercontainer.h"
#include "webpagedata.h"
#include "qwebhistory.h"
#include "qwebframe.h"
#include "SchemeHandlerBr.h"
#include "webnetworkaccessmanager.h"
#ifdef QT_MOBILITY_BEARER_MANAGEMENT
#include "WebNetworkConnectionManager.h"
#endif // QT_MOBILITY_BEARER_MANAGEMENT
#include "webcookiejar.h"
#include "webnavigation.h"
#include "secureuicontroller.h"
#include "LoadController.h"
#include "WebDialogProvider.h"
#include "bedrockprovisioning.h"
#include <QPainter>
#include <QAuthenticator>
#include <QFile>
#include <QMessageBox>

namespace WRT
{

const int historyItemsCount = 20;

WrtBrowserContainerPrivate::WrtBrowserContainerPrivate(QObject* parent,
		WrtBrowserContainer* page/*never NULL*/) : m_schemeHandler(new SchemeHandler())
,   m_pageFactory(0)
,   m_widget(0)
{
    assert(page);
    m_page = page;

    m_secureController = new WRT::SecureUIController(parent);
    m_loadController = new  WRT::LoadController();
 
    // Turn off the scroll bars of main frame
    m_page->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    m_page->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    
}

WrtBrowserContainerPrivate::~WrtBrowserContainerPrivate()
{
	m_page->setView(0);
	if (m_page)
		m_page->disconnect();
	delete m_schemeHandler;
  delete m_secureController;
  delete m_loadController;
}

/*!
 * Static function which creates Wrt page with parent Widget
 * @param parent   : parent widget for the new page
 * @return WrtBrowserContainer : Page handle to the newly created page
 */
WrtBrowserContainer* WrtBrowserContainer::createPageWithWidgetParent(
		QObject* parent, WrtBrowserContainer* page)
{
    if (page)
    {
        page->setParent(parent);
        return page;
    }
    else
        return new WrtBrowserContainer(parent);
}

/*!
 * \class WrtBrowserContainer
 * \brief Description: This class is a handle to open page on which user can
 * load an url , view history items, load an history item into the page etc.
 * @see WrtPageManager
 */

/*!
 * WrtBrowserContainer Constructor
 * @param parent : Widget parent
 */
WrtBrowserContainer::WrtBrowserContainer(QObject* parent) :
	WrtPage(parent), d(new WrtBrowserContainerPrivate(this, this))
{
	// Download related enable "forwardUnsupportedContent" to redirect unsupported content to download manager
	setForwardUnsupportedContent(true);

#ifndef NO_NETWORK_ACCESS_MANAGER	
	setNetworkAccessManager(new WebNetworkAccessManager(this,this));
#endif
	
#ifdef QT_MOBILITY_BEARER_MANAGEMENT
	setNetworkConnectionManager(new WebNetworkConnectionManager(this));
#endif // QT_MOBILITY_BEARER_MANAGEMENT
	history()->setMaximumItemCount(historyItemsCount);

	connect(this,
			SIGNAL(saveFrameStateRequested(QWebFrame*, QWebHistoryItem*)),
			this, SLOT(savePageDataToHistoryItem(QWebFrame*, QWebHistoryItem*)));

    /* Connect secure related signals and slots */
    connect(mainFrame(), SIGNAL(urlChanged(QUrl)), d->m_secureController, SLOT(setTopLevelScheme(const QUrl &)));
    connect(this, SIGNAL(loadFinished(bool)), d->m_secureController, SLOT( endSecureCheck(bool) ) );
    connect(d->m_secureController, SIGNAL(pageSecureState(int)), this, SLOT(pageSecureState(int)));
    connect(networkAccessManager(), SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), d->m_secureController, SLOT(onSslErrors(QNetworkReply *, const QList<QSslError> &)));

    connect(this, SIGNAL(loadStarted()), d->m_loadController, SLOT(loadStarted()));
    connect(this, SIGNAL(loadProgress(int)), d->m_loadController, SLOT(loadProgress(int)));
    connect(this, SIGNAL(loadFinished(bool)), d->m_loadController, SLOT(loadFinished(bool)));
    connect(mainFrame(), SIGNAL(urlChanged(QUrl)), d->m_loadController, SLOT(urlChanged(QUrl)));
    
	// All pages in the browser share the same pageGroup.
	d->m_pageGroupName = "nokia_page_group";
	setPageGroupName(d->m_pageGroupName);
}

/*!
 * WrtBrowserContainer destructor
 */
WrtBrowserContainer::~WrtBrowserContainer()
{
	// gracefully but warn, disconnect anything connected to this page
	if (!disconnect())
		qWarning("WrtPage destructor blanket disconnect failed");

    //setNetworkAccessManager(NULL);
   delete d;
}

QGraphicsWidget* WrtBrowserContainer::webWidget() const
{
    return d->m_widget;
}

void WrtBrowserContainer::setWebWidget(QGraphicsWidget* view)
{
    if(d->m_widget) {
        disconnect(static_cast<WebContentWidget*>(webWidget()), SIGNAL(pageZoomMetaDataChange(QWebFrame*, ZoomMetaData)), this, SLOT(pageZoomMetaDataChange(QWebFrame*, ZoomMetaData)));
    }

    d->m_widget = view;

    if (view)
    {
        QGraphicsWebView* webView = static_cast<QGraphicsWebView*>(view);
        if(webView)
            webView->setPage(this);

        /* Currently connecting it here. Need to revisit if the dialog is enabled in the
         * context of multiple windows */
        connect( d->m_secureController, SIGNAL( showMessageBox(WRT::MessageBoxProxy*)), static_cast<WebContentWidget*>(webWidget())->view(), SLOT( showMessageBox(WRT::MessageBoxProxy*)));

        connect(static_cast<WebContentWidget*>(webWidget()), SIGNAL(pageZoomMetaDataChange(QWebFrame*, ZoomMetaData)), this, SLOT(pageZoomMetaDataChange(QWebFrame*, ZoomMetaData)));
    }
}
 
/*!
 * Handles pageSecureState state 
 */
void WrtBrowserContainer::pageSecureState(int state) {

    emit secureStateChange(state);

}

/*!
 * Returns schemeHandler
 */
SchemeHandler* WrtBrowserContainer::schemeHandler() const
{
   return d->m_schemeHandler;
}

/*!
 * Gets the history count for this page, i.e gets the number of urls loaded
 * onto this page
 * @return  History count of this page
 */
int WrtBrowserContainer::getHistoryCount() const
{
	return history()->count();
}

/*!
 * Triggers public Action
 * @param WebAction : web action to be triggred
 * @param checked  : true/false
 * @see   WebAction
 */
/*void WrtBrowserContainer::triggerAction(QWebPage::WebAction action, bool checked)
 {
 triggerAction(action, checked);
 }
 */

/*!
 * clears cookies
 */
void WrtBrowserContainer::clearCookies()
{
	return ((CookieJar*) networkAccessManager()->cookieJar())->clear();
}

bool WrtBrowserContainer::clearNetworkCache()
{
   bool ok = false;
    QAbstractNetworkCache* diskCache = networkAccessManager()->cache();
    diskCache->clear();
    if (!diskCache->cacheSize()) { 
       // Unless there was failures clearing the cache cacheSize() should return 0 after a call to clear 
       ok = true;
   }
   return ok;
}

/*!
 * This function page thumbnail for this page as specified by X & Y co-ordinate scale factors
 * @param  scaleX :  X Co-ordinate scale factor for the page thumbnail
 * @param  scaleY :  y Co-ordinate scale factor for the page thumbnail
 */
QImage WrtBrowserContainer::pageThumbnail(qreal scaleX, qreal scaleY)
{
    qDebug() << "WrtBrowserContainer::pageThumbnail:" << webWidget()->size();
    QSize size = webWidget()->size().toSize();
    QImage image(size, QImage::Format_RGB32);

    QPainter painter(&image);
    QRect r(0, 0, size.width(), size.height());
    QRegion clip(r);
    painter.save();
    painter.setBrush(Qt::white);
    painter.drawRect(r);
    painter.restore();
    mainFrame()->render(&painter, clip);
    return image;
}

/*!
 * sets the page zoom factor as specified by zoom
 * @param zoom : page zoom factor to be set
 */
void WrtBrowserContainer::setPageZoomFactor(qreal zoom)
{
   // FiX ME ... canvas zoom doesn't seem to support text-only zooming.
   //static_cast<WebCanvasWidget*>(webWidget())->canvas()->setPageZoomFactor(zoom);

	bool isZoomTextOnly = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value(
			"ZoomTextOnly").toBool();

	if (!isZoomTextOnly)
	{
		// mainFrame()->setZoomFactor(zoom);
		static_cast<WebContentWidget*> (webWidget())->setZoomFactor(zoom);
	}
	else
	{
		//mainFrame()->setTextSizeMultiplier(zoom);
		static_cast<WebContentWidget*> (webWidget())->setTextSizeMultiplier(
				zoom);
    }
}

/*!
 * setpageDirtyZoomFactor
 * @param zoom: page zoom factor to be set
 */
void WrtBrowserContainer::setPageDirtyZoomFactor(qreal zoom)
{
   // FIXME: dirty page zoom is not supported yet in single-buffer mode.
   static_cast<WebContentWidget*> (webWidget())->setDirtyZoomFactor(zoom);
}

/*!
 * setpageCenterZoomFactor
 * @param zoom: center page zoom factor to be set
 */
// TODO: hold on this change until zoom/scrolling improvement 
/*
 void WrtBrowserContainer::setPageCenterZoomFactor(qreal zoom)
 {
 static_cast<WebCanvasWidget*>(webWidget())->canvas()->setPageCenterZoomFactor(zoom);
 }
 */
/*!
 * returns the current page zoom factor of this page
 * @return current zoom factor
 */
qreal WrtBrowserContainer::pageZoomFactor() const
{
   return static_cast<WebContentWidget*> (webWidget())->zoomFactor();
}

/*!
 * sets the Canvas ScaleFactor  for this page
 * @param  scaleX :  X Co-ordinate scale factor
 * @param  scaleY :  Y Co-ordinate scale factor
 */
void WrtBrowserContainer::setCanvasScaleFactor(qreal scaleX, qreal scaleY)
{
   QTransform transform;
   transform.scale(scaleX, scaleY);
   //     static_cast<ContentView*>(webWidget())->setTransform(transform);
}

/*!
 * allows offlineStorage for url
 * @param url : url for which offlinestorage has to be enabled
 */
bool WrtBrowserContainer::allowOfflineStorage(const QUrl& url)
{
   if (BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("SecurityLevel").toInt()
         != SecurityHigh)
      return true;

	QStringList yesno;
	yesno << "Yes" << "No";
	return (WebDialogProvider::getButtonResponse(
			0, /* webWidget(), Check, that widget is required */
			QString(
					"Offline Database: This host asking for storing data in offline database, allow?"),
			yesno));
}


/*!
 Pulic Slots:
 void savePageDataToHistoryItem(QWebFrame*, QWebHistoryItem* item);
 void slotAuthenticationRequired(QNetworkReply *, QAuthenticator *);
 void slotProxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *);
 */

/*!
 * Public slot, saves the page data (like thumbail, current position, zoom factor etc ) to WebHistoryItem
 * @frame  main frame for which the data has to be stored
 * @item   handle to QWebHistoryItem into which the page data has to be saved.
 */

void WrtBrowserContainer::savePageDataToHistoryItem(QWebFrame* frame,
      QWebHistoryItem* item)
{
    if (frame != mainFrame() || !item->isValid() || !webWidget())
    {
        return;
    }
    if (restoreSession()) return;
    	
    WebPageData data(this);
    //   WebPageData data = item->userData().value<WebPageData>();
    data.m_zoomFactor = pageZoomFactor();
    data.m_thumbnail = pageThumbnail(1.0, 1.0);//data.m_zoomFactor, data.m_zoomFactor);

    QPoint pos(0, 0);
    //    pos = static_cast<WebCanvasWidget*>(webWidget())->canvas()->canvasToDocument(pos);
    pos = mainFrame()->scrollPosition();
    data.m_contentsPos = pos;
    QVariant variant;
    variant.setValue(data);
    item->setUserData(variant);
    //ii++;
}

/*!
 * Public slot  AuthenticationRequired
 * Launches dialog for user name and password if Authentication is required for page load.
 * @param reply     : network reply
 * @param  athenticator : athenticator
 */
void WrtBrowserContainer::slotAuthenticationRequired(QNetworkReply* reply,
      QAuthenticator* authenticator)
{
	QString username, password;
	if (WebDialogProvider::getUsernamePassword(0/*webWidget()*/, username, password))
	{
		authenticator->setUser(username);
		authenticator->setPassword(password);
    }
}

/*!
 * public slot for setting proxy when Authentication is Required
 * @param networkProxy : network Proxy
 * @param authenticator : authenticator
 */
void WrtBrowserContainer::slotProxyAuthenticationRequired(
      const QNetworkProxy& networkProxy, QAuthenticator* authenticator)
{
	QString username, password;
	if (WebDialogProvider::getUsernamePassword(0/*webWidget()*/, username, password))
	{
		authenticator->setUser(username);
		authenticator->setPassword(password);
    }
}

QWebPage* WrtBrowserContainer::createWindow(
    QWebPage::WebWindowType webWindowType)
{

    /* When WrtPage is created, QWebSettings::JavascriptCanOpenWindows is initialized
     * to popup setting value. Need not do any check here 
     */
    if (d->m_pageFactory)
    {
        // no need to signal in this case
        return d->m_pageFactory->openPage();
    }

    WrtBrowserContainer* wrtPage = new WrtBrowserContainer();
    emit createNewWindow(wrtPage);
    return wrtPage;
}

void WrtBrowserContainer::setPageFactory(BrowserPageFactory* f)
{
    d->m_pageFactory = f;
}

void WrtBrowserContainer::setElementType(
      wrtBrowserDefs::BrowserElementType& aElType)
{
    m_elementType = aElType;
}
wrtBrowserDefs::BrowserElementType WrtBrowserContainer::getElementType()
{
   return m_elementType;
}

QString WrtBrowserContainer::pageTitle(){

    QString title = mainFrame()->title();
    
    // If mainFrame title is empty we may be restoring session
    // Check history title
    if (title.isEmpty()){ 
    	title = history()->currentItem().title();
    }
    
    /* If there is no title, provide the partial url */
    if (title.isEmpty()) {
        QUrl url  = mainFrame()->url(); 
        // If mainframe url is empty, we may be restoring session
        // check history url
        if (url.isEmpty()) {
        	url = history()->currentItem().url();
        }
        title = url.toString();
        QString scheme=url.scheme();
        title.remove(0, scheme.length() + 3); // remove "scheme://"
        if (title.startsWith("www.", Qt::CaseInsensitive)) {
            title.remove(0, 4);
        }

    }
    return title;
}

int WrtBrowserContainer::secureState() {

    return d->m_secureController->secureState();
}
bool WrtBrowserContainer::restoreSession() {
	if (d->m_pageFactory)
    return d->m_pageFactory->m_bRestoreSession;
  else
    return true;
}

bool WrtBrowserContainer::emptyWindow() {

    bool result= false;
    if (mainFrame()->title()  == "" && mainFrame()->url().toString() == "" )
        result = true;

    return result;
}

void WrtBrowserContainer::pageZoomMetaDataChange(QWebFrame* frame,  ZoomMetaData  zoomData ){

    //qDebug() << __func__ << "Frame " << frame << "My Frame " << mainFrame();
    if (frame == mainFrame()) {
        setPageZoomMetaData(zoomData);
    } 
}

ZoomMetaData WrtBrowserContainer::pageZoomMetaData() {
    return d->m_zoomData ;

}
void WrtBrowserContainer::setPageZoomMetaData( ZoomMetaData  zoomData ){

    d->m_zoomData = zoomData;
}


}


