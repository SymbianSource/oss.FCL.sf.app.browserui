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

#ifndef __GINEBRA_BROWSER_H__
#define __GINEBRA_BROWSER_H__

#include <QObject>
#include <QDir>
#include "../ChromeWidget.h"
#include "../ChromeView.h"
#include "GWebContentView.h"

#ifdef Q_WS_MAEMO_5
namespace GVA {
   class WindowsView;
   class BrowserWindow;
}
#endif

class GinebraApplication;

class GinebraBrowser : public QObject
{
  Q_OBJECT
 public:
  GinebraBrowser( QObject * parent = 0, QString *url = 0);
  ~GinebraBrowser();
  void show();
  void showSplashScreen();
  void destroySplashScreen();
  void setApplicationNameVersion();
  void queueOpenUrl(QString url);

 signals:
  void openUrlRequested(QString url);

 public slots:
  void onChromeComplete();
  void openUrl(QString);

#ifdef Q_WS_MAEMO_5
 private slots:
  void addMenuBarAction(QAction *action);
  void setMenuBarEnabled(bool value = true);
  void onTitleChanged(const QString &title);
  
 private:
  void fixupWindowTitle();
#endif

 private:
  void platformSpecificInit();

 private:
  QString m_install;
  QString m_chromeUrl;
  QString m_contentUrl;
  QString m_installBase;
  GVA::ChromeWidget * m_chrome;
  GVA::ChromeView * m_view;
  QGraphicsScene *m_scene;
  GVA::GinebraApplication *m_app;
  QString m_initialUrl;
#ifdef Q_WS_MAEMO_5
  GVA::BrowserWindow *m_mainWindow;
  QSplashScreen *m_splashScreenM5;
  GVA::WindowsView *m_windows;
  QMenu *m_menu;   // not owned
#else
  QLabel *m_splashScreen;  // Owned
#endif
};

#endif
