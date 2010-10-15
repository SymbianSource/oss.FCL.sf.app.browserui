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
#include "serviceipcserverlocalsocket_p.h"
#include "serviceipcserver.h"
#include <QLocalSocket>
#include <QLocalServer>
#include <serviceipcdefs.h>
#include <clientinfo.h>

namespace WRT
{
/*!
 \class ServiceFwIPCServerLocalSocket
 Service Framework IPC Server using QLocalSocket
 */

/*!
 Constructor
 @param aParent parent to this object
 */
ServiceFwIPCServerLocalSocket::ServiceFwIPCServerLocalSocket(QObject* aParent) 
    : QObject(aParent)
    , m_Listener(NULL)
{
    m_Listener = new QLocalServer(this);
    QObject::connect(m_Listener, SIGNAL(newConnection()),
    this, SLOT(handleNewConnection()) );
    m_isValidSession = true;
    m_isKeepServer = false;
    m_sessionIdTable = new SessionIdTable();
}

/*!
 Destructor
 */
ServiceFwIPCServerLocalSocket::~ServiceFwIPCServerLocalSocket()
{
    m_Listener->close();
    if(m_isValidSession){
        char* data = (char*) m_SharedMem.data();
        if (data) {
            strcpy(data, SERVERNOTSTARTED);
        }
    }
    m_SharedMem.detach();

    delete m_sessionIdTable;
}

/*!
 Start listening for new service requests
 @param aServerName name of this server
 @return true if listen was successful
 */
bool ServiceFwIPCServerLocalSocket::listen(const QString& aServerName)
{
    m_isValidSession = false; 
    bool firstLaunch(false);
    // Create the global chunk that this process owns
    m_SharedMem.setKey(aServerName);

    if (!m_SharedMem.attach()) {
        // Single byte of data + \0
        m_SharedMem.create(2);
        firstLaunch = true;
    }

    // Initial data is 0, ie server not started
    // Check if data is NULL, this handles the case where shared mem cannot attach or create
    //
    char* data = (char*) m_SharedMem.data();
    if ((data && (strcmp(data, SERVERNOTSTARTED)==0)) || firstLaunch) {
        strcpy(data, SERVERNOTSTARTED);

        //Try to listen
        for (int i=2; i>0; i--) {
            m_isValidSession = m_Listener->listen(aServerName);
            if (m_isValidSession) {
                // Server started flag
                strcpy(data, SERVERSTARTED);
                break;
            } else {
                QLocalServer::removeServer(aServerName);        
            }
        }

        // Signal the client to continue and that server is started
        QSystemSemaphore sem(aServerName + SERVERSEM, 0);
        sem.release(1);
    }
    return m_isValidSession;
}

/*!
 Shutdown the server and stop serving clients 
 @return void
 */
void ServiceFwIPCServerLocalSocket::disconnect()
{
    m_Listener->close();

    // Clean up all sessions
    int count = m_Sessions.count();
    for (int i = 0; i < count; ++i) {
        QObject::disconnect(m_Sessions[i], SIGNAL(disconnected( ServiceIPCSession* ) ),
        this, SLOT( handleSessionDisconnect( ServiceIPCSession* ) ) );
        m_Sessions[i]->close();
        delete m_Sessions[i];
    }
    m_Sessions.clear();
}

/*!
 Handle a new connection, this slot is connected to the socket's newConnection() signal
 @return void
 */
void ServiceFwIPCServerLocalSocket::handleNewConnection()
{
    // Create a new session to host the client
    //
    QLocalSocket* newSocket = m_Listener->nextPendingConnection();
    ServiceIPCSession* session = new LocalSocketSession(newSocket, Observer());
    if (session) {
        session->setParent(this);

        QObject::connect(session, SIGNAL(disconnected( ServiceIPCSession* ) ),
        this, SLOT( handleSessionDisconnect( ServiceIPCSession* ) ) );
        m_Sessions.append(session);
        stopExitTimer(); 

        ClientInfo *client = new ClientInfo();
        client->setSessionId(m_sessionIdTable->allocate());
        session->setClientInfo(client);
    }
}

/*!
 Handle the end of a session
 @param aSession session to be disconnected
 */
void ServiceFwIPCServerLocalSocket::handleSessionDisconnect(ServiceIPCSession* aSession)
{
    int c = m_Sessions.removeAll(aSession);

    if ((m_Sessions.count() == 0) && (!m_isKeepServer)) {
        startExitTimer(); 
    }  

    qDebug() << "Session cleaned up: " << c;
}
 
/*!
 Config IPC server Life time \n
 Start the shutdown timer if it is the last session and aKeepServer is trues.
 @param aKeepLife to keep or disconnect IPC server when all clients are shutdown. 
*/
void ServiceFwIPCServerLocalSocket::configIpcServerLifetime(bool aKeepServer)
{
    if ((m_isKeepServer) && (m_Sessions.count() == 0) && (!aKeepServer)) {
        startExitTimer(); 
    }  
    m_isKeepServer = aKeepServer;
}
}
// END OF FILE
