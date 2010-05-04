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



#ifndef AUTOSAVER_H
#define AUTOSAVER_H

#include <QtCore/QObject>
#include <QtCore/QBasicTimer>
#include <QtCore/QTime>
#include "wrttypes.h"

namespace WRT {

/*
    This class will call the save() slot on the parent object when the parent changes.
    It will wait several seconds after changed() to combining multiple changes and
    prevent continuous writing to disk.
  */
class AutoSaver : public QObject {

Q_OBJECT

public:
    AutoSaver(QObject *parent);
    ~AutoSaver();
    virtual void saveIfNeccessary()=0;
  
public slots:
    void changeOccurred();
    
protected:
    void timerEvent(QTimerEvent *event);

protected:
    QBasicTimer m_timer;
    QTime m_firstChange;
};

class BookmarkAutoSaver : public AutoSaver {

Q_OBJECT

public:
     BookmarkAutoSaver(QObject *parent);
    ~BookmarkAutoSaver();
     void saveIfNeccessary();
};


class HistoryAutoSaver : public AutoSaver {

Q_OBJECT

public:
     HistoryAutoSaver(QObject *parent);
    ~HistoryAutoSaver();
    void saveIfNeccessary();
};

}
#endif // AUTOSAVER_H

