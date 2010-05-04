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

#ifndef QSTMTIMERINTERFACE_H_
#define QSTMTIMERINTERFACE_H_

namespace qstmUiEventEngine
{
class QStm_TimerInterfaceIf
{
public:
	virtual void startTouchTimer(int delay, int pointerNumber) = 0 ;
	virtual void cancelTouchTimer(int pointerNumber) = 0 ;
	virtual void startHoldTimer(int delay, int pointerNumber) = 0 ;
	virtual void cancelHoldTimer(int pointerNumber) = 0 ;
	virtual void startSuppressTimer(int delay, int pointerNumber) = 0 ;
	virtual void cancelSuppressTimer(int pointerNumber) = 0 ;
};

}
#endif /* QSTMTIMERINTERFACE_H_ */
