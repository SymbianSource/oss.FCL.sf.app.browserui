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




#ifndef HISTORYVIEWCONTEXT_H_
#define HISTORYVIEWCONTEXT_H_

#include <QPoint>
#include <QObject>

#include "BWFGlobal.h"
#include "ViewEventContext.h"


namespace WRT
{

    /// The HistoryViewEventContext class contains information about a context menu, 
    /// or long-press, event that has occurred in a HistoryView.
    class BWF_EXPORT HistoryViewEventContext : public ViewEventContext {
        Q_OBJECT
      public:
        HistoryViewEventContext();
        HistoryViewEventContext(const QString &viewType, 
                                const QPoint pos, 
                                int index, 
                                const QString &title);
        
        HistoryViewEventContext(const HistoryViewEventContext &o);
        ~HistoryViewEventContext() {}

        QString getItemTitle() const { return m_itemTitle; }
        Q_PROPERTY(QString itemTitle READ getItemTitle)

        int getItemIndex() const { return m_itemIndex; }
        Q_PROPERTY(int itemIndex READ getItemIndex)
        
        // May also need a flag to indicate whether the event occured over the center item,
        // over no item, or over a side item.
        
      public:
        QString m_itemTitle;
        int m_itemIndex;
    };

}

#endif /* HISTORYVIEWCONTEXT_H_ */
