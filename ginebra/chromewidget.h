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


#ifndef _GINEBRA_CHROME_WIDGET_H_
#define _GINEBRA_CHROME_WIDGET_H_

#include <QtGui>
#include <QMap>
#include <QPainterPath>
//#include "chromerenderer.h"

//#define G_QUICKSTART

class QWebPage;
class ChromeSnippet;
class QNetworkAccessManager;
class QWebView;
class QWebFrame;
class QNetworkReply;
class ChromeView;
class QGraphicsScene;
class QTimeLine;

/**
 * \brief Manages chrome snippets.
 * 
 * The ChromeWidget class manages the snippets that comprise the "chrome" of 
 * the browser and the underlying web page that renders the snippets. 
 * 
 * \sa ChromeSnippet
 */
class ChromeWidget : public QObject
{
    Q_OBJECT

typedef enum {
  minimized,
  maximized
} Visibility;

public:
    friend class ChromeRenderer;
    ChromeWidget(ChromeView *parentChromeView, QGraphicsItem *parent, const QString &jsName);
    ~ChromeWidget();
    void setChromeUrl(QString url);
    QString chromeUrl() const { return m_chromeUrl; }
    QWebPage * chromePage() { return m_chromePage; }
    void setContentPage(QWebPage * p);
#ifdef Q_OS_SYMBIAN
    QPixmap * buffer();
#else
    QImage* buffer();
#endif
    QPainter* painter();
    void setGeometry(const QRect &rect);
    void setGeometry(int x, int y, int w, int h) { setGeometry(QRect(x, y, w, h)); }
    ChromeSnippet *getSnippet(const QString &docElementId, QGraphicsItem *parent = 0);
    void getInitialChrome();

    void show(const QString& id, int x, int y);
    void show(const QString& id);
    void hide(const QString& id);
    void setLocation(const QString& id, int x, int y);
    void setAnchor(const QString& id, const QString& anchor);
    void toggleVisibility(const QString& id);
    void toggleAttention(const QString& id);
    void setVisibilityAnimator(const QString& elementId, const QString & animatorName);
    void setAttentionAnimator(const QString& elementId, const QString & animatorName);
    QString getDisplayMode();
    
    // Returns the javascript object that represents this object.
	QObject *jsObject();

    //returns the parent chrome view
    ChromeView* getChromeView() { return m_parentChromeView; }

signals:
    void delegateLink(const QUrl& url);
    void loadStarted();
    void loadComplete();
    void dragStarted();
    void dragFinished();
    void viewPortResize(QRect);
   
    
public slots:
    void updateViewPort();
    void setViewPort(QRect viewPort);
    void repaintRequested(QRect dirtyRect);
    
protected slots:
    void frameCreated(QWebFrame* frame);
    void loadFinished(bool ok = true);
    void onLoadStarted();
    void networkRequestFinished(QNetworkReply *);
    void debugAlert(const QString &msg);
public:
    QSize getDocElementSize(const QString &id);
    QRect getDocElementRect(const QString &id);
    void updateChildGeometries();
    void dump();
protected:
    QVariant getDocElement(const QString &id);
    QString getDocElementAttribute(const QString &id, const QString &attribute);
    QVariant getDocIdsByName(const QString &name);
    QVariant getDocIdsByClassName(const QString &name);
    QVariant getChildIdsByClassName(const QString &parentId, const QString &name);
    void resizeBuffer();
    void updateOwnerAreas();
    bool eventFilter(QObject *object, QEvent *event);
    void paintDirtyRegion();

    // Get the snippet that contains the point pos.
    ChromeSnippet *getSnippet(QPoint pos) const;

    //Reconstruct snippets from the cache
    bool isCached(QString url);
    void cacheBuffer();
    void restoreBuffer();

protected:
    //This is the web page that renders the chrome
    QWebPage *m_chromePage;

private:
    QGraphicsItem *m_parentItem;
    ChromeView *m_parentChromeView;
    QMap<QString, ChromeSnippet *> m_snippetMap;
    QString m_chromeUrl;
    Visibility m_state;
#ifdef Q_OS_SYMBIAN
    QPixmap *m_buffer;
#else
    QImage *m_buffer;
#endif
    QPainter *m_painter;
    QRegion m_dirtyRegion;
    QTimer *m_dirtyTimer;
    class ChromeWidgetJSObject *m_jsObject;  // owned
#ifdef G_QUICKSTART
    bool m_bufferCached;
    QString m_bufferFile;
    ChromeSnippet* m_fakeTopSnippet;
    ChromeSnippet* m_fakeBottomSnippet;
#endif
};
#endif
