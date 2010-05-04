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


#ifndef Q_OS_SYMBIAN
#include <QtGui>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <qwebpage.h>
#include <qwebframe.h>
#include <qwebview.h>
#include <assert.h>
#include <QtNetwork>
#include "chromewindow.h"
#include "../chromewidget.h"
#include "../chromejsobject.h"
#include "../chromeview.h"
#include "webcontentview.h"
#include "../utilities.h"
#include "chromeconsole.h"

ChromeWindow::ChromeWindow(const QString chromePath)
{
	init(chromePath);
}

ChromeWindow::ChromeWindow()
{
	init(ChromeView::getChromePath());
}

void ChromeWindow::init(const QString chromePath)
{
  m_view = new ChromeView(chromePath, this);

  WebContentView *webView = static_cast<WebContentView *>(m_view->contentView(WebContentView::Type()));

  setCentralWidget(m_view);

  initActions();
  initToolbars();
  initMenus();

  resize(360,740);
}

ChromeWindow::~ChromeWindow(){
  delete m_view;
  delete m_portraitAction;
  delete m_landscapeAction;
  delete m_networkProxyAction;
  delete m_setToBedrockChromeAction;
  delete m_setToDemoChromeAction;
  delete m_consoleAction;
}

void ChromeWindow::initActions(){
  m_reloadAction = new QAction(QIcon("images/reload.png"), tr("&Reload Chrome"), 0);
  safe_connect(m_reloadAction, SIGNAL(triggered()), m_view, SLOT(reloadChrome()));

  m_networkProxyAction = new QAction(QIcon("images/net_proxy.png"), tr("&Use Network Proxy"), 0);
  safe_connect(m_networkProxyAction, SIGNAL(triggered()), this, SLOT(toggleProxy()));
  m_networkProxyAction->setCheckable(true);


  m_setToDemoChromeAction = new QAction(tr("&Set Demo Chrome"), 0);
  m_setToDemoChromeAction->setData(QVariant(QString("chrome/demochrome/chrome.html")));
  safe_connect(m_setToDemoChromeAction, SIGNAL(triggered()), this, SLOT(setChrome()));

  m_setToBedrockChromeAction = new QAction(tr("&Set Bedrock Chrome"), 0);
  m_setToBedrockChromeAction->setData(QVariant(QString("chrome/bedrockchrome/chrome.html")));
  safe_connect(m_setToBedrockChromeAction, SIGNAL(triggered()), this, SLOT(setChrome()));

  WebContentView *webView = static_cast<WebContentView *>(m_view->contentView(WebContentView::Type()));

  // Display orientation actions.
  QActionGroup *orientGroup = new QActionGroup(this);
  m_portraitAction = new QAction(QIcon("images/orient_north.png"), tr("&North"), 0);
  orientGroup->addAction(m_portraitAction);
  safe_connect(m_portraitAction, SIGNAL(triggered()), this, SLOT(portrait()));
  m_portraitAction->setCheckable(true);
  m_portraitAction->setChecked(true);

  m_landscapeAction = new QAction(QIcon("images/orient_east.png"), tr("&East"), 0);
  orientGroup->addAction(m_landscapeAction);
  safe_connect(m_landscapeAction, SIGNAL(triggered()), this, SLOT(landscape()));
  m_landscapeAction->setCheckable(true);

  /*
  m_orientSouthAction = new QAction(QIcon("images/orient_south.png"), tr("&South"), 0);
  orientGroup->addAction(m_orientSouthAction);
  safe_connect(m_orientSouthAction, SIGNAL(triggered()), m_view, SLOT(orientSouth()));
  m_orientSouthAction->setCheckable(true);

  m_orientWestAction = new QAction(QIcon("images/orient_west.png"), tr("&West"), 0);
  orientGroup->addAction(m_orientWestAction);
  safe_connect(m_orientWestAction, SIGNAL(triggered()), m_view, SLOT(orientWest()));
  m_orientWestAction->setCheckable(true);
  */
  m_consoleAction = new QAction(QIcon("images/console.png"), tr("&Console"), 0);
  orientGroup->addAction(m_consoleAction);
  safe_connect(m_consoleAction, SIGNAL(triggered()), this, SLOT(openConsole()));
}

void ChromeWindow::initMenus(){
  // File menu.
  QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

  fileMenu->addAction(m_reloadAction);
  fileMenu->addAction(m_networkProxyAction);

  // View menu.
  QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction(m_portraitAction);
  viewMenu->addAction(m_landscapeAction);
  //viewMenu->addAction(m_orientSouthAction);
  //viewMenu->addAction(m_orientWestAction);
  viewMenu->addSeparator();
  viewMenu->addAction(m_consoleAction);

  // Chrome menu
  QMenu *chromeMenu = menuBar()->addMenu(tr("&Chrome"));
  chromeMenu->addAction(m_setToDemoChromeAction);
  chromeMenu->addAction(m_setToBedrockChromeAction);
}

void ChromeWindow::initToolbars(){
  QToolBar *toolBar = addToolBar("File");
  toolBar->setIconSize(QSize(16, 16));
  toolBar->addAction(m_reloadAction);
  toolBar->addSeparator();
  toolBar->addAction(m_portraitAction);
  toolBar->addAction(m_landscapeAction);
  //toolBar->addAction(m_orientSouthAction);
  //toolBar->addAction(m_orientWestAction);
  toolBar->addSeparator();
  toolBar->addAction(m_consoleAction);
}

void ChromeWindow::portrait(){
  resize(360,720);
}

void ChromeWindow::landscape(){
  resize(720,360);
}
void ChromeWindow::toggleProxy() {  // slot
  WebContentView *webView = static_cast<WebContentView *>(m_view->contentView(WebContentView::Type()));
}

void ChromeWindow::setChrome()
{
    if(m_view) {
        QAction *action = qobject_cast<QAction *>(sender());
        QString chromePath = action->data().value<QString>();
        qDebug() << "Setting Chrome: " << chromePath;
        m_view->setChromeUrl(chromePath);
        m_view->reloadChrome();
    }
}

void ChromeWindow::openConsole() {  // slot
  ChromeConsole *console = new ChromeConsole(m_view);
  console->show();
}
#endif
