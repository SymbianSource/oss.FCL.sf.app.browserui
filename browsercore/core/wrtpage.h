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


#ifndef __WRTPAGE_H__
#define __WRTPAGE_H__

#include <QWidget>
#include "qwebpage.h"

namespace WRT {

#ifdef QT_MOBILITY_BEARER_MANAGEMENT
class WebNetworkConnectionManager;
#endif // QT_MOBILITY_BEARER_MANAGEMENT

class WrtPage : public QWebPage
{
    Q_OBJECT
public:
    WrtPage(QObject *parent=0);
    virtual ~WrtPage();

    void setPageGroupName(const QString&);

public:
    virtual QWebPage* createWindow(QWebPage::WebWindowType);
    virtual void javaScriptConsoleMessage(const QString& messageSource, const QString& messageLevel, const QString& message, int lineNumber, const QString& sourceID);
#ifdef QT_MOBILITY_BEARER_MANAGEMENT
    void setNetworkConnectionManager(WebNetworkConnectionManager *manager);
#endif // QT_MOBILITY_BEARER_MANAGEMENT

Q_SIGNALS:
    void createWindow(WrtPage* page);
    void jsConsoleMessage(const QString& messageSource, const QString& messageLevel, const QString& message, int lineNumber, const QString& sourceID);


public slots:
    //TODO always let js run for now. This allows sunspider testsuite to finish. But it will break the infinite loop test case.
    //TODO change to true after performance is improved and each sunspider test can finish in 10s.
    bool shouldInterruptJavaScript() {
        return false;
    }

private:
#ifdef QT_MOBILITY_BEARER_MANAGEMENT
    WebNetworkConnectionManager *m_ConnectionManager;
#endif // QT_MOBILITY_BEARER_MANAGEMENT
};

}
#endif
