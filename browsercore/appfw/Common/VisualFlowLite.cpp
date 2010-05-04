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


// INCLUDE FILES
#include "VisualFlowLite.h"

#include <QTimer>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>

#include <QDebug>


// CONSTANTS
const int KVisualFlowViewOffsetX = 2;

//Animation related factors
const int KResizeFactorIncr = 20;
const int KAnimFactorInc = 25;
const int KFastScrollAnimFactorInc = 40;
const int KMaxAnimFactor = 100;
const int KMinAnimFactor = 0;

const int KRepaintTimerCb = 20;
const int KCenterThumbnailWidthPercent = 50;
const int KCenterThumbnailHeightPercent = 90;
const int KSideThumbnailWidthPercent = 25;
const int KSideThumbnailHeightPercent = 70;

#define KCenterImageBorderColor Qt::red
#define KSideImageBorderColor Qt::black


namespace WRT {
const int KScrollTimeout = 200;


VisualFlowLite::VisualFlowLite(QWidget* parent) : FlowInterface(parent),
    m_resizeFactor (KMinAnimFactor),
    m_placeHolderResizeFactor(KMinAnimFactor),
    m_centerPageIndex(-1),
    m_scalingAllowed(false)
{
    m_paintState = paintStateNormal;
    m_scrollTimer = NULL;
}

void VisualFlowLite::init()
{
    m_repaintTimer = new QTimer;
    connect(m_repaintTimer, SIGNAL(timeout()), this, SLOT(calcRepaintRect()));
    resize(parentWidget()->rect().size());

    m_repaintTimer->start(KRepaintTimerCb);
    // FIXME: Cursor undef for symbian?
#if !defined(Q_OS_SYMBIAN)
    setCursor(Qt::ArrowCursor);
#endif
    setFocusPolicy(Qt::WheelFocus);
    setFocus(Qt::OtherFocusReason);
}

void VisualFlowLite::addSlide(const QImage& image)
{
    m_imageList.append(new QImage(image));
}

void VisualFlowLite::clear()
{
    if (m_repaintTimer->isActive()) {
        m_repaintTimer->stop();
    }
    while (!m_imageList.isEmpty())
        delete m_imageList.takeFirst();
    if (m_scrollTimer) {
        m_scrollTimer->stop();
    }

    m_paintState = paintStateNormal;
    m_direction = 0;
    m_repaintTimer->start(KRepaintTimerCb);
}

int VisualFlowLite::slideCount() const
{
    return m_imageList.count();
}

QImage VisualFlowLite::slide(int index) const
{
  const QImage* i = 0;
  if((index >= 0) && (index < m_imageList.count()))
    i = m_imageList.at(index);
  return i ? QImage(*i) : QImage();
}

void VisualFlowLite::setCenterIndex(int i)
{
    setCurrentIndex(i);
}

int VisualFlowLite::centerIndex() const
{
    return m_centerPageIndex;
}

bool VisualFlowLite::slideAnimationOngoing() const
{
    return m_repaintTimer->isActive();
}

void  VisualFlowLite::setCenterThumbnail(int i)
{
    if(m_centerPageIndex + 1 == i) {
        showNext();
    }
    else if(m_centerPageIndex - 1 == i) {
        showPrevious();
    }
    else {
        setCurrentIndex(i);
        calculateLayout();
        update();
    }

}

void VisualFlowLite::setCurrentIndex(int i)
{
    m_currentIndex = i;
    m_centerPageIndex = i;
}

VisualFlowLite::~VisualFlowLite()
{
    if (m_repaintTimer->isActive()) {
        m_repaintTimer->stop();
    }
    delete m_repaintTimer;
    while (!m_imageList.isEmpty())
        delete m_imageList.takeFirst();
    if (m_scrollTimer) {
        m_scrollTimer->stop();
        delete m_scrollTimer;
    }
}

bool VisualFlowLite::calcRepaintRect()
{
    m_repaintTimer->stop();
    disconnect(m_repaintTimer, SIGNAL(timeout()), this, SLOT(calcRepaintRect()));
    connect(m_repaintTimer, SIGNAL(timeout()), this, SLOT(animatePlaceHolderPosition()));
    if (m_resizeFactor == KMaxAnimFactor) {
        m_repaintRect.setRect(rect().x(), rect().y(), rect().width(), rect().height());
        update();
        return false;
    }
    m_repaintRect.setRect(rect().x(), rect().y(), rect().width() * m_resizeFactor/100, rect().height() * m_resizeFactor/100);
    m_resizeFactor += KResizeFactorIncr;
    update();
    return true;
}

void VisualFlowLite::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_Left:
            showPrevious();
            break;
        case Qt::Key_Right:
            showNext();
            break;
        case Qt::Key_Escape:
            emit cancel();
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Select:
            emit ok(m_currentIndex);
            break;
    }
}

