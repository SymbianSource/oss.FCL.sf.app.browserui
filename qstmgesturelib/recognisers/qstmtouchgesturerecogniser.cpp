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


#include "qstmgenericsimplegesture.h"
#include "qstmtouchgesturerecogniser.h"
#include "qstmuievent_if.h"
#include <qstmfilelogger.h>

using namespace qstmGesture ;

QStm_TouchGestureRecogniser::QStm_TouchGestureRecogniser(QStm_GestureListenerIf* listener) :
                                      QStm_GestureRecogniser(listener)
{
}


QStm_TouchGestureRecogniser::~QStm_TouchGestureRecogniser()
{
}

/*!
 * Touch gesture recogniser.  Note that this one never owns the gesture, it just calls
 * the callback if it detects ETouch inside the area being watched.
 * There could be also check for the target window?
 */
QStm_GestureRecognitionState QStm_TouchGestureRecogniser::recognise(int numOfActiveStreams,
                                    QStm_GestureEngineIf* pge)
{
    QStm_GestureRecognitionState state = m_state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;

    if (numOfActiveStreams == 1) {
        // Then look at the event stream, it has to be EHold
        const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
        qstmUiEventEngine::QStm_UiEventCode eventCode = puie->code();

        if (m_loggingenabled) {
            LOGARG("QStm_TouchGestureRecogniser: 0x%x num %d code %d", this, puie->countOfEvents(), eventCode);

        }
        
        if (eventCode == qstmUiEventEngine::ETouch) {
            const QPoint& tapPoint = puie->currentXY();
            if (m_loggingenabled) {
                LOGARG("QStm_TouchGestureRecogniser: 0x%x ETouch: num %d at %d, %d", 
                		this, puie->countOfEvents(), tapPoint.x(), tapPoint.y());
                LOGARG("QStm_TouchGestureRecogniser: area, %d,%d %d,%d", m_area.x(), 
                		m_area.y(), m_area.x() + m_area.width(), m_area.y() + m_area.height());
            }
            
            bool produceGesture ;
            if(!m_area.isEmpty()) {
                produceGesture = m_area.contains(tapPoint);
                if(produceGesture && m_loggingenabled) {
                    LOGARG("QStm_TouchGestureRecogniser: HIT area (%d,%d) in %d,%d %d,%d", 
                    		tapPoint.x(), tapPoint.y(), m_area.y(), m_area.x() + m_area.width(), m_area.y() + m_area.height());
                }
            }
            else {
                produceGesture = (m_powner == puie->target()); // no area defined, touch detected in the window
            }
            
            if (produceGesture) {
                // state = EGestureActive ; do not take ownership, all gestures anyway start with ETouch
                // issue the touch gesture
                qstmGesture::QStm_GenericSimpleGesture pgest(KUid, tapPoint);
                pgest.setTarget(puie->target());
                // Call the listener to inform that a touch has occurred...
                m_listener->gestureEnter(pgest);
            }
        }
    }
    return state;
}

void QStm_TouchGestureRecogniser::release(QStm_GestureEngineIf* /*ge*/)
{
    if (m_loggingenabled) {
        LOGARG("QStm_TouchGestureRecogniser: 0x%x release", this);
    }
}

void QStm_TouchGestureRecogniser::setArea(const QRect& theArea)
{
    m_area = theArea ;
    if (m_loggingenabled) {
        LOGARG("QStm_TouchGestureRecogniser: area, %d,%d %d,%d", 
        		m_area.y(), m_area.x() + m_area.width(), m_area.y() + m_area.height());
    }

}
