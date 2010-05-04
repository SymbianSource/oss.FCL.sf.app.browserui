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


#ifndef __WEBZOOMSLIDER_H__
#define __WEBZOOMSLIDER_H__

#include <QSlider>
#include <QTimer>
#include "BWFGlobal.h"


namespace WRT {
    
#define ZOOMSLIDER_IDLE_TIMEOUT 3000

    // New view api has wrt page manager, and we export this control to make it accessible
    class WrtPageManager;
    class WebZoomSliderPrivate;

    class BWF_EXPORT WebZoomSlider : public QObject
    {
        Q_OBJECT
    public:
        WebZoomSlider(WrtPageManager* pageMgr, QWidget* parent);
        ~WebZoomSlider();

        void setGeometry(const QRect& rect);
        void show(int idleTimeout = ZOOMSLIDER_IDLE_TIMEOUT);

    public Q_SLOTS:
        void zoom(int value);
        void sliderPressed();
        void sliderReleased();

    signals:
        void idleTimeout();

    private:
        WebZoomSliderPrivate * const d;
    };

}
#endif
