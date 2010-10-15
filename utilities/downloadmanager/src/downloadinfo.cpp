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

#include "downloadinfo.h"
#include "dmcommon.h"
#include <QStringList>
#include <QSettings>
#define ORGANIZATION "Nokia"

class DownloadInfoPrivate
{
    DM_DECLARE_PUBLIC(DownloadInfo);
public:
    DownloadInfoPrivate();
    ~DownloadInfoPrivate();

    QSettings* m_dlInfo;
    QString m_clientName;

};

DownloadInfoPrivate::DownloadInfoPrivate()
{
    m_dlInfo = 0;
    m_clientName = "";
}

DownloadInfoPrivate::~DownloadInfoPrivate()
{
    if(m_dlInfo)
    {
         m_dlInfo->sync();
         delete m_dlInfo;
         m_dlInfo = 0;
    }
}

/*
Constructor
*/
DownloadInfo::DownloadInfo(const QString& clientName)
{
    DM_INITIALIZE(DownloadInfo);
    priv->m_clientName = clientName;
    priv->m_dlInfo = new QSettings(ORGANIZATION, clientName);
}

/*
Destructor
*/
DownloadInfo::~DownloadInfo()
{
    DM_UNINITIALIZE(DownloadInfo);
}

/*
Sets a string value. Gives option to updte the info file immediately or later
Returns : 0 on success, non zero on error
*/
int DownloadInfo::setValue(int aDlId, Key aKey, const QString& aStrValue, int aParentId /*= INVALID_DL_ID*/)
{
    DM_PRIVATE(DownloadInfo);
    if(aParentId > INVALID_DL_ID)
        priv->m_dlInfo->setValue(genStrKey(aParentId, aDlId, aKey), aStrValue);
    else
        priv->m_dlInfo->setValue(genStrKey(aDlId, aKey), aStrValue);
    return 0;
}

/*
Sets a string value. Gives option to updte the info file immediately or later
Returns : 0 on success, non zero on error
*/
int DownloadInfo::setValueForChild(int aDlId, Key aKey, const QString& aStrValue, int aChildId /*= INVALID_DL_ID*/)
{
    DM_PRIVATE(DownloadInfo);
    if(aChildId > INVALID_DL_ID)
        priv->m_dlInfo->setValue(genStrKey(aDlId, aChildId, aKey), aStrValue);
    else
        priv->m_dlInfo->setValue(genStrKey(aDlId, aKey), aStrValue);
    return 0;
}

/*
Sets an int value. Gives option to updte the info file immediately or later
Returns : 0 on success, non zero on error
*/
int DownloadInfo::setValue(int aDlId, Key aKeyInt, long aLongValue, int aParentId /*= INVALID_DL_ID*/)
{
    DM_PRIVATE(DownloadInfo);
    QString strKey;
    if(aParentId > INVALID_DL_ID)
        strKey = genStrKey(aParentId, aDlId, aKeyInt);
    else
        strKey = genStrKey(aDlId, aKeyInt);
    QVariant v((int)aLongValue);
    priv->m_dlInfo->setValue(strKey, v);
    return 0;
}

/*
Sets an int value. Gives option to updte the info file immediately or later
Returns : 0 on success, non zero on error
*/
int DownloadInfo::setValueForChild(int aDlId, Key aKeyInt, long aLongValue, int aChildId /*= INVALID_DL_ID*/)
{
    DM_PRIVATE(DownloadInfo);
    QString strKey;
    if(aChildId > INVALID_DL_ID)
        strKey = genStrKey(aDlId, aChildId, aKeyInt);
    else
        strKey = genStrKey(aDlId, aKeyInt);
    QVariant v((int)aLongValue);
    priv->m_dlInfo->setValue(strKey, v);
    return 0;
}

/*
Sets media object list belonging to the parent. Gives option to updte the info file immediately or later
Returns : 0 on success, non zero on error
*/
int DownloadInfo::setValue(int aDlId, Key aKey, const QList<QVariant>& aChildIds)
{
    DM_PRIVATE(DownloadInfo);
    QString strKey = genStrKey(aDlId, aKey);
    QVariant v(aChildIds);
    priv->m_dlInfo->setValue(strKey, v);
    return 0;
}

/*
Updates(serializes) the info with all set values at once.
Returns : 0 on success, non zero on error
*/
int DownloadInfo::update()
{
    DM_PRIVATE(DownloadInfo);
    priv->m_dlInfo->sync();
    return 0;
}

/*
Deletes the download info of a particular download represented by aDlId.
Returns : 0 on success, non zero on error
*/
int DownloadInfo::remove(int aDlId, int aParentId /*= INVALID_DL_ID*/)
{
    DM_PRIVATE(DownloadInfo);
    QString strDlId;
    strDlId = priv->m_clientName;
    strDlId.append("/");

    QString str;
    if(aParentId > INVALID_DL_ID)
    {
        str.setNum(aParentId);
        strDlId.append(str);
        strDlId.append("/");
    }

    str.setNum(aDlId);
    strDlId.append(str);
    priv->m_dlInfo->remove(strDlId);
    return 0;
}

