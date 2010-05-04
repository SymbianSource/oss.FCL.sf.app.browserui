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


#ifndef __PAGEZOOMCONTROL_H__
#define __PAGEZOOMCONTROL_H__

#include <QObject>
#include "BWFGlobal.h"

namespace WRT {

    class WrtPageManager; 
    class PageZoomControlPrivate;

    class BWF_EXPORT PageZoomControl : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(qreal zoomLevel READ zoomLevel)
        Q_PROPERTY(qreal maxZoomLevel READ maxZoomLevel)
        Q_PROPERTY(qreal minZoomLevel READ minZoomLevel)
        Q_PROPERTY(qreal isUserScalable READ isUserScalable)
        Q_PROPERTY(qreal zoomStep READ zoomStep)

    public:
        PageZoomControl(WrtPageManager *mgr);
        ~PageZoomControl();

        qreal zoomLevel();
        qreal maxZoomLevel(); 
        qreal minZoomLevel(); 
        bool isUserScalable();
        qreal zoomStep();

    signals:
        void zoomLevelChanged(qreal zoomLevel);

    public slots:
        void zoomIn();
        void zoomOut();
	
    private:
        PageZoomControlPrivate* d;
    };
}
#endif //__PAGEZOOMCONTROL_H__