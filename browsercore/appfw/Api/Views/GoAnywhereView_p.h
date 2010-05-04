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


#ifndef __GOANYWHEREVIEW_P_H__
#define __GOANYWHEREVIEW_P_H__

#include <QAction>
#include <QTabWidget>
#include <QVector>

class QWidget;
class ControllableView;

namespace WRT {
    class WrtPage;
    class WrtPageManager;
    class GoAnywhereWidget;
    class GoAnywhereView;
    
    class GoAnywhereTabWidget : public QTabWidget
    {
    public:
        GoAnywhereTabWidget(QWidget* parent) : QTabWidget(parent) {};
        virtual ~GoAnywhereTabWidget() {};
        QTabBar* getTabBar() const { return tabBar(); }
    };
    
    class GoAnywhereViewPrivate
    {
    public:
        GoAnywhereViewPrivate(WrtPageManager*, QWidget*, GoAnywhereView*,const QString& aType);
        ~GoAnywhereViewPrivate() {}
        
        QWidget* m_widgetParent;
        QAction * m_actionBack;
        GoAnywhereTabWidget* m_goAnywhereWidget;
        bool m_isActive;
        WrtPageManager * m_pageManager; // not owned
        QVector<ControllableView*> m_views;
        QVector<QString> m_viewIds;
        int m_currentViewIndex;
        QString m_type;
    };
}
#endif // __GOANYWHEREVIEW_P_H__
