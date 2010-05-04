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


#ifndef UTILITIES_H
#define UTILITIES_H

#include <assert.h>

// Throw an assert if the connection fails.
#define safe_connect(src, srcSig, target, targetSlot) { if(!connect(src, srcSig, target, targetSlot)) assert(0); }

#endif // UTILITIES_H
