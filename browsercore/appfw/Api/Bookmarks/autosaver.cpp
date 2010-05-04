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


#include "autosaver.h"

#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QtCore/QMetaObject>
#include <QtDebug>

#define AUTOSAVE_IN   10000    // 10 Sec
#define MAXWAIT       30000  //  30 sec

namespace WRT {

AutoSaver::AutoSaver(QObject *parent) : QObject(parent)
{
    Q_ASSERT(parent);
}

AutoSaver::~AutoSaver()
{
    if (m_timer.isActive())
        qWarning() << "AutoSaver: still active when destroyed, changes not saved.";
}

void AutoSaver::changeOccurred()
{
    if (m_firstChange.isNull())
        m_firstChange.start();

    if (m_firstChange.elapsed() > MAXWAIT) {
        saveIfNeccessary();
    } else {
        m_timer.start(AUTOSAVE_IN, this);
    }
}

void AutoSaver::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timer.timerId()) {
            saveIfNeccessary();
    }
    else {
            QObject::timerEvent(event);
    }
}

BookmarkAutoSaver::BookmarkAutoSaver(QObject *parent) : AutoSaver(parent)
{

}

BookmarkAutoSaver::~BookmarkAutoSaver()
{
}

void BookmarkAutoSaver::saveIfNeccessary()
{
    if (!m_timer.isActive())
        return;
    
    m_timer.stop();
    
    m_firstChange = QTime();
    
    if (!QMetaObject::invokeMethod(parent(), "saveBookMarks", Qt::DirectConnection)) {
        qWarning() << "AutoSaver: error invoking slot saveHistory() on parent";
    }
}

HistoryAutoSaver::HistoryAutoSaver(QObject *parent) : AutoSaver(parent)
{

}

HistoryAutoSaver::~HistoryAutoSaver()
{
}

void HistoryAutoSaver::saveIfNeccessary()
{
    if (!m_timer.isActive())
        return;
    
    m_timer.stop();
    
    m_firstChange = QTime();
    
    if (!QMetaObject::invokeMethod(parent(), "saveHistory", Qt::DirectConnection)) {
        qWarning() << "AutoSaver: error invoking slot saveHistory() on parent";
    }
}

}


