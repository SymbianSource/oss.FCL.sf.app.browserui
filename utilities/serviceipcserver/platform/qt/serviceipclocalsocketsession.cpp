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


#include "serviceipclocalsocketsession.h"
#include "serviceipcobserver.h"
#include "serviceipcrequest.h"
#include <QtNetwork>
#include <clientinfo.h>

namespace WRT
{
const char KIPCSeparator = ';';
const char* REQUEST_COMPLETE_TOKEN = ";ROK";

/*!
 \class LocalSocketSession
 Class managing local socket server side sessions
 */

/*!
 Destructor
 */
LocalSocketSession::~LocalSocketSession()
{
    delete m_curRequest;
    m_curRequest = NULL;
    if (m_clientInfo) {
        delete m_clientInfo;
        m_clientInfo = NULL;
    }
}

/*!
 Constructor
 @param aNewSocket new socket connection to be handled
 @param aObserver observer to the IPC module
 */
LocalSocketSession::LocalSocketSession(QLocalSocket* aNewSocket,
                                       MServiceIPCObserver* aObserver) 
    : ServiceIPCSession(aObserver)
    , m_socket(aNewSocket)
{
    // Take ownership of the socket
    m_socket->setParent(this);
    QObject::connect(m_socket, SIGNAL( readyRead() ),
    this, SLOT( handleRequest() ) );

    QObject::connect(m_socket, SIGNAL( disconnected() ),
    this, SLOT( handleDisconnect() ) );
}

/*!
 Handle a new request
 */
void LocalSocketSession::handleRequest()
{
    // Process data
    QByteArray data = m_socket->readAll();
    // TODO: Get Client info
    ClientInfo *client = new ClientInfo();

    // New Request
    if (!m_curRequest) {
        // Should be very fast, the sperator is at the front
        int separator = data.indexOf(KIPCSeparator);
        int separator2 = data.indexOf(KIPCSeparator, separator + 1);

        // How long is the data, ensure separators are found, ie valid data
        //
        if (separator != -1 && separator2 != -1) {

            bool lengthConverted;
            QByteArray lengthData = data.left(separator);
            qint64 length = lengthData.toLong(&lengthConverted);
            if (lengthConverted) {
                // Compute the data 
                QByteArray operation = data.mid(separator + 1, separator2
                                - separator - 1);
                QByteArray requestData = data.right(data.length() - separator2
                                - 1);

                // New request
                delete m_curRequest;
                m_curRequest = NULL;
                m_curRequest = new ServiceIPCRequest(this, length, operation);

                // Call to observer with request if the length is correct
                // Otherwise wait until the data packet is re-assembled
                //
                if (m_curRequest->addRequestdata(requestData)) {
                    client->setSessionId(m_clientInfo->sessionId());
                    m_curRequest->setClientInfo(client); // ownership passed
                    //m_observer->handleRequest(m_curRequest);
                    handleReq();
                }
            }
        }
    }
    // More data available from the buffer
    else {
        // If all the data has been added, call back to the observer
        //
        if (m_curRequest->addRequestdata(data)) {
            m_curRequest->setClientInfo(client); // ownership passed
            //m_observer->handleRequest(m_curRequest);
            handleReq();
        }
    }
}

/*!
 Write data to the socket to send to the client
 @param aData data to write to the socket
 */
bool LocalSocketSession::write(const QByteArray& aData)
{
    int written = m_socket->write(aData);
    return (written != -1);
}

/*!
 Complete the outstanding IPC request
 */
bool LocalSocketSession::completeRequest()
{
    // Write a request complete token and wait until all data has been written to the socket
    m_socket->write(REQUEST_COMPLETE_TOKEN);
    bool done = m_socket->waitForBytesWritten(-1);
    delete m_curRequest;
    m_curRequest = NULL;
    return done;
}

/*!
 Handles when a client disconnect
 This slot function is connected to the underlying QLocalSocket
 */
void LocalSocketSession::handleDisconnect()
{
    if (m_appendToBList) {
        ((ServiceFwIPCServerLocalSocket*) parent())->removeBroadcastList(m_clientInfo->sessionId());  
    }
    doCancelRequest();

    // Emit signal back to server to cleanup
    emit disconnected(this);
    
    m_observer->handleClientDisconnect(m_clientInfo);
    
    ((ServiceFwIPCServerLocalSocket*) parent())->releaseSessionId(m_clientInfo->sessionId()); //release sessionId
}

/*!
 Closes and shutsdown the local socket session
 */
void LocalSocketSession::close()
{
    doCancelRequest();

    // Close the socket connection
    m_socket->abort();
}

/*! 
 Sends the cancel request callback to the client if a request was pending
 */
void LocalSocketSession::doCancelRequest()
{
    // If we had a request pending, make sure the client handles this
    // gracefully
    if (m_curRequest) {
        // TODO: Get client info
        ClientInfo *client = new ClientInfo();
        m_curRequest->setClientInfo(client); // ownership passed
        m_observer->handleCancelRequest(m_curRequest);
        delete m_curRequest;
        m_curRequest = NULL;
    }
}

}

/*!
 \fn WRT::LocalSocketSession::disconnected( ServiceIPCSession* aSession )
 
 Signal emitted when the session has been disconnected by the client
 @param aSession the session that is disconnected
 */

// END OF FILE
