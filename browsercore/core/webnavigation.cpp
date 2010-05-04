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


#include "webnavigation.h"
#include "qwebpage.h"
#include "qwebframe.h"
#include "webtouchnavigation.h"
#include "webcursornavigation.h"
#include "webhtmltabbednavigation.h"
#include "webdirectionalnavigation.h"

namespace WRT {

/*!
    \class WebNavigation
    \since cwrt 1.0
    \brief cwrt navigation.

    \sa WebTouchNavigation, WebCursorNavigation, WebDirectionalNavigation, WebHtmlTabIndexedNavigation
*/
WebNavigation::WebNavigation(QWebPage* webPage,QObject* view) : m_webPage(webPage)
, m_view(view)
, m_webTouchNavigation(0)
, m_webCursorNavigation(0)
, m_webDirectionalNavigation(0)
, m_webHtmlTabIndexNavigation(0)
{
    m_webPage->settings()->setUserStyleSheetUrl(QUrl("data:text/css;charset=utf-8;base64," \
                        + QByteArray("textarea:hover, textarea:focus {                " \
                        "    outline: auto;                              " \
                        "    outline-style: solid;                       " \
                        "    outline-color: rgb(140,140,255);            " \
                        "    outline-width: 2px;                         " \
                        "}                                               " \
                        "select:hover,                                   " \
                        "select[multiple]:hover,                         " \
                        "select:focus,                                   " \
                        "select[multiple]:focus {                        " \
                        "    outline: auto;                              " \
                        "    outline-style: solid;                       " \
                        "    outline-color: rgb(140,140,255);            " \
                        "    outline-width: 2px;                         " \
                        "}                                               " \
                        "input:hover, input:focus {                      " \
                        "    outline: auto;                              " \
                        "    outline-style: solid;                       " \
                        "    outline-color: rgb(140,140,255);            " \
                        "    outline-width: 2px;                         " \
                        "}                                               " \
                        "object:hover, object:focus {                    " \
                        "    outline: auto;                              " \
                        "    outline-style: solid;                       " \
                        "    outline-color: rgb(140,140,255);            " \
                        "    outline-width: 2px;                         " \
                        "}                                               " \
                        "input:disabled, select:disabled, textarea:disabled { " \
                        "    outline: none;                              " \
                        "}                                               " \
                    		"a:-webkit-any-link {                            " \
                        "    padding: 1px 2px;                           " \
                        "}                                               " \
                        "a:-webkit-any-link:visited {                    " \
                        "    color: red;                                 " \
                        "}                                               " \
                        "a:-webkit-any-link:active {                     " \
                        "    background-color: lightgray;                " \
                        "    -webkit-border-radius: 3px;                 " \
                        "    text-decoration: none;                      " \
                        "}                                               " \
                        "a:-webkit-any-link:focus {                      " \
                        "    background-color: lightgray;                " \
                        "    -webkit-border-radius: 3px;                 " \
                        "    text-decoration: none;                      " \
                        "} ").toBase64()));

    
    setNavigationMode();
    connect(m_webPage->mainFrame(), SIGNAL(initialLayoutCompleted()), this, SLOT(setNavigationMode()));
    connect(this, SIGNAL(longPressEvent()), m_webPage,SIGNAL(longPressEvent()));
    connect(this,SIGNAL(focusElementChanged(wrtBrowserDefs::BrowserElementType &)),
            m_webPage,SLOT(setElementType(wrtBrowserDefs::BrowserElementType &)));
    connect(this, SIGNAL(pageScrollPositionZero()), m_webPage, SIGNAL(pageScrollPositionZero()));

}

/*!
*/
WebNavigation::~WebNavigation() 
{
    delete m_webTouchNavigation;
    delete m_webCursorNavigation;
    delete m_webDirectionalNavigation;
    delete m_webHtmlTabIndexNavigation;
}

void WebNavigation::setPage(QWebPage * page) {

     m_webPage = page;
     if (m_webTouchNavigation)
         m_webTouchNavigation->setPage(page);
     if (m_webCursorNavigation)
         m_webCursorNavigation->setPage(page);
     if (m_webDirectionalNavigation)
         m_webDirectionalNavigation->setPage(page);
     if (m_webHtmlTabIndexNavigation)
         m_webHtmlTabIndexNavigation->setPage(page);
}

/*!
    Sets the navigation mode
*/
void WebNavigation::setNavigationMode()
{
    if (!m_webPage)
        return;

    if (!m_view)
        return;

    if (m_webTouchNavigation)
        m_webTouchNavigation->uninstall();
    if (m_webCursorNavigation)
        m_webCursorNavigation->uninstall();
    if (m_webDirectionalNavigation)
        m_webDirectionalNavigation->uninstall();
    if (m_webHtmlTabIndexNavigation)
        m_webHtmlTabIndexNavigation->uninstall();

    QWebFrame* webFrame = m_webPage->currentFrame();
    if (webFrame) {
        QMultiMap<QString, QString> map = webFrame->metaData();
        if (map.value("touchnavigation") != "off" && map.value("navigation") != "none") {
            if (!m_webTouchNavigation) {
                m_webTouchNavigation = new WebTouchNavigation(m_webPage,m_view);
                connect( (const QObject*)(m_webTouchNavigation),SIGNAL(longPressEvent()),this,SIGNAL(longPressEvent()));
                connect((const QObject*)(m_webTouchNavigation), SIGNAL(focusElementChanged(wrtBrowserDefs::BrowserElementType &)),
                                        this, SIGNAL(focusElementChanged(wrtBrowserDefs::BrowserElementType &)));
                connect( (const QObject*)(m_webTouchNavigation),SIGNAL(pageScrollPositionZero()),this,SIGNAL(pageScrollPositionZero()));
                
            }
            else 
                m_webTouchNavigation->install();
        }

       // Enable key navigation, browser does not depend on meta data for navigation
        if (!m_webDirectionalNavigation) {
            m_webDirectionalNavigation = new WebDirectionalNavigation(m_webPage,m_view);
            connect( (const QObject*)(m_webDirectionalNavigation),SIGNAL(pageScrollPositionZero()),this,SIGNAL(pageScrollPositionZero()));
        }
        else {
            m_webDirectionalNavigation->install();
        }
    }
}

}
