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


#include <assert.h>

#include "actionjsobject.h"
#include "qwebframe.h"
#include "qwebhistory.h"
#include "qgraphicswebview.h"
#include "webpagedata.h"
#include "wrtbrowsercontainer.h"
#include "wrtbrowsercontainer_p.h"
#include "webpagecontroller_p.h"
#include "webpagecontroller.h"
#include "webcontentview.h"
#include "UiUtil.h"
//#include "wrtsettings.h"
#include "bedrockprovisioning.h"
#include "secureuicontroller.h"
#include "LoadController.h"
#include <QFile>
#include <QDir>
#include <QtGui>
#include <qdesktopservices.h>
#include <QCoreApplication>

#define BEDROCK_APPLICATION_NAME "Bedrock"
#define BEDROCK_VERSION_DEFAULT "BetaRelease"
#define MAX_NUM_WINDOWS_TO_RESTORE 5
#define MAX_NUM_WINDOWS 5

static const char KHISTORYEXTENSION[]       = ".history";

void BWF_EXPORT qt_websettings_setLocalStorageDatabasePath(QWebSettings* settings, const QString& path);

// --------------------------------------------

//QEvent::Type WebPageControllerUpdateViewPortEvent::staticType() {
//    static int s_type = QEvent::registerEventType();
//    return (QEvent::Type)s_type;
//}

// --------------------------------------------

WebPageControllerPrivate::WebPageControllerPrivate(WebPageController* qq) :
    q(qq),
    m_widgetParent(0),
    m_currentPage(-1),
    m_secContext(0),
    m_actionsParent(0),
    donotsaveFlag(false)
{
    m_widgetParent = static_cast<QObject*>(qq); //new QWidget();

    // create the manager's actions
    m_actionReload = new QAction("Reload",m_widgetParent);
    m_actionReload->setObjectName("reload");
    m_actionStop = new QAction("Stop",m_widgetParent);
    m_actionStop->setObjectName("stop");
    m_actionBack = new QAction("Back",m_widgetParent);
    m_actionBack->setObjectName("back");
    m_actionForward = new QAction("Forward",m_widgetParent);
    m_actionForward->setObjectName("forward");
    m_actionWebInspector = new QAction("Web Inspector",m_widgetParent);
    m_actionWebInspector->setObjectName("webInspector");

    m_actionReload->setEnabled(false);
    m_actionStop->setEnabled(false);
    m_actionBack->setEnabled(false);
    m_actionForward->setEnabled(false);
    m_actionWebInspector->setVisible(false);
    
}

WebPageControllerPrivate::~WebPageControllerPrivate()
{
    // save history in the persistent storage, delete history files first
    bool enabled = (bool) BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsInt("SaveSession");
    if (enabled)
    {
      q->deleteHistory();
      if(donotsaveFlag == false)
      {	
    	  q->saveNumberOfWindows();    
    	  q->saveHistory();
  	  }
  	  donotsaveFlag = true;
    }
/*    #if defined(Q_OS_SYMBIAN)
    // clean up all pages
    while ( !m_allPages.isEmpty() )
        q->closePage(m_allPages.at(0));

    Q_ASSERT ( m_allPages.isEmpty() );
    #endif*/
//    delete(m_widgetParent);
}



/*!
 * \class WebPageController
 *
 * \brief Manages WrtPages
 *
 * This class is responsible for managing multiple WrtPages.
 * All page management operations go through this class, such as opening a page,
 * managing multiple pages, choosing what the current page among several is,
 * closing a page, etc.
 *
 * Multiple views may well have access to a WebPageManager
 *
 * For most cases, we expect there to be a single instance of WebPageController for a given
 * application, but it is up to the application to specify the instance used.
 *
 * This class is supposed to abstract page events, control into a simple interface
 * so that higher-level functions need not know or care about what the active page is, etc.
 */

WebPageController::WebPageController ( QObject* parent ) :
    QObject ( parent ),
    d(new WebPageControllerPrivate(this))
{
    // Register a new MetaType WebPageData. It is needed to serialize history (starage)
    qRegisterMetaTypeStreamOperators<WebPageData> ("WebPageData");

#ifndef QT_NO_DESKTOPSERVICES
    d->m_historyDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#else
    d->m_historyDir = QDir::homePath();
#endif
#ifdef Q_OS_SYMBIAN
    if (d->m_historyDir.startsWith("Z"))
        d->m_historyDir.replace(0,1,"C");
#endif

    // auto-connect actions
    connect( d->m_actionReload, SIGNAL( triggered() ), this, SLOT( currentReload() ) );
    connect( d->m_actionStop, SIGNAL( triggered() ), this, SLOT( currentStop() ) );
    connect( d->m_actionBack, SIGNAL( triggered() ), this, SLOT( currentBack() ) );
    connect( d->m_actionForward, SIGNAL( triggered() ), this, SLOT( currentForward() ) );
    connect( d->m_actionWebInspector, SIGNAL( triggered() ), this, SLOT( webInspector() ) );

    // Catch changes to settings
    connect(BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning(), SIGNAL(settingChanged(const QString &)), this, SLOT(settingChanged(const QString &)));
    
    // Initialize UA specific params.
    initUASettingsAndData();
    // initialize from settings
    settingChanged("DeveloperExtras");
    updateJSActions();
    
    m_bRestoreSession = false;
    bool enabled = (bool) BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsInt("SaveSession");
    if (enabled) m_bRestoreSession = true;

}
    
