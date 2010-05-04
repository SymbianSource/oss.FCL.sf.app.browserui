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


#ifndef QSTMLONGPRESSGESTURERECOGNISER_H_
#define QSTMLONGPRESSGESTURERECOGNISER_H_

#include <qstmgestureengine_if.h>
#include <qstmgesture_if.h>
#include <qstmgesturelistener_if.h>

namespace qstmGesture
{

/*!
 * QStm_LongPressGestureRecogniser handles long press gesture (generated from Hold UI event)
 *
 */
class QStm_LongPressGestureRecogniser : public QStm_GestureRecogniser
{
public:
    static const QStm_GestureUid KUid = EGestureUidLongPress;

    virtual ~QStm_LongPressGestureRecogniser();

    virtual QStm_GestureRecognitionState recognise(int numOfActiveStreams, QStm_GestureEngineIf* ge) ;
    virtual void release(QStm_GestureEngineIf* ge) ;

    virtual QStm_GestureUid gestureUid() const { return KUid; }

    void setArea(const QRect& theArea) ;

    QStm_LongPressGestureRecogniser(QStm_GestureListenerIf* listener) ;

private:
    bool  m_waitingforsecondtap ;
    QRect m_area ;
};

} // namespace

#endif /* QSTMLONGPRESSGESTURERECOGNISER_H_ */
