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


#include "qwidget.h"
#include "qcursor.h"
#include "qwebpage.h"
#include "qwebview.h"
#include "qwebframe.h"
#include "qwebelement.h"
#include "webcursornavigation.h"
#include "webcontentview.h"

namespace WRT {

const int KPageViewScrollRange = 60;
const int KNormalScrollRange = 40;
const int KFlipAdjust = 10;
const int KMiddleStep = 5;
const int KFullStep = 14;
const int KMinimumParagraphHeight = 30;
const int KHitTestDistance = 100;

/*!
    \class WebCursorNavigation
    \since cwrt 1.0
    \brief cwrt navigation.

    \sa WebNavigation, WebTouchNavigation, WebDirectionalNavigation, WebHtmlTabIndexedNavigation
*/
WebCursorNavigation::WebCursorNavigation(QWebPage* webPage,QObject* view)
  : m_webPage(webPage)
  , m_view(view)
  , m_cursorPosition(0,0)
  , m_flipcounter(1)
  , m_direction(0)
  , m_lastdirection(0)
{
    install();
}

/*!
*/
WebCursorNavigation::~WebCursorNavigation()
{
    uninstall();
}

void WebCursorNavigation::install()
{
    m_view->installEventFilter(this);
    connect(&m_scrollTimer, SIGNAL(timeout()), this, SLOT(scrollTimeout()));
    connect(&m_keypressTimer, SIGNAL(timeout()), this, SLOT(keypressTimeout()));
    connect(m_webPage, SIGNAL(linkHovered(const QString&, const QString&, const QString &)),
            this, SLOT(showLinkHover(const QString&, const QString&)));
}

void WebCursorNavigation::uninstall()
{
    disconnect(&m_scrollTimer, SIGNAL(timeout()), this, SLOT(scrollTimeout()));
    disconnect(&m_keypressTimer, SIGNAL(timeout()), this, SLOT(keypressTimeout()));
    disconnect(m_webPage, SIGNAL(linkHovered(const QString&, const QString&, const QString &)),
                this, SLOT(showLinkHover(const QString&, const QString&)));
    m_view->removeEventFilter(this);
}

bool WebCursorNavigation::eventFilter(QObject *object, QEvent *event)
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
    Timeout for long keypress.
    \sa scrollTimeout()
*/
void WebCursorNavigation::keypressTimeout()
{
    if (!m_scrollTimer.isActive())
      scrollTimeout(); 
}

/*!
    Timeout for scroller. Scrolls the page on a timer every 50 miliseconds
    \sa keypressTimeout()
*/
void WebCursorNavigation::scrollTimeout()
{
    if (!scroll(m_direction))
        moveCursor(m_direction);

    if (!m_scrollTimer.isActive())
        m_scrollTimer.start(50);
}

/*!
    If the key is directional, starts the keyPress timer. The cursor navigation is processed on keyReleaseEvent.
    If the key is <Select> or <Return>, eat the event; we'll do that action once on keyRelease.
    Otherwise, send the keyPress event onwards to QWebPage.
*/
void WebCursorNavigation::keyPressEvent(QKeyEvent* ev)
{
    //stop fast scrolling timers
    m_keypressTimer.stop();
    m_scrollTimer.stop();

    if (ev->key() == Qt::Key_Up || 
        ev->key() == Qt::Key_Down || 
        ev->key() == Qt::Key_Left || 
        ev->key() == Qt::Key_Right ) {

        if (!m_keypressTimer.isActive())
            m_keypressTimer.start(300);

        m_direction = ev->key();
    }

    if (ev->key() == Qt::Key_Return
        || ev->key() == Qt::Key_Enter
        || ev->key() == Qt::Key_Select) {
            Qt::KeyboardModifier modifier = Qt::NoModifier;
            QWebFrame* webFrame = m_webPage->frameAt(m_cursorPosition);
            webFrame = (webFrame) ? webFrame : m_webPage->currentFrame();
            QWebHitTestResult htr = webFrame->hitTestContent(m_cursorPosition);
            if (htr.element().tagName().toLower().compare("select")==0  && htr.element().hasAttribute("multiple"))
                modifier = Qt::ControlModifier;

            QMouseEvent evpress(QEvent::MouseButtonPress, m_cursorPosition, Qt::LeftButton, Qt::NoButton, modifier);
            m_webPage->event(&evpress);
    }

}

/*!
    If the key is directional the cursor navigation is processed.
    If the key is a <Select> or <Return>, send a left button mouse press and release to QWebPage.
    Otherwise just send the keyRelease event onwards to QWebPage.
*/
void WebCursorNavigation::keyReleaseEvent(QKeyEvent* ev)
{
    //stop fast scrolling timers
    m_keypressTimer.stop();
    m_scrollTimer.stop();

    if (ev->key() == Qt::Key_Up
        || ev->key() == Qt::Key_Down
        || ev->key() == Qt::Key_Left
        || ev->key() == Qt::Key_Right ) {

        if (!scroll(ev->key())) {
            moveCursor(ev->key());
            QMouseEvent evmm(QEvent::MouseMove, m_cursorPosition, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
            m_webPage->event(&evmm);
        }
    }

    if (ev->key() == Qt::Key_Return
        || ev->key() == Qt::Key_Enter
        || ev->key() == Qt::Key_Select) {
            Qt::KeyboardModifier modifier = Qt::NoModifier;
            QWebFrame* webFrame = m_webPage->frameAt(m_cursorPosition);
            webFrame = (webFrame) ? webFrame : m_webPage->currentFrame();
            QWebHitTestResult htr = webFrame->hitTestContent(m_cursorPosition);
            if (htr.element().tagName().toLower().compare("select")==0  && htr.element().hasAttribute("multiple"))
                modifier = Qt::ControlModifier;

            QMouseEvent evrel(QEvent::MouseButtonRelease, m_cursorPosition, Qt::LeftButton, Qt::NoButton, modifier);
            m_webPage->event(&evrel);
    }
}

/*!
    Returns true if the cursor is over a editable area
*/
bool WebCursorNavigation::isContentEditable()
{
    QWebFrame* webFrame = m_webPage->frameAt(m_cursorPosition);
    webFrame = (webFrame) ? webFrame : m_webPage->currentFrame();
    QWebHitTestResult htr = webFrame->hitTestContent(m_cursorPosition);
    return htr.isContentEditable();
}

/*!
    Moves the cursor a fixed interval in the given direction
*/
void WebCursorNavigation::moveCursor(int direction)
{
    QRect rect(0, 0,
               m_webPage->viewportSize().width(),
               m_webPage->viewportSize().height());

    switch (direction) {
        case Qt::Key_Left: {
            m_flipcounter = (m_lastdirection == Qt::Key_Right) ? ++m_flipcounter : 1;
            int dx = m_cursorPosition.x() - (KFullStep / m_flipcounter);
            m_cursorPosition.setX((dx > 0) ? dx : 0);
        }
        break;
        case Qt::Key_Right: {
            m_flipcounter = (m_lastdirection == Qt::Key_Left) ? ++m_flipcounter : 1;
            int dx = m_cursorPosition.x() + (KFullStep / m_flipcounter);
            m_cursorPosition.setX((dx < rect.width() - KFullStep) ? dx : rect.width() - KFullStep);
        }
        break;
        case Qt::Key_Up: {
            m_flipcounter = (m_lastdirection == Qt::Key_Down) ? ++m_flipcounter : 1;
            int dy = m_cursorPosition.y() - (KFullStep / m_flipcounter);
            m_cursorPosition.setY((dy > 0) ? dy : 0);
        }
        break;
        case Qt::Key_Down : {
            m_flipcounter = (m_lastdirection == Qt::Key_Up) ? ++m_flipcounter : 1;
            int dy = m_cursorPosition.y() + (KFullStep / m_flipcounter);
            m_cursorPosition.setY((dy < rect.height() - KFullStep) ? dy : rect.height() - KFullStep);
        }
        break;
    }
    m_lastdirection = direction;
    QCursor::setPos(static_cast<WebContentWidget*>(m_view)->mapToGlobal(m_cursorPosition).toPoint());
}

/*!
    Scrolls QWebFrame a fixed interval in a given direction.
*/
bool WebCursorNavigation::scroll(int direction)
{
    QWebFrame* webFrame = m_webPage->frameAt(m_cursorPosition);
    webFrame = (webFrame) ? webFrame : m_webPage->currentFrame();

    QPoint scrollPosition = webFrame->scrollPosition();
    QRect rect(QPoint(0,0),m_webPage->viewportSize());
    int xmargin = 2 * rect.width() / 5;
    int ymargin = 2 * rect.height() / 5;

    switch (direction) {
        case Qt::Key_Left :
            if (m_cursorPosition.x() < rect.x() + xmargin)
                webFrame->scroll(-KNormalScrollRange, 0);
        break;
        case Qt::Key_Right:
            if (m_cursorPosition.x() > (rect.right() - xmargin))
                webFrame->scroll(KNormalScrollRange, 0);
        break;
        case Qt::Key_Up:
            if (m_cursorPosition.y() < rect.y() + ymargin)
                webFrame->scroll(0, -KNormalScrollRange);
        break;
        case Qt::Key_Down:
            if (m_cursorPosition.y() > (rect.bottom() - ymargin))
                webFrame->scroll(0, KNormalScrollRange);
        break;
    }

    if (scrollPosition.y() == 0 || webFrame->scrollPosition().y() == 0) {
        emit pageScrollPositionZero();
    }
    return scrollPosition != webFrame->scrollPosition();
}


/*!
    Returns the distance to scroll to the nearest edge of a text paragraph.
*/
int WebCursorNavigation::getNearestEdge(int scrollRange, int direction)
{
    QSize size = m_webPage->viewportSize();
    //Identify the number of hit tests needed
    int hitTestCount = size.height() / KHitTestDistance;
    int x = direction == Qt::Key_Right ? scrollRange : 0;
    QPoint pos(x,KHitTestDistance);
    for(int i=0;i<hitTestCount;i++)  {
        QWebHitTestResult htr = m_webPage->mainFrame()->hitTestContent(pos);
        QRect rect = htr.boundingRect();
        QPoint scrollPosition = m_webPage->mainFrame()->scrollPosition();
        int d = direction == Qt::Key_Right ? rect.x() - scrollPosition.x() : pos.x() - rect.x();
        if(d > 0 && scrollRange > d && rect.height() > KMinimumParagraphHeight)
           scrollRange = d;
        pos = QPoint(pos.x(), pos.y() + (i + 1) * KHitTestDistance);
    }

    return scrollRange;
}
}