void VisualFlowLite::mousePressEvent(QMouseEvent* event)
{
    m_lastMoveEventPos = event->pos();
    m_scrollTimer = new QTimer;
    connect(m_scrollTimer, SIGNAL(timeout()), this, SLOT(scroll()));
    m_scrollTimer->start(KScrollTimeout);
    scroll();
}

void VisualFlowLite::mouseMoveEvent(QMouseEvent* event)
{
    m_lastMoveEventPos = event->pos();
}

void VisualFlowLite::mouseReleaseEvent(QMouseEvent* event)
{
    m_scrollTimer->stop();
    delete m_scrollTimer;
    m_scrollTimer = NULL;
	if (slideAnimationOngoing()) {
//		qDebug() << "vfl:mouseReleaseEvent slideanimation running, ignoring click";
		return;
	}
    if (event->x() > m_centerPlaceHolderRect.left() && event->x() < m_centerPlaceHolderRect.right()) {
    	emit ok(centerIndex());
    }
}

void VisualFlowLite::scroll()
{
    if (m_lastMoveEventPos.x() < m_leftPlaceHolderRect.right())
        showPrevious();
    else if (m_lastMoveEventPos.x() > m_rightPlaceHolderRect.left())
        showNext();
}

void VisualFlowLite::paintEvent(QPaintEvent* event)
{
    if (m_paintState == paintStateNormal) {
        paintNormalState(event);
    }
    else if (m_paintState == paintStateInTransition) {
        paintInTransition(event);
    }
}

void VisualFlowLite::paintNormalState(QPaintEvent* event)
{
    int imageCount = m_imageList.count();
    const QImage* centerEntry = (m_centerPageIndex >= 0)&&(imageCount>0) ? (m_imageList.at(m_centerPageIndex)) : NULL;    
    const QImage* leftEntry = (m_centerPageIndex - 1 >= 0)&&(imageCount>0) ? (m_imageList.at(m_centerPageIndex - 1)) : NULL;
    const QImage* rightEntry = (m_centerPageIndex + 1 < imageCount) ? (m_imageList.at(m_centerPageIndex + 1)) : NULL;

    QPainter painter(this);
    painter.save();
    painter.setBrush(Qt::white);
    painter.drawRect(rect());
    painter.setBrush(Qt::NoBrush);
    if (leftEntry) {
        //painter.drawImage(m_leftPlaceHolderRect.topLeft(), *leftEntry, QRect(QPoint(0, 0), m_leftPlaceHolderRect.size()));
        paintImage(&painter, m_leftPlaceHolderRect, *leftEntry);

        painter.setPen(KSideImageBorderColor);
        painter.drawRect(m_leftPlaceHolderRect);
    }
    if (rightEntry) {
        //painter.drawImage(m_rightPlaceHolderRect.topLeft(), *rightEntry, QRect(QPoint(0, 0), m_rightPlaceHolderRect.size()));
        paintImage(&painter, m_rightPlaceHolderRect, *rightEntry);

        painter.setPen(KSideImageBorderColor);
        painter.drawRect(m_rightPlaceHolderRect);
    }
    if (centerEntry ) {
        //painter.drawImage(m_centerPlaceHolderRect.topLeft(), *centerEntry, QRect(QPoint(0, 0), m_centerPlaceHolderRect.size()));
        paintImage(&painter, m_centerPlaceHolderRect, *centerEntry);
    }
    painter.setPen(KCenterImageBorderColor);
    painter.drawRect(m_centerPlaceHolderRect);
    painter.restore();
}

