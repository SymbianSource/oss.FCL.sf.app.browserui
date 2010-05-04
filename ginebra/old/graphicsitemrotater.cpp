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


#include "graphicsitemrotater.h"
#include <QtGui>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "utilities.h"
#include <assert.h>


GraphicsItemRotater::GraphicsItemRotater()
  : m_lastRotationValue(0)
{
    m_timeLine.setFrameRange(0, 50);
    safe_connect(&m_timeLine, SIGNAL(valueChanged(qreal)), this, SLOT(updateRotationStep(qreal)));
    safe_connect(&m_timeLine, SIGNAL(finished()), this, SIGNAL(finished()));
}

GraphicsItemRotater::~GraphicsItemRotater() {
    if(m_pixmapItem) delete m_pixmapItem;
}

void GraphicsItemRotater::start(QGraphicsItem *item, int angle, int duration, QTimeLine::CurveShape shape) {
    m_timeLine.setDuration(duration);
    m_timeLine.setCurveShape(shape);

    // Grab a pixmap of the item.
    QRectF rect = item->boundingRect();
    QPixmap *pixmap = new QPixmap(rect.size().toSize());
    QPainter painter(pixmap);
    QStyleOptionGraphicsItem options;
    options.exposedRect = QRect(rect.toRect());
    grabItem(item, &painter, &options);
    painter.end();
//    QLabel *l = new QLabel;
//    l->setPixmap(*pixmap);
//    l->show();

    // Create a pixmap item to represent the real item during the rotation.
    m_pixmapItem = new QGraphicsPixmapItem(item->parentItem());
    item->scene()->addItem(m_pixmapItem);
    m_pixmapItem->setPos(item->pos());
    m_pixmapItem->setTransform(item->transform());
    m_pixmapItem->setPixmap(*pixmap);
    m_pixmapItem->setZValue(item->zValue());
    m_pixmapItem->show();

    m_rotationAngle = angle;
    if(m_rotationAngle < -180)
        m_rotationAngle = 360 + m_rotationAngle;
    else if(m_rotationAngle > 180)
        m_rotationAngle = m_rotationAngle - 360;

    m_rotationAngle = m_rotationAngle % 360;

    m_lastRotationValue = 0;
    m_timeLine.start();
}

void GraphicsItemRotater::updateRotationStep(qreal value)  // slot
{
    QSizeF size = m_pixmapItem->boundingRect().size();
    qreal dx = size.width()/2;
    qreal dy = size.height()/2;
    QTransform transform = m_pixmapItem->transform();
    transform.translate(dx, dy);
    transform.rotate((value - m_lastRotationValue) * m_rotationAngle);
    transform.translate(-dx, -dy);
    m_pixmapItem->setTransform(transform);

    m_lastRotationValue = value;
}

static bool zValueSort(const QGraphicsItem *item1, const QGraphicsItem *item2) {
    return item1->zValue() < item2->zValue();
}

void GraphicsItemRotater::grabItem(QGraphicsItem *item, QPainter *painter, const QStyleOptionGraphicsItem *option,
                                   QWidget *widget) {
    QList<QGraphicsItem *> childs = item->childItems();
    qSort(childs.begin(), childs.end(), zValueSort);
    foreach(QGraphicsItem *child, childs) {
        if(child->isVisible()) {
            QTransform transform = painter->transform();
            transform.translate(child->pos().x(), child->pos().y());
            painter->setTransform(transform);
            child->paint(painter, option, widget);
            transform.translate(-child->pos().x(), -child->pos().y());
            painter->setTransform(transform);
        }
    }
}