/*
Retrieves the string value
Returns : 0 on success, non zero on error
*/
int DownloadInfo::getValue(int aDlId, Key aKeyStr, QString& aStrValue, int aParentId)
{
    DM_PRIVATE(DownloadInfo);
    aStrValue = "";
    QString strDlId;
    if (aParentId > INVALID_DL_ID)
        strDlId = genStrKey(aParentId, aDlId, aKeyStr);
    else
        strDlId = genStrKey(aDlId, aKeyStr);

    if(priv->m_dlInfo->contains(strDlId))
    {
        aStrValue = priv->m_dlInfo->value(strDlId).toString();
        return 0;
    }
    return -1;
}

/*
Retrieves the string value
Returns : 0 on success, non zero on error
*/
int DownloadInfo::getValueForChild(int aDlId, Key aKeyStr, QString& aStrValue, int aChildId /*= INVALID_DL_ID*/)
{
    DM_PRIVATE(DownloadInfo);
    aStrValue = "";
    QString strDlId;
    if (aChildId > INVALID_DL_ID)
        strDlId = genStrKey(aDlId, aChildId, aKeyStr);
    else
        strDlId = genStrKey(aDlId, aKeyStr);

    if(priv->m_dlInfo->contains(strDlId))
    {
        aStrValue = priv->m_dlInfo->value(strDlId).toString();
        return 0;
    }
    return -1;
}

/*
Retrieves the int value
Returns : 0 on success, non zero on error
*/
int DownloadInfo::getValue(int aDlId, Key aKeyInt, long& aLongValue, int aParentId)
{
    DM_PRIVATE(DownloadInfo);
    QString strDlId;
    if(aParentId > INVALID_DL_ID)
        strDlId = genStrKey(aParentId, aDlId, aKeyInt);
    else
        strDlId = genStrKey(aDlId, aKeyInt);

    if(priv->m_dlInfo->contains(strDlId))
    {
        aLongValue = priv->m_dlInfo->value(strDlId).toInt();
        return 0;
    }
    return -1;
}

/*
Retrieves the int value
Returns : 0 on success, non zero on error
*/
int DownloadInfo::getValueForChild(int aDlId, Key aKeyInt, long& aLongValue, int aChildId /*= INVALID_DL_ID*/)
{
    DM_PRIVATE(DownloadInfo);
    QString strDlId;
    if(aChildId > INVALID_DL_ID)
        strDlId = genStrKey(aDlId, aChildId, aKeyInt);
    else
        strDlId = genStrKey(aDlId, aKeyInt);

    if(priv->m_dlInfo->contains(strDlId)) 
    {
        aLongValue = priv->m_dlInfo->value(strDlId).toInt();
        return 0;
    }
    return -1;
}

/*
Retrieves the mediaObject list
Returns : 0 on success, non zero on error
*/
int DownloadInfo::getValue(int aDlId, Key aKey, QList<QVariant>& aChildIds)
{
    DM_PRIVATE(DownloadInfo);
    QString strDlId = genStrKey(aDlId, aKey);
    if(priv->m_dlInfo->contains(strDlId)) 
    {
        QStringList strList(priv->m_dlInfo->value(strDlId).toStringList());
        for (int i = 0; i < strList.count(); i++) 
            aChildIds.append(strList[i]);
        return 0;
    }
    return -1;
}

/*
Returns all download ids in a vector
*/
QVector<int> DownloadInfo::getAllDownloads(const QString& aClientName)
{
    QVector<int> ids;
    QSettings *dlInfo = new QSettings(ORGANIZATION, aClientName);
    dlInfo->beginGroup(aClientName);
    QStringList idList = dlInfo->childGroups();
    dlInfo->endGroup();
    for(int i=0; i<idList.size(); i++)
    {
        ids.append(idList[i].toInt());
    }
    delete dlInfo;
    return ids;
}

/*
Helper function to generate a string key combining dlId and the InfoKey
*/
QString DownloadInfo::genStrKey(int aDlId, Key aKey)
{   
    DM_PRIVATE(DownloadInfo);
    QString strDlId = "";
    strDlId = priv->m_clientName;
    strDlId.append("/");

    QString str;
    str.setNum(aDlId);
    strDlId.append(str);

    strDlId.append("/");
    str.setNum((int)aKey);
    strDlId.append(str);

    return strDlId;
}

/*
Helper function to generate a string key combining dlId, mediaObjdlId and the InfoKey
*/
QString DownloadInfo::genStrKey(int aParentId, int aChildId, Key aKey)
{   
    DM_PRIVATE(DownloadInfo);
    QString strDlId = "";
    strDlId = priv->m_clientName;
    strDlId.append("/");

    QString str;
    str.setNum(aParentId);
    strDlId.append(str);
    strDlId.append("/");

    str.setNum(aChildId);
    strDlId.append(str);

    strDlId.append("/");
    str.setNum((int)aKey);
    strDlId.append(str);

    return strDlId;
}
