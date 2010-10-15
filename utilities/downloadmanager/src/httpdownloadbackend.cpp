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

#include "clientdownload.h"
#include "downloadmanager.h"
#include "downloadcore.h"
#include "downloadstore.h"
#include "httpdownloadbackend.h"
#include "filestorage.h"
#include "dmcommoninternal.h"
#include <QFileInfo>
#include <QString>
#include <QMap>
#include <QIODevice>

// private implementation
class HttpDownloadBackendPrivate
{
    DM_DECLARE_PUBLIC(HttpDownloadBackend);
public:
    HttpDownloadBackendPrivate();
    ~HttpDownloadBackendPrivate();
    DownloadCore *m_downloadCore; // for network operations
    DownloadStore *m_storage; // responsible for handling the storage
    ClientDownload *m_download; // not owned
};  

HttpDownloadBackendPrivate::HttpDownloadBackendPrivate()
{
    m_downloadCore = 0; 
    m_storage = 0;
    m_download = 0;
}

HttpDownloadBackendPrivate::~HttpDownloadBackendPrivate()
{
    if(m_storage)
    {
        delete m_storage;
        m_storage = 0;
    }
}

HttpDownloadBackend::HttpDownloadBackend(DownloadCore *dlCore, ClientDownload *dl, DownloadStore* store)
    :DownloadBackend(dlCore, dl)
{
    DM_INITIALIZE(HttpDownloadBackend);
    priv->m_downloadCore = dlCore;
    priv->m_storage = store; 
    priv->m_download = dl;
    setValue(DownloadInfo::EFinalPath, dl->attributes().value(DlDestPath).toString());

    QString fileName;
    if(dl->isCreatedByDlInfo())
    {
        getValue(DownloadInfo::EFileName, fileName);
        dl->attributes().insert(DlFileName, fileName);
        priv->m_storage->open(QIODevice::Append);           
    }
    else
    {
        fileName = dl->attributes().value(DlFileName).toString();
        // if filename is not set, take it from url
        if(fileName.length() == 0) {
            fileName = priv->m_downloadCore->fileNameFromContentDispositionHeader();
            if(fileName.length() == 0) {
                QUrl url(priv->m_downloadCore->url());
                QFileInfo fileUrl(url.path());
                fileName = fileUrl.fileName();     
            }         
        }
        dl->attributes().insert(DlFileName, fileName);
        // create the new storage
        priv->m_storage->createStore();      
    }
    setValue(DownloadInfo::EFileName, download()->attributes().value(DlFileName).toString());
}

HttpDownloadBackend::~HttpDownloadBackend()
{
    DM_UNINITIALIZE(HttpDownloadBackend);
}

int HttpDownloadBackend::resume()
{
    DM_PRIVATE(HttpDownloadBackend);
    // Open the file in append mode as we need to append the received chunks    
    if(downloadState() != DlCancelled)
    {
        priv->m_storage->open(QIODevice::Append);               
    }
    else
    {
        QString fileName = download()->attributes().value(DlFileName).toString();
        // if filename is not set, take it from url 
        if(fileName.length() == 0) {
            fileName = priv->m_downloadCore->fileNameFromContentDispositionHeader();
            if(fileName.length() == 0) {
                QUrl url(priv->m_downloadCore->url());
                QFileInfo fileUrl(url.path());
                fileName = fileUrl.fileName();     
            }         
        }
        download()->attributes().insert(DlFileName, fileName);
        // create the new storage
        priv->m_storage->createStore(); 
    }
    setValue(DownloadInfo::EFileName, download()->attributes().value(DlFileName).toString());
    return DownloadBackend::resume();
}

void HttpDownloadBackend::store(QByteArray data, bool lastChunk)
{
    DM_PRIVATE(HttpDownloadBackend); 
    // write the chunks to the storage
    priv->m_storage->write(data, lastChunk); 
}

void HttpDownloadBackend::deleteStore()
{
    DM_PRIVATE(HttpDownloadBackend);
    if(priv->m_storage)
    {
        priv->m_storage->deleteStore();
    }
}

qint64 HttpDownloadBackend::storedDataSize()
{
    DM_PRIVATE(HttpDownloadBackend);
    // size of stored data chunk
    return priv->m_storage->storedDataSize();
}

QVariant HttpDownloadBackend::getAttribute(DownloadAttribute attr)
{
    DM_PRIVATE(HttpDownloadBackend);
    switch(attr)
    {
        case DlFileName:
        {
            if(priv->m_download)
                return priv->m_download->attributes().value(attr);
        }
        case DlDestPath:
        {
            if(priv->m_download)
                return priv->m_download->attributes().value(attr);
        }
        default:
            break;
    }  
    return DownloadBackend::getAttribute(attr);
}

int HttpDownloadBackend::setAttribute(DownloadAttribute attr, const QVariant& value)
{
    return DownloadBackend::setAttribute(attr, value);
}

void HttpDownloadBackend::headerReceived()
{
    DM_PRIVATE(HttpDownloadBackend);
    DlEventAttributeMap* attrMap = NULL;
    HttpStatusCode status = (HttpStatusCode) priv->m_downloadCore->reply()->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    switch(status)
    {
        case HttpOK:                    // OK
        case HttpCreated:               // Created
        case HttpAccepted:              // Accepted
        case HttpNonAuthorativeInfo:    // Non-Authorative Information
        case HttpNoContent:             // No Content
        case HttpResetContent:          // Reset Conetent
        {
            // get the content type and save it in dl info
            QString contentType = priv->m_downloadCore->contentType();
            setValue(DownloadInfo::EContentType, contentType);
            setDownloadState(DlInprogress);
            break;
        }

        case HttpPartialContent:        // Partial Conetent Download
        {
            setDownloadState(DlInprogress);
            break;
        }
        
        case HttpPreconditionFailed:    // Precondition Failed
        {
            // attrMap will be deleted in destructor of DownloadEventPrivate class
            attrMap = new DlEventAttributeMap ;
            attrMap->insert(HeaderReceivedStatusCode, QVariant(status));
            break;
        }

        default:
           break;
    }

    postEvent(HeaderReceived, attrMap);
}



