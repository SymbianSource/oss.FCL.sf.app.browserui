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

#include <QTimer>
#include <QTime>
#include <QPoint>
#include <QDebug> 

#include "KineticHelper.h"



KineticHelper::KineticHelper(KineticScrollable* scrollable, qreal decel, int kineticTimeout) :
     m_scrollable(scrollable),
     m_decel(decel), 
     m_kineticTimeout(kineticTimeout)
{
    m_kineticTimer = new QTimer(this);
    QObject::connect(m_kineticTimer, SIGNAL(timeout()), this, SLOT(kineticScroll()));
    m_kineticTimer->setSingleShot(false);
}

KineticHelper::~KineticHelper()
{
    delete m_kineticTimer;
}

bool KineticHelper::isScrolling()
{
    return m_kineticTimer->isActive();
}


void KineticHelper::startScrolling()
{
	m_initialSpeed = m_scrollable->getInitialSpeed() * 1000;	
	m_initialPos = m_scrollable->getInitialPosition();
	if (m_kineticTimer->isActive()) {
	    m_kineticTimer->stop();
	}
	m_kineticSteps = 0;
	m_kineticScrollTime = 0.0;
	m_kineticTimer->start(m_kineticTimeout);
	m_actualTime = QTime::currentTime();
	m_actualTime.start();
	qDebug() << "starting kinetic timer at " << m_initialPos << ", at " << m_actualTime;  
}


void KineticHelper::kineticScroll()
{
    if (++m_kineticSteps == 1) return;
    qreal decelX = m_initialSpeed.x() > 0 ? (-1) * m_decel : m_decel;
    qreal decelY = m_initialSpeed.y() > 0 ? (-1) * m_decel : m_decel;
    qreal dx = 0;
    qreal dy = 0;
    qreal vx = 0;
    qreal vy = 0;

    int t = m_actualTime.elapsed();
    m_kineticScrollTime += (((qreal)t) / 1000);
    
    qDebug() << "kineticScroll(): elapsed: " << t << ", m_kineticSteps: " << m_kineticSteps << 
                ", m_kineticScrollTime: " << m_kineticScrollTime <<
                ", decelX: "<< decelX << ", decelY: " << decelY << ", m_initialSpeed: " << m_initialSpeed;
    if (m_initialSpeed.x()) {
        vx = m_initialSpeed.x() + decelX * m_kineticScrollTime;
        qDebug() << "vx: " << vx;
        if (vx * m_initialSpeed.x() < 0) {
            dx = 0;
            vx = 0;
        }
        else {
            dx = m_kineticScrollTime * m_initialSpeed.x() + 
                0.5 * decelX * m_kineticScrollTime * m_kineticScrollTime;
        }
    }
    
    if (m_initialSpeed.y()) {
        vy = m_initialSpeed.y() + decelY * m_kineticScrollTime;
        qDebug() << "vy: " << vy;
        if (vy * m_initialSpeed.y() < 0) {
            dy = 0;
            vy = 0;
        }
        else {
            dy = m_kineticScrollTime * m_initialSpeed.y() + 
                 0.5 * decelY * m_kineticScrollTime * m_kineticScrollTime;
        }
    }
        
    QPoint scrollPos = m_scrollable->getScrollPosition();
    QPoint distPos = m_initialPos + QPointF(dx, dy).toPoint();
        
    if (vx != 0 || vy != 0) {
        qDebug() << "kineticScroll(): scroll from " << scrollPos << " to " << distPos;
        m_scrollable->scrollTo(distPos);
    }
    
    if ((vx == 0 && vy == 0) || scrollPos == m_scrollable->getScrollPosition()) {
        qDebug() << "kineticScroll(): stopping timer";
        stopScrolling();
    }
}

void KineticHelper::stopScrolling()
{
    if (m_kineticTimer && m_kineticTimer->isActive()) {
        m_kineticTimer->stop();
        m_initialSpeed.setX(0.0);
        m_initialSpeed.setY(0.0);
    }
    m_kineticSteps = 0;
}

