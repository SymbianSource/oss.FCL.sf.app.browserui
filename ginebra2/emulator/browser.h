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


#ifndef __GINEBRA_BROWSER_H__
#define __GINEBRA_BROWSER_H__

#include <QObject>
#include <QDir>
#include "../ChromeWidget.h"
#include "../ChromeView.h"
#include "GWebContentView.h"

class GinebraBrowser : public QObject
{
  Q_OBJECT 
 public:
  GinebraBrowser( QObject * parent = 0);
  ~GinebraBrowser();
  void show();
  void showSplashScreen();
  void destroySplashScreen();
  /*
private:
  QDir getHomeDir();
  void removeFaviconDir();
*/
 public slots:
  void onChromeComplete();
 private:
  QString m_install;
  QString m_chromeUrl;
  QString m_contentUrl;
  QString m_installBase;
  GVA::ChromeWidget * m_chrome;
  GVA::ChromeView * m_view;
  QLabel *m_splashScreen;  // Owned
};

#endif
