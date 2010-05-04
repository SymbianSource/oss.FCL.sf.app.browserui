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


#include <QString>
#include <QFile>
#include <QUrl>
#include "BWFGlobal.h"

namespace WRT {
    class BWF_EXPORT UiUtil
    {
    public:
        static QString loadFiletoString(const QString &name);
        static QUrl guessUrlFromString(const QString &string);
		static QUrl searchUrl(const QString &string);
    };
}
