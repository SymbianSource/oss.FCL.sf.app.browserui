/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/
#include "qstmgenericsimplegesture.h"
#include "qstmmaybetapgesturerecogniser.h"
#include "qstmuievent_if.h"
#include "qstmutils.h"
#include "qstmfilelogger.h"

using namespace qstmGesture ;
using namespace qstmUiEventEngine;

QStm_MaybeTapGestureRecogniser::QStm_MaybeTapGestureRecogniser(QStm_GestureListenerIf* listener) :
                                        QStm_GestureRecogniser(listener)
{
    m_powner = listener->getOwner() ;
    if (listener) {
        addTapListener(listener, m_powner) ;
    }
    m_gestureEnabled = true;
    m_numOfActiveStreams = 0;

}

QStm_MaybeTapGestureRecogniser::~QStm_MaybeTapGestureRecogniser()
{
    m_tapListeners.clear() ;
    m_tapListenerWindows.clear() ;
}

QStm_GestureRecognitionState QStm_MaybeTapGestureRecogniser::recognise(int numOfActiveStreams,
                              QStm_GestureEngineIf* pge)
{
    QStm_GestureRecognitionState state = m_state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;
    
    // Look at the events to see if it looks like a tap or double tap
    if (numOfActiveStreams == 1) {
        m_numOfActiveStreams = numOfActiveStreams; 
        // Then look at the event stream, it has to be tap and release
        const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
        int countOfEvents = puie->countOfEvents() ;
        qstmUiEventEngine::QStm_UiEventCode eventCode = puie->code() ;

        if (m_loggingenabled) {
            LOGARG("QStm_MaybeTapGestureRecogniser: event: %s, countOfEvents: %d, code: %d", 
                   event_name(eventCode), countOfEvents, eventCode);
        }
        void* target = puie->target();
        
        if (countOfEvents == 2) { // Do we have touch and release in the stream, check if there are two events

            // Then look at the events to see if they are suitable for us
            if (target && eventCode == qstmUiEventEngine::ERelease) {// The last one is release

                qstmUiEventEngine::QStm_UiEventIf* puieFirst = puie->previousEvent();

                if (puieFirst != NULL && 
                    (eventCode = puieFirst->code()) == qstmUiEventEngine::ETouch && // is the first one ETouch
                    isPointClose(puie->currentXY(), puie->previousXY())) { // if system failed to deliver move event between 
                	                                                       // down and up we can get tap with two points
                	                                                       // too far from each other, so check for it

                    if (m_loggingenabled) {
                        LOGARG("QStm_MaybeTapGestureRecogniser: 0x%x TAP: countOfEvents: %d, event: %s", this, countOfEvents, event_name(eventCode));
                    }
                    
                    m_firstTapXY = puieFirst->currentXY() ;
                    m_firstTapTarget = target ;
                    m_firstTapSpeed = puie->speed() ;
                    int inx = m_tapListenerWindows.indexOf(m_firstTapTarget) ;
                    if (inx == -1) {
                        inx = 0;
                    }
                    qstmGesture::QStm_GenericSimpleGesture pgest(
                                        qstmGesture::EGestureUidMaybeTap, 
                                        m_firstTapXY, 
                                        puie->timestamp(),
                                        qstmGesture::ETapTypeSingle, 
                                        puieFirst) ; // TODO: speed is 0?
                                    pgest.setTarget(puie->target());
                                    QStm_GestureListenerIf* plistener = m_tapListeners[inx] ;
                                    plistener->gestureEnter(pgest) ;
                }
            }
        }
    }
    m_state = state;
    return state;
}

void QStm_MaybeTapGestureRecogniser::release(QStm_GestureEngineIf* /*ge*/)
{
    if (m_loggingenabled) {
        LOGARG("QStm_MaybeTapGestureRecogniser: 0x%x release, %d %d", this, m_firstTapXY.x(), m_firstTapXY.y());
    }
    m_state = ENotMyGesture;
}

void QStm_MaybeTapGestureRecogniser::enableLogging(bool loggingOn)
{
    m_loggingenabled = loggingOn;
}

void QStm_MaybeTapGestureRecogniser::setOwner(void* owner)
{
    m_powner = owner;
}


void QStm_MaybeTapGestureRecogniser::enable(bool enabled)
{
    m_gestureEnabled = enabled ;
}

bool QStm_MaybeTapGestureRecogniser::isEnabled()
{
    return m_gestureEnabled ;
}

void QStm_MaybeTapGestureRecogniser::setTapRange(int rangeInMillimetres)
{
    m_rangesizeInPixels = QStm_Utils::mm2Pixels(rangeInMillimetres) ;
}

bool QStm_MaybeTapGestureRecogniser::isPointClose(const QPoint&  firstPoint, const QPoint& secondPoint)
{
    QRect tolerance = QStm_Utils::toleranceRectPx(secondPoint, m_rangesizeInPixels) ;
    bool aretheyclose = tolerance.contains(firstPoint);
    return aretheyclose ;
}

void QStm_MaybeTapGestureRecogniser::addTapListener(QStm_GestureListenerIf* listener, void* listenerOwner)
{
    m_tapListeners.append(listener) ;
    m_tapListenerWindows.append(listenerOwner) ;
}

void QStm_MaybeTapGestureRecogniser::removeTapListener(QStm_GestureListenerIf* listener, void* /*listenerOwner*/)
{
    int inx = m_tapListeners.indexOf(listener) ;
    if(inx != -1) {
        m_tapListeners.removeAt(inx) ;
        m_tapListenerWindows.removeAt(inx) ;
    }
}

