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


#include <QPainter>
#include <QPaintEvent>
#include "TnEngineView.h"
#include "TnEngineHandler.h"

const int ScrollingDelta = 60;

namespace WRT {

TnEngineView::TnEngineView(QWidget* parent, TnEngineHandler* handler)
: QWidget(parent), m_TnEngineHandler(handler)
{
    //connect(this, SIGNAL(closeViewRequested()), WebController::webController()->wrtController(), SLOT(closeViewRequested()));
    //connect(this, SIGNAL(cancelViewRequested()), WebController::webController()->wrtController(), SLOT(cancelViewRequested()));
}

void TnEngineView::initview()
{
    setGeometry(parentWidget()->rect());
    m_TnEngineHandler->setFullScreenMode(true);
    m_TnEngineHandler->setContainerRect(QRect(QPoint(0, 0), rect().size()));
    m_TnEngineHandler->setVisible(true);
    // ensure that system cursor is an arrow, not a random icon.
    // This is not an issue if the platform does not have a system cursor
    
    // FIXME: Cursor undef for symbian?
#if !defined(Q_OS_SYMBIAN)
    setCursor(Qt::ArrowCursor);
#endif
    setFocusPolicy(Qt::WheelFocus);
    setFocus(Qt::OtherFocusReason);
}


TnEngineView* TnEngineView::initiWithParentAndTnEngineHandler(QWidget* parent, TnEngineHandler* handler)
{
    TnEngineView* that = new TnEngineView(parent, handler);
    that->initview();
    return that;
}

TnEngineView::~TnEngineView()
{
    //m_TnEngineHandler->setVisible(false);
    //m_TnEngineHandler->setFullScreenMode(false);
}

void TnEngineView::paintEvent(QPaintEvent* e)
{
    TnEngineView* that = const_cast<TnEngineView*>(this);
    QPainter painter(that);
    m_TnEngineHandler->setVisible(true);
    m_TnEngineHandler->draw(painter, e->rect());
}

void TnEngineView::keyPressEvent(QKeyEvent* ev)
{
    if (    ev->key() == Qt::Key_Enter
         || ev->key() == Qt::Key_Return
         || ev->key() == Qt::Key_Select ) {
        emit ok();
        return;
    }
    if (ev->key() == Qt::Key_Escape) {
        emit cancel();
        return;
    }
    if (ev->key() == Qt::Key_Up || ev->key() == Qt::Key_Down || ev->key() == Qt::Key_Left || ev->key() == Qt::Key_Right) {
        int x = ScrollingDelta / 2, y = ScrollingDelta;
        switch (ev->key()) {
            case Qt::Key_Down:
                x = 0;
                break;
            case Qt::Key_Up:
                x = 0;
                y *= -1;
                break;
            case Qt::Key_Left:
                x *= -1;
                y = 0;
                break;
            case Qt::Key_Right:
                y = 0;
                break;
            }
        emit scrollBy(x, y);
    }
}

void TnEngineView::resizeEvent(QResizeEvent* event)
{
    m_TnEngineHandler->setContainerRect(QRect(QPoint(0, 0), rect().size()));
}

void TnEngineView::mouseMoveEvent(QMouseEvent* e)
{
    QRect indicatorRect = m_TnEngineHandler->indicatorRect();
    int zoomLevel = m_TnEngineHandler->zoomOutLevel();
    int xDelta = ((e->x() - indicatorRect.x() - indicatorRect.width() / 2) * zoomLevel ) / 100;
    int yDelta = ((e->y() - indicatorRect.y() - indicatorRect.height() / 2) * zoomLevel ) / 100;
    emit scrollBy(xDelta, yDelta);
    m_currPoint.setX(e->x());
    m_currPoint.setY(e->y());
}

void TnEngineView::mousePressEvent(QMouseEvent* e)
{
    m_currPoint.setX(e->x());
    m_currPoint.setY(e->y());
}

void TnEngineView::mouseReleaseEvent(QMouseEvent* e)
{
    m_currPoint.setX(e->x());
    m_currPoint.setY(e->y());
    emit ok();
}

}

