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

#include "downloadmanager.h"
#include "clientdownload.h"
#include "downloadcore.h"
#include "downloadevent.h"
#include "wmdrmdownloadbackend.h"
#include <cameseutility.h>
#include <utf.h>
#include <QFileInfo>
#include <QUrl>

WMDRMDownloadBackend::WMDRMDownloadBackend(DownloadCore *dlCore, ClientDownload *dl)
    :DownloadBackend(dlCore, dl), CActive( EPriorityStandard )
{    
    m_downloadCore = dlCore;
    m_download = dl;
    m_contentDownload = NULL;
    m_utility = NULL;
    m_contentUrl = NULL;
     
    CActiveScheduler::Add( this );
} 

WMDRMDownloadBackend::~WMDRMDownloadBackend()
{
    if (m_utility) {
        delete m_utility;
        m_utility = NULL;
    }
    if (m_contentUrl) {
        delete m_contentUrl;
        m_contentUrl = NULL;
    }
    if (m_contentDownload) {
        m_contentDownload->unregisterEventReceiver(this);
        delete m_contentDownload;
        m_contentDownload = NULL;
    }  
}

int WMDRMDownloadBackend::pause()
{
    if (m_contentDownload) {
        m_contentDownload->pause();
        setDownloadState(DlPaused);
    }
    return 0;
}

int WMDRMDownloadBackend::resume()
{
    if (m_contentDownload)
        return (m_contentDownload->resume());
    return 0;
}

int WMDRMDownloadBackend::cancel()
{
    if (m_contentDownload) 
        return (m_contentDownload->cancel());
    
    return 0;
}

QVariant WMDRMDownloadBackend::getAttribute(DownloadAttribute attr)
{
    switch(attr) {
    case DlFileName:
    {
        if(m_contentDownload)
            return m_contentDownload->getAttribute(DlFileName);
    }
    case DlContentType:
    {
        if(m_contentDownload) {
            return m_contentDownload->getAttribute(DlContentType);
        }
    }
    default:
        return DownloadBackend::getAttribute(attr);
    }    
    return QVariant();
}

int WMDRMDownloadBackend::setAttribute(DownloadAttribute attr, const QVariant& value)
{
    return DownloadBackend::setAttribute(attr, value);
}

// stores the data in storage
void WMDRMDownloadBackend::store(QByteArray /*data*/, bool /*lastChunk=false*/)
{
    return;
}

// deletes the storage
void WMDRMDownloadBackend::deleteStore()
{
    return;
}

// returns the size of stored data
qint64 WMDRMDownloadBackend::storedDataSize()
{
    return 0;
}

void WMDRMDownloadBackend::bytesRecieved(qint64 /*bytesRecieved*/, qint64 /*bytesTotal*/)
{
     //Do nothing. This is here to avoid this signal to reach to base class' slot.
     return;
}

void WMDRMDownloadBackend::handleFinished()
{
    QByteArray data = m_downloadCore->reply()->readAll();
    TPtrC8 drmHeader((TUint8 *)(data.constData()));
    // delete the persistant information if saved any
    deleteInfo(); 
    
    TRAPD(err, m_utility = CCameseUtility::NewL());
    if (err != KErrNone) {
        markDownloadFailed(tr("General Error"));
        return;
    }
    // Use the Camese Interface, passing in
    // the DRM header url. Camese uses it for
    // error reporting.
    ASSERT( !m_contentUrl );
    QByteArray url = m_downloadCore->reply()->url().toEncoded();
    TPtrC8 s60Url((TUint8 *)(url.constData()));

    m_contentUrl = s60Url.Alloc();
    if (!m_contentUrl) {
        markDownloadFailed(tr("General Error"));
        return;
    }
    // post the event
    postEvent(WMDRMLicenseAcquiring, NULL);
    m_requestResult = KErrNone;
    m_utility->AcquireLicense( drmHeader, m_contentUrl, iStatus );
    SetActive();
    // Wait for completion of the license aquisition    
    m_wait.Start();     
    
    // Check request results
    if ( !m_requestResult ) {
        if (!m_contentUrl || (m_contentUrl->Length() == 0)) {
            markDownloadFailed(tr("File is corrupt"));
            return;
        } 
    }
    else if (m_requestResult == KErrCancel) {
        // Check if the content URL was correctly set.                
        markDownloadFailed(tr("Cancelled"));
        return;
    }
    else {
        // Check if the content URL was correctly set.                
        markDownloadFailed(tr("General Error"));
        return;
    }
    
    if ( m_contentUrl && m_contentUrl->CompareF( s60Url ) ) {   
        HBufC* s60str;  
        TRAPD(error, s60str = CnvUtfConverter::ConvertToUnicodeFromUtf8L(m_contentUrl->Des()));
        if (error != KErrNone) {
            markDownloadFailed(tr("General Error"));
            return;      
        }
        QString contentUrl = QString::fromUtf16(s60str->Ptr(),s60str->Length());
        delete s60str;
 
        QUrl qtUrl(contentUrl);
        if (qtUrl.isValid()) {
            deleteInfo(); 
            m_contentDownload = new ClientDownload(m_download->downloadManager(), contentUrl, m_download->id()); 
            if (!m_contentDownload) {                
                markDownloadFailed(tr("General Error"));
                return;               
            }
            // set the file name
            m_download->attributes().insert(DlFileName, m_contentDownload->getAttribute(DlFileName));
            m_contentDownload->registerEventReceiver(this);
            m_contentDownload->start();            
        } else {
            markDownloadFailed(tr("Malformed Url"));
            return; 
        }
    } else {
        markDownloadFailed(tr("No valid redirection Url"));
        return;  
    }
    
    // reset
    delete m_contentUrl;
    m_contentUrl = NULL;
    
    return;     
}          

bool WMDRMDownloadBackend::event(QEvent *event)
{
    DEventType type = (DEventType)event->type();
    switch(type) {
    case Started:
    case HeaderReceived:
        break;
    case Progress:
    {
        QVariant tSize = m_contentDownload->getAttribute(DlTotalSize);
        setTotalSize(tSize.toInt());
        QVariant curDlsize = m_contentDownload->getAttribute(DlDownloadedSize);
        setDownloadedDataSize(curDlsize.toInt());
        setDownloadState(DlInprogress);
        postEvent(Progress, NULL);
        break;
    }
    case NetworkLoss:
    {
        postEvent(NetworkLoss, NULL);
        break;
    }
    case Cancelled:
    { 
        setDownloadState(DlCancelled);
        postEvent(Cancelled, NULL);
        break;
    }
    case Completed:
    {
        setDownloadState(DlCompleted);
        postEvent(Completed, NULL); 
        break;
    }
    case Error:
    {
        setDownloadState(DlFailed);
        postEvent(Error, NULL);
        break;
    } 
    default:
        break;
    }
    return true;
}

void WMDRMDownloadBackend::markDownloadFailed(const QString& error)
{
    m_download->setError(error);
    setDownloadState(DlFailed);
    postEvent(Error, NULL);
    return;
}

void WMDRMDownloadBackend::RunL()
{
    TInt result( iStatus.Int() );   
    // Handle request completion
    m_requestResult = result;
    // Stop Wait loop                            
    m_wait.AsyncStop();            
}

void WMDRMDownloadBackend::DoCancel()
{   
    m_utility->Cancel();    
    // Cleanup
    m_requestResult = KErrCancel;
    // Stop Wait loop           
    m_wait.AsyncStop();
}
 
