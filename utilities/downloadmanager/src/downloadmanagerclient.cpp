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

#include "downloadmanagerclient.h"
#include "backgrounddownloadmanager.h"
#include "backgrounddownload.h"
#include <serviceipc.h>
#include <QTimer>
#include <QtNetwork>

class DownloadManagerClientPrivate
{
    DM_DECLARE_PUBLIC(DownloadManagerClient);
public:
    DownloadManagerClientPrivate();
    ~DownloadManagerClientPrivate();
    QString m_clientName;
    bool m_isConnected;
    int m_error;
    WRT::ServiceFwIPC* m_session;
    DownloadManager* m_downloadManager;
    QTimer* m_timer;
};

DownloadManagerClientPrivate::DownloadManagerClientPrivate()
{
    m_clientName = "";
    m_isConnected = false;
    m_error = 0;
    m_session = 0;
    m_downloadManager = 0;
    m_timer = 0;
}

DownloadManagerClientPrivate::~DownloadManagerClientPrivate()
{
    if (m_timer) {
        if (m_timer->isActive())
            m_timer->stop();
        delete m_timer;
        m_timer = 0;
    }

    if (m_session) {
        m_session->disconnect();
        delete m_session;
        m_session = 0;
    }
}

DownloadManagerClient::DownloadManagerClient(DownloadManager* downloadManager)
{
    DM_INITIALIZE(DownloadManagerClient);
    priv->m_downloadManager = downloadManager;
    priv->m_timer = new QTimer;
    priv->m_clientName = downloadManager->getAttribute(DlMgrClientName).toString();
    priv->m_session = new WRT::ServiceFwIPC(this);
    connect(priv->m_session, SIGNAL(error(int aError)), this, SLOT(setServerError(int error)));
    connect(priv->m_timer, SIGNAL(timeout()), this, SLOT(getEvents()));

    // establish communication with server
    initServer();
}

DownloadManagerClient::~DownloadManagerClient()
{
    DM_UNINITIALIZE(DownloadManagerClient);
}

bool DownloadManagerClient::initServer()
{
    DM_PRIVATE(DownloadManagerClient);

    // start server (if required) and connect to it
    connectToServer();

    if (priv->m_isConnected) {
        // set startup details - proxy, download path, progress mode
        // and attach to downloads
        setStartupInfo();

        // start timer to get events from server
        priv->m_timer->start(EVENTS_REQUEST_INTERVAL);
        priv->m_downloadManager->postEvent(ConnectedToServer, NULL);
    }

    return priv->m_isConnected;
}

bool DownloadManagerClient::connectToServer()
{
    DM_PRIVATE(DownloadManagerClient);
    if (priv->m_session) {
        int retry(2);
        for (;;)
        {
            priv->m_isConnected = priv->m_session->connect(DMSERVER);
            if (priv->m_isConnected) {
                break;
            }
            else {
                if (!priv->m_session->startServer(DMSERVER, DMSERVEREXE)) {
                    // start server failed.
                    break;
                }
            }
            if (0==--retry) {
                break;
            }
        }
    }
    if (!priv->m_isConnected) {
        delete priv->m_session;
        priv->m_session = 0;
    }

    return priv->m_isConnected;
}