WebPageController::~WebPageController()
{
    delete d;
}



/*!
 * Open an empty WRT::WrtBrowserContainer
 * @param parent  Widget Parent
 * @return        WRT::WrtBrowserContainer Handle
 * @see WRT::WrtBrowserContainer                      
 */
WRT::WrtBrowserContainer* WebPageController::openPage(QObject* parent, WRT::WrtBrowserContainer* pg)
{
    WRT::WrtBrowserContainer* page =  currentPage();
    if (  d->m_allPages.count() < MAX_NUM_WINDOWS ) {


        // create without parent
        page = WRT::WrtBrowserContainer::createPageWithWidgetParent(parent, pg);

        // emit signal for creating network connection.

        Q_ASSERT( page );
        page->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, (bool) BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsInt("DeveloperExtras"));

        /* Add the new page after the current page */
        WRT::WrtBrowserContainer * theCurrentPage = currentPage();
        int index = d->m_allPages.indexOf(theCurrentPage);
        d->m_allPages.insert (index+1, page );


        // emit signal
        emit pageCreated( page );

       
        // AUTO-activate this page ONLY if there are no other pages
        if(d->m_allPages.size() == 1) {
            setCurrentPage(page);
        }
        int count = historyWindowCount();
        if(index+1 < count)
        	startupRestoreHistory(NULL, index+1, page);
    	  
    }
    return page;
}


QWebPage* WebPageController::openPage()
{
	WRT::WrtBrowserContainer* page = NULL;
	
	page = openPage(this, 0);
	page->setPageFactory(this);
	
	return page;
}

QWebPage* WebPageController::openPageFromHistory(int index)
{
    QDir dir(d->m_historyDir);
    QFileInfoList fileList(dir.entryInfoList(QDir::Files));
    QString indexStr;
    indexStr.setNum(index);
    
    
    QString historyFile = d->m_historyDir + QLatin1String("/history") + indexStr + QLatin1String(".history");
    QFile file(historyFile);    
    if(file.open(QIODevice::ReadOnly)) 
    {
    	  if(file.size() <= 12) // empty file
    	  {	
        	file.remove();
        	file.close();
        	return NULL;
        }
        else
        	return openPage();
    }    
    else // can not open file
    	return NULL;   
}

/*!
 * Given a WRT::WrtBrowserContainer instance, close it
 * @param page   WRT::WrtBrowserContainer instance to be closed 
 * @see  WRT::WrtBrowserContainer
 */
void WebPageController::closePage ( WRT::WrtBrowserContainer *page )
{
    WRT::WrtBrowserContainer * theCurrentPage = currentPage();
    bool updateCurrentPageIndex = false;

    // get index of page we want to remove
    int closeIndex = d->m_allPages.indexOf(page);
    if(closeIndex < 0)
        return;
    
    // was this page the "current page?"
    // if so, we need to select the page to become the new one
    // select the previous page unless at the beginning, then select next
    if(page == theCurrentPage) {
        int newCurrIndex = closeIndex - 1;
        if(closeIndex == 0 ) {
            newCurrIndex = closeIndex + 1;
        }
        
        // change the current page
        if(newCurrIndex >= 0) {
            theCurrentPage = d->m_allPages.at(newCurrIndex);
            setCurrentPage(theCurrentPage);
            updateCurrentPageIndex = true;
        }
        else {
            d->m_currentPage = -1;
            emit pageChanged(page, NULL);
        }
    }
    else {
        /* Adjust m_currentPage if the index of the page deleted is less than 
         * current page 
         */
       if (closeIndex < d->m_currentPage ) 
           updateCurrentPageIndex = true;
    }

    // actually delete the page from the list
    d->m_allPages.removeAt(closeIndex);

    // update the current page index if necessary
    // (this will just update the index now that we've removed the page from the list)
    if(updateCurrentPageIndex) {
        int index = d->m_allPages.indexOf(theCurrentPage);
        if(index >= 0)
            d->m_currentPage = index;
    }

    // and emit sig that it was done
    emit pageDeleted(page);

    delete page;
}

/*!
 *  Activate the given page as the current page
 *  @param page   page handle for setting current page
 *  @see  WRT::WrtBrowserContainer
 */
