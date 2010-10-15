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

#ifndef __WEBPAGEDATA_H__
#define __WEBPAGEDATA_H__

#include <QImage>
#include <QPoint>
#include <QMetaType>
#include "BWFGlobal.h"

class BWF_EXPORT WebPageData
{
static const unsigned int DataMagic = 0x5AFEF00D;

public:
    WebPageData() :
                    magic(0) // default constructor should create valid item
            ,       minScale(1.0)
            ,       maxScale(1.0)
            ,       userScalable(true)
            ,       initialScale(1.0)
            ,       rect(QRectF())
            ,       webViewRect(QRectF())
            ,       scale(1.0)
            ,       viewportSize(QSizeF())
            ,       specifiedWidth(QString())
            ,       specifiedHeight(QString())
            ,       fitToScreen(0)
    {};

    WebPageData(qreal aMaxScale, qreal aMinScale, qreal aUserScalable, qreal aInitialScale) :
                    magic(DataMagic)
            ,       minScale(aMinScale)
            ,       maxScale(aMaxScale)
            ,       userScalable(aUserScalable)
            ,       initialScale(aInitialScale)
            ,       rect(QRectF())
            ,       webViewRect(QRectF())
            ,       scale(1.0)
            ,       viewportSize(QSizeF())
            ,       specifiedWidth(QString())
            ,       specifiedHeight(QString())
            ,       fitToScreen(0)
    {};

    WebPageData(qreal aMaxScale, qreal aMinScale, qreal aUserScalable, qreal aInitialScale, const QRectF& aRect, const QRectF& aWebViewRect,
                qreal aScale, const QSizeF& aViewportSize, const QString& aSpecifiedWidth, const QString& aSpecifiedHeight, bool aFitToScreen) :
                    magic(DataMagic)
            ,       minScale(aMinScale)
            ,       maxScale(aMaxScale)
            ,       userScalable(aUserScalable)
            ,       initialScale(aInitialScale)
            ,       rect(aRect)
            ,       webViewRect(aWebViewRect)
            ,       scale(aScale)
            ,       viewportSize(aViewportSize)
            ,       specifiedWidth(aSpecifiedWidth)
            ,       specifiedHeight(aSpecifiedHeight)
            ,       fitToScreen(aFitToScreen)
    {};

    WebPageData(const WebPageData& o) : //: QObject()
                    magic(o.magic)
            ,       minScale(o.minScale)
            ,       maxScale(o.maxScale)
            ,       userScalable(o.userScalable)
            ,       initialScale(o.initialScale)
            ,       rect(o.rect)
            ,       webViewRect(o.webViewRect)
            ,       scale(o.scale)
            ,       viewportSize(o.viewportSize)
            ,       specifiedWidth(o.specifiedWidth)
            ,       specifiedHeight(o.specifiedHeight)
            ,       fitToScreen(o.fitToScreen)
    {};

    bool isValid() const { return magic == DataMagic; }

    unsigned int magic;
    qreal minScale;
    qreal maxScale;
    bool userScalable;
    qreal initialScale;
    QRectF rect;
    QRectF webViewRect;
    qreal scale;
    QSizeF viewportSize;
    QString specifiedWidth;
    QString specifiedHeight;
    bool fitToScreen;
};
Q_DECLARE_METATYPE (WebPageData)
QDataStream &operator<<(QDataStream &out, const WebPageData &myObj);
QDataStream &operator>>(QDataStream &in, WebPageData &myObj);
#endif