void VisualFlowLite::resizeEvent(QResizeEvent* event)
{
    calculateLayout();
    update();
}

void VisualFlowLite::calculateLayout()
{
    QSize screenSize = rect().size();
    //QSize screenSize = sizeHint();
    
    QSize viewSize((screenSize.width() - 2 * KVisualFlowViewOffsetX), screenSize.height());
    int centerImageWidth(viewSize.width() * KCenterThumbnailWidthPercent / 100);
    int centerImageHeight(viewSize.height() * KCenterThumbnailHeightPercent/100);
    QSize centerImageSize(centerImageWidth, centerImageHeight);

    int sideImageWidth(viewSize.width() * KSideThumbnailWidthPercent/100);
    int sideImageHeight(viewSize.height() * KSideThumbnailHeightPercent/100);
    QSize sideImageSize(sideImageWidth, sideImageHeight);

    int centerImageX(KVisualFlowViewOffsetX + sideImageWidth);
    int centerImageY(viewSize.height() * (100 - KCenterThumbnailHeightPercent) / (2 * 100));
    QPoint centerImageOrigin(centerImageX, centerImageY);

    m_centerPlaceHolderRect.setRect(centerImageOrigin.x(), centerImageOrigin.y(), centerImageSize.width(), centerImageSize.height());
    int sideImageY(viewSize.height() * (100 - KSideThumbnailHeightPercent) / (2 * 100));
    QPoint leftThumbnailOrigin(KVisualFlowViewOffsetX, sideImageY );
    QPoint rightThumbnailOrigin((int)(KVisualFlowViewOffsetX + sideImageWidth + centerImageWidth), sideImageY );

    m_leftPlaceHolderRect.setRect(leftThumbnailOrigin.x(), leftThumbnailOrigin.y(), sideImageSize.width(), sideImageSize.height());
    m_rightPlaceHolderRect.setRect(rightThumbnailOrigin.x(), rightThumbnailOrigin.y(), sideImageSize.width(), sideImageSize.height());
}

bool VisualFlowLite::animatePlaceHolderPosition()
{
    m_repaintTimer->stop();
    if (m_placeHolderResizeFactor >= KMaxAnimFactor) {
        //Restore old values and end AO
        if (m_direction == 1) {
            m_centerPageIndex++;
        }
        if (m_direction == -1) {
            m_centerPageIndex--;
        }
        m_placeHolderResizeFactor = 0;
        m_direction = 0;
        m_paintState = paintStateNormal;
        update();
        emit centerIndexChanged(m_centerPageIndex);
        return false;
    }
    update();
    return true;
}

