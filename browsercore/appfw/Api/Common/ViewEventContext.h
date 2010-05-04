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


#ifndef VIEWEVENTCONTEXT_H_
#define VIEWEVENTCONTEXT_H_

#include <QPoint>
#include <QObject>

#include "BWFGlobal.h"

#include "scriptobjects.h"


    /*!
     * \brief Describes the context of a UI event in a ControllableView.  
     * The ViewEventContext class contains parameters that describe the context of a UI event 
     * that occurs in a ControllableView.  In the case of a long-press event, for example, it
     * would detail what link or image the user pressed.  These objects are visible to javascript.
     * \sa ControllableView
     */
    class BWF_EXPORT ViewEventContext : public QObject {
        Q_OBJECT
      public:
        ViewEventContext() {}
        ViewEventContext(const QString &viewType, const QPoint pos);
        ViewEventContext(const ViewEventContext &o);
        ~ViewEventContext() {}
                              
        // The geometric position of the event (relative to the parent widget). 
        QObject *getPos() { return static_cast<QObject*>(&m_pos); }
        Q_PROPERTY(QObject * pos READ getPos)
                
        // The type of the view in which the event occured.
        // \sa ControllableView::type()
        QString getViewType() const { return m_viewType; }
        Q_PROPERTY(QString viewType READ getViewType)
        
      protected:
        QPoint getPosition() const { return m_pos; }
        
      protected:
        QString m_viewType;
        ScriptPoint m_pos;        
    };

#endif /* VIEWEVENTCONTEXT_H_ */

