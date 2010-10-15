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

#include "storageutility.h"
#include <QFile>
#include <QDir>
#include <qregexp.h>


int StorageUtility::generateUniqueFileName(const QString &fileName, const QString &tempPath,
        const QString& permPath, QString &outFileName)
{
    int uniqueExt = 0;
    QString orgFilename = fileName;    
    QFileInfo fileinfo(tempPath, fileName);
    QFileInfo fileinfoPerm(permPath, fileName);

    while(true)
    {
        uniqueExt++;
        // check if file exists
        if (fileinfo.exists() || fileinfoPerm.exists()) {
            // a file with this name already exists in either temporary
            // or permanent download location
            QString tempFileName = orgFilename;
            QString ext = fileinfo.suffix();
            QString fileNoExt = tempFileName;
            if (!ext.isEmpty())
                fileNoExt = tempFileName.mid(0, tempFileName.length()-ext.length()-1);

            // add index to filename
            QString strExt;
            strExt.setNum(uniqueExt);
            tempFileName =  fileNoExt+"("+strExt+")";
            if (!ext.isEmpty())
                tempFileName = tempFileName+"."+ext;

            // set fileinfo to check again if any file with 
            // this new filename already exists
            fileinfo.setFile(tempPath, tempFileName);
            fileinfoPerm.setFile(permPath, tempFileName);
        }
        else {
            // this is a unique filename
            outFileName = fileinfo.fileName();
            return 0;
        }
    }   
    return -1;
}

void StorageUtility::validateFileName(const QString &fileName, QString &outFileName)
{
    QString orgFileName = fileName;
    // Remove invalid filename characters
    orgFileName.replace( QRegExp("[?:*\"\\/<>|]"), "" );

    // Check if filename is empty
    outFileName = orgFileName.trimmed();
    if (outFileName.isEmpty())
        outFileName = FILENAME_UNKNOWN;
}

QString StorageUtility::createTemporaryPath(const QString& clientName)
{
    /* create "downloads" folder : ./<clientname>/downloads */ 
    QString orgClientName = clientName;
    orgClientName.replace( QRegExp("[\\\\/:\\*\\?\"<>\\|]+"), "_" );
    QDir currentDir(QDir::current());
    QString tempPath = currentDir.absolutePath() + "/" + orgClientName + "/" + QObject::tr(TEMPORARY_FOLDER);
    currentDir.mkpath(tempPath);
    return tempPath;
}
