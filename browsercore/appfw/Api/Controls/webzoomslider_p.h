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


#ifndef __WEBZOOMSLIDER_P_H__
#define __WEBZOOMSLIDER_P_H__

#include <QSlider>
#include <QTimer>
#include "BWFGlobal.h"

namespace WRT {

    class WrtPageManager;
    class WebZoomSliderPrivate;

    class WebZoomSliderPrivate
    {     
    public:
        WebZoomSliderPrivate(WrtPageManager* pageMgr, QWidget* parent);
        ~WebZoomSliderPrivate();
        
        WrtPageManager    *m_pageMgr;
        QSlider           *m_slider; // owned
        QTimer            *m_timer;
        QWidget           *m_widgetParent;

        int m_idleTimeout;
        bool m_dragging;
    };
}
#endif //__WEBZOOMSLIDER_P_H__
