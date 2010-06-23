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

#include "browser.h"
#ifndef NO_QSTM_GESTURE
#include "WebGestureHelper.h"
#endif
#include "../ChromeWidget.h"
#include "HistoryFlowView.h"
#include "WindowFlowView.h"
#include "webpagecontroller.h"
#include "bedrockprovisioning.h"

#include <QDebug>
#ifdef _GVA_INSPECT_CHROME_
#include <QWebInspector>
#endif
#include <QWebSettings>

#ifndef Q_OS_SYMBIAN
// Uncomment the next line to enable the javascript console dialog.
//#define CHROME_CONSOLE 1
#ifdef CHROME_CONSOLE
#include "ChromeConsole.h"
#endif
#endif

GinebraBrowser::GinebraBrowser(QObject * parent, QString *url)
  : QObject(parent),
    m_splashScreen(NULL)
{
  // The initial url to go to when the browser is called from another app
  if (url != 0) {
      qDebug() << "GinebraBrowser::GinebraBrowser - initialurl=" << *url;
//      m_initialUrl = url->mid(2);
      m_initialUrl = *url;
  }
  QString startUpChrome(BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("StartUpChrome"));
  m_install = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("ChromeBaseDirectory")
      + startUpChrome.section('/', 0, -2) + "/";
  m_chromeUrl = startUpChrome.section('/', -1);
  m_contentUrl = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("StartPage");

  setApplicationNameVersion();

  //qDebug() << "GinebraBrowser::GinebraBrowser: " << m_install << " " << m_chromeUrl;
  //GVA::Settings * settings = GVA::Settings::instance();
  //settings->setInstallRoot(m_install);
  // Create the chrome widget
  m_chrome = new GVA::ChromeWidget();

  //removeFaviconDir();

  /*QDir homeDir = getHomeDir();
  homeDir.mkdir("favicon");

  QString iconDBPath = homeDir.absolutePath() + "/favicon";
  QWebSettings::globalSettings()->setIconDatabasePath(iconDBPath);*/


#ifdef _GVA_INSPECT_CHROME_
  QWebSettings * s = m_chrome->page()->settings();
  s->setAttribute(QWebSettings::DeveloperExtrasEnabled, true );
  QWebInspector *inspector = new QWebInspector;
  inspector->setPage(m_chrome->page());
  inspector->resize(400,600);
  inspector->show();
  connect(m_chrome->page(), SIGNAL(webInspectorTriggered(QWebElement)), inspector, SLOT(show()));
#endif
  //Create a view onto the chrome
  m_view = new GVA::ChromeView(m_chrome);
#ifndef NO_QSTM_GESTURE
  WebGestureHelper* gh = new WebGestureHelper(m_view);
  browserApp->setGestureHelper(gh);
  browserApp->setMainWindow(m_view);
  m_view->grabGesture(QStm_Gesture::assignedType());
#endif

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5)
  m_view->showFullScreen();
#else
  m_view->setGeometry(0,0,360,640);
#endif

  showSplashScreen();

#ifndef __gva_no_chrome__
  QObject::connect(m_chrome, SIGNAL(internalChromeComplete()), this, SLOT(onChromeComplete()));
   //Load the chrome
  m_chrome->setChromeBaseDirectory(m_install);
  m_chrome->setChromeFile(m_chromeUrl);
#ifdef CHROME_CONSOLE
  // Show the javascript console.
  ChromeConsole *console = new ChromeConsole(m_chrome);
  console->show();
  console->move(m_view->geometry().topLeft() + QPoint(m_view->width()+6, 0));
#endif
#else
  onChromeComplete();

#endif

  // Handle openUrl signals
  connect(this, SIGNAL(openUrlRequested(QString)), this, SLOT(openUrl(QString)));
}

GinebraBrowser::~GinebraBrowser()
{
  delete m_chrome;
  delete WebPageController::getSingleton();
  delete m_view;
  destroySplashScreen();

#ifndef NO_QSTM_GESTURE
  WebGestureHelper* gh = browserApp->gestureHelper();
  delete gh;
#endif
}

/*
QDir GinebraBrowser::getHomeDir()
{
    QDir homeDir;

#ifndef QT_NO_DESKTOPSERVICES
    homeDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#else
    homeDir = QDir::homePath();
#endif
#ifdef Q_OS_SYMBIAN
    if (homeDir.startsWith("Z"))
        homeDir.replace(0,1,"C");
#endif
    return homeDir;
}

void GinebraBrowser::removeFaviconDir()
{
    QDir homeDir = getHomeDir();
    homeDir.cd("favicon");
    homeDir.cdUp();
    homeDir.rmdir("favicon");
}
*/
void GinebraBrowser::show()
{
  m_view->show();
}

