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

#include "downloadcore.h"
#include "downloadcoremanager.h"
#include <QVector>

// private implementation class
class DownloadCoreManagerPrivate
{
    DM_DECLARE_PUBLIC(DownloadCoreManager);
public:
    DownloadCoreManagerPrivate();
    ~DownloadCoreManagerPrivate();

    QVector<DownloadCore*> m_downloadCores;
    QString m_clientName;
};

DownloadCoreManagerPrivate::DownloadCoreManagerPrivate()
{
    m_clientName = "";
}

DownloadCoreManagerPrivate::~DownloadCoreManagerPrivate()
{
    int totalSize = m_downloadCores.size();
    for( int i=0; i < totalSize; i++)
    {
       delete m_downloadCores[i];
    }
    m_downloadCores.clear();
}

DownloadCoreManager::DownloadCoreManager(const QString &clientName)
{
    DM_INITIALIZE(DownloadCoreManager);
    priv->m_clientName = clientName;
}

DownloadCoreManager::~DownloadCoreManager()
{
   DM_UNINITIALIZE(DownloadCoreManager);
}

DownloadCore* DownloadCoreManager::createDownloadCore(const QString &aUrl)
{
    DM_PRIVATE(DownloadCoreManager);
    // create download core by url
    DownloadCore* dlCore = new DownloadCore(aUrl);
    (priv->m_downloadCores).append(dlCore);
    return dlCore;     
}

DownloadCore* DownloadCoreManager::createDownloadCore(QNetworkReply *reply)
{
    DM_PRIVATE(DownloadCoreManager);
    // create download core by network reply
    DownloadCore* dlCore = new DownloadCore(reply);
    (priv->m_downloadCores).append(dlCore);
    return dlCore;     
}

QString& DownloadCoreManager::clientName(void)
{
    DM_PRIVATE(DownloadCoreManager);
    return priv->m_clientName;
}
