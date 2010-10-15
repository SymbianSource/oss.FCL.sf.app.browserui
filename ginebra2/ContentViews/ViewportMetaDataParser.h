/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ViewportMetaDataParser_h
#define ViewportMetaDataParser_h

#include "ViewportMetaData.h"

namespace GVA {

class ViewportMetaDataParser : public QObject {
public:
    ViewportMetaDataParser(const QRect& clientRect);
    ~ViewportMetaDataParser();

    void parse(const QString& viewportParams, ViewportMetaData& viewportMetaData);

private:
    void adjustViewportData(ViewportMetaData& viewportMetaData);
    bool isSeparator(QChar c);
    void processArguments(const QString& features, ViewportMetaData& viewportMetaData);
    void setViewportFeature(const QString& keyString, const QString& valueString, ViewportMetaData& viewportMetaData);

    QRect m_clientRect;
};

} //namespace GVA

#endif //ViewportMetaDataParser_h
