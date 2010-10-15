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

#include "drmstorage.h"
#include "storageutility.h"
#include "dmcommoninternal.h"
#include <QFile>
#include <QDir>
#include <QVariant>
#include <qregexp.h>
#include <f32file.h>


DrmStorage::DrmStorage(const QString& clientName, ClientDownload* download)
{
    m_file = 0;
    m_clientName = clientName;
    m_download = download;
    // create temporary path
    m_tempPath = StorageUtility::createTemporaryPath(clientName);
}

DrmStorage::~DrmStorage()
{
    if (m_file) {
        close();
        delete m_file;
        m_file = 0;
    }
}

int DrmStorage::createStore()
{
    // validate filename
    QString validFilename;
    QString filename = m_download->attributes().value(DlFileName).toString();
    StorageUtility::validateFileName(filename, validFilename);
    // generate unique filename 
    QString permPath = m_download->attributes().value(DlDestPath).toString();
    QString uniqueFilename;
    StorageUtility::generateUniqueFileName(validFilename, m_tempPath, permPath, uniqueFilename);  
    
    // set the new filename
    m_download->attributes().insert(DlFileName, uniqueFilename);
    
    // convert to symbian descriptors
    QString path = m_tempPath;
    path.replace(QString("/"), QString("\\"));
    TPtrC ptrPath = SymbianStringToQtString(path);
    TPtrC ptrFileName = SymbianStringToQtString(uniqueFilename);
    _LIT8(KOmaDrm1DrmMessageContentType, OMA_DRM_MESSAGE_CONTENT_TYPE);
    
    // create supplier and get importfile which will actually write to the file
    QT_TRAP_THROWING(ContentAccess::CSupplier* supplier = ContentAccess::CSupplier::NewLC();
                     ContentAccess::CMetaDataArray* metaData = ContentAccess::CMetaDataArray::NewLC();
                     supplier->SetOutputDirectoryL(ptrPath);
                     m_file = supplier->ImportFileL(KOmaDrm1DrmMessageContentType(), *metaData, ptrFileName));

    // perform cleanup
    CleanupStack::PopAndDestroy(2); // metaData, supplier
    return 0;
}


int DrmStorage::open(QIODevice::OpenMode /*mode*/)
{
    // we cannot open an existing file, so we delete the
    // store and create a new one. Download will start/resume
    // from beginning
    deleteStore();
    if (!m_file)
        createStore();
    return 0;
}

int DrmStorage::write(const QByteArray& data, bool lastChunk)
{
    if (!m_file)
        return -1;
    
    // convert the data chunk to 8 bit descriptor
    QString strChunk(data);
    TPtrC ptrChunk = SymbianStringToQtString(strChunk);
    HBufC8* buf8 = q_check_ptr(HBufC8::New(ptrChunk.Length()));
    buf8->Des().Copy(ptrChunk);
    TPtrC8 ptrData8(*buf8);
    
    if (m_file) {
        // write the data chunk
        m_file->WriteData(ptrData8);
        if (lastChunk) {
            // close the file
            close();
            // move the file to permanent destination path
            // note that moveDlFile uses m_file
            moveDlFile();
            // clean up memory
            delete m_file;
            m_file = 0;
        }
    }
    
    delete buf8;
    buf8 = NULL;
    return 0;
}

int DrmStorage::close()
{
    if (m_file) {    
        // close storage and get filename
        m_file->WriteDataComplete();
    }
    return 0;
}

int DrmStorage::deleteStore()
{
    // create filename (with path)
    QDir tempFilePath(m_tempPath);
    QFileInfo tempFileInfo(tempFilePath, m_download->attributes().value(DlFileName).toString());
    QString tempFileName = tempFileInfo.filePath();
    
    // remove the temporary file
    if (QFile::exists(tempFileName)) {
        // close the store
        close();
        if (m_file) {
            delete m_file;
            m_file = 0;
        }
        // remove file
        QFile::remove(tempFileName);
    }

    return 0;
}

int DrmStorage::storedDataSize()
{
    // build temporary path with filename
    QDir tempFilePath(m_tempPath);
    QFileInfo tempFileinfo(tempFilePath, m_download->attributes().value(DlFileName).toString());
    QString tempFilename = tempFileinfo.filePath();
    QFile tempFile(tempFilename);

    // return file size if file exists
    if(tempFile.exists())
        return tempFile.size();
    else
        return 0;
}

void DrmStorage::moveDlFile()
{
    // source file
    QString fileName = m_download->attributes().value(DlFileName).toString();
    QFileInfo srcFileInfo(m_tempPath, fileName);
    QString srcFile = srcFileInfo.filePath();
    
    // if source file does not exist, return
    if (!QFile::exists(srcFile))
        return;

    // create the permanent directory
    QString permPath = m_download->attributes().value(DlDestPath).toString();
    QDir permDir(permPath);
    if(!permDir.exists()) {
        if(!permDir.mkpath(permPath))
            return;
    }

    if (m_file) {
        // name of the file to which 
        TPtrC outputFileName(KNullDesC);
        QT_TRAP_THROWING(outputFileName.Set(m_file->OutputFileL(0).FileName()));
        // convert filename to Qt equivalent
        srcFile = SymbianStringToQtString(outputFileName);
        // check if file exists and update filename attribute
        if (!QFile::exists(srcFile))
            return;
        
        // name without extension
        TParse p;
        p.SetNoWild(outputFileName, NULL, NULL);
        TPtrC namePtr = p.Name();
        // suggested extension
        TBuf<4> extension;
        m_file->GetSuggestedOutputFileExtension(extension);
    
        // build filename with new extension and convert to Qt equivalent 
        HBufC* name = q_check_ptr(HBufC::New(namePtr.Length() + extension.Length()));
        TPtr newNamePtr(name->Des());
        newNamePtr.Format(_L("%S%S"), &namePtr, &extension);
        QString newFileName = SymbianStringToQtString(newNamePtr); 
        delete name;
        name = NULL;
    
        // extension has changed, generate unique filename
        StorageUtility::generateUniqueFileName(newFileName, m_tempPath, permPath, fileName);
    }
    
    // remove the file if it already exists
    QFileInfo fileInfo(permPath, fileName);
    QString destFile = fileInfo.filePath();
    if (QFile::exists(destFile))
        QFile::remove(destFile);
    
    // Copy the file to destination and remove old file
    QFile::copy(srcFile, destFile);
    QFile::remove(srcFile);
    
    // set the new filename
    m_download->attributes().insert(DlFileName, fileName);
}

QString DrmStorage::SymbianStringToQtString(TDesC& symbianString)
{
    // convert symbian string to Qt string
    return QString::fromUtf16(symbianString.Ptr(), symbianString.Length());
}

TPtrC DrmStorage::SymbianStringToQtString(QString& qtString)
{
    // convert QString string to symbian string
    return reinterpret_cast<const TText*> (qtString.constData());
}
