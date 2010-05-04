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


#ifndef __BRTGLOBAL_H_
#define __BRTGLOBAL_H_

#include <QtCore/qglobal.h>
#include <QWidget>
#include <QVariant>

#define WRT_ORGANIZATION_DOMAIN "com.nokia"
#define WRT_ORGANIZATION_NAME "Nokia"

#ifndef WRT_BROWSER_EXPORT
# if defined (BUILDING_BROWSER_CORE)
#  define WRT_BROWSER_EXPORT Q_DECL_EXPORT
# else
#  define WRT_BROWSER_EXPORT Q_DECL_IMPORT
# endif

#endif


#endif // QWRTGLOBAL_H

