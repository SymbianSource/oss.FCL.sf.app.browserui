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

#include "chromejsobject.h"
#include "qwebpage.h"
#include "qwebframe.h"
#include "chromewidget.h"
#include "chromeview.h"
#include "webcontentview.h"
#include <QNetworkReply>
#include "scriptobjects.h"
#include "UiUtil.h"
#include "webpagecontroller.h"
#include <qdesktopservices.h>

#ifdef USE_DOWNLOAD_MANAGER
#include "download.h"
#endif

ChromeJSObject::ChromeJSObject(ChromeView* chrome)
 : m_chromeView(chrome)
{
  setObjectName("chrome");
#ifdef ENABLE_LOG
  initLogFile();
#endif
}

void ChromeJSObject::loadChrome(const QString url)  // slot
{
  m_chromeView->loadChrome(url);
}

void ChromeJSObject::reloadChrome()  // slot
{
  m_chromeView->reloadChrome();
}

#ifdef USE_STATEMACHINE
void ChromeJSObject::requestChrome(const QString &url)  // slot
{
  qDebug() << "ChromeJSObject::requestChrome: " << url;
  emit onRequestChrome(url);
}
#endif

void ChromeJSObject::updateViewPort(){
  m_chromeView->updateViewPort();
}

void ChromeJSObject::setViewPort(int x1, int y1, int x2, int y2){
  QRect viewPort(x1,y1,x2,y2);
  m_chromeView->setViewPort(viewPort);
}
void ChromeJSObject::flipFromCurrentView(const QString& toId){
  m_chromeView->flipFromCurrentView(toId);
}

void ChromeJSObject::flipToCurrentView(const QString& fromId){
  m_chromeView->flipToCurrentView(fromId);
}

void ChromeJSObject::setLocation(const QString& id, int x, int y){
  m_chromeView->setLocation(id,x,y);
}

void ChromeJSObject::setAnchor(const QString& id, const QString& anchor){
  m_chromeView->setAnchor(id, anchor);
}

void ChromeJSObject::toggleVisibility(const QString& id){
  m_chromeView->toggleVisibility(id);
}

void ChromeJSObject::show(const QString& id, int x, int y){
  m_chromeView->show(id, x, y);
}

void ChromeJSObject::hide(const QString& id){
  m_chromeView->hide(id);
}


void ChromeJSObject::toggleAttention(const QString& id){
  m_chromeView->toggleAttention(id);

}

void ChromeJSObject::setVisibilityAnimator(const QString& elementId, const QString & animatorName){
  m_chromeView->setVisibilityAnimator(elementId, animatorName);
}

void ChromeJSObject::setAttentionAnimator(const QString& elementId, const QString & animatorName){
  qDebug() << "Set attention animator";
  m_chromeView->setAttentionAnimator(elementId, animatorName);
}

QString ChromeJSObject::guessUrlFromString(const QString &s){
    QUrl u = WRT::UiUtil::guessUrlFromString(s);
    return u.toString();
}

QString ChromeJSObject::guessAndGotoUrl(const QString &s){
    QString guessedUrl(guessUrlFromString(s));
    WebPageController::getSingleton()->currentLoad(guessedUrl);
    return guessedUrl;
}

QString ChromeJSObject::searchUrl(const QString &s){
    QUrl u = WRT::UiUtil::searchUrl(s);
    return u.toString();
}

