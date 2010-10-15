/**
   This file is part of CWRT package **

   Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies). **

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU (Lesser) General Public License as 
   published by the Free Software Foundation, version 2.1 of the License. 
   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of 
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
   (Lesser) General Public License for more details. You should have 
   received a copy of the GNU (Lesser) General Public License along 
   with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DOWNLOADEVENT_H
#define DOWNLOADEVENT_H

#include <QEvent>
#include <QVariant>
#include "dmcommon.h"
#include "dmpimpl.h"

// forward declarations
class DownloadEventPrivate;
class DownloadManagerEventPrivate;

// typedef
typedef QMap<DownloadEventAttribute, QVariant> DlEventAttributeMap;
typedef QMap<DownloadManagerEventAttribute, QVariant> DlManagerEventAttributeMap;


// class declarations

// DownloadEvent for download events
class DownloadEvent : public QEvent
{
    DM_DECLARE_PRIVATE(DownloadEvent); // private implementation
public:
    DownloadEvent(DEventType type, DlEventAttributeMap* attrMap, int id);
    ~DownloadEvent();

    // to fetch the download event related attributes
    Q_DECL_EXPORT QVariant getAttribute(DownloadEventAttribute attr);
    // to fetch the id of download object which is the sender of this event
    Q_DECL_EXPORT int getId();
};  

// DownloadManagerEvent for download manager events
class DownloadManagerEvent : public QEvent
{
    DM_DECLARE_PRIVATE(DownloadManagerEvent); // private implementation
public:
    DownloadManagerEvent(DEventType type, DlManagerEventAttributeMap* attrMap);
    ~DownloadManagerEvent();

// to fetch the download manager event related attributes
    QVariant getAttribute(DownloadManagerEventAttribute attr);
};
    
#endif //DOWNLOADEVENT_H  
