/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#include "downloadproxy_p.h"

#include <QNetworkReply>
#include <QtDebug>

#ifdef USE_DOWNLOAD_MANAGER
#include "download.h"
#include "downloadmanager.h"
#endif // USE_DOWNLOAD_MANAGER

DownloadProxyData::DownloadProxyData(Download * download)
: m_download(download)
{}

DownloadProxyData::~DownloadProxyData()
{
    // Nothing to do, Download * is owned by DownloadManager.
}

#ifdef USE_DOWNLOAD_MANAGER

// Helper functions for translating various download attribute enum values.

static const char * downloadState(int state)
{
    switch (state) {
    case DlNone:
	return "None";
    case DlCreated:
	return "Created";
    case DlStarted:
	return "Started";
    case DlInprogress:
	return "InProgress";
    case DlPaused:
	return "Paused";
    case DlCompleted:
	return "Completed";
    case DlFailed:
	return "Failed";
    case DlCancelled:
	return "Cancelled";
    case DlDescriptorUpdated:
	return "DescriptorUpdated";
    default:
        return "???";
    }
}

static const char * downloadError(QNetworkReply::NetworkError error)
{
    switch (error) {
    case QNetworkReply::NoError:
        return "QNetworkReply::NoError";
    case QNetworkReply::ConnectionRefusedError:
        return "QNetworkReply::ConnectionRefusedError";
    case QNetworkReply::RemoteHostClosedError:
        return "QNetworkReply::RemoteHostClosedError";
    case QNetworkReply::HostNotFoundError:
        return "QNetworkReply::HostNotFoundError";
    case QNetworkReply::TimeoutError:
        return "QNetworkReply::TimeoutError";
    case QNetworkReply::OperationCanceledError:
        return "QNetworkReply::OperationCanceledError";
    case QNetworkReply::SslHandshakeFailedError:
        return "QNetworkReply::SslHandshakeFailedError";
    case QNetworkReply::ProxyConnectionRefusedError:
        return "QNetworkReply::ProxyConnectionRefusedError";
    case QNetworkReply::ProxyConnectionClosedError:
        return "QNetworkReply::ProxyConnectionClosedError";
    case QNetworkReply::ProxyNotFoundError:
        return "QNetworkReply::ProxyNotFoundError";
    case QNetworkReply::ProxyTimeoutError:
        return "QNetworkReply::ProxyTimeoutError";
    case QNetworkReply::ProxyAuthenticationRequiredError:
        return "QNetworkReply::ProxyAuthenticationRequiredError";
    case QNetworkReply::ContentAccessDenied:
        return "QNetworkReply::ContentAccessDenied";
    case QNetworkReply::ContentOperationNotPermittedError:
        return "QNetworkReply::ContentOperationNotPermittedError";
    case QNetworkReply::ContentNotFoundError:
        return "QNetworkReply::ContentNotFoundError";
    case QNetworkReply::AuthenticationRequiredError:
        return "QNetworkReply::AuthenticationRequiredError";
    case QNetworkReply::ContentReSendError:
        return "QNetworkReply::ContentReSendError";
    case QNetworkReply::ProtocolUnknownError:
        return "QNetworkReply::ProtocolUnknownError";
    case QNetworkReply::ProtocolInvalidOperationError:
        return "QNetworkReply::ProtocolInvalidOperationError";
    case QNetworkReply::UnknownNetworkError:
        return "QNetworkReply::UnknownNetworkError";
    case QNetworkReply::UnknownProxyError:
        return "QNetworkReply::UnknownProxyError";
    case QNetworkReply::UnknownContentError:
        return "QNetworkReply::UnknownContentError";
    case QNetworkReply::ProtocolFailure:
        return "QNetworkReply::ProtocolFailure";
    default:
        return "???";
    }
}

// Helper functions to get download attribute of a particular type.

static int intAttribute(Download * download, DownloadAttribute which)
{
    return download->getAttribute(which).toInt();
}

