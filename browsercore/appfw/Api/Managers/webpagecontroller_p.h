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


#ifndef __WRTPAGEMANAGER_P_H__
#define __WRTPAGEMANAGER_P_H__

#include <QWidget>
#include <QAction>
#include "BWFGlobal.h"

#define WEBPAGE_ZOOM_RANGE_MIN 25
#define WEBPAGE_ZOOM_RANGE_MAX 300
#define WEBPAGE_ZOOM_PAGE_STEP 20

namespace WRT {
    class WrtBrowserContainer;
    class SecContext;
}

class WebPageController;

class WebPageControllerPrivate
{
public: // public actions available for this view
    QAction * m_actionReload;
    QAction * m_actionStop;
    QAction * m_actionBack;
    QAction * m_actionForward;
    QAction * m_actionWebInspector;

public:
    WebPageControllerPrivate(WebPageController* qq);
    ~WebPageControllerPrivate();

    WebPageController * const q;

    QObject* m_widgetParent;
    QList<WRT::WrtBrowserContainer*> m_allPages;
    int m_currentPage;
    WRT::SecContext * m_secContext;
    QString m_historyDir;
    QObject* m_actionsParent;    
    bool donotsaveFlag;
};
#endif // __WRTPAGEMANAGER_P_H__
