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

#ifndef WMDRMDOWNLOADBACKEND_H
#define WMDRMDOWNLOADBACKEND_H

#include <bldvariant.hrh>
#if defined(RD_WMDRM_DLA_ENABLERS)

#include "dmcommon.h"
#include "dmcommoninternal.h"
#include "downloadbackend.h"
#include <e32base.h>

// forward declarations
class DownloadCore;
class ClientDownload;
class CCameseUtility;

// class declaration

// concrete download implementation for WM DRM downloads
class WMDRMDownloadBackend : public DownloadBackend, public CActive
{
    Q_OBJECT
public:
    WMDRMDownloadBackend(DownloadCore *dlCore, ClientDownload *dl);
    ~WMDRMDownloadBackend();

    // fetches the wmdrm download attributes
    QVariant getAttribute(DownloadAttribute attr);
    // sets the wmdrm download specific attributes
    int setAttribute(DownloadAttribute attr, const QVariant& value);
    // overloaded function for pausing the download
    int pause();
    // overloaded function for resuming paused download
    int resume();
    // overloaded function for cancelling the download
    int cancel();

    // stores the data in storage
    void store(QByteArray data, bool lastChunk=false);
    // deletes the storage
    void deleteStore();
    // returns the size of stored data
    qint64 storedDataSize();
 
    // from base class CActive
    void RunL();
    void DoCancel();


private slots:
    void bytesRecieved(qint64 bytesRecieved, qint64 bytesTotal);
    void handleFinished();    
private:
    // event handler
    bool event(QEvent *event);
    // mark download failed
    void markDownloadFailed(const QString& error);

private:
    DownloadCore *m_downloadCore;
    ClientDownload *m_download;
    ClientDownload *m_contentDownload; // not owned
    CCameseUtility* m_utility;
    HBufC8* m_contentUrl;
    
    // Error status for the synchronous request
    TInt m_requestResult;

    //Waiter for the synchronous request  
    CActiveSchedulerWait m_wait;   
};
#endif
#endif   
