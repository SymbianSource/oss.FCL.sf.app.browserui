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


#include "qstmupdowngesturerecogniser.h"
#include "qstmgenericsimplegesture.h"
#include "qstmuievent_if.h"
#include "qstmutils.h"
#include "qstmfilelogger.h"

using namespace qstmGesture ;

QStm_UpdownGestureRecogniser::QStm_UpdownGestureRecogniser(QStm_GestureListenerIf* listener) : 
		                                       QStm_GestureRecogniser(listener)
{
}


QStm_UpdownGestureRecogniser::~QStm_UpdownGestureRecogniser()
{
}

QStm_GestureRecognitionState QStm_UpdownGestureRecogniser::recognise(int numOfActiveStreams,
                                         QStm_GestureEngineIf* pge)
{
    QStm_GestureRecognitionState state = m_state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;

    // Look at the events to see if it looks like a tap or double tap
    if (numOfActiveStreams == 1) {
        // Then look at the event stream, it has to be tap and release
        const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
        int countOfEvents = puie->countOfEvents() ;
        qstmUiEventEngine::QStm_UiEventCode eventCode = puie->code() ;
        
        if (countOfEvents > 1) {// do we have more than one event in the stream?
            // Then look at the events to see if they are suitable for us
            // should we check that all of the events are targeted to our window?
            // currently we only check if the last one is for us and is EMove, then check if |x| > |y|
            if (puie->target() == m_powner &&
                    eventCode == qstmUiEventEngine::EMove) { // The last one is move in our window

            	if (m_loggingenabled) {
                    LOGARG("QStm_UpdownGestureRecogniser: UpDown: num %d code %d", countOfEvents, eventCode);
                }
                // Is it leftright gesture in our window?
                const QPoint& p = puie->currentXY() ;
                QPoint dp = p - puie->previousXY() ;
                // check that the Y movement is bigger
                if (ABS(dp.x()) < ABS(dp.y())) {
                    state = EGestureActive;
                    qstmGesture::QStm_GenericSimpleGesture pgest(KUid, p, dp.y(), puie) ;
                    // Give the gesture a name
                    pgest.setName(QString("Updown")) ;
                    pgest.setTarget(puie->target());
                    // Call the listener to inform that a UpDown has occurred...
                    m_listener->gestureEnter(pgest) ;
                }
            }
        }
    }
    m_state = state;
    return state;
}

void QStm_UpdownGestureRecogniser::release(QStm_GestureEngineIf* pge)
{
	const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
    using qstmUiEventEngine::QStm_UiEventSpeed;
    const QPoint& p = puie->currentXY() ;
    QPoint dp = p - puie->previousXY() ;
    qstmGesture::QStm_GenericSimpleGesture pgest(KUid, p, dp.y(), puie) ;
    pgest.setTarget(puie->target());
    m_listener->gestureExit(pgest) ;
    m_state = ENotMyGesture;
}

