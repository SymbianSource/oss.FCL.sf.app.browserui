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

#include <qnetworkaccessmanager.h>
#include <QNetworkReply>
#include <QNetworkProxy>
#include "downloadcore.h"

// Http Headers
#define RANGE_HEADER "Range"
#define ETAG_HEADER "ETag"
#define ECONTENT_DISPOSITION_HEADER "Content-Disposition"
#define IF_MATCH_HEADER "If-Match"

enum DownloadMethod
{
    Invalid,
    FromURL,
    FromReply
};

// private implementation class
class DownloadCorePrivate
{
    DM_DECLARE_PUBLIC(DownloadCore);
public:
    DownloadCorePrivate();
    ~DownloadCorePrivate();

    // member variables
    QString m_url; // url
    QNetworkReply *m_reply;
    QNetworkAccessManager *m_networkAccessManager;
    // content type
    QString m_contentType;
    // ETag header value
    QString m_entityTag;
    // total size
    qint64 m_sizeInHeader;
    // Last error code
    QNetworkReply::NetworkError m_lastError;
    QString m_lastErrorString;
    QNetworkProxy *m_proxy;  //not owned
    DownloadMethod m_dlMethod; 
    bool networkAccessManagerOwned;
    QString m_fileNameFromContentDisposition;
};

DownloadCorePrivate::DownloadCorePrivate()
{
    m_url = "";
    m_reply = 0;
    m_networkAccessManager = 0;
    m_contentType = "";
    m_entityTag = "";
    m_sizeInHeader = 0;
    m_lastError = QNetworkReply::NoError;
    m_lastErrorString = "";
    m_proxy = 0;
    m_dlMethod = Invalid;
    networkAccessManagerOwned = false;
    m_fileNameFromContentDisposition ="";
}

DownloadCorePrivate::~DownloadCorePrivate()
{
    if(m_reply)
    {
        m_reply->deleteLater();
        m_reply = 0;
    }
    if(networkAccessManagerOwned && m_networkAccessManager)
    {
        m_networkAccessManager->disconnect();    
        delete m_networkAccessManager;
        m_networkAccessManager = 0;
    }
}       

DownloadCore::DownloadCore(const QString& url)
{
    DM_INITIALIZE(DownloadCore);
    priv->m_url = url;
    priv->m_dlMethod = FromURL;

    if(!priv->m_networkAccessManager)
    {
        priv->m_networkAccessManager = new QNetworkAccessManager(this);
        priv->networkAccessManagerOwned = true;
    }
}

DownloadCore::DownloadCore(QNetworkReply *reply)
{
    DM_INITIALIZE(DownloadCore);
    priv->m_reply = reply;
    priv->m_dlMethod = FromReply;

    priv->m_networkAccessManager = (priv->m_reply)->manager();
    priv->m_url = (priv->m_reply)->url().toString(QUrl::None);

    QVariant header = (priv->m_reply)->header(QNetworkRequest::ContentLengthHeader);
    if(header.isValid())
    {
        priv->m_sizeInHeader = header.toInt();
    }
    
    header = (priv->m_reply)->header(QNetworkRequest::ContentTypeHeader);
    if(header.isValid())
    {
        priv->m_contentType = header.toString();
    }
}

DownloadCore::~DownloadCore()
{
    DM_UNINITIALIZE(DownloadCore);
}

int DownloadCore::doDownload(void)
{
    DM_PRIVATE(DownloadCore);
    switch(priv->m_dlMethod)
    {
        case FromURL:
        {
            if((priv->m_url).length() <= 0 )
            {
                return -1;
            }
            if(priv->m_proxy)
            {
                priv->m_networkAccessManager->setProxy(*(priv->m_proxy));
            }
            
            /* submit the HTTP request */
            QNetworkRequest req(priv->m_url);
            priv->m_reply = (priv->m_networkAccessManager)->get(req);

            /* establish all HTTP listeners */
            connect(priv->m_reply, SIGNAL(metaDataChanged()),                     this, SLOT(parseHeaders()));    
            connect(priv->m_reply, SIGNAL(downloadProgress(qint64 , qint64 )),    this, SIGNAL(downloadProgress(qint64 , qint64 )));
            connect(priv->m_reply, SIGNAL(finished()),                            this, SIGNAL(finished()));
            connect(priv->m_reply, SIGNAL(error(QNetworkReply::NetworkError)),    this, SIGNAL(error(QNetworkReply::NetworkError)));
            break;
        }
        case FromReply:
        { 
            if(!priv->m_reply)
            {
                return -1;
            }
            /* establish all HTTP listeners */
            connect(priv->m_reply, SIGNAL(downloadProgress(qint64 , qint64 )),this, SIGNAL(downloadProgress(qint64 , qint64 )));
            connect(priv->m_reply, SIGNAL(finished()),                        this, SIGNAL(finished()));
            connect(priv->m_reply, SIGNAL(error(QNetworkReply::NetworkError)),this, SIGNAL(error(QNetworkReply::NetworkError)));            
            break;
        }
        default:
        {
            return -1;// error
        }
    }
    return 0; // successful
}

int DownloadCore::post(const QString& url, const QByteArray& data)
{
    DM_PRIVATE(DownloadCore);
    QNetworkRequest req(url); 
    // cancel the ongoing transaction if any
    abort();
    // post the data
    priv->m_reply = priv->m_networkAccessManager->post(req, data);
    if(priv->m_reply)
    {
        connect(priv->m_reply, SIGNAL(uploadProgress(qint64, qint64)), this, SIGNAL(uploadProgress(qint64, qint64)));
        return 0; // successful
    }
    else
    {
        return -1; // error
    }
}

