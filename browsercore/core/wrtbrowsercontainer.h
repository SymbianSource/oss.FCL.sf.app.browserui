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


#ifndef __WRTBROWSERCONTAINER_H__
#define __WRTBROWSERCONTAINER_H__

#include "brtglobal.h"

#include "qwebpage.h"
#include "wrtpage.h"
#include "wrtbrowsercontainer_p.h"
#include "wrtBrowserDefs.h"
#include "ZoomMetaData.h"


#include <QWidget>
#include <QNetworkProxy>

struct BrowserPageFactory;
struct ZoomMetaData;
class QWebFrame;
class QWebHistoryItem;
class QNetworkReply;
class QAuthenticator;
class QNetworkProxy;
class QNetworkAccessManager;

namespace WRT {

class WrtPage;
class WrtController;
class WrtBrowserContainerPrivate;
class SchemeHandler;
class SecureUIController;
class LoadController;

class WRT_BROWSER_EXPORT WrtBrowserContainer : public WrtPage
{
    Q_OBJECT
public:
   /*!
    * enum for security Level
    */
    enum {
        /**Low security*/
        SecurityLow,
        /**Meduium Security*/
        SecurityMedium,
        /**High Security*/
        SecurityHigh
    };

public:
    static WrtBrowserContainer* createPageWithWidgetParent(QObject* parent=0,WrtBrowserContainer* page=0);
    explicit WrtBrowserContainer(QObject* parent = 0);
    virtual ~WrtBrowserContainer();
    
    QGraphicsWidget* webWidget() const;
    void setWebWidget(QGraphicsWidget* view);
    SchemeHandler* schemeHandler() const;

    int getHistoryCount() const;
    void clearCookies();
    bool clearNetworkCache();
	
    QImage pageThumbnail(qreal scaleX, qreal scaley);
    void setPageZoomFactor(qreal zoom);
    void setPageDirtyZoomFactor(qreal zoom);
//   TODO: Hold on this hookup after zooming and scrolling improvment	
//    void setPageCenterZoomFactor(qreal zoom); 
    qreal pageZoomFactor() const;
    void setCanvasScaleFactor(qreal scaleX, qreal scaleY);

    bool allowOfflineStorage(const QUrl& url);

    QWebPage* createWindow(QWebPage::WebWindowType webWindowType);
    void setPageFactory(BrowserPageFactory* f);
    wrtBrowserDefs::BrowserElementType getElementType();

    QString pageTitle();
    
    int secureState();
    WRT::LoadController * loadController( ) {return d->m_loadController;}

    ZoomMetaData pageZoomMetaData() ;
    void setPageZoomMetaData( ZoomMetaData zoomData );
    
    /* Indicates whether this is a blank window with no page loaded*/
    bool emptyWindow();
    bool restoreSession();
    
Q_SIGNALS:
    void createNewWindow(WrtBrowserContainer* page);

    void pageScrollPositionZero();

    void longPressEvent();
    void secureStateChange(int);

public slots:
    void savePageDataToHistoryItem(QWebFrame*, QWebHistoryItem* item);
    void slotAuthenticationRequired(QNetworkReply *, QAuthenticator *);
    void slotProxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *);
    
private slots:
    void setElementType(wrtBrowserDefs::BrowserElementType& aElType);
    void pageSecureState(int);
    void pageZoomMetaDataChange(QWebFrame*, ZoomMetaData);

private:
    WrtBrowserContainerPrivate* d;
    wrtBrowserDefs::BrowserElementType m_elementType;
};

}
#endif
