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


#ifndef WEBNAVIGATION_H
#define WEBNAVIGATION_H

#include "brtglobal.h"

#include <qobject.h>
#include "wrtBrowserDefs.h"

class QWebPage;

namespace WRT {

class WebTouchNavigation;
class WebCursorNavigation;
class WebDirectionalNavigation;
class WebHtmlTabIndexedNavigation;

class WRT_BROWSER_EXPORT WebNavigation : public QObject
{
Q_OBJECT
public:
    WebNavigation(QWebPage* webPage,QObject* view);
    virtual ~WebNavigation();
    void setPage( QWebPage * page);

public slots:
    void setNavigationMode();

signals:
        void longPressEvent();// mouse long press signal   
        void focusElementChanged(wrtBrowserDefs::BrowserElementType &);
        void pageScrollPositionZero();

protected:
    WebTouchNavigation* m_webTouchNavigation;
    WebCursorNavigation* m_webCursorNavigation;
    WebDirectionalNavigation* m_webDirectionalNavigation;
    WebHtmlTabIndexedNavigation* m_webHtmlTabIndexNavigation;
private:
    QWebPage* m_webPage;
    QObject* m_view;
};

};

#endif
