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
#include "qstmlongpressgesturerecogniser.h"
#include "qstmuievent_if.h"
#include "qstmfilelogger.h"

using namespace qstmGesture ;

QStm_LongPressGestureRecogniser::QStm_LongPressGestureRecogniser(QStm_GestureListenerIf* listener) :
                                    QStm_GestureRecogniser(listener)
{
}

QStm_LongPressGestureRecogniser::~QStm_LongPressGestureRecogniser()
{
}

/*!
 * recognise the long press; basically it is just the EHold UI event
 */
QStm_GestureRecognitionState QStm_LongPressGestureRecogniser::recognise(int numOfActiveStreams,
                                 QStm_GestureEngineIf* pge)
{
    QStm_GestureRecognitionState state = m_state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;


    // Look at the events to see if it looks like long press with one pointer
    if (numOfActiveStreams == 1) {
        // Then look at the event stream, it has to be EHold
        const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
        int countOfEvents = puie->countOfEvents();
        qstmUiEventEngine::QStm_UiEventCode eventCode = puie->code();

        if (m_loggingenabled) {
            LOGARG("QStm_LongPressGestureRecogniser: %d num %d code %d", eventCode, countOfEvents, eventCode);
        }
        if (puie->target() == m_powner && eventCode == qstmUiEventEngine::EHold) { 
        	// The last one is EHold, look if it is near our borders
            const QPoint& p = puie->currentXY() ;
            if (m_loggingenabled) {
                LOGARG("QStm_LongPressGestureRecogniser: (%d, %d) in (%d,%d)(%d,%d)", p.x(), p.y(),
                        m_area.x(), m_area.y(), m_area.x() + m_area.width(), m_area.y() + m_area.height());
            }
            // check where the point is inside the area defined
            if (m_area.contains(p)) {
                state = EGestureActive ;
                // issue the long press gesture
                qstmGesture::QStm_GenericSimpleGesture pgest(KUid, p, 0, puie) ; // TODO: speed is 0?
                pgest.setTarget(puie->target());
                // Call the listener to inform that the gesture has occurred...
                m_listener->gestureEnter(pgest) ;
            }
        }
    }
    m_state = state;
    return state;
}

void QStm_LongPressGestureRecogniser::release(QStm_GestureEngineIf* /*ge*/)
{
    if (m_loggingenabled) {
        LOGARG("QStm_LongPressGestureRecogniser: 0x%x release", this);
    }
    m_state = ENotMyGesture;
}

void QStm_LongPressGestureRecogniser::setArea(const QRect& theArea)
{
    m_area = theArea ;
}
