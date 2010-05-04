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


#ifndef QSTMLEFTRIGHTGESTURERECOGNISER_H_
#define QSTMLEFTRIGHTGESTURERECOGNISER_H_

#include <qstmgestureengine_if.h>
#include <qstmgesture_if.h>
#include <qstmgesturelistener_if.h>

namespace qstmGesture
{

class QStm_LeftrightGestureRecogniser : public QStm_GestureRecogniser
{
public:
    static const QStm_GestureUid KUid = EGestureUidLeftRight;

    virtual ~QStm_LeftrightGestureRecogniser();
    virtual QStm_GestureRecognitionState recognise(int numOfActiveStreams, QStm_GestureEngineIf* ge) ;
    virtual void release(QStm_GestureEngineIf* ge) ;

    virtual QStm_GestureUid gestureUid() const { return KUid; }
    QStm_LeftrightGestureRecogniser(QStm_GestureListenerIf* listener) ;
};

}

#endif /* QSTMLEFTRIGHTGESTURERECOGNISER_H_ */
