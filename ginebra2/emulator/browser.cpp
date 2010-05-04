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


#include "browser.h"
#ifndef NO_QSTM_GESTURE
#include "WebGestureHelper.h"
#endif
#include "../ChromeWidget.h"
#include "HistoryFlowView.h"
#include "WindowFlowView.h"
#include "WrtPageManager.h"
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

GinebraBrowser::GinebraBrowser(QObject * parent)
  : QObject(parent),
    m_splashScreen(NULL)
{
  QString startUpChrome(BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("StartUpChrome"));
  m_install = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("ChromeBaseDirectory2")
      + startUpChrome.section('/', 0, -2) + "/";
  m_chromeUrl = startUpChrome.section('/', -1);
  m_contentUrl = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("StartPage");

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
 
#ifdef Q_OS_SYMBIAN
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
#endif
  //Create a content window and add it to the chrome
  GVA::GWebContentView *content = new GVA::GWebContentView(m_chrome, 0, "WebView");

  //Load the initial content after the chrome loads. This makes sure that an initial bad
  //content page won't hang up rendering the chrome.
  m_chrome->addView(content);
  
  QString startPage = m_install + m_contentUrl;
  qDebug() << "GinebraBrowser::onChromeComplete: startPage: " << startPage;

  bool enabled = (bool) BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsInt("SaveSession");
  if (enabled)
    WebPageController::getSingleton()->loadFromHistory();
  else
    content->loadUrlToCurrentPage(startPage);
    
#ifndef __gva_no_chrome__
  WRT::HistoryFlowView *historyView = new WRT::HistoryFlowView(WrtPageManager::getSingleton(), m_chrome);
  historyView->setObjectName("HistoryView");
  m_chrome->addView(historyView);
#endif
  m_chrome->showView("WebView");
  destroySplashScreen();
}

void GinebraBrowser::showSplashScreen() {
  QString splashImage = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("SplashImage");
  QString baseDir = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("ChromeBaseDirectory2");
  QString imagePath =	baseDir + splashImage;
  
  qDebug() << "ChromeView::showSplashScreen: " << imagePath; 
  if(!imagePath.isNull()) {
    m_splashScreen = new QLabel(NULL);
    m_splashScreen->setAlignment(Qt::AlignCenter);
    m_splashScreen->setStyleSheet("background-color: #FFF");
    m_splashScreen->setPixmap(QPixmap(imagePath));
    if(m_splashScreen->pixmap()->isNull()) {
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
  if(m_splashScreen) 
  {
    delete m_splashScreen;
    m_splashScreen = NULL;
  }
}