void WebPageController::setCurrentPage(WRT::WrtBrowserContainer* page)
{
    // verify page
    Q_ASSERT(page);
    int index = d->m_allPages.indexOf(page);
    if(index < 0)
        return;
    
    // fetch current page (if any)
    WRT::WrtBrowserContainer * oldPage = currentPage();

    // update the index of the current page
    // (even if the page hasn't changed, it's index might have)
    d->m_currentPage = index;

    // do not proceed any further if setting to the same page
    if(page == oldPage)
       return;

    // disconnect any existing aggregate signalling for pgMgr
    if(oldPage) {
        disconnect(oldPage, 0, this, 0);
        disconnect(oldPage->mainFrame(), 0, this, 0);
        disconnect(oldPage->networkAccessManager(), 0, this, 0);
        disconnect(oldPage->loadController(), 0, this, 0);
        connect(oldPage, SIGNAL(createNewWindow(WrtBrowserContainer*)), this, SLOT(createWindow(WrtBrowserContainer*)));
    }

    // aggregate "current page" signalling from this page to PageMgr clients
    connect(page, SIGNAL(secureStateChange(int)), this, SLOT(secureStateChange(int)));
    connect( page->mainFrame(), SIGNAL ( titleChanged ( const QString& ) ), SIGNAL ( titleChanged ( const QString& ) ) );
    connect( page, SIGNAL( loadStarted() ), SIGNAL( loadStarted() ) );
    connect( page, SIGNAL( loadProgress(int) ), SIGNAL( loadProgress(int) ) );
    connect( page, SIGNAL( loadFinished(bool) ), SIGNAL( loadFinished(bool) ) );
    connect( page->mainFrame(), SIGNAL( iconChanged() ), SIGNAL( pageIconChanged() ) );
    connect( page->loadController(), SIGNAL( pageLoadStarted() ), SIGNAL( pageLoadStarted() ) );
    connect( page->loadController(), SIGNAL( pageLoadProgress(int) ), SIGNAL( pageLoadProgress(int) ) );
    connect( page->loadController(), SIGNAL( pageLoadFinished(bool) ), SIGNAL( pageLoadFinished(bool) ) );
    connect( page->loadController(), SIGNAL( pageUrlChanged(QString) ), SIGNAL( pageUrlChanged(QString) ) );
    connect( page, SIGNAL(createNewWindow(WrtBrowserContainer*)), this, SLOT(createWindow(WrtBrowserContainer*)));

    // Connect the unsuportedContent signal to start a new download
    connect( page, SIGNAL(unsupportedContent(QNetworkReply *)), this, SLOT(unsupportedContentArrived(QNetworkReply *)));

    connect( page, SIGNAL( loadStarted() ), SLOT( updateStatePageLoading() ) );
    connect( page, SIGNAL( loadFinished(bool) ), SLOT( updateStatePageLoadComplete(bool) ) );

    connect( page->mainFrame(), SIGNAL( iconChanged() ), this, SIGNAL( currentPageIconChanged() ) );
    connect( page->mainFrame(), SIGNAL( urlChanged( const QUrl &)), this, SIGNAL( currentPageUrlChanged( const QUrl &) ));
    connect( page->mainFrame(), SIGNAL( urlChanged( const QUrl &)), this, SLOT( urlChanged( const QUrl &) ));

    connect( page, SIGNAL( networkRequestStarted(QWebFrame*, QNetworkRequest*)), this, SIGNAL( networkRequestStarted(QWebFrame*, QNetworkRequest*)));
    connect( page->networkAccessManager(), SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this, SIGNAL( sslErrors(QNetworkReply *, const QList<QSslError> &)));
    connect( page->networkAccessManager(), SIGNAL(showMessageBox(WRT::MessageBoxProxy*)), this, SIGNAL(showMessageBox(WRT::MessageBoxProxy*)));
    connect(currentPage(), SIGNAL(pageScrollPositionZero()), this, SIGNAL(pageScrollPositionZero()) );
    connect(page, SIGNAL(scrollRequested(int, int, const QRect & )), this, SIGNAL(pageScrollRequested(int, int, const QRect & )) );

    Q_ASSERT ( d->m_currentPage >= 0 );

    updateActions(false);
    
    emit pageChanged( oldPage, page );
}

/*!
 * Retrieve a pointer to the current WRT::WrtBrowserContainer
 * @return    Returns current page handle
 * @see WRT::WrtBrowserContainer
 */
WRT::WrtBrowserContainer* WebPageController::currentPage() const
{
    if ( d->m_currentPage >= 0 && d->m_currentPage < d->m_allPages.count() )
        return d->m_allPages.at ( d->m_currentPage );
    else
        return NULL;
}

/*!
  Close the current page
*/
void WebPageController::closeCurrentPage()
{
    closePage(currentPage());
}

/*! 
 * Retrieve a list of all of the pages managed by WebPageController
 * @return   List of all the pages opened by WebPageController
 * @see WRT::WrtBrowserContainer
 */
QList<WRT::WrtBrowserContainer*>* WebPageController::allPages()
{
    return &d->m_allPages;
}

/*! 
 * Retrieve the number of pages managed by WebPageController
 * @return  count of all the pages currently opend by WrtPagemanager
 */
int WebPageController::pageCount()
{
    return d->m_allPages.count();
}

/*!
 * Find a given page by looking for its window name
 * @param  name : main frame name of the page to be searched
 * @return : Handle to WRT::WrtBrowserContainer with frame name 
 * @see refer WRT::WrtBrowserContainer
 */
WRT::WrtBrowserContainer * WebPageController::findPageByMainFrameName(const QString & name)
{
    foreach(WRT::WrtBrowserContainer* page, d->m_allPages) {
        if(name == page->mainFrame()->frameName())
            return page;
    }
    return NULL;
}

/*!
  Retrieve the title of the current page's main frame or
  an empty string if there is no current page
*/
QString WebPageController::title()
{
    WRT::WrtBrowserContainer* page = currentPage();
    if ( page )
        return page->pageTitle();
    else
        return QString();
}

