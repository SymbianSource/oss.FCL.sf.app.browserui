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


#include "webhtmltabbednavigation.h"
#include "qwebpage.h"
#include "qwebframe.h"
#include "qwebelement.h"
#include <QDebug>

namespace WRT {

/*!
    \class WebHtmlTabIndexedNavigation
    \since cwrt 1.0
    \brief cwrt navigation.

    \sa WebNavigation, WebTouchNavigation, WebCursorNavigation, WebDirectionalNavigation
*/
WebHtmlTabIndexedNavigation::WebHtmlTabIndexedNavigation(QWebPage* webPage,QObject* view) : m_webPage(webPage)
, m_view(view)
, m_radioKeyPressed(Qt::Key_Down)
, m_radioGroupFocused(false)
{
    install();
}

/*!
*/
WebHtmlTabIndexedNavigation::~WebHtmlTabIndexedNavigation() 
{
    uninstall();
}

void WebHtmlTabIndexedNavigation::install()
{
    m_view->installEventFilter(this);
}

void WebHtmlTabIndexedNavigation::uninstall()
{
    m_view->removeEventFilter(this);
}

bool WebHtmlTabIndexedNavigation::eventFilter(QObject *object, QEvent *event)
{
    if (object == m_view) {
        switch (event->type()) {
            case QEvent::KeyPress: {
                QKeyEvent* ev = static_cast<QKeyEvent*>(event);
                keyPressEvent(ev);
                return ev->isAccepted();
            }
            case QEvent::KeyRelease: {
                QKeyEvent* ev = static_cast<QKeyEvent*>(event);
                keyReleaseEvent(ev);
                return ev->isAccepted();
            }
            default:
                break;
        }
    }
    return false;
}

/*!
    If the key is directional up or left sends a tab key press with a shift modifier to QWebPage
    If the key is directional right or down sends a tab key press with out a shift modifer to QWebPage
    Otherwise sends the key press event to QWebPage
*/
void WebHtmlTabIndexedNavigation::keyPressEvent(QKeyEvent* ev)
{
    switch (ev->key()) {
    case Qt::Key_Up:
    case Qt::Key_Left: {
        QKeyEvent tabEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::ShiftModifier);
        m_webPage->event(&tabEvent);
    }
    break;
    case Qt::Key_Down:
    case Qt::Key_Right: {
        QKeyEvent tabEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
        m_webPage->event(&tabEvent);
    }
    break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Select: {
        QWebElement webElement = m_webPage->mainFrame()->findFirstElement(":focus");
        QString attribute = webElement.attribute("type");
        if (attribute.toLower().compare("checkbox")==0) {
            QKeyEvent spaceEvent(QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier);
            m_webPage->event(&spaceEvent);
        } else if (attribute.toLower().compare("radio")==0) {
            m_radioKeyPressed = (m_radioGroupFocused) ? Qt::Key_Down : Qt::Key_Up; 
            QKeyEvent keyevent(QEvent::KeyPress, m_radioKeyPressed, Qt::NoModifier);
            m_webPage->event(&keyevent);
            m_radioGroupFocused = (webElement.document().findFirst(":checked") != webElement) ? m_radioGroupFocused : !m_radioGroupFocused;
        } else if (webElement.tagName().toLower().compare("select")==0) {
            QKeyEvent keyevent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
            m_webPage->event(&keyevent);
        }
    }
    break;
    default:
        if (m_webPage)
            m_webPage->event(ev);
    break;
    };
}

/*!
    If the key is directional up or left sends a tab key release with a shift modifier to QWebPage
    If the key is directional right or down sends a tab key release with out a shift modifer to QWebPage
    Otherwise sends the key press event to QWebPage
*/
void WebHtmlTabIndexedNavigation::keyReleaseEvent(QKeyEvent* ev)
{
    switch (ev->key()) {
    case Qt::Key_Up:
    case Qt::Key_Left: {
        QKeyEvent tabEvent(QEvent::KeyRelease, Qt::Key_Tab, Qt::ShiftModifier);
        m_webPage->event(&tabEvent);
    }
    break;
    case Qt::Key_Down:
    case Qt::Key_Right: {
        QKeyEvent tabEvent(QEvent::KeyRelease, Qt::Key_Tab, Qt::NoModifier);
        m_webPage->event(&tabEvent);
    }
    break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Select: {
        QWebElement webElement = m_webPage->mainFrame()->findFirstElement(":focus");
        QString attribute = webElement.attribute("type");
        if (attribute.toLower().compare("checkbox")==0) {
            QKeyEvent spaceEvent(QEvent::KeyRelease, Qt::Key_Space, Qt::NoModifier);
            m_webPage->event(&spaceEvent);
        } else if (attribute.toLower().compare("radio")==0 || attribute.toLower().compare("select")==0) {
            QKeyEvent keyevent(QEvent::KeyRelease, m_radioKeyPressed, Qt::NoModifier);
            m_webPage->event(&keyevent);
        } else if (webElement.tagName().toLower().compare("select")==0) {
            QKeyEvent keyevent(QEvent::KeyRelease, Qt::Key_Down, Qt::NoModifier);
            m_webPage->event(&keyevent);
        }
    }
    default:
        if (m_webPage)
            m_webPage->event(ev);
    break;
    };
}

}
