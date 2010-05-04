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


#ifndef QSTMPINCHGESTURERECOGNISER_H_
#define QSTMPINCHGESTURERECOGNISER_H_

#include <qstmgestureengine_if.h>
#include <qstmgesture_if.h>
#include <qstmgesturelistener_if.h>


namespace qstmGesture
{
/*!
 * Recognise pinching gesture. !!! very experimental !!!
 * It seems that in capacitive touch in Symbian 9.4 touching with two
 * fingers causes fast EDrag events which are far from each other.
 * This gesture recogniser tries to use that feature
 * to determine when the gestures looks like a pinch.
 * Would requires a lot of tuning to be really useful, if at all possible.
 * With current implementation it is easy to zoom in but
 * almost impossible to zoom out.
 */
class QStm_PinchGestureRecogniser : public QStm_GestureRecogniser
{
public:
    static const QStm_GestureUid KUid = EGestureUidPinch;

    virtual ~QStm_PinchGestureRecogniser();
    virtual QStm_GestureRecognitionState recognise(int numOfActiveStreams, QStm_GestureEngineIf* ge) ;
    virtual void release(QStm_GestureEngineIf* ge) ;

    virtual QStm_GestureUid gestureUid() const { return KUid; }

    /* define the pinch triggering as speed value */
    /* after EHold has been seen, then any move which is faster than this will enter pinch gesture */
    /* note that the value varies depending on techology, speed is measured as pixels / ms */
    void setPinchingSpeed(float aSpeed) /*__SOFTFP */;

    QStm_PinchGestureRecogniser(QStm_GestureListenerIf* listener) ;

    void calculateZoomingLine() ;
    float calculateDistance() ;
    int adjustPinchMove(float& aPreviousDistance, float aNewDistance) ;

private:
    bool     m_pinching ;
    float    m_pinchingspeed ;
    QPoint   m_pinchstart ;
    QPoint   m_pinchend ;

    float m_m ;
    float m_b ;

    float   m_ddistance ;
    bool    m_holdseen ;
    QPoint  m_holdseenAtPos ;
    QTime   m_holdseenAtTime ;
};

}

#endif /* QSTMPINCHGESTURERECOGNISER_H_ */