void VisualFlowLite::paintInTransition(QPaintEvent* event)
{
    QRect rects[4];
    int dw = 0;
    int dh = 0;
    int dx = 0;
    //left movement
    if (m_direction == -1) {
        QPoint pt(m_leftPlaceHolderRect.x(), m_leftPlaceHolderRect.y() + m_leftPlaceHolderRect.height() / 2);
        dw = m_leftPlaceHolderRect.width() * m_placeHolderResizeFactor / 200;
        dh = m_leftPlaceHolderRect.height() * m_placeHolderResizeFactor / 200;
        rects[0].setRect(pt.x(), pt.y() - dh, 2 * dw, 2 * dh);

        dx = (m_centerPlaceHolderRect.x() - m_leftPlaceHolderRect.x()) * m_placeHolderResizeFactor / 100;
        dw = (m_centerPlaceHolderRect.width() - m_leftPlaceHolderRect.width()) * m_placeHolderResizeFactor / 200;
        dh = (m_centerPlaceHolderRect.height() - m_leftPlaceHolderRect.height()) * m_placeHolderResizeFactor / 200;
        rects[1].setRect(m_leftPlaceHolderRect.x() + dx, m_leftPlaceHolderRect.y() - dh, m_leftPlaceHolderRect.width() + 2 * dw, m_leftPlaceHolderRect.height() + 2 * dh);

        dx = (m_rightPlaceHolderRect.x() - m_centerPlaceHolderRect.x()) * m_placeHolderResizeFactor / 100;
        dw = (m_centerPlaceHolderRect.width() - m_rightPlaceHolderRect.width()) * m_placeHolderResizeFactor / 200;
        dh = (m_centerPlaceHolderRect.height() - m_rightPlaceHolderRect.height()) * m_placeHolderResizeFactor / 200;
        rects[2].setRect(m_centerPlaceHolderRect.x() + dx, m_centerPlaceHolderRect.y() + dh, m_centerPlaceHolderRect.width() - 2 * dw, m_centerPlaceHolderRect.height() - 2 * dh);

        dw = (m_rightPlaceHolderRect.width()) * m_placeHolderResizeFactor / 200;
        dh = (m_rightPlaceHolderRect.height()) * m_placeHolderResizeFactor / 200;
        rects[3].setRect(m_rightPlaceHolderRect.x() + 3 * dw, m_rightPlaceHolderRect.y() + dh, m_rightPlaceHolderRect.width() - 2 * dw, m_rightPlaceHolderRect.height() - 2 * dh);

        const QImage* item = 0;
        QPainter painter(this);
        painter.setPen(Qt::NoPen);
        QBrush whiteBrush(Qt::white, Qt::SolidPattern);
        painter.setBrush(whiteBrush);
        painter.drawRect(rect());

        for (int i = 0; i < 4; i++) {
            item = (m_centerPageIndex - 2 + i >= 0 && m_centerPageIndex - 2 + i < m_imageList.count()) ?
                m_imageList.at(m_centerPageIndex - 2 + i) : NULL;
            if (item) {
                QRect intersectRect(rect());
                intersectRect = intersectRect.intersected(rects[i]);
                if (!intersectRect.isEmpty()) {
                    //painter.drawImage(intersectRect.topLeft(), *item, QRect(0, 0, rects[i].width(), rects[i].height()));
                    paintImage(&painter, intersectRect, *item);

                    QPen sideImagePen(KSideImageBorderColor);
                    sideImagePen.setStyle(Qt::SolidLine);
                    painter.setPen(sideImagePen);
                    painter.setBrush(Qt::NoBrush);
                    painter.drawRect(intersectRect);
                }
            }
        }
    }

    if (m_direction == 1) {
        dw = m_leftPlaceHolderRect.width() * m_placeHolderResizeFactor / 200;
        dh = m_leftPlaceHolderRect.height() * m_placeHolderResizeFactor / 200;
        rects[0].setRect(m_leftPlaceHolderRect.x() + 3 * dw, m_leftPlaceHolderRect.y() + dh, m_leftPlaceHolderRect.width() - 2 * dw, m_leftPlaceHolderRect.height() - 2 * dh);

        dx = (m_centerPlaceHolderRect.x() - m_leftPlaceHolderRect.x()) * m_placeHolderResizeFactor / 100;
        dw = (m_centerPlaceHolderRect.width() - m_leftPlaceHolderRect.width()) * m_placeHolderResizeFactor / 200;
        dh = (m_centerPlaceHolderRect.height() - m_leftPlaceHolderRect.height()) * m_placeHolderResizeFactor / 200;
        rects[1].setRect(m_centerPlaceHolderRect.x() - dx, m_centerPlaceHolderRect.y() + dh, m_centerPlaceHolderRect.width() - 2 * dw, m_centerPlaceHolderRect.height() - 2 * dh);

        dx = (m_rightPlaceHolderRect.x() - m_centerPlaceHolderRect.x()) * m_placeHolderResizeFactor / 100;
        dw = (m_centerPlaceHolderRect.width() - m_rightPlaceHolderRect.width()) * m_placeHolderResizeFactor / 200;
        dh = (m_centerPlaceHolderRect.height() - m_rightPlaceHolderRect.height()) * m_placeHolderResizeFactor / 200;
        rects[2].setRect(m_rightPlaceHolderRect.x() - dx, m_rightPlaceHolderRect.y() -dh, m_rightPlaceHolderRect.width() + 2 * dw, m_rightPlaceHolderRect.height() + 2 * dh);

        QPoint pt(m_rightPlaceHolderRect.x() + m_rightPlaceHolderRect.width(), m_rightPlaceHolderRect.y() + m_rightPlaceHolderRect.height() / 2);
        dw = (m_rightPlaceHolderRect.width()) * m_placeHolderResizeFactor / 200;
        dh = (m_rightPlaceHolderRect.height()) * m_placeHolderResizeFactor / 200;
        rects[3].setRect(pt.x() - 2 * dw, pt.y() - dh, pt.x(), pt.y() + 2 * dh);

        const QImage* item = 0;
        QPainter painter(this);
        painter.setPen(Qt::NoPen);
        QBrush whiteBrush(Qt::white, Qt::SolidPattern);
        painter.setBrush(whiteBrush);
        painter.drawRect(rect());

        for (int i = 0; i < 4; i++)  {
            item = (m_centerPageIndex - 1 + i >= 0 && m_centerPageIndex - 1 + i < m_imageList.count()) ?
                m_imageList.at(m_centerPageIndex - 1 + i) : NULL;
            if (item) {
                QRect intersectRect(rect());
                intersectRect = intersectRect.intersected(rects[i]);
                if (!intersectRect.isEmpty()) {
                    //painter.drawImage(intersectRect.topLeft(), *item, QRect(0, 0, rects[i].width(), rects[i].height()));
                    paintImage(&painter, intersectRect, *item);

                    QPen sideImagePen(KSideImageBorderColor);
                    sideImagePen.setStyle(Qt::SolidLine);
                    painter.setPen(sideImagePen);
                    painter.setBrush(Qt::NoBrush);
                    painter.drawRect(intersectRect);
                }
            }
        }
    }
    if (!m_fastScroll) {
        m_placeHolderResizeFactor += KAnimFactorInc;
    }
    else {
        m_placeHolderResizeFactor += KFastScrollAnimFactorInc;
    }
    m_repaintTimer->start(KRepaintTimerCb);
}

