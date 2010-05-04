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


#ifndef __GINEBRA_APPCONTENTVIEW_H__
#define __GINEBRA_APPCONTENTVIEW_H__

#include "ContentViewDelegate.h"
#include <QGraphicsWebView>
#include <QNetworkAccessManager>

class QTimeLine;

namespace GVA {
  
  class AppContentView : public ContentViewDelegate
  {
    Q_OBJECT
  public:
    AppContentView(ChromeWidget * chrome, QObject * parent = 0);
    virtual ~AppContentView();
    QGraphicsWidget * view() {return static_cast<QGraphicsWidget*>(m_view);}
    QWebPage * page() {return m_page;}
#ifndef NO_QSTM_GESTURE
    bool eventFilter(QObject* object, QEvent* event);
#endif
    void addJSObjectToWindow(QObject *object);
  public slots:
    void load(const QString& url);
    void setHtml(const QString& html);
    void triggerAction(const QString & action);
    void stop();
    void back();
    void forward();
    void reload();
    void zoomBy(qreal delta);
    void zoom(bool in);
    void toggleZoom();
    void stopZoom();
    void scrollBy(int deltaX, int deltaY);
    int scrollX();
    int scrollY();
    int contentWidth();
    int contentHeight();
    void onUrlChanged(const QUrl & url);
  private slots:
    void updateZoom(qreal delta);
  signals:
    void iconChanged();
    void loadFinished(bool ok);
    void loadProgress(int progress);
    void loadStarted();
    void statusBarMessage(const QString & text);
    void titleChanged(const QString & title);
    void urlChanged(const QString & url);
    void javaScriptWindowObjectCleared();
  private:
    QGraphicsWebView * m_view;
    QWebPage * m_page;
    QTimeLine * m_timeLine;
    bool m_zoomIn;
  };

} // end of namespace GVA

#endif // __GINEBRA_APPCONTENTVIEW_H__
