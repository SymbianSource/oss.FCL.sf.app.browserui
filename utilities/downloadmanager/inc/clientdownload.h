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

#ifndef CLIENTDOWNLOAD_H
#define CLIENTDOWNLOAD_H

#include "dmcommon.h"
#include "dmpimpl.h"
#include "downloadevent.h"
#include "download.h"
#include <QObject>

class DownloadManager;
class QNetworkReply;
class DownloadInfo;
class ClientDownloadPrivate;

typedef QList<QObject*> EventReceiverList; 

// class declaration
class ClientDownload : public Download
{
    Q_OBJECT
    DM_DECLARE_PRIVATE(ClientDownload);
public:
    // client download constructor
    // note that parentdlId is applicable in parent-child relationship where
    // parent download can be oma download and childs can be media downloads
    ClientDownload(DownloadManager *mgr, const QString& url
                                       , int dlId 
                                       , DownloadType type = Parallel
                                       , int parentdlId =INVALID_DL_ID); 
    ClientDownload(DownloadManager *mgr, QNetworkReply *reply
                                       ,int dlId
                                       , int parentdlId =INVALID_DL_ID); 
    ClientDownload(DownloadManager *mgr, int dlId, int parentdlId = INVALID_DL_ID); 
    ~ClientDownload();

    // starts the download
    int start();
    // id of the download
    int id();
    // sets the attributes for the download
    int setAttribute(DownloadAttribute attr, const QVariant& value);
    // fetches the attributes of download
    QVariant getAttribute(DownloadAttribute attr);
    // pauses the download
    int pause();
    // resumes the download
    int resume();
    // cancels the download
    int cancel();
    // registers the event listener
    void registerEventReceiver(QObject *receiver);
    // unregisters the event listener
    void unregisterEventReceiver(QObject *receiver);
    // returns the download manager
    DownloadManager *downloadManager();
    // returns the list of child downloads
    void getChildren(QList<Download*>& list);
    // to indicate download is created by perstistant stored info
    bool isCreatedByDlInfo(void);
   
private:
    // returns the event listeners
    EventReceiverList& eventReceivers();
    // returns the download info
    DownloadInfo* downloadInfo();
    // posts the event to recievers event loop
    void postEvent(DEventType type, DlEventAttributeMap* attrMap);
    // sets the error string
    void setError(const QString& errorStr);
    //sets the download state
    void setDownloadState(DownloadState state);
    // sets the id of the parent download
    void setParentId(int parentId);
    // parent id
    int parentId();
    // returns the attribute map
    // note that these attributes are the only once which are 'set'able by clients
    QMap<DownloadAttribute, QVariant>& attributes(void);
    // actually starts the download
    int startDownload();
    // actually pauses the download
    int pauseDownload();
    // actually resumes the download
    int resumeDownload();
    // actually cancels the download
    int cancelDownload();

private slots:
    // creates the concrete download implementation based on content type
    void createDownloadImplementation();

    friend class DownloadBackend; 
    friend class OMADownloadBackend;
    friend class HttpDownloadBackend;
    friend class OMA2DownloadBackend;
#if defined(RD_WMDRM_DLA_ENABLERS) && defined(Q_OS_SYMBIAN)
    friend class WMDRMDownloadBackend;
#endif
    friend class FileStorage;
    friend class DrmStorage;
    friend class SequentialDownloadManager;
    friend class ParallelDownloadManager;
};

#endif