/*!
  Retrieve the icon of the current page's main frame or
  an NULL icon if there is no current page
*/
QIcon WebPageController::pageIcon()
{
    WRT::WrtBrowserContainer* page = currentPage();
    if ( page )
        return page->mainFrame()->icon();
    else
        return QIcon();
}
/*!
 * Return the widgetParent of this pageManager
 * @return : Parent Widget of PageManager
 */
/*QWidget * WebPageController::widgetParent()
{
    return d->m_widgetParent;
}*/

/*! 
  Return the list of public QActions most relevant to the managers's current context
  (most approptiate for contextual menus, etc.)
 * @return : returns public actions list (Reload, Stop, Forward, Back, WebInspector)
 */
QList<QAction*> WebPageController::getContext()
{
    // return all actions for now
    QList<QAction*> contextList;
    contextList << 
        d->m_actionBack <<
        d->m_actionForward <<
        d->m_actionReload <<
        d->m_actionStop <<
        d->m_actionForward <<
        d->m_actionWebInspector;
    return contextList;
}

/*!
 * Action for reloading the current page
 * @return : Returns public Reload action handle
*/
QAction * WebPageController::getActionReload()
{
    return d->m_actionReload;
}

/*!
 * Action for stopping the load of the current page
 * @return : returns Public Stop action handle
*/
QAction * WebPageController::getActionStop()
{
    return d->m_actionStop;
}

/*!
 * Action for going to the previous site from history for the current page
 * @return : Returns Public Back action (Ownership with WebPageController)
*/
QAction * WebPageController::getActionBack()
{
    return d->m_actionBack;
}

/*!
 * Action for going to the next site from history for the current page
 * @return : Returns handle to Forward action(Ownership with WebPageController)
*/
QAction * WebPageController::getActionForward()
{
    return d->m_actionForward;
}

/*!
 * Action for invoking the WebInspector
 * @return : Returns handle to WebInspector public handle (Ownership with WebPageController)
*/
QAction * WebPageController::getActionWebInspector()
{
    return d->m_actionWebInspector;
}

/*!
*  Reload for the current page
*/
void WebPageController::currentReload()
{
    WRT::WrtBrowserContainer * activePage = currentPage();
    if(activePage)
        activePage->triggerAction(QWebPage::Reload);
}

/*!
  Stop loading for the current page
*/
void WebPageController::currentStop()
{
    WRT::WrtBrowserContainer * activePage = currentPage();
    if(activePage){
        currentPage()->loadController()->setCanceled(true);
        activePage->triggerAction(QWebPage::Stop);
    }
}

/*!
  Go back for the current page
*/
void WebPageController::currentBack()
{
    WRT::WrtBrowserContainer * activePage = currentPage();
    if(activePage)
        activePage->triggerAction(QWebPage::Back);
}

/*!
  Go forward for the current page
*/
void WebPageController::currentForward()
{
    WRT::WrtBrowserContainer * activePage = currentPage();
    if(activePage)
        activePage->triggerAction(QWebPage::Forward);
}

/*!
  Load the given /a url into the current page
*/
void WebPageController::currentLoad(const QUrl & url)
{
    WRT::WrtBrowserContainer * activePage = currentPage();
    if(activePage) {
        activePage->mainFrame()->load(url);
    }
}

void WebPageController::currentLoad(const QString &url)
{
    currentLoad(QUrl(url));
}

/*!
  Load an item into the current page from a given /a historyIndex
*/
void WebPageController::currentSetFromHistory(int historyIndex)
{

    WRT::WrtBrowserContainer* activePage = currentPage();
    if(activePage) {
        QList<QWebHistoryItem> items = activePage->history()->items();
        QWebHistoryItem item = items.at(historyIndex);
        if (item.isValid()) {
            // userData will be set by canvas upon Commit
            currentPage()->history()->goToItem(item);
        }
    }
}

void WebPageController::gotoCurrentItem()
{
    WRT::WrtBrowserContainer* activePage = currentPage();
    if(activePage) {
        QList<QWebHistoryItem> items = activePage->history()->items();
        QWebHistoryItem item = activePage->history()->currentItem();
        if (item.isValid()) {
            // userData will be set by canvas upon Commit
            currentPage()->history()->goToItem(item);
        }
    }
}

void WebPageController::pageGotoCurrentItem(int index)
{

    WRT::WrtBrowserContainer* page = d->m_allPages.at(index);
    if(page) {
        QList<QWebHistoryItem> items = page->history()->items();
        QWebHistoryItem item = page->history()->currentItem();
        if (item.isValid()) {
           page->history()->goToItem(item);
        }
    }
}

void WebPageController::pageReload(int index)
{

    WRT::WrtBrowserContainer* page = d->m_allPages.at(index);
    if(page) {
        QList<QWebHistoryItem> items = page->history()->items();
        QWebHistoryItem item = page->history()->currentItem();
        if (item.isValid()) {
          page->mainFrame()->load(item.url());
        }
    }
}


/*! 
  Initialize User Agent specific data / params. 
 */
void WebPageController::initUASettingsAndData()
{
    QCoreApplication::setApplicationName(BEDROCK_APPLICATION_NAME);
    //Set BrowserVersion: Format - VersionMajor.VersionMinor.SVNRevision
    QString browserAppVersion = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("BedrockVersion");
    if (browserAppVersion.isEmpty())
    {
      browserAppVersion = QString(BEDROCK_VERSION_DEFAULT);
    }
    QCoreApplication::setApplicationVersion(browserAppVersion);
}