void ChromeJSObject::loadFromHistory()
{
    int count = WebPageController::getSingleton()->historyWindowCount();
        
    if(!count)
    {
    		loadLocalFile(); // load start page for first startup of the browser
    }
    else
    {	
    	 int count2 = WebPageController::getSingleton()->restoreNumberOfWindows();
    	 
    	 // "sanity check" count shall be always equal to count2. If not, something wrong. Open only one window - start page in this case.
    	 	
    	 if((count > 0) && (count == count2)) // load all browser windows from the last session
    	 {
    	 	
    	 	for(int i = 0; i < count2-1; i++) //all windows except current
    	 	{
    	 		WebPageController::getSingleton()->openPageFromHistory(i);
    	 		WebPageController::getSingleton()->pageGotoCurrentItem(i);
    	 		WebPageController::getSingleton()->pageReload(i);
    	 	}		
    	 
    	 // current window
    	 WebPageController::getSingleton()->currentLoad("");	
    	 WebPageController::getSingleton()->gotoCurrentItem();
    	 WebPageController::getSingleton()->currentReload();	
    	     	 		
    	 WebPageController::getSingleton()->deleteHistory();
    	 	
    	 }
    	 else // browser did not close correctly; exception handler: delete all files and load start page as in first startup
    	 {
    	 	WebPageController::getSingleton()->deleteHistory();
    	 	WebPageController::getSingleton()->deleteDataFiles();
    	 	loadLocalFile();
    	 }			    	 
    }	 
    
    return ;
}

	
	
QString ChromeJSObject::getLastUrl()
{
	QString lastUrl = WebPageController::getSingleton()->getLastUrl();
	if(lastUrl == "")
		return "http://";
	else
		return lastUrl ;	
}

int ChromeJSObject::numberOfWindows()
{
	int numWin = WebPageController::getSingleton()->restoreNumberOfWindows();
	
	return numWin;
}

void ChromeJSObject::clearHistory()
{
	WebPageController::getSingleton()->deleteHistory();
	WebPageController::getSingleton()->clearHistoryInMemory();
	WebPageController::getSingleton()->deleteDataFiles();
}

void ChromeJSObject::clearCookies()
{
	WebPageController::getSingleton()->deleteCookies();
}

void ChromeJSObject::clearCache()
{
	WebPageController::getSingleton()->deleteCache();
}

void ChromeJSObject::loadLocalFile() {
// for symbian deployment, local file root at C:/data/Others/
    QString chromeBaseDir = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("ChromeBaseDirectory");
    QString startPage = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("StartPage");
    QString startPagePath = chromeBaseDir + startPage;
    qDebug() << "ChromeJSObject::loadLocalFile: " << startPagePath;
    WebPageController::getSingleton()->currentLoad(startPagePath);
}

void ChromeJSObject::setViewofCurrentPage() {
    m_chromeView->setViewofCurrentPage();
}

QObject *ChromeJSObject::getContentViewRect() const {
    ScriptRect *rect = new ScriptRect(m_chromeView->geometry());
    m_chromeView->getChromeWidget()->chromePage()->mainFrame()->addToJavaScriptWindowObject(rect->objectName(), rect, QScriptEngine::ScriptOwnership);
    return rect;
}

void ChromeJSObject::alert(const QString msg) {
  // To do: open a dialog box showing msg.
  qDebug() << msg;
}

#ifdef ENABLE_LOG


QString ChromeJSObject::getLogPath()
{
#if defined(QT_NO_DESKTOPSERVICES)
#ifdef Q_OS_SYMBIAN
	static const QString LogPath("C:\\Data\\GinebraLog.txt");
#else
	static const QString LogPath("./GinebraLog.txt");
#endif
#else
    static const QString LogPath(QDesktopServices::storageLocation(QDesktopServices::TempLocation) + "./GinebraLog.txt");
#endif
    return LogPath;
}

void ChromeJSObject::initLogFile() {
    QFile file(getLogPath());
    file.remove();
    log("Log started " + QDate::currentDate().toString("dd.MM.yyyy"));
}

void ChromeJSObject::log(const QString msg) {
    QFile file(getLogPath());
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        qDebug() << "ChromeJSObject::log: open failed";
        return;
    }
    
    QTextStream out(&file);
    out << "\r\n" << QTime::currentTime ().toString("hh:mm:ss.zzz: ") << msg;
    
    file.flush();
    file.close();
}
#endif   // ENABLE_LOG

void ChromeJSObject::setBackgroundColor(int r, int g, int b){
    m_chromeView->setBackgroundColor(QColor(r, g, b));
}

