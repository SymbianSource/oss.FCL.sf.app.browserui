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


#ifndef GRAPHICSITEMROTATER_H
#define GRAPHICSITEMROTATER_H

#include <QObject>
#include <QTimeLine>

class QGraphicsPixmapItem;
class QGraphicsItem;

class GraphicsItemRotater : public QObject
{
    Q_OBJECT

public:
    GraphicsItemRotater();
    ~GraphicsItemRotater();

    void start(QGraphicsItem *item, int angle, int duration = 500, QTimeLine::CurveShape shape = QTimeLine::EaseOutCurve);
    void stop() { m_timeLine.stop(); }

signals:
    void finished();

protected slots:
    void updateRotationStep(qreal value);

protected:
    void grabItem(QGraphicsItem *item, class QPainter *painter, const class QStyleOptionGraphicsItem *option, QWidget *widget = 0);

protected:
    QGraphicsPixmapItem *m_pixmapItem;
    QTimeLine m_timeLine;
    int m_rotationAngle;
    qreal m_lastRotationValue;
};

#endif // GRAPHICSITEMROTATER_H
