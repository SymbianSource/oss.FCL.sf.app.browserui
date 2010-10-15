/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
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
        static QString removeScheme(const QString & str);
        static QUrl encodeQueryItems(QUrl& url, bool encodeNonAscii);
    private:
        static QUrl searchUrlWithoutCheck(const QString &string);
    };
}
