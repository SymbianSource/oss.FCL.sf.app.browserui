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

#ifndef DOWNLOAD_CORE_H_
#define DOWNLOAD_CORE_H_

#include <QObject>
#include <QNetworkReply>
#include "dmpimpl.h"

// forward declarations
class QNetworkAccessManager;
class DownloadCorePrivate; 
class QNetworkProxy;

// class declaration
class  DownloadCore : public QObject
{
    Q_OBJECT
    DM_DECLARE_PRIVATE(DownloadCore); // private implementation

public:
    // to start new download transaction
    DownloadCore(const QString &aUrl);
    // to carry forward the downloads transaction which has been started already
    DownloadCore(QNetworkReply *reply);
    virtual ~DownloadCore();
private:
    // default constructor made private to enforce creation of object only 
    // by passing url or network reply
    DownloadCore(void){};

signals:
    void finished(void);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void metaDataChanged(); 
    void error(QNetworkReply::NetworkError code);
    void uploadProgress( qint64 bytesSent, qint64 bytesTotal );

private slots:
    void parseHeaders(void);

private: // copy constructor and assaignment operator
      DownloadCore(const DownloadCore &);
      DownloadCore &operator=(const DownloadCore &);
    
public:
    // starts the new download transaction if it is not started already addition with
    // all necessary initialisarion or does initialisation if the transaction has been already started
    int doDownload(void);
    // for http "post" requests
    int post(const QString& url, const QByteArray& data);
    // pauses the download transaction
    int resumeDownload(qint64 startOffeset);
    // aborts the network transaction
    int abort(void);
    // sets the proxy
    void setProxy(QNetworkProxy *proxy);
    // returnts the url
    QString& url(void);
    // start download from given url
    void changeUrl(QString& url);
    // returns the network reply
    QNetworkReply* reply(void);
    // returns the network access manager
    QNetworkAccessManager* networkAccessManager(void);
    // returns the content type
    QString& contentType(void);
    // returns the content type
    QString& entityTag(void);
    // returns the total size
    qint64 sizeInHeader(void);
    // returns the last error occurred
    QNetworkReply::NetworkError lastError(void);
    // returns the last error string
    QString lastErrorString();
    // returns proxy
    QNetworkProxy* proxy();
    // returns file name from content disposition header
    QString fileNameFromContentDispositionHeader();

    // the following funtions used in the cases where 
    // download has loaded from persistant state. Careful 
    // while usig the following functions in normal cases
    void setContentType(const QString& contentType);
    void setEntityTag(const QString& entityTag);
    // the folowing setLastError and setLastErrorString should be used
    // together to keep the consistancy between error code and error string
    void setLastError(QNetworkReply::NetworkError code);
    void setLastErrorString(const QString& errStr); 
};

#endif /*DOWNLOAD_CORE_H_*/
