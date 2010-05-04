/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "downloadcontroller.h"
#include "downloadcontroller_p.h"

#include <QNetworkProxy>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>
#include <QWebPage>

#ifdef USE_DOWNLOAD_MANAGER
#include "download.h"
#include "downloadmanager.h"

static const char * downloadErrorToString(QNetworkReply::NetworkError error)
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
        return 0;
    }
}

static const char * downloadEventToString(DEventType type)
{
    switch (type) {
    case DownloadCreated:
        return "DownloadManager:DownloadCreated";
    case DownloadsCleared:
        return "DownloadManager:DownloadsCleared";
    case ConnectedToServer:
        return "DownloadManager:ConnectedToServer";
    case DisconnectedFromServer:
        return "DownloadManager:DisconnectedFromServer";
    case ServerError:
        return "DownloadManager:ServerError";
    case Started:
        return "Download:Started";
    case HeaderReceived:
        return "Download:HeaderReceived";
    case Progress:
        return "Download:Progress";
    case Completed:
        return "Download:Completed";
    case Paused:
        return "Download:Paused";
    case Cancelled:
        return "Download:Cancelled";
    case Failed:
        return "Download:Failed";
    case DescriptorUpdated:
        return "Download:DescriptorUpdated";
    case NetworkLoss:
        return "Download:NetworkLoss";
    case Error:
        return "Download:Error";
    case OMADownloadDescriptorReady:
        return "Download:OMADownloadDescriptorReady";
    case WMDRMLicenseAcquiring:
        return "Download:WMDRMLicenseAcquiring";
    default:
        return 0;
    }
}

static const char * downloadPriorityToString(DownloadPriority priority)
{
    switch (priority) {
    case High:
        return "High";
    case Low:
        return "Low";
    default:
        return 0;
    }
}

static const char * downloadScopeToString(DownloadScope scope)
{
    switch (scope) {
    case Normal:
        return "Normal";
    case Background:
        return "Background";
    default:
        return 0;
    }
}

static const char * downloadStateToString(DownloadState state)
{
    switch (state) {
    case DlNone:
	return "DlNone";
    case DlCreated:
	return "DlCreated";
    case DlStarted:
	return "DlStarted";
    case DlInprogress:
	return "DlInprogress";
    case DlPaused:
	return "DlPaused";
    case DlCompleted:
	return "DlCompleted";
    case DlFailed:
	return "DlFailed";
    case DlCancelled:
	return "DlCancelled";
    case DlDescriptorUpdated:
	return "DlDescriptorUpdated";
    default:
        return 0;
    }
}

static const char * downloadTypeToString(DownloadType type)
{
    switch (type) {
    case Parallel:
        return "Parallel";
    case Sequential:
        return "Sequential";
    default:
        return 0;
    }
}

static void debugDownloadStr(
    Download * download,
    DownloadAttribute attribute,
    const char * name)
{
    QString value = download->getAttribute(attribute).toString();
    if (value.length() == 0) {
        return;
    }

    qDebug() << "DL" << download->id() << name << value;
}

static void debugDownloadInt(
    Download * download,
    DownloadAttribute attribute,
    const char * name)
{
    int value = download->getAttribute(attribute).toInt();
    if (value == 0) {
        return;
    }

    qDebug() << "DL" << download->id() << name << value;
}

static void debugDownloadUInt(
    Download * download,
    DownloadAttribute attribute,
    const char * name)
{
    uint value = download->getAttribute(attribute).toUInt();
    if (value == 0) {
        return;
    }

    qDebug() << "DL" << download->id() << name << value;
}

static void debugDownloadError(
    Download * download,
    DownloadAttribute attribute,
    const char * name)
{
    int num = download->getAttribute(attribute).toInt();

    const char * str = downloadErrorToString(static_cast<QNetworkReply::NetworkError>(num));
    if (str == 0) {
        str = "???";
    }

    qDebug() << "DL" << download->id() << name << num << str;
}