void ChromeJSObject::displayModeChanged(ChromeView::DisplayMode displayMode){
    emit onDisplayModeChanged(displayMode == ChromeView::DisplayModePortrait ? "portrait" : "landscape");
}

void ChromeJSObject::displayModeChangeStart(ChromeView::DisplayMode displayMode) {

    emit onDisplayModeChangeStart(displayMode == ChromeView::DisplayModePortrait ? "portrait" : "landscape");
}

//-------------------------------------
// Property getters and setters
//-------------------------------------

QList<int> ChromeJSObject::getDisplaySize() const
{
    QList<int> list;
    list.append(m_chromeView->size().width());
    list.append(m_chromeView->size().height());
    return list;
}

QString ChromeJSObject::getDisplayMode() const{
    return m_chromeView->displayMode() == ChromeView::DisplayModePortrait ? "portrait" : "landscape";
}
/*
int ChromeJSObject::getDisplayOrientation() const{
    return m_chromeView->orientationAngle();
}
*/

// Support downloads

#ifdef USE_DOWNLOAD_MANAGER

static int dlInt(Download * download, DownloadAttribute attribute)
{
    return download->getAttribute(attribute).toInt();
}

static QString dlStr(Download * download, DownloadAttribute attribute)
{
    return download->getAttribute(attribute).toString();
}

void ChromeJSObject::downloadCreated(Download * download)
{
    // Note: the DlFileName at this point has not been
    // adjusted to account for duplicate file names in
    // the destination directory.  That doesn't happen
    // until the download starts.

    emit onDownloadCreated(download->id());
}

void ChromeJSObject::downloadStarted(Download * download)
{
    emit onDownloadStarted(
            download->id(),
            dlStr(download, DlFileName),
            dlInt(download, DlTotalSize));
}

void ChromeJSObject::downloadProgress(Download * download)
{
    emit onDownloadProgress(
            download->id(),
            dlStr(download, DlFileName),
            dlInt(download, DlTotalSize),
            dlInt(download, DlDownloadedSize),
            dlInt(download, DlPercentage));
}

void ChromeJSObject::downloadFinished(Download * download)
{
    emit onDownloadFinished(
            download->id(),
            dlStr(download, DlFileName),
            dlInt(download, DlTotalSize),
            dlInt(download, DlDownloadedSize),
            dlInt(download, DlPercentage));
}

void ChromeJSObject::downloadPaused(Download * download, const QString & error)
{
    emit onDownloadPaused(
            download->id(),
            dlStr(download, DlFileName),
            dlInt(download, DlTotalSize),
            dlInt(download, DlDownloadedSize),
            dlInt(download, DlPercentage),
            error);
}

void ChromeJSObject::downloadCancelled(Download * download, const QString & error)
{
    emit onDownloadCancelled(
            download->id(),
            dlStr(download, DlFileName),
            dlInt(download, DlTotalSize),
            dlInt(download, DlDownloadedSize),
            dlInt(download, DlPercentage),
            error);
}

void ChromeJSObject::downloadFailed(Download * download, const QString & error)
{
    emit onDownloadFailed(
            download->id(),
            dlStr(download, DlFileName),
            dlInt(download, DlTotalSize),
            dlInt(download, DlDownloadedSize),
            dlInt(download, DlPercentage),
            error);
}

void ChromeJSObject::downloadNetworkLoss(Download * download, const QString & error)
{
    emit onDownloadNetworkLoss(
            download->id(),
            dlStr(download, DlFileName),
            dlInt(download, DlTotalSize),
            dlInt(download, DlDownloadedSize),
            dlInt(download, DlPercentage),
            error);
}

void ChromeJSObject::downloadError(Download * download, const QString & error)
{
    emit onDownloadError(
            download->id(),
            dlStr(download, DlFileName),
            dlInt(download, DlTotalSize),
            dlInt(download, DlDownloadedSize),
            dlInt(download, DlPercentage),
            error);
}

#endif // USE_DOWNLOAD_MANAGER
