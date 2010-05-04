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

#ifndef QSTMHOVERINGGESTURERECOGNISER_H_
#define QSTMHOVERINGGESTURERECOGNISER_H_

#include <qstmgestureengine_if.h>
#include <qstmgesture_if.h>
#include <qstmgesturelistener_if.h>

namespace qstmGesture
{
/*!
 * Recognise hovering gesture.
 * If the EMove distance from previous is short enough, then
 * we assume hovering gesture.  If the previous UI event is ETouch, EHold or EMove
 * and the distance is short enough the gesture takes control.
 */
class QStm_HoveringGestureRecogniser : public QStm_GestureRecogniser
{
public:
    static const QStm_GestureUid KUid = EGestureUidHover;

    virtual ~QStm_HoveringGestureRecogniser();
    virtual QStm_GestureRecognitionState recognise(int numOfActiveStreams, QStm_GestureEngineIf* ge) ;
    virtual void release(QStm_GestureEngineIf* ge) ;

    virtual QStm_GestureUid gestureUid() const { return KUid; }

    void setHoveringSpeed(float aSpeed) /*__SOFTFP */;

    QStm_HoveringGestureRecogniser(QStm_GestureListenerIf* listener) ;
private:
    bool m_hovering ;
    float m_hoveringspeed ;
};

}

#endif /* QSTMHOVERINGGESTURERECOGNISER_H_ */
