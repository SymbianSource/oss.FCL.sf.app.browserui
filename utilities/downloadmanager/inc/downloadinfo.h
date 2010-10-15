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

#ifndef DOWNLOAD_INFO_H
#define DOWNLOAD_INFO_H

#include "dmpimpl.h"
#include "dmcommon.h"
#include <QString>
#include <QVector>
#include <QVariant>

// forward declarations
class QSettings;
class DownloadInfoPrivate;

// class declaration
class DownloadInfo
{
    DM_DECLARE_PRIVATE(DownloadInfo); // private implementation
public:
    /*
    InfoKey - key enumerations for the download info
    */
    enum Key 
    {
        EUrl = 0,           // 0    http://xyz.com/a.mp3
        EFileName,          // 1    a.mp3
        ETempPath,          // 2    ./com.nokia.browser/
        EFinalPath,         // 3    /home/downloads/
        ETempFullPath,      // 4    ./com.nokia.browser/a(1).mp3
        ETotalSize,         // 5    xxx bytes <int>
        EDlState,           // 6    int 
        EContentType,       // 7    audio/mp3 
        EETag,              // 8    ETag
        EChildIdList,       // 9    list of child ids
        EType,              // 10   sequential or parallel
        EScope,             // 11   client side or background download
        EPriority           // 12   priority of the download
    };

    DownloadInfo(const QString& aClientName);
    ~DownloadInfo();

public:
    /*
    Sets a string value.
    Returns : 0 on success, non zero on error
    */
    int setValue(int aDlId, Key aKeyStr, const QString& aStrValue, int aParentId = INVALID_DL_ID);

    /*
    Sets a string value.
    Returns : 0 on success, non zero on error
    */
    int setValue(int aDlId, Key aKeyStr, long aLongValue, int aParentId = INVALID_DL_ID);

    /*
    Sets a string value.
    Returns : 0 on success, non zero on error
    */
    int setValueForChild(int aDlId, Key aKeyStr, const QString& aStrValue, int aChildId = INVALID_DL_ID);
    
    /*
    Sets an int value.
    Returns : 0 on success, non zero on error
    */
    int setValueForChild(int aDlId, Key aKeyInt, long aLongValue, int aChildId = INVALID_DL_ID);

    /*
    Sets the mediaObject list
    Returns : 0 on success, non zero on error
    */
    int setValue(int aDlId, Key aKey, const QList<QVariant>& aChildIds);

    /*
    Updates(serializes) the info with all set values at once.
    Returns : 0 on success, non zero on error
    */
    int update();

    /*
    Retrieves the string value
    Returns : 0 on success, non zero on error
    */
    int getValue(int aDlId, Key aKeyStr, QString& aStrValue, int aParentId = INVALID_DL_ID);
    
    /*
    Retrieves the int value
    Returns : 0 on success, non zero on error
    */
    int getValue(int aDlId, Key aKeyInt, long& aLongValue, int aParentId = INVALID_DL_ID);

    /*
    Retrieves the string value
    Returns : 0 on success, non zero on error
    */
    int getValueForChild(int aDlId, Key aKeyStr, QString& aStrValue, int aChildId = INVALID_DL_ID);
    
    /*
    Retrieves the int value
    Returns : 0 on success, non zero on error
    */
    int getValueForChild(int aDlId, Key aKeyInt, long& aLongValue, int aChildId = INVALID_DL_ID);

    /*
    Retrieves the mediaObject list
    Returns : 0 on success, non zero on error
    */
    int getValue(int aParentId, Key aKey, QList<QVariant>& aChildIds);

    /*
    Deletes the download info of a particular download represented by aDlId.
    Returns : 0 on success, non zero on error
    */
    int remove(int aDlId, int aParentId = INVALID_DL_ID);

    /*
    Returns all download ids in a vector
    */
    static QVector<int> getAllDownloads(const QString& aClientName);

private:

    /*
    Helper function to generate a string key combining dlId and the InfoKey
    */
    QString genStrKey(int aDlId, Key aKey);

    /*
    Helper function to generate a string key combining parentId, childId and the InfoKey
    */
    QString genStrKey(int aParentId, int aChildId, Key aKey);
};

#endif
