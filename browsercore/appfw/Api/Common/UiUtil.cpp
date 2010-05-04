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


#include <QRegExp>
#include "UiUtil.h"

#define GOOGLE_SEARCH_ENGINE "http://www.google.com/search?q="

namespace WRT {

QString UiUtil::loadFiletoString(const QString &name)
{
    QFile file(name);
    file.open(QFile::ReadOnly);
    QString fileString = QLatin1String(file.readAll());
    file.close();
    return fileString;
}

QUrl UiUtil::searchUrl(const QString &string)
{
	QString urlStr = string.trimmed();
	QUrl url(QLatin1String(GOOGLE_SEARCH_ENGINE) + urlStr, QUrl::TolerantMode);
    if(url.isValid())
		return url;
}

QUrl UiUtil::guessUrlFromString(const QString &string)
{
    QString urlStr = string.trimmed();
    QRegExp schemaTest(QLatin1String("^[a-zA-Z]+\\:.*"));

    // Check if it looks like a qualified URL. Try parsing it and see.
    bool hasSchema = schemaTest.exactMatch(urlStr);
    bool hasSpaces = urlStr.contains(" ");
    if (hasSchema) {
        if(!hasSpaces){
			QUrl url(urlStr, QUrl::TolerantMode);
			if (url.isValid())
				return url;
		}
		else{
			if(urlStr.contains("https://")){
				urlStr.remove("https://" ,Qt::CaseInsensitive);
			}
			else if(urlStr.contains("http://")){
				urlStr.remove("http://" ,Qt::CaseInsensitive);
			}
			else if(urlStr.contains("rtsp://")){
				urlStr.remove("rtsp://" ,Qt::CaseInsensitive);
			}
			QUrl url(QLatin1String(GOOGLE_SEARCH_ENGINE) + urlStr, QUrl::TolerantMode);
			if(url.isValid())
				return url;
		}
    }
    // Might be a file.
    if (QFile::exists(urlStr))
        return QUrl::fromLocalFile(urlStr);

    // Might be a shorturl - try to detect the schema.
    if (!hasSchema) {
        int dotIndex = urlStr.indexOf(QLatin1Char('.'));
        if (dotIndex != -1 && !hasSpaces) {
            QUrl url;
            urlStr.endsWith(".") ? ( url.setUrl(QLatin1String("http://") + urlStr + QLatin1String("com"), QUrl::TolerantMode) ) : ( url.setUrl(QLatin1String("http://") + urlStr, QUrl::TolerantMode) );
            if (url.isValid())
                return url;
        }
        //The string parameter is simple text and a search should be performed.
		else {
            QUrl url(QLatin1String(GOOGLE_SEARCH_ENGINE) + urlStr, QUrl::TolerantMode);
            if(url.isValid())
                return url;
        }
    }

    // Fall back to QUrl's own tolerant parser.
    return QUrl(string, QUrl::TolerantMode);
}

}
