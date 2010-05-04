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


#ifndef __IMAGELITEVIEW_H__
#define __IMAGELITEVIEW_H__

#include <QObject>
#include <QAction>
#include "ImageView.h"
#include "controllableview.h"

class QWebHistory;

namespace WRT {

    class WrtPageManager;

    class BWF_EXPORT ImageLiteView : public ImageView
    {
        Q_OBJECT
    public:
        ImageLiteView(WrtPageManager * mgr, QWidget* parent);
        ImageLiteView(WrtPageManager * mgr, QGraphicsWidget* parent);
        static const QString Type() { return "ImageLiteView"; }
        virtual QString type() const { return Type(); }

    public slots:
        void activate();
        void setCenterIndex(int i);
    };

}
#endif // __IMAGELITEVIEW_H__
