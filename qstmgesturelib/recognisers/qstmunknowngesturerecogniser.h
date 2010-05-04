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


#ifndef QSTMUNKNOWNGESTURERECOGNISER_H_
#define QSTMUNKNOWNGESTURERECOGNISER_H_

#include <qstmgestureengine_if.h>
#include <qstmgesture_if.h>
#include <qstmgesturelistener_if.h>

namespace qstmGesture
{

/*!
 * QStm_UnknownGestureRecogniser handles unknown gestures at ERelease
 * I.e. if no-one else has recognised a gesture when ERelease arrives,
 * This recogniser can be at the end of the list to inform the app
 * that something weird is happening...
 *
 */
class QStm_UnknownGestureRecogniser : public QStm_GestureRecogniser
{
public:
    static const QStm_GestureUid KUid = EGestureUidUnknown;
    virtual ~QStm_UnknownGestureRecogniser();
    virtual QStm_GestureRecognitionState recognise(int numOfActiveStreams, QStm_GestureEngineIf* ge) ;
    virtual void release(QStm_GestureEngineIf* ge) ;

    virtual QStm_GestureUid gestureUid() const { return KUid; }

    QStm_UnknownGestureRecogniser(QStm_GestureListenerIf* listener) ;

};

} // namespace

#endif /* QSTMUNKNOWNGESTURERECOGNISER_H_ */
