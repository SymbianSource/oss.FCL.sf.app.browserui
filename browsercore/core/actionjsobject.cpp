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

#include "actionjsobject.h"

#include <QDebug>

ActionJSObject::ActionJSObject(QObject *parent, QAction *action) 
    : QObject(parent),
      m_action(action)
{
    connect(m_action, SIGNAL(triggered(bool)), this, SIGNAL(triggered(bool)));
    connect(m_action, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));
    connect(m_action, SIGNAL(changed()), this, SIGNAL(changed()));
    
    // Copy object name of action.
    setObjectName(action->objectName());
}

void ActionJSObject::initClass() {   // static
    qMetaTypeId<QObjectList>();
    qRegisterMetaType<QObjectList>("QObjectList");

    qMetaTypeId<ActionJSObject*>();
    qRegisterMetaType<ActionJSObject>("ActionJSObject*");
}

void ActionJSObject::trigger() { 
    if(m_action) {
        qDebug() << "ActionJSObject::trigger: " << m_action->text(); 
        m_action->trigger(); 
    }
}