int DownloadCore::resumeDownload(qint64 startOffeset)
{
    DM_PRIVATE(DownloadCore);

    QNetworkRequest req(priv->m_url);
    // set the RANGE header
    QString buf;
    buf.sprintf("bytes=%ld-", (long int)startOffeset);
    req.setRawHeader(RANGE_HEADER, buf.toAscii());
    // set ETag header
    if (!priv->m_entityTag.isEmpty())
        req.setRawHeader(IF_MATCH_HEADER, (priv->m_entityTag).toAscii());

    if(!priv->m_networkAccessManager)
    {
        priv->m_networkAccessManager = new QNetworkAccessManager(this);
        priv->networkAccessManagerOwned = true;
    }
    if(priv->m_proxy)
    {
        priv->m_networkAccessManager->setProxy(*(priv->m_proxy));
    }

    /* submit the HTTP request */
    priv->m_reply = (priv->m_networkAccessManager)->get(req);

    /* establish all HTTP listeners */
    connect(priv->m_reply, SIGNAL(metaDataChanged()),                     this, SLOT(parseHeaders()));
    connect(priv->m_reply, SIGNAL(downloadProgress(qint64 , qint64 )),    this, SIGNAL(downloadProgress(qint64 , qint64 )));
    connect(priv->m_reply, SIGNAL(finished()),                            this, SIGNAL(finished()));
    connect(priv->m_reply, SIGNAL(error(QNetworkReply::NetworkError)),    this, SIGNAL(error(QNetworkReply::NetworkError)));
    return 0; // successful
}

int DownloadCore::abort(void)
{
    DM_PRIVATE(DownloadCore);
    if(priv->m_reply)
    {
        (priv->m_reply)->abort();
        priv->m_reply->deleteLater();
        priv->m_reply = 0;
    }
    return 0; // successful
}

QString& DownloadCore::url(void)
{
    DM_PRIVATE(DownloadCore);
    return priv->m_url;
}

void DownloadCore::changeUrl(QString& url)
{
    DM_PRIVATE(DownloadCore);
    priv->m_url = url;
    priv->m_dlMethod = FromURL;
}

QNetworkReply* DownloadCore::reply(void)
{
    DM_PRIVATE(DownloadCore);
    return priv->m_reply;
}

QNetworkAccessManager* DownloadCore::networkAccessManager(void)
{
    DM_PRIVATE(DownloadCore);
    return priv->m_networkAccessManager;
}

QString& DownloadCore::contentType(void)
{
    DM_PRIVATE(DownloadCore);
    return priv->m_contentType;
}

QString& DownloadCore::entityTag(void)
{
    DM_PRIVATE(DownloadCore);
    return priv->m_entityTag;
}

void DownloadCore::setContentType(const QString& contentType)
{
    DM_PRIVATE(DownloadCore);
    priv->m_contentType = contentType;
    return;
}

void DownloadCore::setEntityTag(const QString& entityTag)
{
    DM_PRIVATE(DownloadCore);
    priv->m_entityTag = entityTag;
}

qint64 DownloadCore::sizeInHeader(void)
{
    DM_PRIVATE(DownloadCore);
    return priv->m_sizeInHeader;
}

QNetworkReply::NetworkError DownloadCore::lastError(void)
{
    DM_PRIVATE(DownloadCore);
    return priv->m_lastError;
}

void DownloadCore::setLastError(QNetworkReply::NetworkError code)
{
    DM_PRIVATE(DownloadCore);
    priv->m_lastError = code;
}

QString DownloadCore::lastErrorString(void)
{
    DM_PRIVATE(DownloadCore);
    return priv->m_lastErrorString;
}

void DownloadCore::setLastErrorString(const QString& errStr)
{
    DM_PRIVATE(DownloadCore);
    priv->m_lastErrorString = errStr;
}

void DownloadCore::setProxy(QNetworkProxy *proxy)
{
    DM_PRIVATE(DownloadCore);
    priv->m_proxy = proxy;
}

QNetworkProxy* DownloadCore::proxy()
{
    DM_PRIVATE(DownloadCore);
    return priv->m_proxy;
}

QString DownloadCore::fileNameFromContentDispositionHeader()
{
     DM_PRIVATE(DownloadCore);
     return priv->m_fileNameFromContentDisposition;
}

void DownloadCore::parseHeaders(void)
{
    DM_PRIVATE(DownloadCore);
    QVariant lenHeader = (priv->m_reply)->header(QNetworkRequest::ContentLengthHeader);
    if(lenHeader.isValid())
    {
        priv->m_sizeInHeader = lenHeader.toInt();
    }                
    priv->m_contentType = (priv->m_reply)->header(QNetworkRequest::ContentTypeHeader).toString();

    // ETag
    priv->m_entityTag = (priv->m_reply)->rawHeader(ETAG_HEADER);
    
    if (priv->m_reply->hasRawHeader(ECONTENT_DISPOSITION_HEADER)) {
        const QString value = priv->m_reply->rawHeader(ECONTENT_DISPOSITION_HEADER);
        const int pos = value.indexOf("filename=");
        if (pos != -1) {
            QString name = value.mid(pos + 9);
            if (name.startsWith('"') && name.endsWith('"'))
                name = name.mid(1, name.size() - 2);
            priv->m_fileNameFromContentDisposition = name;
        }
    }
    // emit the signal
    emit metaDataChanged();
    return;
}
   