static uint uintAttribute(Download * download, DownloadAttribute which)
{
    return download->getAttribute(which).toUInt();
}

static QString stringAttribute(Download * download, DownloadAttribute which)
{
    return download->getAttribute(which).toString();
}

// Helper functions for reporting download attributes.

template<typename T>
void debugDownloadAttribute(
        Download * download,
        char const * name,
        const T & value)
{
    qDebug() << "DL" << download->id() << name << value;
}

static void debugDownloadInt(
    Download * download,
    DownloadAttribute which,
    const char * name)
{
    int value = intAttribute(download, which);

    debugDownloadAttribute(download, name, value);
}

static void debugDownloadUInt(
    Download * download,
    DownloadAttribute which,
    const char * name)
{
    uint value = uintAttribute(download, which);

    debugDownloadAttribute(download, name, value);
}

static void debugDownloadStr(
    Download * download,
    DownloadAttribute which,
    const char * name)
{
    QString value = stringAttribute(download, which);

    debugDownloadAttribute(download, name, value);
}

static void debugDownloadState(Download * download)
{
    int num = intAttribute(download, DlDownloadState);

    const char * state = downloadState(num);

    debugDownloadAttribute(download, "DownloadState", state);

}

static void debugDownloadError(Download * download)
{
    int num = intAttribute(download, DlLastError);

    const char * error = downloadError(static_cast<QNetworkReply::NetworkError>(num));

    debugDownloadAttribute(download, "DownloadError", error);
}

// Helper function for implementing DownloadProxyData::debug().

static void debugDownload(Download * download)
{
    debugDownloadState(download);

    debugDownloadError(download);

    debugDownloadStr(download,
            DlLastErrorString,
            "LastErrorString");

    debugDownloadStr(download,
            DlSourceUrl,
            "SourceUrl");

    debugDownloadStr(download,
            DlContentType,
            "ContentType");

    debugDownloadStr(download,
            DlDestPath,
            "DestPath");

    debugDownloadStr(download,
            DlFileName,
            "FileName");

    debugDownloadInt(download,
            DlDownloadedSize,
            "DownloadedSize");

    debugDownloadInt(download,
            DlTotalSize,
            "TotalSize");

    debugDownloadInt(download,
            DlLastPausedSize,
            "LastPausedSize");

    debugDownloadInt(download,
            DlPercentage,
            "Percentage");

    debugDownloadStr(download,
            DlStartTime,
            "StartTime");

    debugDownloadStr(download,
            DlEndTime,
            "EndTime");

    debugDownloadUInt(download,
            DlElapsedTime,
            "ElapsedTime");

    debugDownloadStr(download,
            DlRemainingTime,
            "RemainingTime");

    debugDownloadStr(download,
            DlSpeed,
            "Speed");

    debugDownloadInt(download,
            DlProgressInterval,
            "ProgressInterval");
}

#endif // USE_DOWNLOAD_MANAGER

void DownloadProxyData::debug()
{
#ifdef USE_DOWNLOAD_MANAGER
    if (m_download == 0) {
        return;
    }

    int id = m_download->id();

    DownloadManager * manager = m_download->downloadManager();

    Download * download = manager->findDownload(id);

    if (download != 0) {
        debugDownload(download);
    }
#endif // USE_DOWNLOAD_MANAGER
}

void DownloadProxyData::remove()
{
#ifdef USE_DOWNLOAD_MANAGER
    if (m_download == 0) {
        return;
    }

    DownloadManager * manager = m_download->downloadManager();

    manager->removeOne(m_download);

    // Download is no longer valid.

    m_download = 0;
#endif // USE_DOWNLOAD_MANAGER
}

QString DownloadProxyData::fileName()
{
#ifdef USE_DOWNLOAD_MANAGER
    if (m_download == 0) {
        return QString();
    }

    return stringAttribute(m_download, DlFileName);
#else  // USE_DOWNLOAD_MANAGER
    return QString();
#endif // USE_DOWNLOAD_MANAGER
}