/*!
  Activate Web Inspector
*/
void WebPageController::webInspector()
{
    if (QWebSettings::globalSettings()->testAttribute(QWebSettings::DeveloperExtrasEnabled)) {
        WRT::WrtBrowserContainer * activePage = currentPage();
        if(activePage)
            activePage->triggerAction(QWebPage::InspectElement);
    }
}

/*!
 * Private slot
 */
void WebPageController::settingChanged(const QString &key)
{
    if (d->m_actionWebInspector && key.endsWith("DeveloperExtras"))
        d->m_actionWebInspector->setVisible(BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsInt("DeveloperExtras"));
}

/*!
 * Private slot
 */
void WebPageController::updateStatePageLoading()
{
    updateActions(true);
    
    // This is needed for loading a restored window (otherwise url bar is empty)
    currentPage()->loadController()->setUrlText(currentPage()->history()->currentItem().url().toString());
}

/*!
 * Private slot
 */
void WebPageController::updateStatePageLoadComplete(bool)
{
    updateActions(false);
}

/*!
 * Private slot
 */
void WebPageController::updateActions(bool pageIsLoading)
{
    d->m_actionReload->setEnabled(!pageIsLoading);
    d->m_actionStop->setEnabled(pageIsLoading);

    WRT::WrtBrowserContainer* activePage = currentPage();    

    if(activePage) {
        int histLength = activePage->history()->count();
        int currItemIndex = activePage->history()->currentItemIndex();
        d->m_actionBack->setEnabled((histLength > 1) && (currItemIndex > 0));
        d->m_actionForward->setEnabled((histLength > 1) && (currItemIndex < (histLength-1)));
        d->m_actionWebInspector->setVisible(QWebSettings::globalSettings()->testAttribute(QWebSettings::DeveloperExtrasEnabled));
        	
    }
}

/*!
 * Private slot
 */
void WebPageController::unsupportedContentArrived(QNetworkReply *reply)
{
    emit unsupportedContent(reply);
}

/*!
 * Private slot
 */
void WebPageController::createWindow(WrtBrowserContainer* page)
{
   openPage(this,page) ;
}

/*!
 * Private slot
 * Emits signals based on secure state 
 */
void WebPageController::secureStateChange(int state)
{
    //qDebug() << __func__ << state;
    switch(state) {
        case SecureUIController::secureLoadFinished:
        case SecureUIController::untrustedLoadFinished:
        case SecureUIController::mixedLoadFinished:
        case SecureUIController::untrustedMixedLoadFinished:
            emit showSecureIcon();
            break;
        case SecureUIController::unsecureLoadFinished:
            emit hideSecureIcon();
            break;
        default:
            break; 
    }
}


WebPageController* WebPageController::getSingleton()
{
    static WebPageController* singleton = 0;
    if(!singleton) 
    {
    	singleton = new WebPageController;
        singleton->setObjectName("pageController"); 
    } // if(! singleton)

    assert(singleton);
    return singleton;
}

QWebPage* BrowserPageFactory::openBrowserPage()
{
    return WebPageController::getSingleton()->openPage();
}

QString WebPageController::guessUrlFromString(const QString &s){
    QUrl u = WRT::UiUtil::guessUrlFromString(s);
    return u.toString();
}


void WebPageController::loadLocalFile()
	{
    QString chromeBaseDir = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("ChromeBaseDirectory2");
    QString startPage = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("StartPage");
    QString startPagePath = chromeBaseDir + startPage;
    currentLoad(startPagePath);
	}

void WebPageController::loadFromHistory() 
{

   int count = historyWindowCount();
        
   if(!count)
   {
   	    m_bRestoreSession = FALSE;
    		loadLocalFile(); // load start page for first startup of the browser
   }
    else
    {	
    	 int count2 = restoreNumberOfWindows();
    	 
    	 // "sanity check" count shall be always equal to count2. If not, something wrong. Open only one window - start page in this case.
    	 	
    	 if((count > 0) && (count == count2)) // load all browser windows from the last session
    	 {
    	 	
    	 	for(int i = 0; i < count2-1; i++) //all windows except current
    	 	{
    	 		openPageFromHistory(i);
    	 	}		
    	 m_bRestoreSession = false;
    	 // current window
    	 gotoCurrentItem();
    	 	
    	 }
    	 else // browser did not close correctly; exception handler: delete all files and load start page as in first startup
    	 {
    	 	deleteHistory();
    	 	deleteDataFiles();
    	 	loadLocalFile();
    	 }			    	 
    }	 
    m_bRestoreSession = false;
    return ;

}

WRT::WrtBrowserContainer* WebPageController::restoreHistory(QWidget* parent, int index)
{
    Q_UNUSED(parent)
    // restore the history state from the disk for the current page
    QString indexStr;
    indexStr.setNum(index);
    QString historyFile = d->m_historyDir + QLatin1String("/history") + indexStr + QLatin1String(".history");
    WRT::WrtBrowserContainer* page = openPage(this,0);

    qDebug() << "historyFile>>:" << historyFile;
    QFile file(historyFile);
    if(file.open(QIODevice::ReadOnly)) {
        QDataStream restore(&file);
        restore >> *(page->history());
        file.close();
    }
    return page;
}