static void debugDownloadPriority(
    Download * download,
    DownloadAttribute attribute,
    const char * name)
{
    int num = download->getAttribute(attribute).toInt();

    const char * str = downloadPriorityToString(static_cast<DownloadPriority>(num));
    if (str == 0) {
        str = "???";
    }

    qDebug() << "DL" << download->id() << name << num << str;
}

static void debugDownloadScope(
    Download * download,
    DownloadAttribute attribute,
    const char * name)
{
    int num = download->getAttribute(attribute).toInt();

    const char * str = downloadScopeToString(static_cast<DownloadScope>(num));
    if (str == 0) {
        str = "???";
    }

    qDebug() << "DL" << download->id() << name << num << str;
}

static void debugDownloadState(
    Download * download,
    DownloadAttribute attribute,
    const char * name)
{
    int num = download->getAttribute(attribute).toInt();

    const char * str = downloadStateToString(static_cast<DownloadState>(num));
    if (str == 0) {
        str = "???";
    }

    qDebug() << "DL" << download->id() << name << num << str;
}

static void debugDownloadType(
    Download * download,
    DownloadAttribute attribute,
    const char * name)
{
    int num = download->getAttribute(attribute).toInt();

    const char * str = downloadTypeToString(static_cast<DownloadType>(num));
    if (str == 0) {
        str = "???";
    }

    qDebug() << "DL" << download->id() << name << num << str;
}

void DownloadController::debugDownload(Download * download)
{
    debugDownloadState(download,
            DlDownloadState,
            "DlDownloadState");

    debugDownloadError(download,
            DlLastError,
            "DlLastError");

    debugDownloadStr(download,
            DlLastErrorString,
            "DlLastErrorString");

    debugDownloadStr(download,
            DlSourceUrl,
            "DlSourceUrl");

    debugDownloadStr(download,
            DlContentType,
            "DlContentType");

    debugDownloadStr(download,
            DlDestPath,
            "DlDestPath");

    debugDownloadStr(download,
            DlFileName,
            "DlFileName");

    debugDownloadInt(download,
            DlDownloadedSize,
            "DlDownloadedSize");

    debugDownloadInt(download,
            DlTotalSize,
            "DlTotalSize");

    debugDownloadInt(download,
            DlLastPausedSize,
            "DlLastPausedSize");

    debugDownloadInt(download,
            DlPercentage,
            "DlPercentage");

    debugDownloadStr(download,
            DlStartTime,
            "DlStartTime");

    debugDownloadStr(download,
            DlEndTime,
            "DlEndTime");

    debugDownloadUInt(download,
            DlElapsedTime,
            "DlElapsedTime");

    debugDownloadStr(download,
            DlRemainingTime,
            "DlRemainingTime");

    debugDownloadStr(download,
            DlSpeed,
            "DlSpeed");

    debugDownloadScope(download,
            DlDownloadScope,
            "DlDownloadScope");

    debugDownloadType(download,
            DlDownloadType,
            "DlDownloadType");

    debugDownloadPriority(download,
            DlPriority,
            "DlPriority");

    debugDownloadInt(download,
            DlProgressInterval,
            "DlProgressInterval");

    debugDownloadStr(download,
            OMADownloadDescriptorName,
            "OMADownloadDescriptorName");

    debugDownloadStr(download,
            OMADownloadDescriptorVersion,
            "OMADownloadDescriptorVersion");

    debugDownloadStr(download,
            OMADownloadDescriptorType,
            "OMADownloadDescriptorType");

    debugDownloadStr(download,
            OMADownloadDescriptorSize,
            "OMADownloadDescriptorSize");

    debugDownloadStr(download,
            OMADownloadDescriptorVendor,
            "OMADownloadDescriptorVendor");

    debugDownloadStr(download,
            OMADownloadDescriptorDescription,
            "OMADownloadDescriptorDescription");

    debugDownloadStr(download,
            OMADownloadDescriptorNextURL,
            "OMADownloadDescriptorNextURL");
}