void VisualFlowLite::showPrevious()
{
    if (m_centerPageIndex > 0) {
        if (m_repaintTimer->isActive()) {
            m_repaintTimer->stop();
            m_resizeFactor = KMaxAnimFactor;
        }
        m_paintState = paintStateInTransition;
        m_direction = -1;
        m_repaintTimer->start(KRepaintTimerCb);
    }
}

void VisualFlowLite::showNext()
{
    int count = m_imageList.count();
    if (m_centerPageIndex < ( count -1 )) {
        if (m_repaintTimer->isActive()) {
            m_repaintTimer->stop();
            m_resizeFactor = KMaxAnimFactor;
        }
        m_paintState = paintStateInTransition;
        m_direction = 1;
        m_repaintTimer->start(KRepaintTimerCb);
    }
}

void VisualFlowLite::paintImage(QPainter* painter, QRect rect, QImage image)
{
    // scaling images up does not look good so we only scale down if image is too big
    bool shouldScale(false);
    if (m_scalingAllowed)
         shouldScale = (image.height() > rect.height() || image.width() > rect.width()); // scale down needed


    QImage img = (shouldScale) ?  image.scaled(rect.width(), rect.height(), Qt::KeepAspectRatio) : image;

    int originX = rect.topLeft().x();
    int originY = rect.topLeft().y();
    // center the image if needed
    if (rect.width() > img.width())
        originX += (rect.width() - img.width()) / 2;
    if (rect.height() > img.height())
        originY += (rect.height() - img.height()) / 2;

    painter->drawImage(QPoint(originX, originY), img, QRect(QPoint(0, 0), rect.size()));
}

QRect VisualFlowLite::centralRect() const 
{
    return QRect();
}

}
