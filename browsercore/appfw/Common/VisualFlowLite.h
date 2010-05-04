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


#ifndef VISUALFLOWLITE_H
#define VISUALFLOWLITE_H

#include <QWidget>
#include <QKeyEvent>
#include "FlowInterface.h"

class QPainter;

namespace WRT {

class VisualFlowLite : public FlowInterface
{
Q_OBJECT
public:
    VisualFlowLite(QWidget* parent);
    void init();
    void addSlide(const QImage& image);
    void clear();
    int slideCount() const;
    QImage slide(int index) const;
    void setCenterIndex(int i);
    int centerIndex() const;
    bool slideAnimationOngoing() const;
    void setCenterThumbnail(int i);
    void setCurrentIndex(int i);
    virtual ~VisualFlowLite();
    void keyPressEvent(QKeyEvent* event);
    void paintEvent(QPaintEvent* event);
    void calculateLayout();
    void showPrevious();
    void showNext();
    void paintNormalState(QPaintEvent* event);
    void paintInTransition(QPaintEvent* event);
    void setImagesScaled(bool scaled) { m_scalingAllowed = scaled; }
    QRect centralRect() const;

signals:
    void centerIndexChanged(int index);
    void ok(int index);
    void cancel();

public slots:
    bool calcRepaintRect();
    bool animatePlaceHolderPosition();
    void scroll();

protected:
    void mousePressEvent(QMouseEvent* event);
    void resizeEvent(QResizeEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

private:
    enum PaintState {
        paintStateNormal,
        paintStateInTransition
    };

    void paintImage(QPainter* painter, QRect rect, QImage image);

    // indicates the index of the center thumbnail
    int                      m_centerPageIndex;
    QRect                    m_offscreenRect;
    // represents the entry corresponding to the selected page in view
    // timer used to do animation
    QTimer*                   m_repaintTimer;
    //repaint rect using during animating initial display of the view
    QRect                    m_repaintRect;
    //resize factor applied during initial display animation
    int                      m_resizeFactor;
    //left place holder rectangle
    QRect                    m_leftPlaceHolderRect;
    //left place holder rectangle
    QRect                    m_centerPlaceHolderRect;
    //left place holder rectangle
    QRect                    m_rightPlaceHolderRect;
    //Left Arrow position
    QPoint                   m_leftArrow[3];
    //Right arrow position
    QPoint                   m_rightArrow[3];
    //Factor used during animation of place holders
    int                      m_placeHolderResizeFactor;
    //Direction of movement
    int                      m_direction;
    //Flag to check if fast animation is needed or not
    bool                     m_fastScroll;
    PaintState               m_paintState;
    // Initial position 
    int m_currentIndex;
    QList<const QImage*> m_imageList;
    QPoint m_lastMoveEventPos;
    QTimer* m_scrollTimer;
    bool m_scalingAllowed;
};

}
#endif      // VISUALFLOWLITE_H