static void debugDownloadEvent(DEventType type)
{
    const char * name = downloadEventToString(type);
    if (name == 0) {
        return;
    }

    qDebug() << "Received event" << name;
}

// DownloadControllerPrivate implementation

DownloadControllerPrivate::DownloadControllerPrivate(
    DownloadController * downloadController,
    const QString & client,
    const QNetworkProxy & proxy)
{
    m_downloadController = downloadController;

    m_downloadManager = new DownloadManager(client);
    m_downloadManager->registerEventReceiver(this);
    if (proxy.type() != QNetworkProxy::NoProxy)
        m_downloadManager->setProxy(proxy.hostName(), proxy.port());
}

DownloadControllerPrivate::~DownloadControllerPrivate()
{
    delete m_downloadManager;
}

static QString downloadFileName(QUrl url)
{
    QString scheme = url.scheme();

    // For http and https, let the download manager determine the filename.

    if (scheme == "http" || scheme == "https")
        return QString();

    // For data scheme (see http://en.wikipedia.org/wiki/Data_URI_scheme)
    // we don't have a file name per-se, so construct one from the content
    // type.

    if (scheme == "data") {
        // Typical example: data:image/png;base64,...

        QString path = url.path();
        QString type = path.section('/', 0, 0);
        QString subtype = path.section('/', 1, 1).section(';', 0, 0);

        // For now we just use type as base name and subtype
        // as extension.  For the common case of image/jpg
        // and stuff like that this works fine.

        return type + "." + subtype;
    }

    // For all other schemes, let the download manager determine the filename.

    return QString();
}

void DownloadControllerPrivate::startDownload(QNetworkReply * reply)
{
    QUrl url = reply->url();

    Download * download = m_downloadManager->createDownload(reply);

    startDownload(download, url);
}

void DownloadControllerPrivate::startDownload(const QNetworkRequest & request)
{
    QUrl url = request.url();

    Download * download = m_downloadManager->createDownload(url.toString());

    startDownload(download, url);
}

void DownloadControllerPrivate::startDownload(Download * download, const QUrl & url)
{
    // If necessary suggest an alternate file name.
    // The download manager will adjust the file name for us to handle
    // duplicates in the destination directory.

    QString file = downloadFileName(url);

    if (file.length() > 0) {
        QVariant value(file);
        download->setAttribute(DlFileName, value);
    }

    // Start download.

    emit m_downloadController->downloadCreated(download);

    download->registerEventReceiver(this);
    download->start();
}

bool DownloadControllerPrivate::handleDownloadManagerEvent(DownloadEvent * event)
{
    DEventType type = static_cast<DEventType>(event->type());

    switch (type) {
    case DownloadCreated:
        // Instead of waiting for the DownloadManager DownloadCreated event
        // we emit downloadCreated in startDownload above so that we can add
        // a pointer to the download created as a parameter.
        return true;

    case DownloadsCleared:
        emit m_downloadController->downloadsCleared();
        return true;

    case ConnectedToServer:
    case DisconnectedFromServer:
    case ServerError:
        return true;

    default:
        qDebug() << "Unexpected download manager event:" << type;
        return false;
    }
}

