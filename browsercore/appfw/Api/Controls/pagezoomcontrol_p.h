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


#ifndef __PAGEZOOMCONTROL_P_H__
#define __PAGEZOOMCONTROL_P_H__

#include <QTimer>

namespace WRT {

    class WrtPageManager; 

    class PageZoomControlPrivate : public QObject
    {     
        Q_OBJECT

    public:
        enum ZoomDirection{zoomOut = -1, zoomReset = 0, zoomIn = 1};

        PageZoomControlPrivate(WrtPageManager *mgr);
        ~PageZoomControlPrivate();

    public slots:
        void smoothZoomIn();
        void smoothZoomOut();   

    public:
        void createZoomTimer(ZoomDirection direction);
        void deleteTimer();

    private:
        void setZoom(ZoomDirection direction);

    public:
        WrtPageManager          *m_mgr; 
        qreal                   m_value;
        QTimer                  *m_smoothZoomTimer;
    };
}
#endif    //__PAGEZOOMCONTROL_P_H__
