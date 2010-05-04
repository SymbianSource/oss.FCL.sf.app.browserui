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


#include "qmath.h"
#include "qpainter.h"
#include "qwebpage.h"
#include "qwebview.h"
#include "qwebframe.h"
#include "qwebelement.h"
#include "webdirectionalnavigation.h"

namespace WRT {

const int KInitialSize = 20;
const int KNormalScrollRange = 40;
const int KFullStep = 14;
const double KMaxDistance = 1000000;

/*!
    \class WebDirectionalNavigation
    \since cwrt 1.0
    \brief cwrt navigation.

    \sa WebNavigation, WebTouchNavigation, WebCursorNavigation, WebHtmlTabIndexedNavigation
*/
WebDirectionalNavigation::WebDirectionalNavigation(QWebPage* webPage,QObject* view) :
    m_webPage(webPage)
,   m_view(view)
{
    install();
}

WebDirectionalNavigation::~WebDirectionalNavigation()
{
    uninstall();
}

void WebDirectionalNavigation::install()
{
    m_view->installEventFilter(this);
    //initiallayoutCompleted();
}

void WebDirectionalNavigation::uninstall()
{
    if (m_view)
        m_view->removeEventFilter(this);
}

void WebDirectionalNavigation::initiallayoutCompleted()
{
    m_webElement = m_webPage->currentFrame()->findFirstElement(QString("a,input,select,textarea,object"));
    if (!m_webElement.isNull()) {
        m_webElement.setFocus();
        m_focusPoint = m_webElement.geometry().topLeft();
        setCurrentFrameScrollPosition(m_focusPoint);
    }
}

double WebDirectionalNavigation::calculateElementDistance(int direction, const QRect& possibleRect) 
{
    // Roughly based on this algorithm http://www.w3.org/TR/WICD/#nav-distance-fkt 
    // Deviates in that the overlap is not calculated. Instead, if there is any overlap
    // selection is restricted to x or y direction only. This helps for differnt size elments
    // all lying on the same plane, but might mess up backtracking the naviation path.

    QRect focusedRect(0,0,1,1);
    if (!m_webElement.isNull())
        focusedRect = QRect(m_webElement.geometry());

    //calculate the next focuspoint
    switch (direction) {
    case Qt::Key_Up:
        if (m_focusPoint.x() < focusedRect.x())
            m_focusPoint.setX(focusedRect.x());
        else if (m_focusPoint.x() > focusedRect.right())
            m_focusPoint.setX(focusedRect.right());
        m_focusPoint.setY(focusedRect.y());
        break;
    case Qt::Key_Down:
        if (m_focusPoint.x() < focusedRect.x())
            m_focusPoint.setX(focusedRect.x());
        else if (m_focusPoint.x() > focusedRect.right())
            m_focusPoint.setX(focusedRect.right());
        m_focusPoint.setY(focusedRect.bottom());
        break;
    case Qt::Key_Right:
        m_focusPoint.setX(focusedRect.right());
        if (m_focusPoint.y() < focusedRect.y())
            m_focusPoint.setY(focusedRect.y());
        else if (m_focusPoint.y() > focusedRect.bottom())
            m_focusPoint.setY(focusedRect.bottom());
        break;
    case Qt::Key_Left:
        m_focusPoint.setX(focusedRect.x());
        if (m_focusPoint.y() < focusedRect.y())
            m_focusPoint.setY(focusedRect.y());
        else if (m_focusPoint.y() > focusedRect.bottom())
            m_focusPoint.setY(focusedRect.bottom());
        break;
    }

    // Make sure the rectangle falls within the search area
    if (direction == Qt::Key_Up && possibleRect.bottom() > m_focusPoint.y()
        || direction == Qt::Key_Down && possibleRect.y() < m_focusPoint.y()
        || direction == Qt::Key_Right && possibleRect.x() < m_focusPoint.x()
        || direction == Qt::Key_Left && possibleRect.right() > m_focusPoint.x())
        return KMaxDistance;

    // The absolute distance (dx or dy) on the navigation axis between the opposing edges of the currently focused
    // element and each of the candidates.
    double distanceX = 0;
    double distanceY = 0;

    if (direction == Qt::Key_Up) {
        // adjust the x distance based on the closest edge
        if (m_focusPoint.x() < possibleRect.x()) 
            distanceX = possibleRect.x() - m_focusPoint.x();
        else if (m_focusPoint.x() > possibleRect.right())
            distanceX = m_focusPoint.x() - possibleRect.right();
       distanceY = m_focusPoint.y() - possibleRect.bottom();
    } else if (direction == Qt::Key_Down) {
        // adjust the x distance based on the closest edge
        if (m_focusPoint.x() < possibleRect.x()) 
            distanceX = possibleRect.x() - m_focusPoint.x();
        else if (m_focusPoint.x() > possibleRect.right())
            distanceX = m_focusPoint.x() - possibleRect.right();
        distanceY = possibleRect.y() - m_focusPoint.y();
    } else if (direction == Qt::Key_Right) {
        distanceX = possibleRect.x() - m_focusPoint.x();
        // adjust the y distance based on the closest edge
        if (m_focusPoint.y() < possibleRect.y())
            distanceY = possibleRect.y() - m_focusPoint.y();
        else if (m_focusPoint.y() > possibleRect.bottom())
            distanceY = m_focusPoint.y() - possibleRect.bottom();
    } else if (direction == Qt::Key_Left) {
        distanceX = m_focusPoint.x() - possibleRect.right();
        // adjust the y distance based on the closest edge
        if (m_focusPoint.y() < possibleRect.y()) 
            distanceY = possibleRect.y() - m_focusPoint.y();
        else if (m_focusPoint.y() > possibleRect.bottom())
            distanceY = m_focusPoint.y() - possibleRect.bottom();
    }

    // The absolute distance on the axis orthogonal to the navigation axis between
    // the opposing edges of currently focused element and each of candidates
    double displacement = 0;

    // The euclidean distance
    double euclideanDist = sqrt(distanceX * distanceX + distanceY * distanceY);

    // Area of the document to search
    QRect searchRect(m_webPage->currentFrame()->scrollPosition(),m_webPage->viewportSize());

    if (direction == Qt::Key_Up || direction == Qt::Key_Down) {
        // if the rectangles are on the same plane set the euclideanDist to zero to favor this navigation
        if ((possibleRect.x() == focusedRect.x())
            || (possibleRect.right() > focusedRect.x() && possibleRect.right() < focusedRect.right())
            || (possibleRect.x() > focusedRect.x() && possibleRect.x() < focusedRect.right())
            || (possibleRect.x() > focusedRect.x() && possibleRect.right() < focusedRect.right())
            || (possibleRect.x() < focusedRect.x() && possibleRect.right() > focusedRect.right())) {
            euclideanDist = 0;
            distanceX = 0;
        } else {
            // displacement is added for being orthogonally far from the current rectangle.
            if (possibleRect.x() > focusedRect.right())
                displacement = possibleRect.x() - focusedRect.right();
            if (possibleRect.right() < focusedRect.x())
                displacement = focusedRect.x() - possibleRect.right();

            // This is a little sketchy, but if the rectangles are all 
            // touching set the displacment to 1 so rects on the same plane win
            if (displacement == 0)
                displacement = 1;

            // if the focus is within the view limit the diagonal search by half the search rectangle
            if (searchRect.intersects(focusedRect) && (displacement > (searchRect.width()/2)))
                return KMaxDistance;
 
        }

    } else if (direction == Qt::Key_Left || direction == Qt::Key_Right) {

        // if the rectangles are on the same plane set the euclideanDist to zero to favor this navigation
        if ((possibleRect.y() == focusedRect.y())
            || (possibleRect.bottom() > focusedRect.y() && possibleRect.bottom() < focusedRect.bottom())
            || (possibleRect.y() > focusedRect.y() && possibleRect.y() < focusedRect.bottom())
            || (possibleRect.y() > focusedRect.y() && possibleRect.bottom() < focusedRect.bottom())
            || (possibleRect.y() < focusedRect.y() && possibleRect.bottom() > focusedRect.bottom())) {
           euclideanDist = 0;
           distanceY = 0; 
        } else {
            // displacement is added for being orthogonally far from the current rectangle.
            if (possibleRect.y() > focusedRect.bottom())
                displacement = possibleRect.y() - focusedRect.bottom();
            if (possibleRect.bottom() < focusedRect.y())
                displacement = focusedRect.y() - possibleRect.bottom();

            // This is a little sketchy, but if the rectangles are all 
            // touching set the displacment to 1 so rects on the same plane win
            if (displacement == 0) 
                displacement = 1;

            //  if the focus is within the view limit the diagonal search by half the search rectangle
            if (searchRect.intersects(focusedRect) && (displacement > (searchRect.height()/2)))
                return KMaxDistance; 
        }
    }

    return euclideanDist + distanceX + distanceY + 2 * (displacement);
}



bool WebDirectionalNavigation::nextElementInDirection(int direction)
{
    double bestDistance = KMaxDistance;
    QWebElement bestElement;
#if QT_VERSION < 0x040600
    QList<QWebElement> elementList = m_webPage->currentFrame()->findAllElements(QString("a,input,select,textarea,object"));
#else
    QList<QWebElement> elementList = m_webPage->currentFrame()->findAllElements(QString("a,input,select,textarea,object")).toList();
#endif
    QList<QWebElement>::iterator it;
    for (it = elementList.begin(); it != elementList.end(); it++) {
        QWebElement el(*it);
        QRect nRect(el.geometry());
        if (nRect.isValid()) {
            if (QRect(m_webPage->currentFrame()->scrollPosition(),m_webPage->viewportSize()).intersects(nRect)) {
                double distance = calculateElementDistance (direction, nRect);
                if (bestDistance > distance) {
                    bestDistance = distance;
                    bestElement = el;
                }
            }
        }
    }

    if (!bestElement.isNull() && bestElement != m_webElement) {
        m_webElement.setStyleProperty("outline", m_webElementStyle);    	  
        m_webElement = bestElement;
        m_webElement.setFocus();
        m_webElementStyle = m_webElement.styleProperty("outline", QWebElement::ComputedStyle);
        m_webElement.setStyleProperty("outline", "3px ridge rgb(140,140,255)");             
        scrollFrame(direction);
        return true;
    }

    scrollFrameOneDirection (direction, KNormalScrollRange);
    return false;
}


bool WebDirectionalNavigation::eventFilter(QObject *object, QEvent *event)
{
    if (object == m_view) {
        switch (event->type()) {
            case QEvent::KeyPress: {
                QKeyEvent* ev = static_cast<QKeyEvent*>(event);
                if (ev->key() == Qt::Key_Up || ev->key() == Qt::Key_Down 
                || ev->key() == Qt::Key_Left || ev->key() == Qt::Key_Right )
                    {
                    return true;
                    }
                if (ev->key() ==  Qt::Key_Select )
                    {
                    QKeyEvent rockerEnterEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
                    m_webPage->event(&rockerEnterEvent); 
                    return true; 
                    }                
            }
            break; 
            case QEvent::KeyRelease: {
                QKeyEvent* ev = static_cast<QKeyEvent*>(event);
                if (ev->key() == Qt::Key_Up || ev->key() == Qt::Key_Down 
                || ev->key() == Qt::Key_Left || ev->key() == Qt::Key_Right ) 
                    {
                    nextElementInDirection(ev->key());    
                    return true;
                    }
                if (ev->key() == Qt::Key_Select )
                    {
                    QKeyEvent rockerEnterEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
                    m_webPage->event(&rockerEnterEvent); 
                    return true; 
                    }
            }
            break;
            default: 
                return false; 
            break; 
        } // end of switch statement 
    }  // end of if statement 
    return false;
}


void WebDirectionalNavigation::scrollFrame(int direction)
{

    int xadjust = m_webPage->viewportSize().width()/5;
    int yadjust = m_webPage->viewportSize().height()/5;

    switch (direction) {
        case Qt::Key_Left: {
            int distanceX =  xadjust - m_webElement.geometry().x();
            if (distanceX > 0)
                scrollCurrentFrame(-distanceX,0);
        }
        break;
        case Qt::Key_Right: {
            int distanceX = m_webElement.geometry().right() - m_webPage->viewportSize().width()-xadjust;
            if (distanceX > 0)
                scrollCurrentFrame(distanceX,0);
        }
        break;
        case Qt::Key_Up: {
            int distanceY = yadjust - m_webElement.geometry().y();
            if (distanceY > 0)
                scrollCurrentFrame(0,-distanceY);
        }
        break;
        case Qt::Key_Down: {
            int distanceY = m_webElement.geometry().bottom() - m_webPage->viewportSize().height()-yadjust;
            if (distanceY > 0)
                scrollCurrentFrame(0,distanceY);
        }
        break;
    }
}

void WebDirectionalNavigation::scrollFrameOneDirection (int direction, int distance)
{
    int dx = 0;
    int dy = 0;

    switch (direction) {
        case Qt::Key_Up: {
            dy = -distance;
        }
        break;
        case Qt::Key_Down: {
            dy = +distance;
        }
        break;
        case Qt::Key_Left: {
            dx = -distance;
        }
        break;
        case Qt::Key_Right: {
            dx = distance;
        }
        break;
        default:
        break;
    }

    scrollCurrentFrame(dx, dy);
}

void WebDirectionalNavigation::scrollCurrentFrame (int dx, int dy)
{
    QPoint scrollPosition = m_webPage->currentFrame()->scrollPosition();
    m_webPage->currentFrame()->scroll(dx, dy);

    /* emit pageScrollPositionZero singal if it's mainFrame scrolling or scroll to top*/
    if (m_webPage->currentFrame() == m_webPage->mainFrame()) {
        if (scrollPosition.y() == 0 || m_webPage->currentFrame()->scrollPosition().y() == 0) {
            emit pageScrollPositionZero();
        }
    }
}

void WebDirectionalNavigation::setCurrentFrameScrollPosition (QPoint& pos)
{
    QPoint scrollPosition = m_webPage->currentFrame()->scrollPosition();
    m_webPage->currentFrame()->setScrollPosition(pos);

    /* emit pageScrollPositionZero singal if it's mainFrame scrolling or scroll to top*/
    if (m_webPage->currentFrame() == m_webPage->mainFrame()) {
        if (scrollPosition.y() == 0 || m_webPage->currentFrame()->scrollPosition().y() == 0) {
            emit pageScrollPositionZero();
        }
    }
}

}