bool DownloadControllerPrivate::handleDownloadEvent(DownloadEvent * event)
{
    DEventType type = static_cast<DEventType>(event->type());

    DownloadEvent * dlEvent = static_cast<DownloadEvent*>(event);

    int dlId = dlEvent->getId();

    Download * download = m_downloadManager->findDownload(dlId);

    if (!download) {
        qDebug() << "Cannot found download with id" << dlId;
        return false;
    }

    int errorNum = download->getAttribute(DlLastError).toInt();

    const char * errorStr = downloadErrorToString(
            static_cast<QNetworkReply::NetworkError>(errorNum));

    QString error;
    if (errorStr != 0)
        error = errorStr;

    switch (type)
    {
    case Started:
        emit m_downloadController->downloadStarted(download);
        return true;

    case HeaderReceived:
        emit m_downloadController->downloadHeaderReceived(download);
        return true;

    case Progress:
        emit m_downloadController->downloadProgress(download);
        return true;

    case Completed:
        emit m_downloadController->downloadFinished(download);
        return true;

    case Paused:
        emit m_downloadController->downloadPaused(download, error);
        return true;

    case Cancelled:
        emit m_downloadController->downloadCancelled(download, error);
        return true;

    case Failed:
        emit m_downloadController->downloadFailed(download, error);
        return true;

    case DescriptorUpdated:
        // FIXME ;;; Update to support OMA and DRM.
        return true;

    case NetworkLoss:
        emit m_downloadController->downloadNetworkLoss(download, error);
        return true;

    case Error:
        emit m_downloadController->downloadError(download, error);
        return true;

    case OMADownloadDescriptorReady:
        // FIXME ;;; Update to support OMA and DRM.
        return true;

    case WMDRMLicenseAcquiring:
        // FIXME ;;; Update to support OMA and DRM.
        return true;

    default:
        qDebug() << "Unexpected download event:" << type;
        break;
    }

    return false;
}

bool DownloadControllerPrivate::event(QEvent * e)
{
    DownloadEvent * event = static_cast<DownloadEvent *>(e);

    DEventType type = static_cast<DEventType>(event->type());

    debugDownloadEvent(type);

    switch (type) {
    case DownloadCreated:
    case DownloadsCleared:
    case ConnectedToServer:
    case DisconnectedFromServer:
    case ServerError:
        return handleDownloadManagerEvent(event);

    case Started:
    case HeaderReceived:
    case Progress:
    case Completed:
    case Paused:
    case Cancelled:
    case Failed:
    case DescriptorUpdated:
    case NetworkLoss:
    case Error:
    case OMADownloadDescriptorReady:
    case WMDRMLicenseAcquiring:
        return handleDownloadEvent(event);

    default:
        return false;
    }
}

// DownloadController implementation

DownloadController::DownloadController(
    const QString & client,
    const QNetworkProxy & proxy)
{
    d = new DownloadControllerPrivate(this, client, proxy);
}

DownloadController::~DownloadController()
{
    delete d;
}

bool DownloadController::handlePage(QWebPage * page)
{
    bool succeeded = true;

    // Handle click on link when the link type is not supported.
    page->setForwardUnsupportedContent(true);
    if (!connect(page, SIGNAL(unsupportedContent(QNetworkReply *)),
            this, SLOT(startDownload(QNetworkReply *)))) {
        succeeded = false;
    };

    // Handle Save Link and Save Image requests from the context menu.
    if (!connect(page, SIGNAL(downloadRequested(const QNetworkRequest &)),
            this, SLOT(startDownload(const QNetworkRequest &)))) {
        succeeded = false;
    }

    return succeeded;
}

void DownloadController::startDownload(QNetworkReply * reply)
{
    QUrl url = reply->url();

    qDebug() << "Download unsupported content" << url;

    d->startDownload(reply);
}

void DownloadController::startDownload(const QNetworkRequest & request)
{
    QUrl url = request.url();

    qDebug() << "Save link or image" << url;

    d->startDownload(request);
}

#else // USE_DOWNLOAD_MANAGER

// Empty implementation for when DownloadManager is unsupported.

DownloadController::DownloadController(
    const QString & client,
    const QNetworkProxy & proxy)
{}

DownloadController::~DownloadController()
{}

bool DownloadController::handlePage(QWebPage * page)
{
    return true;
}

void DownloadController::startDownload(QNetworkReply * reply)
{}

void DownloadController::startDownload(const QNetworkRequest & request)
{}

#endif // USE_DOWNLOAD_MANAGER
