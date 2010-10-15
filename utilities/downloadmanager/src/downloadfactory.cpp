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
#include "downloadfactory.h"
#include "clientdownload.h"
#include "downloadcore.h"
#include "downloadstore.h"
#include "filestorage.h"
#include "downloadbackend.h"
#include "httpdownloadbackend.h"
#include "omadownloadbackend.h"
#include "oma2downloadbackend.h"
#include <QNetworkReply>
#include <QNetworkRequest>

#ifdef __SYMBIAN32__
#include <bldvariant.hrh>
#include "drmstorage.h"
#ifdef RD_WMDRM_DLA_ENABLERS
#include "wmdrmdownloadbackend.h"
#endif
#endif

// This is responsible for creation of concrete download download implementation class
// based on the content type
DownloadBackend* DownloadAbstractFactory::createDownloadImplementation(DownloadCore *dlCore, ClientDownload *dl)
{
    QString contentType = dlCore->contentType();
    if(contentType == "")
    {
        QNetworkReply *reply = dlCore->reply();
        if(reply)
        {
            contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
        }
    }
    
    QString clientName = dl->downloadManager()->getAttribute(DlMgrClientName).toString();
    DownloadBackend *dlBackend = NULL;
    DownloadStore* store = NULL;
    if(contentType == OMA_CONTENT_TYPE) {
        dlBackend = new OMADownloadBackend(dlCore, dl);
    }
    else if(contentType == OMA2_CONTENT_TYPE) {
        dlBackend = new OMA2DownloadBackend(dlCore, dl);
    }

#ifdef __SYMBIAN32__
    else if(contentType == OMA_DRM_MESSAGE_CONTENT_TYPE) {
        store = new DrmStorage(clientName, dl);
        dlBackend = new HttpDownloadBackend(dlCore, dl, store);
    }
#ifdef RD_WMDRM_DLA_ENABLERS
    else if(contentType.contains(WMDRM_CONTENT_TYPE, Qt::CaseInsensitive))
        dlBackend = new WMDRMDownloadBackend(dlCore, dl);
#endif
#endif
    //we can have other if-else cases for different type of downloads
    // by default the download is http download
    else {
        store = new FileStorage(clientName, dl);
        dlBackend = new HttpDownloadBackend(dlCore, dl, store);
    }
    return dlBackend;
}




