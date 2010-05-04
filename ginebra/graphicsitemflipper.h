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


#ifndef GRAPHICSITEMFLIPPER_H
#define GRAPHICSITEMFLIPPER_H

#include <QtGui>

// Implements a "flipping" animation between two QGraphicsItems.  The source
// item should be hidden after start() is called and the target item should be 
// shown in response to the finished() signal.  
class GraphicsItemFlipper : public QObject {
  Q_OBJECT

public:
    GraphicsItemFlipper(QGraphicsItem *parent, QGraphicsScene *scene, int duration = 500, uint updateInterval = 10, int zValue = 0);
    ~GraphicsItemFlipper();

    void start(QGraphicsView *view, QGraphicsWidget *sourceWidget, QGraphicsWidget *targetWidget, bool forward = true);

signals:
    void starting();
    void finished();

protected:
    void getPixmaps(QGraphicsView *view, QGraphicsWidget *sourceWidget, QGraphicsWidget *targetWidget);

protected slots:
    void updateFlipStep(qreal value);
    void onFinished();

protected:
    QGraphicsPixmapItem *m_pixmapItem;
    QPixmap *m_sourcePixmap;
    QPixmap *m_targetPixmap;
    QRectF m_sourceRect;
    QRectF m_targetRect;
    QTimeLine m_timeLine;
};

#endif // GRAPHICSITEMFLIPPER_H
