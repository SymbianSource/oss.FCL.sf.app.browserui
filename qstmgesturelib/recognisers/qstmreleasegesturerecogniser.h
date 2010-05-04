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

#ifndef QSTMRELEASEGESTURERECOGNISER_H_
#define QSTMRELEASEGESTURERECOGNISER_H_

#include <qstmgestureengine_if.h>
#include <qstmgesture_if.h>
#include <qstmgesturelistener_if.h>


namespace qstmGesture
{

/*!
 * QStm_ReleaseGestureRecogniser handles release "gesture"
 * This is countarpart to touch gesture recogniser, just
 * informing the app that ERelease has been seen.
 * This should be placed towards the end of the list of gesture
 * recognisers, all other recognisers which handle ERelease with
 * some other events preceding the ERelease must be before this one.
 *
 */
class QStm_ReleaseGestureRecogniser : public QStm_GestureRecogniser
{
public:
    static const QStm_GestureUid KUid = EGestureUidRelease;
    virtual ~QStm_ReleaseGestureRecogniser();

    virtual QStm_GestureRecognitionState recognise(int numOfActiveStreams, QStm_GestureEngineIf* ge) ;
    virtual void release(QStm_GestureEngineIf* ge) ;

    virtual QStm_GestureUid gestureUid() const  { return KUid; }
    void setArea(const QRect& theArea) ;

    QStm_ReleaseGestureRecogniser(QStm_GestureListenerIf* listener) ;

private:
    QRect m_area ;
};

} // namespace

#endif /* QSTMRELEASEGESTURERECOGNISER_H_ */
