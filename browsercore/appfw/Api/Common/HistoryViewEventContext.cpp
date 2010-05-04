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


#include "HistoryViewEventContext.h"
#include <QWebHitTestResult>
#include <QWebElement>

namespace WRT
{

static const QString s_ObjectName = "historyViewEventContext";

HistoryViewEventContext::HistoryViewEventContext()
{
    setObjectName(s_ObjectName);
}

HistoryViewEventContext::HistoryViewEventContext(const QString &viewType, 
                                                 const QPoint pos, 
                                                 int index, 
                                                 const QString &title) 
  : ViewEventContext(viewType, pos),
    m_itemTitle(title),
    m_itemIndex(index) 
{
    setObjectName(s_ObjectName);
}

HistoryViewEventContext::HistoryViewEventContext(const HistoryViewEventContext &o)
  : ViewEventContext(o.getViewType(), o.getPosition()),
    m_itemTitle(o.getItemTitle()),
    m_itemIndex(o.getItemIndex())
{
    setObjectName(o.objectName());
}


}