WRT::WrtBrowserContainer* WebPageController::startupRestoreHistory(QWidget* parent, int index, WRT::WrtBrowserContainer* page)
{
    Q_UNUSED(parent)
    // restore the history state from the disk for the current page
    QString indexStr;
    indexStr.setNum(index);
    QString historyFile = d->m_historyDir + QLatin1String("/history") + indexStr + QLatin1String(".history");
   
    qDebug() << "historyFile>>:" << historyFile;
    QFile file(historyFile);
    if(file.open(QIODevice::ReadOnly)) {
        QDataStream restore(&file);
        restore >> *(page->history());
        file.close();
    }
    
    return page;
}

void WebPageController::setLastUrl(QString url)
{
	QString winFile = d->m_historyDir + QLatin1String("/lasturl") + QLatin1String(".dat");
    
  QFile file(winFile);
  if(file.open(QIODevice::WriteOnly)) {
        QTextStream textStream(&file);
        textStream << url;
        textStream.flush();
        file.close();
   }
	
}

QString WebPageController::getLastUrl()
{
	  QString winFile = d->m_historyDir + QLatin1String("/lasturl") + QLatin1String(".dat");
    QFile file(winFile);
    QString winStr = "";
    if(file.open(QIODevice::ReadOnly)) {
        QTextStream textStream(&file);
        textStream >> winStr;
        file.close();
     }
    
	  return winStr;
}

void WebPageController::saveHistory()
{
    QString indexStr;
    unsigned int pageCount =  d->m_allPages.count();
    int index = 0;
    
    WRT::WrtBrowserContainer* theCurrentPage = currentPage();
    int currentIndex = d->m_allPages.indexOf(theCurrentPage);
    
    //if(pageCount == 1)
    //{	
    	QWebHistoryItem	item = theCurrentPage->history()->currentItem();
	  	
	  	setLastUrl(item.url().toString());
	  //}
    
    for (int tIndex = 0; tIndex < pageCount; tIndex++)
    {
    	  if(tIndex == pageCount-1)
    	  	index = currentIndex;
    	  else if(tIndex == currentIndex)
    	  	index = pageCount-1;
    	  else
    	  	index = tIndex;	
    	
        if (index >= MAX_NUM_WINDOWS_TO_RESTORE) return;
        indexStr.setNum(tIndex);
        QString historyFile = d->m_historyDir + QLatin1String("/history") + indexStr + QLatin1String(".history");
        // Save the History state to disk
        QFile file(historyFile);
        if(file.open(QIODevice::WriteOnly)) {
           QDataStream save(&file);
           save << *(d->m_allPages.at(index)->history());
           qDebug() << "Saved History<<:" << historyFile;
           file.flush();
           file.close();
        }
    }
}

void WebPageController::deleteDataFiles()
{
    QDir dir(d->m_historyDir);
    
    QString lasturlFile = d->m_historyDir + QLatin1String("/lasturl") + QLatin1String(".dat");
    QFile file1(lasturlFile);
    if(file1.open(QIODevice::ReadOnly)) 
    {
         file1.remove();
         file1.close();
    }
    
    QString winFile = d->m_historyDir + QLatin1String("/numwindow") + QLatin1String(".dat");
    QFile file2(winFile);
    if(file2.open(QIODevice::ReadOnly)) 
    {
         file2.remove();
         file2.close();
    }
          
}

void WebPageController::savePopupSettings(int val)
{
	  BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->setValue("PopupBlocking", val);
}

bool WebPageController::getPopupSettings()
{
		bool val = (bool)BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("PopupBlocking").toInt();
		return val;
		
}

void WebPageController::saveNumberOfWindows()
{
	QString winFile = d->m_historyDir + QLatin1String("/numwindow") + QLatin1String(".dat");
  int winCount =  d->m_allPages.count();
    
  QFile file(winFile);
  if(file.open(QIODevice::WriteOnly)) {
        QTextStream textStream(&file);
        textStream << winCount;
        textStream.flush();
        file.close();
   }
	
}

int WebPageController::restoreNumberOfWindows()
{
	QString winFile = d->m_historyDir + QLatin1String("/numwindow") + QLatin1String(".dat");
    QFile file(winFile);
    QString winStr = "0";
    if(file.open(QIODevice::ReadOnly)) {
        QTextStream textStream(&file);
        textStream >> winStr;
        file.close();
    }
    return winStr.toInt();
}

int WebPageController::historyWindowCount()
{
    QDir dir(d->m_historyDir);
    QFileInfoList fileList(dir.entryInfoList(QDir::Files));
    int windowCount = 0;
    
    foreach (const QFileInfo fileInfo, fileList) {
        const QString filePath(fileInfo.absoluteFilePath());
        if (filePath.endsWith(QString(KHISTORYEXTENSION), Qt::CaseInsensitive)) {        	  
            windowCount++;            
        }
    }
    return windowCount;
}

void WebPageController::networkRequestFinished(QNetworkReply *reply){
  if(reply->error() != QNetworkReply::NoError) {
    emit networkRequestError(reply);
  }
}

QString WebPageController::currentDocTitle()
{
    assert(currentPage());
    return currentPage()->pageTitle();
}

QString WebPageController::currentDocUrl() const
{
    assert(currentPage());
    return currentPage()->mainFrame()->url().toString();
}