void DownloadManagerClient::setStartupInfo()
{
    DM_PRIVATE(DownloadManagerClient);
    if (!priv->m_isConnected)
        return;

    if (priv->m_downloadManager) {
        // get data from download manager
        QNetworkProxy* proxy = priv->m_downloadManager->proxy();
        QString dlPath = priv->m_downloadManager->getAttribute(DlMgrDestPath).toString();
        int progressMode = priv->m_downloadManager->getAttribute(DlMgrProgressMode).toInt();
        int persistantMode = priv->m_downloadManager->getAttribute(DlMgrPersistantMode).toInt();
        QString data;

        // client name
        data.append(encodeString(priv->m_clientName));

        // proxy info
        if (proxy) {
            data.append(DM_MSG_DELIMITER);
            data.append(QString::number(SetProxy));
            data.append(DM_FIELD_DELIMITER);
            data.append(encodeString(proxy->hostName()));
            data.append(DM_FIELD_DELIMITER);
            data.append(QString::number(proxy->port()));
        }

        // download path
        data.append(DM_MSG_DELIMITER);
        data.append(QString::number(SetDownloadManagerAttribute));
        data.append(DM_FIELD_DELIMITER);
        data.append(QString::number(DlMgrDestPath));
        data.append(DM_FIELD_DELIMITER);
        data.append(encodeString(dlPath));

        // progress mode (quiet/non-quiet)
        data.append(DM_MSG_DELIMITER);
        data.append(QString::number(SetDownloadManagerAttribute));
        data.append(DM_FIELD_DELIMITER);
        data.append(QString::number(DlMgrProgressMode));
        data.append(DM_FIELD_DELIMITER);
        data.append(encodeString(QString::number(progressMode)));

        // persistant mode (active / inactive)
        data.append(DM_MSG_DELIMITER);
        data.append(QString::number(SetDownloadManagerAttribute));
        data.append(DM_FIELD_DELIMITER);
        data.append(QString::number(DlMgrPersistantMode));
        data.append(DM_FIELD_DELIMITER);
        data.append(encodeString(QString::number(persistantMode)));

        // send to sever
        if (priv->m_session->sendSync(QString::number(StartupInfo), data.toAscii()))
            priv->m_session->readAll();
    }
}

void DownloadManagerClient::setProxy(const QString& proxyServer, const int port)
{
    DM_PRIVATE(DownloadManagerClient);
    if (!priv->m_isConnected) {
        if (!initServer())
            return;
    }

    // create string to send
    QString data;
    data.append(encodeString(priv->m_clientName));
    data.append(DM_FIELD_DELIMITER);
    data.append(encodeString(proxyServer));
    data.append(DM_FIELD_DELIMITER);
    data.append(QString::number(port));

    // send to sever
    if (priv->m_session->sendSync(QString::number(SetProxy), data.toAscii()))
        priv->m_session->readAll();
    else
        setServerError(WRT::ServiceFwIPC::EIPCError);
}

int DownloadManagerClient::createDownload(const QString& url, DownloadType type)
{
    DM_PRIVATE(DownloadManagerClient);
    int downloadId(INVALID_DL_ID);
    if (!priv->m_isConnected) {
        if (!initServer())
            return downloadId;
    }

    // create string to send
    QString data;
    data.append(encodeString(priv->m_clientName));
    data.append(DM_FIELD_DELIMITER);
    data.append(encodeString(url));
    data.append(DM_FIELD_DELIMITER);
    data.append(QString::number(type));

    // send to sever
    if (priv->m_session->sendSync(QString::number(CreateDownload), data.toAscii())) {
        // expected response is
        // list[0] -> downloadId
        QByteArray bytes = priv->m_session->readAll();
        QStringList list = QString(bytes).split(DM_FIELD_DELIMITER);
        if (list.count() == 1)
            downloadId = list[0].toInt();
    }
    else {
        setServerError(WRT::ServiceFwIPC::EIPCError);
    }
    return downloadId;
}

bool DownloadManagerClient::attachToDownload(int dlId)
{
    DM_PRIVATE(DownloadManagerClient);
    bool status(false);
    if (!priv->m_isConnected) {
        if (!initServer())
            return status;
    }

    // create string to send
    QString data;
    data.append(encodeString(priv->m_clientName));
    data.append(DM_FIELD_DELIMITER);
    data.append(QString::number(dlId));

    // send to sever
    if (priv->m_session->sendSync(QString::number(AttachToDownload), data.toAscii())) {
        // expected response is
	// list[1] -> status
        QByteArray bytes = priv->m_session->readAll();
        QStringList list = QString(bytes).split(DM_FIELD_DELIMITER);
        if (list.count() == 1) {
            status = list[0].toInt();
	}
    }
    else {
        setServerError(WRT::ServiceFwIPC::EIPCError);
    }
	
    return status;
}

void DownloadManagerClient::removeDownload(int downloadId)
{
    DM_PRIVATE(DownloadManagerClient);
    if (!priv->m_isConnected) {
        if (!initServer())
            return;
    }

    // create string to send
    QString data;
    data.append(encodeString(priv->m_clientName));
    data.append(DM_FIELD_DELIMITER);
    data.append(QString::number(downloadId));

    // send to sever
    if (priv->m_session->sendSync(QString::number(RemoveDownload), data.toAscii()))
        priv->m_session->readAll();
    else
        setServerError(WRT::ServiceFwIPC::EIPCError);
}

