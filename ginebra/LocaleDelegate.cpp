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


#include <QString>
//#include <QLocale>
#include "chromeview.h"
#include "LocaleDelegate.h"

#include <qdebug.h>

const QString JS_PREFIX = "txt_browser_";

LocaleDelegate::LocaleDelegate(ChromeView* chrome)
{   
    setObjectName("localeDelegate");
}

LocaleDelegate::~LocaleDelegate()
{      
}

/*
* Load the localized string based on the string id
*/
QString LocaleDelegate::translateText(const QString & textToTranlsate) 
{
    QString ret = tr((JS_PREFIX + textToTranlsate).toLatin1());
    qDebug() << "LocaleDelegate::translateText "  << ret;
    return ret;
}