void GinebraBrowser::onChromeComplete()
{
#ifndef __gva_no_chrome__
  ControllableViewBase *windowView = WRT::WindowFlowView::createNew(m_chrome);
  windowView->setObjectName("WindowView");
  m_chrome->addView(windowView);

  ControllableViewBase *historyView = WRT::HistoryFlowView::createNew(m_chrome);
  historyView->setObjectName("HistoryView");
  m_chrome->addView(historyView);

#endif
  //Create a content window and add it to the chrome
  GVA::GWebContentView *content = new GVA::GWebContentView(m_chrome, 0, "WebView");
  //Load the initial content after the chrome loads. This makes sure that an initial bad
  //content page won't hang up rendering the chrome.
  m_chrome->addView(content);

  QString chromeBaseDir = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("ChromeBaseDirectory");
  QString startPage = chromeBaseDir + m_contentUrl;
  qDebug() << "GinebraBrowser::onChromeComplete: startPage: " << startPage;

  bool enabled = (bool) BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsInt("SaveSession");

  // If the browser was launched by some other app calling QDesktopServices.openUrl, go to that url
  if (!m_initialUrl.isEmpty()) {
      qDebug() << "called from openurl, loading page " << m_initialUrl;
      openUrl(m_initialUrl);
  }
  // Otherwise, load the previous page from history (if that option is enabled)
  else if (enabled && m_initialUrl.isEmpty()) {
    qDebug() << "loading page from history";
    WebPageController::getSingleton()->loadFromHistory();
  }
  // Otherwise, load the start page
  else {
    qDebug() << "loading startpage " << startPage;
    content->loadUrlToCurrentPage(startPage);
  }

  m_chrome->showView("WebView");
  destroySplashScreen();
}

void GinebraBrowser::queueOpenUrl(QString url)
{
    emit openUrlRequested(url);
}

void GinebraBrowser::openUrl(QString url)
{
#ifdef Q_OS_SYMBIAN
    // Handle the url as per the old standard at
    // http://wiki.forum.nokia.com/index.php/TSS000340_-_Launching_the_Web_browser_on_S60_3rd_Edition_to_open_a_specified_URL
    QChar urlType = url.at(0);
    // Bookmark (by uid) - not handled
    if (urlType == '1') {
        return;
    }
    // Saved deck (by uid) - not handled
    else if (urlType == '2') {
        return;
    }
    // Start page - not handled
    else if (urlType == '5') {
        return;
    }
    // Bookmark folder (by uid) - not handled
    else if (urlType == '6') {
        return;
    }
    // Url or Url + space + access point
    else if (urlType == '4') {
        url = url.mid(2); // Get the real url
        if (url.contains(' ')) { // Chop off the access point if there is one because it's not currently handled
            url = url.left(url.indexOf(' '));
        }
    }
    // If no number then it's just a plain url
#endif /* Q_OS_SYMBIAN */
    // Bring the browser to the front (QDesktopServices openurl is supposed to do this but doesn't)
    if (m_view) {
        m_view->activateWindow();
        m_view->raise();
    }
    qDebug() << "GinebraBrowser::openUrl - " << url;
    m_contentUrl = url;
    WebPageController::getSingleton()->loadInitialUrlFromOtherApp(url);
//    GVA::GWebContentView *webView = (GVA::GWebContentView *)m_chrome->getView("WebView");
//    if (webView != 0) {
//        m_contentUrl = url;
//        webView->loadUrlToCurrentPage(url);
//    }
}

void GinebraBrowser::showSplashScreen() {
  QString splashImage = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("SplashImage");
  QString baseDir = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("ChromeBaseDirectory");
  QString imagePath =   baseDir + splashImage;

  qDebug() << "ChromeView::showSplashScreen: " << imagePath;
  if (!imagePath.isNull()) {
    m_splashScreen = new QLabel(NULL);
    m_splashScreen->setAlignment(Qt::AlignCenter);
    m_splashScreen->setStyleSheet("background-color: #FFF");
    m_splashScreen->setPixmap(QPixmap(imagePath));
    if (m_splashScreen->pixmap()->isNull()) {
        ;//qDebug() << "ChromeView::chromeLoaded: ERROR splashscreen creation failed. " << imagePath;
    }
    else {
        m_splashScreen->show();
            #ifdef Q_OS_SYMBIAN
                m_splashScreen->showFullScreen();
                m_view->showFullScreen();
                #else
                m_splashScreen->setGeometry(0,0,360,640);
                #endif
    }
  }
}

void GinebraBrowser::destroySplashScreen()
{
  if (m_splashScreen)
  {
    delete m_splashScreen;
    m_splashScreen = NULL;
  }
}

void GinebraBrowser::setApplicationNameVersion()
{
  QCoreApplication::setApplicationName(BEDROCK_APPLICATION_NAME);
  QString browserAppVersion = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("BedrockVersion");
  QCoreApplication::setApplicationVersion(browserAppVersion);
}
