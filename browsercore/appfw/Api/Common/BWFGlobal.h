/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef BWFGLOBAL_H
#define BWFGLOBAL_H

#include <QtCore/qglobal.h>

#define BWF_ORGANIZATION_DOMAIN "com.nokia"
#define BWF_ORGANIZATION_NAME "Nokia"

// deprecated: inherit BrowserCore rules directly
//#define BWF_EXPORT WRT_BROWSER_EXPORT

// Define Export rules specifically for AppFW
// (deprecated: define symbian32 export explicitly as empty, here's how it all used to look:)
// #ifndef BWF_EXPORT
// # if defined (BUILDING_BWF_CORE)
// #  define BWF_EXPORT Q_DECL_EXPORT
// # else
// #  ifdef __SYMBIAN32__
// #    define BWF_EXPORT
// #  else
// #    define BWF_EXPORT Q_DECL_IMPORT
// #  endif
// # endif
// #endif

#ifndef BWF_EXPORT
# if defined (BUILDING_BWF_CORE)
#  define BWF_EXPORT Q_DECL_EXPORT
# else
#  define BWF_EXPORT Q_DECL_IMPORT
# endif
#endif

#endif // BWFGLOBAL_H