void DownloadManagerClient::pauseAll()
{
    DM_PRIVATE(DownloadManagerClient);
    if (!priv->m_isConnected) {
        if (!initServer())
            return;
    }

    // create string to send
    QString data;
    data.append(encodeString(priv->m_clientName));

    // send to sever
    if (priv->m_session->sendSync(QString::number(PauseAll), data.toAscii()))
        priv->m_session->readAll();
    else
        setServerError(WRT::ServiceFwIPC::EIPCError);
}

void DownloadManagerClient::resumeAll()
{
    DM_PRIVATE(DownloadManagerClient);
    if (!priv->m_isConnected) {
        if (!initServer())
            return;
    }

    // create string to send
    QString data;
    data.append(encodeString(priv->m_clientName));

    // send to sever
    if (priv->m_session->sendSync(QString::number(ResumeAll), data.toAscii()))
        priv->m_session->readAll();
    else
        setServerError(WRT::ServiceFwIPC::EIPCError);
}

void DownloadManagerClient::removeAll()
{
    DM_PRIVATE(DownloadManagerClient);
    if (!priv->m_isConnected) {
        if (!initServer())
            return;
    }

    // create string to send
    QString data;
    data.append(encodeString(priv->m_clientName));

    // send to sever
    if (priv->m_session->sendSync(QString::number(RemoveAll), data.toAscii()))
        priv->m_session->readAll();
    else
        setServerError(WRT::ServiceFwIPC::EIPCError);
}

QVariant DownloadManagerClient::getDownloadManagerAttribute(DownloadManagerAttribute attr)
{
    DM_PRIVATE(DownloadManagerClient);
    QVariant returnValue;
    if (!priv->m_isConnected) {
        if (!initServer())
            return returnValue;
    }

    // create string to send
    QString data;
    data.append(encodeString(priv->m_clientName));
    data.append(DM_FIELD_DELIMITER);
    data.append(QString::number(attr));

    // send to sever
    if (priv->m_session->sendSync(QString::number(GetDownloadManagerAttribute), data.toAscii())) {
        // expected response is
        // list[0] -> attribute
        // list[1] -> value
        QByteArray bytes = priv->m_session->readAll();
        QStringList list = QString(bytes).split(DM_FIELD_DELIMITER);
        if (list.count() == 2) {
            if (list[0] == QString::number(attr))
                returnValue = QVariant(list[1]);
        }
    }
    else {
        setServerError(WRT::ServiceFwIPC::EIPCError);
    }
    return returnValue;
}

int DownloadManagerClient::setDownloadManagerAttribute(DownloadManagerAttribute attr, const QVariant& value)
{
    DM_PRIVATE(DownloadManagerClient);
    int returnValue(-1);
    if (!priv->m_isConnected) {
        if (!initServer())
            return returnValue;
    }

    // create string to send
    QString data;
    data.append(encodeString(priv->m_clientName));
    data.append(DM_FIELD_DELIMITER);
    data.append(QString::number(attr));
    data.append(DM_FIELD_DELIMITER);
    data.append(encodeString(value.toString()));

    // send to sever
    if (priv->m_session->sendSync(QString::number(SetDownloadManagerAttribute), data.toAscii())) {
        // expected response is
        // list[0] -> attribute
        // list[1] -> returnValue
        QByteArray bytes = priv->m_session->readAll();
        QStringList list = QString(bytes).split(DM_FIELD_DELIMITER);
        if (list.count() == 2) {
            if (list[0] == QString::number(attr))
                returnValue = list[1].toInt();
        }
    }
    else {
        setServerError(WRT::ServiceFwIPC::EIPCError);
    }
    return returnValue;
}

int DownloadManagerClient::startDownload(int dlId)
{
    DM_PRIVATE(DownloadManagerClient);
    int returnValue(-1);
    if (!priv->m_isConnected) {
        if (!initServer())
            return returnValue;
    }

    // create string to send
    QString data;
    data.append(encodeString(priv->m_clientName));
    data.append(DM_FIELD_DELIMITER);
    data.append(QString::number(dlId));

    // send to sever
    if (priv->m_session->sendSync(QString::number(StartDownload), data.toAscii())) {
        // expected response is
        // list[0] -> downloadId
        // list[1] -> returnValue
        QByteArray bytes = priv->m_session->readAll();
        QStringList list = QString(bytes).split(DM_FIELD_DELIMITER);
        if (list.count() == 2) {
            if (list[0] == QString::number(dlId))
                returnValue = list[1].toInt();
        }
    }
    else {
        setServerError(WRT::ServiceFwIPC::EIPCError);
    }
    return returnValue;
}

