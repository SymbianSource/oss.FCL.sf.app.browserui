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


#ifndef __WINDOWFLOWVIEW_H__
#define __WINDOWFLOWVIEW_H__

#include <QObject>
#include <QAction>
#include "WindowView.h"
#include "controllableview.h"

class QWebWindow;

namespace WRT {

    class WrtPageManager;

    class BWF_EXPORT WindowFlowView : public WindowView
    {
        Q_OBJECT
    public:
        WindowFlowView(WrtPageManager * mgr, QWidget* parent,const QString& aType);
        WindowFlowView(WrtPageManager * mgr, QGraphicsWidget* parent,const QString& aType);
        virtual QString type() const { return m_type; }

        static const QString Type() { return "WindowView" ;}
        static ControllableView  *createNew(QWidget* parent);
        static ControllableViewBase  *createNew(QGraphicsWidget* parent);

    public slots:
        void activate();
    protected:
        QString m_type;
    };

}
#endif // __WINDOWFLOWVIEW_H__
