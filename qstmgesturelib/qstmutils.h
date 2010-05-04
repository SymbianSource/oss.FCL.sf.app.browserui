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


#ifndef _GESTURE_UTILS_H_
#define _GESTURE_UTILS_H_

#include <QtCore>
#include <QtGui>
#ifdef Q_OS_SYMBIAN
#include <e32math.h>
#define SQRT(out,x) Math::Sqrt(out,x)
#else
#define SQRT(out,x) (out = sqrt(x))
#endif

#define MAX(a,b) ((a>=b) ? a : b)
#define MIN(a,b) ((a<=b) ? a : b)
#define ABS(x)   ((x>=0) ? x : -x)

namespace qstmUiEventEngine
{

    class QStm_Utils
    {
    public:
        static QRect  toleranceRectMm( const QPoint& aCenterPoint, int size_mm );
        static QRect  toleranceRectPx( const QPoint& aCenterPoint, int size_px );
        static long   mm2Pixels(long mm);
        static long   inches2Pixels(double inches);
        static qreal  distanceF(const QPoint& p1, const QPoint& p2);
        static int    distance(const QPoint& p1, const QPoint& p2);
        static int    longerEdge( const QSize& size );
    };
}

#endif // _GESTURE_UTILS_H_
