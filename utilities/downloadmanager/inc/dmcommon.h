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

#ifndef DOWNLOAD_COMMON_H_
#define DOWNLOAD_COMMON_H_

#define INVALID_DL_ID 0
#define IPC_ERR_START_VALUE -5000

// download states
enum DownloadState
{
    DlNone = 0,
    DlCreated,
    DlStarted,
    DlInprogress,
    DlPaused,
    DlCompleted,
    DlFailed,
    DlCancelled,
    DlDescriptorUpdated
};

// download attributes
enum DownloadAttribute
{
    /* general download attributes */
    DlDestPath = 0,                  // destination path(set)
    DlDownloadedSize,                // downloaded size(get)
    DlTotalSize,                     // total size(get)
    DlDownloadState,                 // download state(get)
    DlFileName,                      // file name(set/get)
    DlSourceUrl,                     // source url(get)
    DlContentType,                   // content type(get)
    DlLastError,                     // last error occured(get)
    DlLastErrorString,               // last error string(get)
    DlLastPausedSize,                // last download paused size(get)

    /* OMA Download Attributes */
    OMADownloadDescriptorName,       // "name" in OMA dd
    OMADownloadDescriptorVersion,    // "version" in OMA dd
    OMADownloadDescriptorType,       // "type" in OMA dd
    OMADownloadDescriptorSize,       // "size" in OMA dd
    OMADownloadDescriptorVendor,     // "vendor" in OMA dd
    OMADownloadDescriptorDescription,// "description" in OMA dd
    OMADownloadDescriptorNextURL,    // "nextURL" in OMA dd

    DlDownloadType,                  // download type (get)
    DlPriority,                      // download priority(set/get)
    DlDownloadScope,                  // download scope i.e normal/background(get)

    DlStartTime,                      // download start/resumed time in QDateTime (get)
    DlEndTime,                        // download end time in QDateTime if successfully completed (get)
    DlElapsedTime,                    // time elapsed since the download started/resumed( in secs (get)
    DlRemainingTime,                  // remaining time to download in secs(get)
    DlSpeed,                          // speed of the download in Bytes/sec(get)
    DlPercentage,                     // percentage of download(get)
    DlProgressInterval                // KiloBytes at which progress event has to be sent(set/get)
};

// download event attributes
enum DownloadEventAttribute
{
    HeaderReceivedStatusCode = 0
};

// download manager attributes
enum DownloadManagerAttribute
{
    DlMgrDestPath = 0,     // destination path
    DlMgrDlCount,          // total number of downloads
    DlMgrClientName,       // client name
    DlMgrServerError,      // last server error
    DlMgrProgressMode,     // quiet/nonquiet
    DlMgrPersistantMode    // Active/InActive
};

// download manager event attributes
enum DownloadManagerEventAttribute
{
};

// event types
enum DEventType
{
    DownloadCreated = 2000,        // atleast one dowload exists - DownloadManager event
    DownloadsCleared,              // no more downloads exist - DownloadManager event
    ConnectedToServer,             // connected to download manager server
    DisconnectedFromServer,        // disconnected from download manager server
    ServerError,                   // server error - DownloadManager event
    Started,                       // download started - Download event
    HeaderReceived,                // download headers recieved - Download event
    Progress,                      // download is in progress, bytes recieved - Download event
    Completed,                     // download is completed - Download event
    Paused,                        // download is paused - Download event
    Cancelled,                     // download is cancelled - Download event
    Failed,                        // download is failed - Download event
    DescriptorUpdated,             // download is restarted - Download event
    NetworkLoss,                   // Network is lost - Download event
    Error,                         // error occurred during download - Download event
    OMADownloadDescriptorReady,    // OMA DD recieved and ready to be displayed - Download event
    WMDRMLicenseAcquiring          // License acquisition in progress for wmdrm
};

// download manager type
enum DownloadType
{
    Parallel = 0,
    Sequential
};

// Sequential Download Priority
enum DownloadPriority
{
    High = 0,
    Low
};

// Download scope
enum DownloadScope
{
    Normal = 0,
    Background
};

enum DownloadMgrProgressMode
{
    Quiet = 0,
    NonQuiet
};

enum DownloadMgrPersistantMode
{
    Active = 0,
    InActive
};

#endif /*DOWNLOAD_COMMON_H_ */