QString WebPageController::currentRequestedUrl() const
{
    assert(currentPage());
    return currentPage()->mainFrame()->requestedUrl().toString();
}

QString WebPageController::currentPartialUrl() 
{
    assert(currentPage());
    return (partialUrl(currentPage()->mainFrame()->url()));
}

QVariant WebPageController::currentContentWindowObject()
{
    try {
        return currentPage()->mainFrame()->evaluateJavaScript("window");
    }
    catch(...) {
        qDebug() << "WebPageController::currentContentWindowObject: caught expection";
        return QVariant();
    }
}

int WebPageController::contentsYPos()
{
    assert(currentPage());
    return currentPage()->mainFrame()->scrollPosition().y();
}

int WebPageController::currentPageIndex(){

    return d->m_currentPage;
}

int WebPageController::secureState() {

    //qDebug() << __func__ << currentPage()->secureState();
    return (currentPage()->secureState());
}

// copy/paste from controllableviewjstobject. TODO: merge common code
void WebPageController::updateJSActions()
{   // slot
    if(d->m_actionsParent) {
        delete d->m_actionsParent;
        d->m_actionsParent = 0;
    }
    if(!getContext().isEmpty()) {
        d->m_actionsParent = new QObject(this);
        d->m_actionsParent->setObjectName("actions");
        foreach(QAction *action, getContext()) {
            ActionJSObject *jso = new ActionJSObject(d->m_actionsParent, action);
        }
    }
}

void WebPageController::clearHistoryInMemory()
{
    WRT::WrtBrowserContainer * activePage = currentPage();
    if(activePage)
    {
        activePage->history()->clear();
    }
    d->donotsaveFlag = true;
}

void WebPageController::deleteHistory()
{
    QDir dir(d->m_historyDir);
    QFileInfoList fileList(dir.entryInfoList(QDir::Files));
    QString indexStr;
    int index = 0;    
    foreach (const QFileInfo fileInfo, fileList) {
        const QString filePath(fileInfo.absoluteFilePath());
        if (filePath.endsWith(QString(KHISTORYEXTENSION), Qt::CaseInsensitive)) {
            indexStr.setNum(index);
            QString historyFile = d->m_historyDir + QLatin1String("/history") + indexStr + QLatin1String(".history");
            QFile file(historyFile);
            if(file.open(QIODevice::ReadOnly)) {
               file.remove();
               file.close();
            }
            index++;          
        }
    }
}

void WebPageController::deleteCookies()
{
	  QDir dir(d->m_historyDir);
    QFileInfoList fileList(dir.entryInfoList(QDir::Files));
    QString indexStr;
    int index = 0;    
    foreach (const QFileInfo fileInfo, fileList) {
        const QString filePath(fileInfo.absoluteFilePath());
        if (filePath.endsWith(QString(KHISTORYEXTENSION), Qt::CaseInsensitive)) {
            indexStr.setNum(index);
            QString cookiesFile = d->m_historyDir + QLatin1String("/cookies.ini");
            QFile file(cookiesFile);
            if(file.open(QIODevice::ReadOnly)) {
               file.remove();
               file.close();
            }
            index++;          
        }
    }
}

void WebPageController::deleteCache()
{
	  if ( !BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("DiskCacheEnabled").toBool() ) 
			return;
		
		QString diskCacheDir = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("DiskCacheDirectoryPath").toString();
			
    if(diskCacheDir.isEmpty()) 
    	return;
    	
    //QDir dir1(d->m_historyDir +"/cwrtCache/http");	  
    QDir dir1(diskCacheDir + "/brCache/http");
    	
    QFileInfoList fileList1(dir1.entryInfoList(QDir::Files));
      
    foreach (const QFileInfo fileInfo, fileList1) {
            const QString filePath(fileInfo.absoluteFilePath());
            QFile file(filePath);
            if(file.open(QIODevice::ReadOnly)) {
               file.remove();
               file.close();
            }            
    }
    
    //QDir dir2(d->m_historyDir +"/cwrtCache/https");
    QDir dir2(diskCacheDir +"/brCache/https");
    
    QFileInfoList fileList2(dir2.entryInfoList(QDir::Files));
      
    foreach (const QFileInfo fileInfo, fileList2) {
            const QString filePath(fileInfo.absoluteFilePath());
            QFile file(filePath);
            if(file.open(QIODevice::ReadOnly)) {
               file.remove();
               file.close();
            }            
    }
    
    //QDir dir3(d->m_historyDir +"/brCache/prepared");
    QDir dir3(diskCacheDir +"/cwrtCache/prepared");
    
    QFileInfoList fileList3(dir3.entryInfoList(QDir::Files));
      
    foreach (const QFileInfo fileInfo, fileList3) {
            const QString filePath(fileInfo.absoluteFilePath());
            QFile file(filePath);
            if(file.open(QIODevice::ReadOnly)) {
               file.remove();
               file.close();
            }            
    }
    
    
}


void WebPageController::urlChanged(const QUrl& url)
{
    //private slot
    QString urlString = partialUrl(url);
    emit partialUrlChanged(urlString);    
}


QString WebPageController::partialUrl(const QUrl& url)
{

    QString urlString = url.toString();
    QString scheme=url.scheme();
    urlString.remove(0, scheme.length() + 3); // remove "scheme://"
    if (urlString.startsWith("www.", Qt::CaseInsensitive)) {
        urlString.remove(0, 4);
    }
    return urlString;
}

