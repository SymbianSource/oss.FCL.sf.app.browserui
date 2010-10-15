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

#include "filestorage.h"
#include "storageutility.h"
#include <QFile>
#include <QDir>
#include <QVariant>
#include <qregexp.h>

class FileStoragePrivate
{
    DM_DECLARE_PUBLIC(FileStorage);
public:
    FileStoragePrivate();
    ~FileStoragePrivate();
    // client name
    QString m_clientName;
    // folder paths
    QString m_tempPath;                     // temporary path till dl completes
    QFile *m_file;
    ClientDownload* m_download;
};  

FileStoragePrivate::FileStoragePrivate()
{
    m_tempPath = "";
    m_file = 0;
    m_download = 0;
}

FileStoragePrivate::~FileStoragePrivate()
{
    if(m_file)
    {
        // close the file if it is open
        if(m_file->isOpen())
        {
            m_file->close();
        }
        delete m_file;
        m_file = 0;
    }
}

FileStorage::FileStorage(const QString& clientName, ClientDownload* download)
{
    DM_INITIALIZE(FileStorage);
    priv->m_clientName = clientName;
    priv->m_file = new QFile();
    priv->m_download = download;
    priv->m_tempPath = StorageUtility::createTemporaryPath(clientName);
}

FileStorage::~FileStorage()
{
    DM_UNINITIALIZE(FileStorage);
}

int FileStorage::createStore()
{
    DM_PRIVATE(FileStorage);

    // validate filename
    QString validFilename;
    QString filename = priv->m_download->attributes().value(DlFileName).toString();
    StorageUtility::validateFileName(filename, validFilename);
    // generate unique filename
    QString permPath = priv->m_download->attributes().value(DlDestPath).toString();
    QString uniqueFilename;
    StorageUtility::generateUniqueFileName(validFilename, priv->m_tempPath, permPath, uniqueFilename);  
    
    // set the new filename
    QDir filePath(priv->m_tempPath);
    priv->m_download->attributes().insert(DlFileName, uniqueFilename);
    QFileInfo newFileinfo(filePath, uniqueFilename);
    QString newFilename = newFileinfo.filePath();
    priv->m_file->setFileName(newFilename);   
    if (!priv->m_file->open(QIODevice::ReadWrite))
        return -1;
    else
        return 0;
}


int FileStorage::open(QIODevice::OpenMode mode)
{
    DM_PRIVATE(FileStorage);

    QDir filePath(priv->m_tempPath);
    QFileInfo fileinfo(filePath, priv->m_download->attributes().value(DlFileName).toString());
    QString filename = fileinfo.filePath();
    // if already exists, open the file
    if(priv->m_file && (QFile::exists(filename)) && !priv->m_file->isOpen())
    {
        priv->m_file->setFileName(filename);
        if (!priv->m_file->open(mode))
            return -1;
        else
            return 0;
    }
    return -1;  
}

int FileStorage::write(const QByteArray& data,bool lastChunk)
{
    DM_PRIVATE(FileStorage);
    // write the data chunk
    int value = priv->m_file->write(data);
    if(lastChunk)
    {
        close();
        // move the file to permanent destination path
        moveDlFile();
    }
    return value;
}

int FileStorage::close()
{
    DM_PRIVATE(FileStorage);
    if(priv->m_file && priv->m_file->isOpen())
    {
        priv->m_file->close();
    }
    return 0;
}

int FileStorage::deleteStore()
{
    DM_PRIVATE(FileStorage);
    // remove the temporary file
    QDir tempFilePath(priv->m_tempPath);
    QFileInfo tempFileinfo(tempFilePath, priv->m_download->attributes().value(DlFileName).toString());
    QString tempFileName = tempFileinfo.filePath();

    // remove the temporary file
    if(QFile::exists(tempFileName))
    {
        close();
        QFile::remove(tempFileName); 
    }

    return 0;
}

int FileStorage::storedDataSize()
{
    DM_PRIVATE(FileStorage);
    QDir tempFilePath(priv->m_tempPath);
    QFileInfo tempFileinfo(tempFilePath, priv->m_download->attributes().value(DlFileName).toString());
    QString tempFilename = tempFileinfo.filePath();
    QFile tempFile(tempFilename);

    if(tempFile.exists())
        return tempFile.size();
    else
        return 0;
}

void FileStorage::moveDlFile()
{
    DM_PRIVATE(FileStorage);
    QString permPath = priv->m_download->attributes().value(DlDestPath).toString();
    QString filename = priv->m_download->attributes().value(DlFileName).toString();
    QDir permDir(permPath);
    if(!permDir.exists())
    {
        QDir tempDir;
        if(!tempDir.mkpath (permPath))
        {
            return;
        }
    }
    
    QDir newFilePath(permPath);
    QFileInfo newFileinfo(newFilePath, filename);
    QString newFileName = newFileinfo.filePath();
    
    // remove the file if already exists
    if(QFile::exists(newFileName)) {
        QFile::remove(newFileName);    
    }

    QDir tempFilePath(priv->m_tempPath);
    QFileInfo tempFileinfo(tempFilePath, filename);
    QString tempFilename = tempFileinfo.filePath();
    
    QFile::copy(tempFilename, newFileName);

    // remove the temporary file
    QFile::remove(tempFilename);    
    return;
}
