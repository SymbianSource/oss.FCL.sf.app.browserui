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

#ifndef STORAGEUTILITY_H
#define STORAGEUTILITY_H

#include <QObject>

#define TEMPORARY_FOLDER "downloads"
#define FILENAME_UNKNOWN "Unknown"

// class declaration

// utility class
class StorageUtility
{
public:
    // generate unique filename
    static int generateUniqueFileName(const QString &fileName, const QString &tempPath,
            const QString& permPath, QString &outFileName);
    // validate filename
    static void validateFileName(const QString &fileName, QString &outFileName);
    // create temporary path for writing the data
    static QString createTemporaryPath(const QString& clientName);
};

#endif
