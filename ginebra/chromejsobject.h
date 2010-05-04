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


#ifndef _GINEBRA_CHROME_JS_OBJECT_H_
#define _GINEBRA_CHROME_JS_OBJECT_H_

#include <QObject>
#include <QString>
#include <QList>
#include "chromeview.h"

// Comment out this define to disable logging via javascript.
#define ENABLE_LOG

class ChromeWindow;
class QNetworkReply;
class QUrl;
class Download;

/**
 * \brief The ChromeJSObject class provides a javascript API to global or system-wide
 * functionality.
 */
class ChromeJSObject : public QObject

{
  Q_OBJECT

  friend class ChromeView;

 public:
  ChromeJSObject(ChromeView* chrome);

  QList<int> getDisplaySize() const;
  Q_PROPERTY(QList<int> displaySize READ getDisplaySize)

  // \brief Returns either "portrait" or "landscape".
  QString getDisplayMode() const;
  Q_PROPERTY(QString displayMode READ getDisplayMode)

  // \brief Returns the bounding rectangle of the content window.
  QObject *getContentViewRect() const;
  Q_PROPERTY(QObject *contentViewRect READ getContentViewRect)

public slots:   // Visible to javascript.
  // Load a new chrome from \a url.
  void loadChrome(const QString url);
  
  // Reload the current chrome.
  void reloadChrome();
  
#ifdef USE_STATEMACHINE
  void requestChrome(const QString &url);
#endif
  void updateViewPort();
  void setViewPort(int x1, int y1, int x2, int y2);
  void flipFromCurrentView(const QString& toId);
  void flipToCurrentView(const QString& fromId);
  
  // Write string \a msg to debug output.
  void alert(const QString msg);
  

#ifdef ENABLE_LOG
  // Write string \a msg to log file "c:\Data\GinebraLog.txt".
  void log(const QString msg);
#endif
  
  void setBackgroundColor(int r, int g, int b);  // !!! This should be a property.
  
  // Toggle the visibility of the snippet with id \a id.
  void toggleVisibility(const QString& id);
  
  // Show the snippet with id \a id.
  void show(const QString& id, int x=0, int y=0);
  
  // Hide the snippet with id \a id.
  void hide(const QString& id);
  
  void setLocation(const QString& id, int x, int y);
  void setAnchor(const QString& id, const QString& anchor);
  void toggleAttention(const QString & id);
  void setVisibilityAnimator(const QString& elementId, const QString & animatorName);
  void setAttentionAnimator(const QString& elementId, const QString & animatorName);
  QString guessUrlFromString(const QString &s);
  QString guessAndGotoUrl(const QString &s);
  QString searchUrl(const QString &s);
  void loadFromHistory();
  void clearHistory();
  void clearCookies();
  void clearCache();
  QString getLastUrl();
  int numberOfWindows();
  void loadLocalFile();
  void setViewofCurrentPage();

#ifdef USE_DOWNLOAD_MANAGER
  void downloadCreated(Download * download);
  void downloadStarted(Download * download);
  void downloadProgress(Download * download);
  void downloadFinished(Download * download);
  void downloadPaused(Download * download, const QString & error);
  void downloadCancelled(Download * download, const QString & error);
  void downloadFailed(Download * download, const QString & error);
  void downloadNetworkLoss(Download * download, const QString & error);
  void downloadError(Download * download, const QString & error);
#endif

 signals:
  // Sent when the chrome has finished loading.
  void loadComplete();
  // Sent when the viewport has been resized
  void viewPortResize(int x, int y, int w, int h);
  // Sent when the display mode changes from landscape to protrait or vice versa.
  void onDisplayModeChanged(const QString &orientation);
  // Sent when the display mode change starts from landscape to protrait or vice versa.
  void onDisplayModeChangeStart(const QString &orientation);

#ifdef USE_STATEMACHINE
  // Sent when the chrome wants to load a different chrome.
  void onRequestChrome(const QString url);
#endif

#ifdef USE_DOWNLOAD_MANAGER
  void onDownloadCreated(int);
  void onDownloadStarted(int, const QString &, int);
  void onDownloadProgress(int, const QString &, int, int, int);
  void onDownloadFinished(int, const QString &, int, int, int);
  void onDownloadPaused(int, const QString &, int, int, int, const QString &);
  void onDownloadCancelled(int, const QString &, int, int, int, const QString &);
  void onDownloadFailed(int, const QString &, int, int, int, const QString &);
  void onDownloadNetworkLoss(int, const QString &, int, int, int, const QString &);
  void onDownloadError(int, const QString &, int, int, int, const QString &);
#endif

 protected:
  void displayModeChanged(ChromeView::DisplayMode orientation);
  void displayModeChangeStart(ChromeView::DisplayMode orientation);
  void displayOrientationChanged(int angle);
  QString getLogPath();

#ifdef ENABLE_LOG  
  void initLogFile();
#endif

 private:
  ChromeView* m_chromeView;
};

#endif
