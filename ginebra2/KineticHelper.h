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


#ifndef __KINETIC_HELPER_H__
#define __KINETIC_HELPER_H__

#include <QObject>
#include <QPoint>
#include <QTime>

class QTimer;

#define KINETIC_TIMEOUT  60
#define DECELERATION     100


class KineticScrollable
{
public:
    virtual void scrollTo(QPoint& pos) = 0;
    
    virtual QPoint getScrollPosition() = 0;
    
    virtual QPoint getInitialPosition() = 0;
    
    virtual QPointF getInitialSpeed() = 0;
};



class KineticHelper: public QObject
{
    Q_OBJECT

public:
    KineticHelper(KineticScrollable* scrollable, qreal decel = DECELERATION, int kineticTimeout = KINETIC_TIMEOUT);
    ~KineticHelper();
    
    void setDeceleration(qreal decel) { m_decel = decel; }
    void setKineticTimeout(int timeout) { m_kineticTimeout = timeout; }
    void stopScrolling();
    void startScrolling();
    bool isScrolling();
private slots:    
    void kineticScroll();
private:
    KineticScrollable* m_scrollable;
    QPointF            m_initialSpeed;
    QPoint             m_initialPos;
    QTimer*            m_kineticTimer;
    int                m_kineticSteps;
    qreal              m_decel;
    int                m_kineticTimeout;
    QTime              m_actualTime;
    qreal              m_kineticScrollTime;
};

#endif //__KINETIC_HELPER_H__