int DownloadManagerClient::pauseDownload(int dlId)
{
    DM_PRIVATE(DownloadManagerClient);
    int returnValue(-1);
    if (!priv->m_isConnected) {
        if (!initServer())
            return returnValue;
    }

    // create string to send
    QString data;
    data.append(encodeString(priv->m_clientName));
    data.append(DM_FIELD_DELIMITER);
    data.append(QString::number(dlId));

    // send to sever
    if (priv->m_session->sendSync(QString::number(PauseDownload), data.toAscii())) {
        // expected response is
        // list[0] -> downloadId
        // list[1] -> returnValue
        QByteArray bytes = priv->m_session->readAll();
        QStringList list = QString(bytes).split(DM_FIELD_DELIMITER);
        if (list.count() == 2) {
            if (list[0] == QString::number(dlId))
                returnValue = list[1].toInt();
        }
    }
    else {
        setServerError(WRT::ServiceFwIPC::EIPCError);
    }
    return returnValue;
}

int DownloadManagerClient::resumeDownload(int dlId)
{
    DM_PRIVATE(DownloadManagerClient);
    int returnValue(-1);
    if (!priv->m_isConnected) {
        if (!initServer())
            return returnValue;
    }

    // create string to send
    QString data;
    data.append(encodeString(priv->m_clientName));
    data.append(DM_FIELD_DELIMITER);
    data.append(QString::number(dlId));

    // send to sever
    if (priv->m_session->sendSync(QString::number(ResumeDownload), data.toAscii())) {
        // expected response is
        // list[0] -> downloadId
        // list[1] -> returnValue
        QByteArray bytes = priv->m_session->readAll();
        QStringList list = QString(bytes).split(DM_FIELD_DELIMITER);
        if (list.count() == 2) {
            if (list[0] == QString::number(dlId))
                returnValue = list[1].toInt();
        }
    }
    else {
        setServerError(WRT::ServiceFwIPC::EIPCError);
    }
    return returnValue;
}

int DownloadManagerClient::cancelDownload(int dlId)
{
    DM_PRIVATE(DownloadManagerClient);
    int returnValue(-1);
    if (!priv->m_isConnected) {
        if (!initServer())
            return returnValue;
    }

    // create string to send
    QString data;
    data.append(encodeString(priv->m_clientName));
    data.append(DM_FIELD_DELIMITER);
    data.append(QString::number(dlId));

    // send to sever
    if (priv->m_session->sendSync(QString::number(CancelDownload), data.toAscii())) {
        // expected response is
        // list[0] -> downloadId
        // list[1] -> returnValue
        QByteArray bytes = priv->m_session->readAll();
        QStringList list = QString(bytes).split(DM_FIELD_DELIMITER);
        if (list.count() == 2) {
            if (list[0] == QString::number(dlId))
                returnValue = list[1].toInt();
        }
    }
    else {
        setServerError(WRT::ServiceFwIPC::EIPCError);
    }
    return returnValue;
}

QVariant DownloadManagerClient::getDownloadAttribute(int dlId, DownloadAttribute attr)
{
    DM_PRIVATE(DownloadManagerClient);
    QVariant returnValue;
    if (!priv->m_isConnected) {
        if (!initServer())
            return returnValue;
    }

    // create string to send
    QString data;
    data.append(encodeString(priv->m_clientName));
    data.append(DM_FIELD_DELIMITER);
    data.append(QString::number(dlId));
    data.append(DM_FIELD_DELIMITER);
    data.append(QString::number(attr));

    // send to sever
    if (priv->m_session->sendSync(QString::number(GetDownloadAttribute), data.toAscii())) {
        // expected response is
        // list[0] -> downloadId
        // list[1] -> attribute
        // list[2] -> value
        QByteArray bytes = priv->m_session->readAll();
        QStringList list = QString(bytes).split(DM_FIELD_DELIMITER);
        if (list.count() == 3) {
            if ((list[0] == QString::number(dlId)) &&
                (list[1] == QString::number(attr)))
                returnValue = QVariant(list[2]);
        }
    }
    else {
        setServerError(WRT::ServiceFwIPC::EIPCError);
    }
    return returnValue;
}

