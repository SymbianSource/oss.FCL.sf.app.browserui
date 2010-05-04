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


#include <assert.h>
#include "graphicsitemflipper.h"
#include "utilities.h"

GraphicsItemFlipper::GraphicsItemFlipper(QGraphicsItem *parentItem, QGraphicsScene *scene, int duration,
                                         uint updateInterval, int zValue)
  : m_timeLine(duration)
{
    m_timeLine.setUpdateInterval(updateInterval);
    m_timeLine.setCurveShape(QTimeLine::EaseInOutCurve);
    safe_connect(&m_timeLine, SIGNAL(valueChanged(qreal)), this, SLOT(updateFlipStep(qreal)));
    safe_connect(&m_timeLine, SIGNAL(finished()), this, SLOT(onFinished()));

    m_pixmapItem = new QGraphicsPixmapItem(parentItem);
    scene->addItem(m_pixmapItem);
    m_pixmapItem->setZValue(zValue);
}

GraphicsItemFlipper::~GraphicsItemFlipper()
{
    delete m_pixmapItem;
    if(m_sourcePixmap) delete m_sourcePixmap;
    if(m_targetPixmap) delete m_targetPixmap;
}

void GraphicsItemFlipper::start(QGraphicsView *view, QGraphicsWidget *sourceWidget, QGraphicsWidget *targetWidget,
                                bool forward)
{
    emit starting();

    getPixmaps(view, sourceWidget, targetWidget);

    m_pixmapItem->setPixmap(forward ? *m_sourcePixmap : *m_targetPixmap);

    m_pixmapItem->show();

    m_timeLine.setDirection(forward ? QTimeLine::Forward : QTimeLine::Backward);
    m_timeLine.start();
}

void GraphicsItemFlipper::getPixmaps(QGraphicsView *view, QGraphicsWidget *sourceWidget, QGraphicsWidget *targetWidget)
{
    Q_UNUSED(view)
    QStyleOptionGraphicsItem options;

    m_sourceRect = sourceWidget->geometry();
    m_targetRect = targetWidget->geometry();

    // Get a snapshot of the source widget.
    m_sourcePixmap = new QPixmap(m_sourceRect.size().toSize());
    QPainter painter(m_sourcePixmap);
    options.exposedRect = m_sourceRect;
    sourceWidget->paint(&painter, &options);
    painter.end();

    // Get a snapshot of the target widget and flip it left to right.
    QPixmap tmpPixmap(m_targetRect.size().toSize());
    QPainter targetPainter(&tmpPixmap);
    options.exposedRect = m_targetRect;
    targetWidget->paint(&targetPainter, &options);
    targetPainter.end();
    m_targetPixmap = new QPixmap(tmpPixmap.transformed(QTransform().rotate(180, Qt::YAxis)));
}

void GraphicsItemFlipper::updateFlipStep(qreal value)  // slot
{
    //qreal scale = 1 - sin(3.14 * value) * 0.5;
    qreal sourceW = m_sourceRect.width();
    qreal sourceH = m_sourceRect.height();
    qreal deltaW = m_targetRect.width() - sourceW;
    qreal deltaH = m_targetRect.height() - sourceH;
    qreal deltaX = m_targetRect.left() - m_sourceRect.left();
    qreal deltaY = m_targetRect.top() - m_sourceRect.top();
    qreal xScale = (sourceW + (deltaW * value))/sourceW;
    qreal yScale = (sourceH + (deltaH * value))/sourceH;
    qreal newW = sourceW * xScale;
    qreal newH = sourceH * yScale;
    qreal dx = m_pixmapItem->pos().x() + (deltaX * value) + (newW/2);

//    qDebug() << "GraphicsItemFlipper::updateFlipStep: value=" << value;
//            << " xScale=" << xScale
//            << " newW=" << newW
//            << " deltaX=" << deltaX;

    // Set the appropriate pixmap.  If we've gone past the halfway point, ie. the item is edge-on,
    // switch pixmaps.
    // To do: Only set the pixmap twice, once when the animation starts and once at the halfway
    //        point.  Need to be sure resolution is correct at the end of the animation.
    if(value > 0.5) {
        m_pixmapItem->setPixmap(m_targetPixmap->scaled(int(newW), int(newH)));
    }
    else {
        m_pixmapItem->setPixmap(m_sourcePixmap->scaled(int(newW), int(newH)));
    }

    QTransform transform;
    transform.translate(dx, 0);
    transform.rotate(180 * value, Qt::YAxis);
    transform.translate(-dx, 0);
    transform.translate(deltaX * value, deltaY * value);
    m_pixmapItem->setTransform(transform);
}

void GraphicsItemFlipper::onFinished()
{
    m_pixmapItem->hide();
    emit finished();
}