QGraphicsWebView* WebPageController::webView()
{
    for (int i = 0; i < allPages()->count(); i++) {
        WRT::WrtBrowserContainer* page = allPages()->at(i);
        if (page->webWidget()) {
            return qobject_cast<QGraphicsWebView*> (page->webWidget());
        }
    }
    return NULL;
}

void WebPageController::checkAndUpdatePageThumbnails()
{
//    WebContentWidget* view = qobject_cast<WebContentWidget*> (webView());
    QGraphicsWebView *view = webView();
    WRT::WrtBrowserContainer* savedPage = qobject_cast<WRT::WrtBrowserContainer*> (view->page());
    if(!savedPage) return;
    QSize currSize = view->size().toSize();

    bool needRestore =  false;

    for (int i = 0; i < allPages()->count(); i++) {
         WRT::WrtBrowserContainer* page = allPages()->at(i);
         QWebHistoryItem item = page->history()->currentItem();

         WebPageData data = item.userData().value<WebPageData>();

        // If not still a blank window, check whether we need to update the img
        if (!page->emptyWindow() ){
            QImage img = data.m_thumbnail;
    
             bool isSameMode = ( (img.size().width() > img.size().height()) == (currSize.width() > currSize.height()) );
             if (img.isNull() || !isSameMode) {
         
                 qDebug() << "need new thumbnail!!!" << img.size() << ":" << currSize;
                 needRestore = true;
                 view->setPage(page);
                 page->setWebWidget(view);
                 QCoreApplication::sendEvent(view, new WebPageControllerUpdateViewPortEvent());
                 page->savePageDataToHistoryItem(page->mainFrame(), &item);
             }
         }
    }

    // restore
    if (needRestore)
    {    
        view->setPage(savedPage);
        savedPage->setWebWidget(view);
    }
}

void WebPageController::updatePageThumbnails()
{
    // update current page's thumbnail forcely
    WRT::WrtBrowserContainer *page = currentPage();
    QWebHistoryItem item = page->history()->currentItem();

    page->savePageDataToHistoryItem(page->mainFrame(), &item);

    WebPageData data = item.userData().value<WebPageData>();

    checkAndUpdatePageThumbnails();
}

void WebPageController::resizeAndUpdatePageThumbnails(QSize& s)
{
    webView()->resize(s); // resize the view

    checkAndUpdatePageThumbnails();
}

void WebPageController::urlTextChanged(QString str ) {

   currentPage()->loadController()->setUrlText(str);
}

int WebPageController::loadState() {
    //if (currentPage())

    return currentPage()->loadController()->mode();
}

void WebPageController::setLoadState(int mode) {

    //qDebug() << __func__;
    currentPage()->loadController()->setMode((WRT::LoadController::GotoBrowserMode)mode);
    //qDebug() << __func__ << currentPage()->loadController()->mode();
}

int WebPageController::loadProgressValue() {

    //qDebug()  << __func__ << currentPage()->loadController()->progressValue();
    return currentPage()->loadController()->progressValue();
}

QString WebPageController::loadText() {

    //qDebug() << __func__ << currentPage()->loadController()->urlText();
    return currentPage()->loadController()->urlText();
}

bool WebPageController::isPageLoading() {
    return currentPage()->loadController()->isPageLoading();
}

bool WebPageController::loadCanceled() {
    return currentPage()->loadController()->loadCanceled();
}
/*!
  \fn void WebPageController::pageCreated(WrtPage* newPage);
  emitted when a page is created
*/

/*!
  \fn void WebPageController::pageDeleted(WrtPage* newPage);
  emitted when a page is deleted (i.e. closed)
*/

/*!
  \fn void WebPageController::pageChanged(WrtPage* oldPage, WrtPage* NewPage);
  emitted when the current page is changed (note, either old or new could be NULL)
*/

/*!
  \fn void WebPageController::titleChanged(const QString &);
  emitted when the title of the current page changes
*/

/*!
  \fn void WebPageController::loadStarted();
  emitted when a load of the current page has begun
*/

/*!
  \fn void WebPageController::loadProgress( int progress );
  emitted to indicate load progress of the current page
*/

/*!
  \fn void WebPageController::loadFinished( bool ok );
  emitted to indicate load progress of the current page
  \a ok indicates whether load was successful
*/

/*!
  \fn void WebPageController::currentPageIconChanged();
  emitted when the favicon of the current page has changed
*/

/*!
  \fn void WebPageController::currentPageUrlChanged( const QUrl & url );
  emitted when the url of the current page changed
*/

/*!
  \fn void WebPageController::partialUrlChanged(QString url);
  emitted when the url of the current page changed
*/

/*!
  \fn void WebPageController::unsupportedContent(QNetworkReply *);
  emitted when unsupported content has arrived
*/

/*!
  \fn void WebPageController::networkRequestStarted(QWebFrame*, QNetworkRequest*);
  emitted when a frame of the current page requests a web resource.
*/

/*!
  \fn void WebPageController::sslErrors(QNetworkReply *, const QList<QSslError> &);
  emitted when sslError occurs for the current page
*/

/*!
  \fn void pageScrollRequested(int, int);
  emitted when page gets scrolled
*/

