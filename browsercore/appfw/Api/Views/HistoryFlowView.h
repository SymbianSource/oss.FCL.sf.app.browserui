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


#ifndef __HISTORYFLOWVIEW_H__
#define __HISTORYFLOWVIEW_H__

#include <QObject>
#include <QAction>
#include "HistoryView.h"

class QWebHistory;

namespace WRT {

    class WrtPageManager;

    class BWF_EXPORT HistoryFlowView : public HistoryView
    {
        Q_OBJECT
    public:
        HistoryFlowView(WrtPageManager * mgr, QWidget* parent);
        HistoryFlowView(WrtPageManager * mgr, QGraphicsWidget* parent);
        QImage getCurrentSlide();
//        static const QString Type() { return "HistoryFlowView"; }
        static const QString Type() { return "historyView"; }
        virtual QString type() const { return Type(); }

        static ControllableView *createNew(QWidget *parent);

    public slots:
        void activate();
        void setCenterIndex(int i);
    };

}
#endif // __HISTORYFLOWVIEW_H__
