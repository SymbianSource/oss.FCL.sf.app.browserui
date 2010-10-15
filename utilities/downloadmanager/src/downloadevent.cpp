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

#include "downloadevent.h"

// private implementation for DownloadEvent
class DownloadEventPrivate
{
    DM_DECLARE_PUBLIC(DownloadEvent);
public:
    DownloadEventPrivate();
    ~DownloadEventPrivate();
    DlEventAttributeMap *m_attrMap; // hold <key, value> pairs
    DEventType m_type;
    int m_id;
};

DownloadEventPrivate::DownloadEventPrivate()
{
    m_attrMap = 0;
    m_id = -1;
}

DownloadEventPrivate::~DownloadEventPrivate()
{
    if(m_attrMap)
    {
        delete m_attrMap;
        m_attrMap = 0;
    }
} 

DownloadEvent::DownloadEvent(DEventType type, DlEventAttributeMap* attrMap, int id)
    :QEvent((Type)type)
{
    DM_INITIALIZE(DownloadEvent);
    priv->m_type = type;
    priv->m_attrMap = attrMap;
    priv->m_id = id;
}

DownloadEvent::~DownloadEvent()
{
    DM_UNINITIALIZE(DownloadEvent);
}

Q_DECL_EXPORT QVariant DownloadEvent::getAttribute(DownloadEventAttribute attr)
{
    DM_PRIVATE(DownloadEvent);
    bool exists = (priv->m_attrMap) && (priv->m_attrMap->contains(attr));
    if(exists)
    {
        return priv->m_attrMap->value(attr);
    }
    else
    {
        return QVariant();
    }
}

Q_DECL_EXPORT int DownloadEvent::getId()
{
    DM_PRIVATE(DownloadEvent);
    // returns the download id
    return priv->m_id;
}


// private implementation for DownloadManagerEvent   
class DownloadManagerEventPrivate
{
    DM_DECLARE_PUBLIC(DownloadManagerEvent);
public:
    DownloadManagerEventPrivate();
    ~DownloadManagerEventPrivate();
    DlManagerEventAttributeMap *m_attrMap;
    DEventType type;
};

DownloadManagerEventPrivate::DownloadManagerEventPrivate()
{
    m_attrMap = 0;
}

DownloadManagerEventPrivate::~DownloadManagerEventPrivate()
{
    if(m_attrMap)
    {
        delete m_attrMap;
        m_attrMap = 0;
    }
} 

DownloadManagerEvent::DownloadManagerEvent(DEventType type, DlManagerEventAttributeMap* attrMap)
    :QEvent((Type)type)
{
    DM_INITIALIZE(DownloadManagerEvent);
    priv->type = type;
    priv->m_attrMap = attrMap;
}

DownloadManagerEvent::~DownloadManagerEvent()
{
    DM_UNINITIALIZE(DownloadManagerEvent);
}

QVariant DownloadManagerEvent::getAttribute(DownloadManagerEventAttribute attr)
{
    DM_PRIVATE(DownloadManagerEvent);
    bool exists = priv->m_attrMap->contains(attr);
    if(exists)        
    {
        return priv->m_attrMap->value(attr);
    }
    else
    {
        return QVariant();
    }
}



