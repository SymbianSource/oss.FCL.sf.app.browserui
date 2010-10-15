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

#include "progressivedownloadserver.h"
#include "download.h"
#include "dmcommon.h"
#include "dmcommoninternal.h"
#include <QTcpSocket>
#include <QHostAddress>

#define SERVER_WAIT_INTERVAL 1000

// private implementation
class ProgressiveDownloadServerPrivate
{
    DM_DECLARE_PUBLIC(ProgressiveDownloadServer);
public:
    ProgressiveDownloadServerPrivate();
    ~ProgressiveDownloadServerPrivate();
    QTcpServer* m_serverSocket;
    QTcpSocket* m_clientConnection;
    Download* m_download;
    DownloadState m_previousDlState;
};

ProgressiveDownloadServerPrivate::ProgressiveDownloadServerPrivate()
{
    m_serverSocket = 0;
    m_clientConnection = 0;
    m_download = 0;
    m_previousDlState = DlNone;
}

ProgressiveDownloadServerPrivate::~ProgressiveDownloadServerPrivate()
{
    if (m_serverSocket) {
        delete m_serverSocket;
        m_serverSocket = 0;
    }
}

Q_DECL_EXPORT ProgressiveDownloadServer::ProgressiveDownloadServer(Download* download)
{
    DM_INITIALIZE(ProgressiveDownloadServer);
    priv->m_download = download;

    // create server socket
    priv->m_serverSocket = new QTcpServer(this);
    connect(priv->m_serverSocket, SIGNAL(newConnection()), this, SLOT(handleConnection()));
}

ProgressiveDownloadServer::~ProgressiveDownloadServer()
{
    stopServer();
    DM_UNINITIALIZE(ProgressiveDownloadServer);
}

// starts the server
Q_DECL_EXPORT int ProgressiveDownloadServer::startServer(void)
{
    DM_PRIVATE(ProgressiveDownloadServer);
    if (!priv->m_serverSocket->isListening()) {
        QHostAddress localHost(QHostAddress::LocalHost);
        if (!priv->m_serverSocket->listen(localHost))
            return -1;
    }
    return 0;
}

// stops the server
Q_DECL_EXPORT int ProgressiveDownloadServer::stopServer(void)
{
    DM_PRIVATE(ProgressiveDownloadServer);
    // unregister the event listener
    priv->m_download->unregisterEventReceiver(this);

    // send the server down signal
    if ((priv->m_clientConnection) && (priv->m_clientConnection->state() == QAbstractSocket::ConnectedState)) {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << (quint16)ProgressiveDlServerDown;
        priv->m_clientConnection->write(block);
        priv->m_clientConnection->flush();

        // wait till the data is written
        if (priv->m_clientConnection->waitForBytesWritten(SERVER_WAIT_INTERVAL))
            return 0;
        else
            return -1;
    }

    if (priv->m_serverSocket->isListening())
        priv->m_serverSocket->close();

    return 0;
}

Q_DECL_EXPORT quint16 ProgressiveDownloadServer::serverPort(void)
{
    DM_PRIVATE(ProgressiveDownloadServer);
    if (priv->m_serverSocket)
        return priv->m_serverSocket->serverPort();
    else
        return 0;
}

// last error occurred
Q_DECL_EXPORT ProgressiveDlServerError ProgressiveDownloadServer::lastError(void)
{
    DM_PRIVATE(ProgressiveDownloadServer);
    return priv->m_serverSocket->serverError();
}

// last error occurred
Q_DECL_EXPORT QString ProgressiveDownloadServer::lastErrorString(void)
{
    DM_PRIVATE(ProgressiveDownloadServer);
    return priv->m_serverSocket->errorString();
}

void ProgressiveDownloadServer::handleConnection(void)
{
    DM_PRIVATE(ProgressiveDownloadServer);
    priv->m_clientConnection = priv->m_serverSocket->nextPendingConnection();
    connect(priv->m_clientConnection, SIGNAL(readyRead()),
            this, SLOT(handleRequest()));
    priv->m_download->registerEventReceiver(this);

    // right now only one client is supported
    priv->m_serverSocket->close();
}

void ProgressiveDownloadServer::handleRequest(void)
{
    // handles the request from client
    DM_PRIVATE(ProgressiveDownloadServer);
    quint16 requestCode;
    QDataStream in(priv->m_clientConnection);
    in.setVersion(QDataStream::Qt_4_0);

    if (priv->m_clientConnection->bytesAvailable() < (int)sizeof(quint16))
        return;

    in >> requestCode;
    // act according to the request
    if ((ProgressiveOperation)requestCode == DlPause)
        priv->m_download->pause();
    else if ((ProgressiveOperation)requestCode == DlResume)
        priv->m_download->resume();
    else if ((ProgressiveOperation)requestCode == DlCancel)
        priv->m_download->cancel();
    else if ((ProgressiveOperation)requestCode == DlGetAttribute) {
        // getAttribute is synchronous, so send the response here
        quint16 attr;
        in >> attr;
        QVariant value;
        value = priv->m_download->getAttribute((DownloadAttribute)attr);
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << (quint16)ProgressiveDlGetAttribute;
        out << value;
        priv->m_clientConnection->write(block);
        priv->m_clientConnection->flush();
    }
}

bool ProgressiveDownloadServer::event(QEvent *event)
{
    DM_PRIVATE(ProgressiveDownloadServer);
    DEventType type = (DEventType)event->type();
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    // handle the events from the download and send the response to the client
    switch(type) {
    case Progress:
        // send the response only once when it is resumed
        // to avoid continueously writing to the response buffer and increasing the ipc overhead 
        if (priv->m_previousDlState != DlInprogress) {
            priv->m_previousDlState = DlInprogress;
            out << (quint16)ProgressiveDlInprogress;
            priv->m_clientConnection->write(block);
            priv->m_clientConnection->flush();
        }
        break;
    case Paused:
        priv->m_previousDlState = DlPaused;
        out << (quint16)ProgressiveDlPaused;
        priv->m_clientConnection->write(block);
        priv->m_clientConnection->flush();
        break;
    case Completed:
        priv->m_previousDlState = DlCompleted;
        out << (quint16)ProgressiveDlCompleted;
        priv->m_clientConnection->write(block);
        priv->m_clientConnection->flush();
        break;
    case Failed:
        priv->m_previousDlState = DlFailed;
        out << (quint16)ProgressiveDlFailed;
        priv->m_clientConnection->write(block);
        priv->m_clientConnection->flush();
        break;
    case Cancelled:
        priv->m_previousDlState = DlCancelled;
        out << (quint16)ProgressiveDlCancelled;
        priv->m_clientConnection->write(block);
        priv->m_clientConnection->flush();
        break;
    case NetworkLoss:
        priv->m_previousDlState = DlPaused;
        out << (quint16)ProgressiveDlPaused;
        priv->m_clientConnection->write(block);
        priv->m_clientConnection->flush();
        break;
    default:
        break;
    }
    // event is consumed in any case.
    return true;
}
       

