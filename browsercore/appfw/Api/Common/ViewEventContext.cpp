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


#include "ViewEventContext.h"


#include "ViewEventContext.h"
#include <QWebHitTestResult>
#include <QWebElement>

static const QString s_ObjectName = "viewEventContext";

ViewEventContext::ViewEventContext(const QString &viewType, const QPoint pos)
  : m_viewType(viewType), 
    m_pos(pos)
{
    setObjectName(s_ObjectName);
    m_pos.setParent(this);
}

ViewEventContext::ViewEventContext(const ViewEventContext &o)
  : m_viewType(o.getViewType()), 
    m_pos(o.getPosition())  
{
    setObjectName(o.objectName());
    m_pos.setParent(this);
}


