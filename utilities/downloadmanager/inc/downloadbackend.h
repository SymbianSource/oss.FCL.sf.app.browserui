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

#ifndef DOWNLOADBACKEND_H
#define DOWNLOADBACKEND_H

#include "dmpimpl.h"
#include "downloadinfo.h"
#include "downloadevent.h"
#include <QObject>
#include <QNetworkReply>

// forward declarations
class DownloadBackendPrivate;
class DownloadCore;
class Download;
class ClientDownload;

// class declaration
class DownloadBackend : public QObject
{
    Q_OBJECT
    DM_DECLARE_PRIVATE(DownloadBackend);
public:
    DownloadBackend(DownloadCore *dlCore, ClientDownload *dl);
    virtual ~DownloadBackend();

    // pauses the download
    virtual int pause() ;
    // resumes the download
    virtual int resume() ;
    // cancels the download
    virtual int cancel() ;
    // fetches the attributes
    virtual QVariant getAttribute(DownloadAttribute attr);
    // sets the attributes
    virtual int setAttribute(DownloadAttribute attr, const QVariant& value);
    // stores the data in storage
    // derived classes expected to have their own implementation
    virtual void store(QByteArray data, bool lastChunk=false) = 0;
    // deletes the storage
    // derived classes expected to have their own implementation
    virtual void deleteStore() = 0;
    // returns the size of stored data
    // derived classes expected to have their own implementation
    virtual qint64 storedDataSize()= 0;
    //returns the list of child downloads
    virtual void getChildren(QList<Download*>&) { }
    // initialises the download by reading from persistant storage
    virtual void init() { return; }

private: // copy constructor and assaignment operator
      DownloadBackend(const DownloadBackend &);
      DownloadBackend &operator=(const DownloadBackend &);

public:
    // functions which will store and retrieve the download persistant data
    int setValue(DownloadInfo::Key aKey, const QString& aStrValue);
    int setValue(DownloadInfo::Key aKey, long aIntValue);
    int setValue(DownloadInfo::Key aKey, const QList<QVariant>& aChildIds);
    int setValueForChild(DownloadInfo::Key aKey, const QString& aStrValue, int aChildId = INVALID_DL_ID);
    int setValueForChild(DownloadInfo::Key aKey, long aIntValue, int aChildId = INVALID_DL_ID);

    int getValue(DownloadInfo::Key aKey, QString& aStrValue);
    int getValue(DownloadInfo::Key aKey, long& aIntValue);
    int getValue(DownloadInfo::Key aKey, QList<QVariant>& aChildIds);
    int getValueForChild(DownloadInfo::Key aKey, QString& aStrValue, int aChildId = INVALID_DL_ID);
    int getValueForChild(DownloadInfo::Key aKey, long& aIntValue, int aChildId = INVALID_DL_ID);

    // deletes the persistant data of download
    int deleteInfo();
    // posts the event to recievers event loop
    void postEvent(DEventType type, DlEventAttributeMap* attrMap);
    // download state
    DownloadState downloadState(void);
    // sets the download state
    void setDownloadState(DownloadState state);
    // sets the current downloaded data size
    void setDownloadedDataSize(qint64 size);
    // set total download size
    void setTotalSize(qint64 size);
    // returns the download object
    ClientDownload* download(void);
    // sets the start time
    void setStartTime();

private:
    void postDownloadEvent(DEventType type, DlEventAttributeMap* attrMap);

public slots:
    virtual void bytesRecieved(qint64 bytesRecieved, qint64 bytesTotal);
    virtual void handleFinished();
    virtual void error(QNetworkReply::NetworkError);
    virtual void headerReceived(){}
    virtual void bytesUploaded(qint64, qint64){}
};    

#endif    
