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

#ifndef DOWNLOAD_COMMON_INTERNAL_H_
#define DOWNLOAD_COMMON_INTERNAL_H_

#define OMA_CONTENT_TYPE "application/vnd.oma.dd+xml"
#define OMA_VERSION_1 "1.0"
#define OMA2_CONTENT_TYPE "application/vnd.oma.dd2+xml"
#define OMA_VERSION_2 "2.0"
#define OMA_DRM_MESSAGE_CONTENT_TYPE "application/vnd.oma.drm.message"
#define WMDRM_CONTENT_TYPE "application/vnd.ms-wmdrm.lic-chlg-req"
#define ROOT_PATH "/root/"

// DownloadManagerServer definitions
#define DM_FIELD_DELIMITER ";"
#define DM_MSG_DELIMITER "&"
#define CLIENTNAME_SUFFIX "_Background"
#define DMSERVER "WrtDownloadManagerServer"
#ifdef Q_OS_LINUX 
#define DMSERVEREXE "./WrtDownloadManagerServer"
#else
#define DMSERVEREXE "WrtDownloadManagerServer.exe"
#endif

// DownloadManagerClient definitions
#define EVENTS_REQUEST_INTERVAL 50

// enum for progressive download operations supported
enum ProgressiveOperation
{
    DlPause = 0,
    DlResume,
    DlCancel,
    DlGetAttribute
};

// enum for operations supported
enum ProgressiveResponse
{
    ProgressiveDlPaused = 0,
    ProgressiveDlInprogress,
    ProgressiveDlCompleted,
    ProgressiveDlCancelled,
    ProgressiveDlFailed,
    ProgressiveDlGetAttribute,
    ProgressiveDlServerDown
};

// enum for http status codes
// NOTE: do not change order
enum HttpStatusCode
{
    HttpOK = 200,                   // 200 = ok
    HttpCreated,                    // 201 = created
    HttpAccepted,                   // 202 = accepted
    HttpNonAuthorativeInfo,         // 203 = non-authorative information
    HttpNoContent,                  // 204 = no content
    HttpResetContent,               // 205 = reset content
    HttpPartialContent,             // 206 = partial content
    HttpNotFound = 404,             // 404 = not found
    HttpPreconditionFailed = 412    // 412 = precondition failed
};

enum ClientServerOpCode
{
    // Events
    GetEvents = 0,                  // 0
    // Download Manager operation
    StartupInfo,                    // 1
    SetDownloadManagerAttribute,    // 2
    GetDownloadManagerAttribute,    // 3
    SetProxy,                       // 4
    CreateDownload,                 // 5
    RemoveDownload,                 // 6
    PauseAll,                       // 7
    ResumeAll,                      // 8
    RemoveAll,                      // 9
    // Download operation
    StartDownload,                  // 10
    PauseDownload,                  // 11
    ResumeDownload,                 // 12
    CancelDownload,                 // 13
    SetDownloadAttribute,           // 14
    GetDownloadAttribute,           // 15
    GetDownloadIds,                 // 16
    AttachToDownload                // 17
};

enum ClientServerEventMessage
{
    EventDownloadManager = 0,       // 0
    EventDownload                   // 1
};

#endif /*DOWNLOAD_COMMON_INTERNAL_H_ */