int DownloadManagerClient::setDownloadAttribute(int dlId, DownloadAttribute attr, const QVariant& value)
{
    DM_PRIVATE(DownloadManagerClient);
    int returnValue(-1);
    if (!priv->m_isConnected) {
        if (!initServer())
            return returnValue;
    }

    // create string to send
    QString data;
    data.append(encodeString(priv->m_clientName));
    data.append(DM_FIELD_DELIMITER);
    data.append(QString::number(dlId));
    data.append(DM_FIELD_DELIMITER);
    data.append(QString::number(attr));
    data.append(DM_FIELD_DELIMITER);
    data.append(encodeString(value.toString()));

    // send to sever
    if (priv->m_session->sendSync(QString::number(SetDownloadAttribute), data.toAscii())) {
        // expected response is
        // list[0] -> downloadId
        // list[1] -> attribute
        // list[2] -> returnValue
        QByteArray bytes = priv->m_session->readAll();
        QStringList list = QString(bytes).split(DM_FIELD_DELIMITER);
        if (list.count() == 3) {
            if ((list[0] == QString::number(dlId)) &&
                (list[1] == QString::number(attr)))
                returnValue = list[2].toInt();
        }
    }
    else
    {
        setServerError(WRT::ServiceFwIPC::EIPCError);
    }
    return returnValue;
}

// poll server for events
void DownloadManagerClient::getEvents()
{
    DM_PRIVATE(DownloadManagerClient);
    if (!priv->m_isConnected) {
        if (!initServer())
            return;
    }

    // create string to send
    QString data;
    data.append(encodeString(priv->m_clientName));

    // send to sever
    if (priv->m_session->sendSync(QString::number(GetEvents), data.toAscii())) {
        // expected response has a list of downloadmanager and download events
        QString eventMsg = priv->m_session->readAll();
        if (!eventMsg.isEmpty())
            processEvents(eventMsg);

        // start timer for getting next set of events
        priv->m_timer->start(EVENTS_REQUEST_INTERVAL);
    }
    else {
        setServerError(WRT::ServiceFwIPC::EIPCError);
    }
}

// process Download and DownloadManager events
void DownloadManagerClient::processEvents(QString eventMsg)
{
    DM_PRIVATE(DownloadManagerClient);

    // Separate out the fields of an event
    QStringList list = eventMsg.split(DM_FIELD_DELIMITER);
    if (list.count() >= 2) {
        QString eventName = list[0];
        // // check type of event
        if (list[0] == QString::number(EventDownloadManager)) {
            // Download Manager event
            DEventType type = (DEventType)list[1].toInt();
            // post event
            priv->m_downloadManager->postEvent(type, NULL);
        }
        else if (list[0] == QString::number(EventDownload)) {
            // Download event
            int dlId = list[1].toInt();
            DEventType type = (DEventType)list[2].toInt();
            BackgroundDownload* dl = dynamic_cast<BackgroundDownload*>(priv->m_downloadManager->findDownload(dlId));
            // post event
            if (dl)
                dl->postEvent(type, NULL);
        }
    }
}

// encode string so that it does not contain any communication delimiter
QString DownloadManagerClient::encodeString(const QString& inputString)
{
    QByteArray bytes = QUrl::toPercentEncoding(inputString);
    return bytes.data();
}

// get server connection/communication error
int DownloadManagerClient::serverError()
{
    DM_PRIVATE(DownloadManagerClient);
    return priv->m_error;
}

// set server IPC error
void DownloadManagerClient::setServerError(int error)
{
    DM_PRIVATE(DownloadManagerClient);
    priv->m_error = error;
    // client could have died
    // mark client as disconnected so that it can reconnect again
    priv->m_isConnected = false;
    if (priv->m_timer->isActive())
        priv->m_timer->stop();
    priv->m_downloadManager->postEvent(DisconnectedFromServer, NULL);
    priv->m_downloadManager->postEvent(ServerError, NULL);
}
