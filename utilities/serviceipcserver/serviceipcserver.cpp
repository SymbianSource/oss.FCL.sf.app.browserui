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


#include "serviceipcserver.h"
#include "serviceipcserver_p.h"
#include "serviceipcserverfactory.h"
#include "serviceipcobserver.h"
#include "serviceipcserversession.h"

namespace WRT
{

const int KServerShutdownDelay=10000;
/*!
 \class ServiceFwIPCServer
 Service Framework IPC Server public class
 */

/*!
    Constructor
    @param aBackend backend to instantiate
    @param aObserver engine observer to call back to handle new requests
    @param aParent QObject's parent
*/
ServiceFwIPCServer::ServiceFwIPCServer( MServiceIPCObserver* aObserver, 
                                        QObject* aParent, 
                                        TServiceIPCBackends aBackend ) 
    : QObject(aParent )
    , m_Observer( aObserver )
{
    d = ServiceFwIPCServerFactory::createBackend( aParent,aBackend );
    d->q = this;
    m_Timer = new QTimer();
    connect(m_Timer, SIGNAL(timeout()), this, SIGNAL(handleExit()));
}

/*!
 Destructor
 */
ServiceFwIPCServer::~ServiceFwIPCServer()
{
    delete d;
    if(m_Timer){
        if (m_Timer->isActive()) {
            m_Timer->stop();          
        }
        delete m_Timer;
        m_Timer = NULL;
    }
}

/*!
 Start listening for new service requests
 @param aServerName name of the server
 @return bool if connected
 */
bool ServiceFwIPCServer::listen(const QString& aServerName)
{
    return d->listen(aServerName);
}

/*!
 Shutdown the server and stop serving clients 
 @return void
 */
void ServiceFwIPCServer::disconnect()
{
    d->disconnect();
}

/*!
 Start the server shutdown timer
 @return void
 */
void ServiceFwIPCServer::startTimer()
{
   if (m_Timer){        
       m_Timer->start(KServerShutdownDelay);
   }
}


/*!
 Stop the server shutdown timer
 @return void
 */
void ServiceFwIPCServer::stopTimer()
{
    if (m_Timer){
        if (m_Timer->isActive()){
            m_Timer->stop();
        }
    }
}

/*!
 IPC server lifetime should be configurable 
 @param aKeepLife to keep or disconnect IPC server when all clients are shutdown. 
 @return void
 */
void ServiceFwIPCServer::configIpcServerLifetime(bool aKeepServer)
{
    d->configIpcServerLifetime(aKeepServer);
}

/*!
 * Send a message to all broadcast clients connected to this server
 * @param aMessage the contents of the message
 */ 
void ServiceFwIPCServer::broadcast( const QByteArray& aMessage )
{
    QHash<int, ServiceIPCSession*> sessions = d->getBroadcastSessions(); 
    for (int i = 0; i < sessions.count(); ++i) {
        ServiceIPCSession* session = sessions[i];
        if ((session->getReadyToSend()) && (session->messageListIsEmpty())) {
            session->write(aMessage);
            session->completeRequest();
            session->setReadyToSend(false);
        }
        else {
            //queue the aMessage to be sent
            session->appendMessageList(aMessage);
        }
    }
}
 
/*!
 * Send a message to a particular client connected to this server
 * @param aSessionId, id of a session to broadcast to
 * @param aMessage the contents of the message
 */ 
void ServiceFwIPCServer::sendMessage( qint32 aSessionId, QByteArray& aMessage )
{
    QHash<int, ServiceIPCSession*> sessions = d->getBroadcastSessions(); 
    ServiceIPCSession* session = sessions.value(aSessionId);
    if (session != NULL) {
        if ((session->getReadyToSend()) && (session->messageListIsEmpty())) { 
            session->write(aMessage);
            session->completeRequest();
            session->setReadyToSend(false);
        }
        else {
            //queue the aMessage to be sent
            session->appendMessageList(aMessage);
        }
    }
}
                                
}

/*!
 \fn WRT::ServiceFwIPCServer::handleExit()

 Handle server shutdown\n
 Emitted when all client sessions were closed and server should shutdown
 @return void
 */
// END OF FILE
