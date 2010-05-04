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
#include "qstmflickgesturerecogniser.h"
#include "qstmuievent_if.h"
#include "qstmfilelogger.h"

using namespace qstmGesture ;

QStm_FlickGestureRecogniser::QStm_FlickGestureRecogniser(QStm_GestureListenerIf* listener) :
                              QStm_GestureRecogniser(listener)
{
}

QStm_FlickGestureRecogniser::~QStm_FlickGestureRecogniser()
{
}

/*!
 * Release gesture recogniser.  Note that this one never owns the gesture, it just calls
 * the callback if it detects ERelease inside the area being watched.
 * There could be also check for the target window?
 */
QStm_GestureRecognitionState QStm_FlickGestureRecogniser::recognise(int numOfActiveStreams,
                                                                    QStm_GestureEngineIf* pge)
{
    QStm_GestureRecognitionState state = m_state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;

    // Look at the events to see if it looks like flick with one pointer
    if (numOfActiveStreams == 1) {
        const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
        int countOfEvents = puie->countOfEvents();
        qstmUiEventEngine::QStm_UiEventCode eventCode = puie->code();
        if (m_loggingenabled) {
            LOGARG("QStm_FlickGestureRecogniser: %d num %d code %d", eventCode, countOfEvents, eventCode);
        }
        
        if (m_powner == puie->target() && eventCode == qstmUiEventEngine::ERelease)
        {
            if (m_loggingenabled) {
                LOGARG("QStm_FlickGestureRecogniser: 0x%x ERelease: num %d code %d, %d", 
                		this, countOfEvents, puie->currentXY().x(), puie->currentXY().y());
            }
            // Check if the speed before release was fast enough for flick
            const qstmUiEventEngine::QStm_UiEventIf* puieprev = puie->previousEvent() ;
            if (puieprev && puieprev->code() == qstmUiEventEngine::EMove) {
                using qstmUiEventEngine::QStm_UiEventSpeed;

                float thespeed = puieprev->speed() ;
                if (m_loggingenabled) {
                    LOGARG("QStm_FlickGestureRecogniser: prev speed: %f (limit: %f)", double(thespeed), double(m_speed)) ;
                }
                if (thespeed > m_speed) {
                    state = EGestureActive ;

                    QStm_UiEventSpeed speedIf(thespeed, puieprev->speedVec());

                    // issue the flick gesture using the TDirectionalGesture (it has the speed and direction)
                    qstmGesture::QStm_DirectionalGesture pgest(
                            KUid,
                            puieprev->currentXY(),
                            puieprev->previousXY(),
                            &speedIf,
                            m_loggingenabled);
                    pgest.setTarget(puie->target());

                    // Call the listener to inform that a flick has occurred...
                    m_listener->gestureEnter(pgest);
                }
            }
        }
    }
    m_state = state;
    return state;
}
void QStm_FlickGestureRecogniser::release(QStm_GestureEngineIf* /*ge*/)
{
    if (m_loggingenabled) {
        LOGARG("QStm_FlickGestureRecogniser: 0x%x flick", this);
    }
    m_state = ENotMyGesture;
}

void QStm_FlickGestureRecogniser::setFlickingSpeed(float aSpeed) /* __SOFTFP */
{
    m_speed = aSpeed ;
}
