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


#include "serviceipcserversession.h"
#include "serviceipcobserver.h"
#include "serviceipcrequest.h"
#include "serviceipcserver_p.h"

namespace WRT
{
/*!
 \class ServiceIPCSession
 Session class to hold the state of each connected client
 */

/*!
 Constructor
 @param aObserver observer to the session
 */
ServiceIPCSession::ServiceIPCSession(MServiceIPCObserver* aObserver) 
    : m_isPendingRequest(false)
    , m_observer(aObserver)
    , m_curRequest(NULL)
    , m_clientInfo(NULL)
    , m_appendToBList(false)
    , m_readyToSend(false)
{

}

/*!
 Destructor
 */
ServiceIPCSession::~ServiceIPCSession()
{
    m_messageList.clear();
}


void ServiceIPCSession::handleReq()
{
    if (m_curRequest->getOperation() == GETSESSIONID) {
        QByteArray sessionId;
        sessionId.setNum(m_clientInfo->sessionId());
        m_curRequest->write(sessionId);
       
        m_curRequest->completeRequest();
        m_observer->handleClientConnect(m_clientInfo);
    }
    else if (m_curRequest->getOperation() == SETSESSIONINFO) {
        qint32 sessionId = m_curRequest->getData().toInt();
        // release session id which allocate during new session. 
        // 3 ipc client in one client wrapper share same session id  
        releaseSessionId(m_clientInfo->sessionId()); 
        m_clientInfo->setSessionId(sessionId);

        m_curRequest->completeRequest();
        m_observer->handleClientConnect(m_clientInfo);
    }
    else if (m_curRequest->getOperation() == SUBSCRIBEBROADCASTMSG) {
        m_readyToSend = true;
        if (!m_appendToBList) {
            //add to broadcast list
            appendBroadcastList(m_clientInfo->sessionId(), this);
            m_appendToBList = true;
        }
        if (!m_messageList.isEmpty()) {
            write(m_messageList.takeFirst());
            completeRequest();
            m_readyToSend = false;
        }
    }
    else {
        m_observer->handleRequest(m_curRequest);
    }
}

/*!
 \fn WRT::ServiceIPCSession::write( const QByteArray& aData )
 
 Write some data in response to a request
 @param aData some data to write as response
 @return bool if write was successful
 */

/*!
 \fn WRT::ServiceIPCSession::completeRequest()
 
 Complete a Request
 @return bool if request completed 
 */

/*!
 \fn WRT::ServiceIPCSession::close()
 
 Close a session and gracefully shutdown
 */
}
// END OF FILE
