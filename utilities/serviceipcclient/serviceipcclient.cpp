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

 
#include "serviceipcclient.h"
#include "serviceipc.h"

namespace WRT
{

/*!
 \class ServiceIPCClient
  This class is an extension to the current ServiceIPCClient interface
  with the additional functionality of receiving messages from the server 
 */

/*!
 Destructor
 */
ServiceIPCClient::~ServiceIPCClient()
{
    delete m_syncIPC;
    delete m_asyncIPC;
    delete m_broadcastIPC;
}

/*!
 Constructor 
 @param aBackend IPC backend to use
 @param aParent Parent to this QObject
 */
ServiceIPCClient::ServiceIPCClient(QObject* aParent, TServiceIPCBackends aBackend) :
                                   QObject(aParent)
{  
    m_syncIPC = new ServiceFwIPC(aParent, aBackend);
    m_asyncIPC = new ServiceFwIPC(aParent, aBackend);
    m_broadcastIPC = new ServiceFwIPC(aParent, aBackend);
    
    QObject::connect(m_asyncIPC, SIGNAL(readyRead()), this, SLOT(handleAsyncMsg()));
    QObject::connect(m_broadcastIPC, SIGNAL(readyRead()), this, SLOT(handleBroadcastMsg()));
}

/*!
 Connect to the server
 @param aServerName name of the server to connect to
 @return true if connected, false if not
 */
bool ServiceIPCClient::connect(const QString& aServerName)
{
    bool ret;
    ret = m_syncIPC->connect(aServerName);
    if (ret) {
        int sessionId;
        ret = m_syncIPC->getSessionId(sessionId);
        if (ret) {
            m_sessionId = sessionId;
            ret = m_asyncIPC->connect(aServerName);
            if (ret) {
                ret = m_asyncIPC->setSessionId(sessionId);
                if (ret) {
                    ret = m_broadcastIPC->connect(aServerName);
                    if (ret) {
                        ret = m_broadcastIPC->setSessionId(sessionId);
                        if (!ret) {
                            //async. IPC set session ID fail, disconnect broadcast IPC
                            m_broadcastIPC->disconnect();
                        }
                        else {
                            subscribeBroadcastMsg();
                        }
                    }
                }
                if (!ret)  {
                    //async. IPC set session ID fail or broadcast IPC connect fail, disconnect async IPC
                    m_asyncIPC->disconnect();
                }
            }
        }
        if (!ret) {
            m_syncIPC->disconnect(); //get sessionId failed or async. IPC connect fail, disconnect sync IPC
        }
    }
    return ret;
}

/*!
 Disconnect from the server
 @return void
 */
void ServiceIPCClient::disconnect()
{
    m_syncIPC->disconnect();
    m_asyncIPC->disconnect();
    m_broadcastIPC->disconnect();
}

/*!
 Starts the service
 @param aServerName name of the server
 @param aExeName executable of the server
 @return true if connected
 */
bool ServiceIPCClient::startServer(const QString& aServerName,
                                   const QString& aExeName)
{
    return m_syncIPC->startServer(aServerName, aExeName);
}

/*!
 Send a request synchronously
 @param aRequestType name of the request
 @param aData data to send
 @return true if sent successful, otherwise false
 */
bool ServiceIPCClient::sendSync(const QString& aRequestType,
                                const QByteArray& aData)
{
    return m_syncIPC->sendSync(aRequestType, aData);
}

/*!
 Send a request asynchronously
 @param aRequestType name of the request
 @param aData data to send
 @note Errors will be emitted via errors() signal
 @return void                                              
 */
void ServiceIPCClient::sendAsync(const QString& aRequestType,
                                 const QByteArray& aData)
{
    m_asyncIPC->sendAsync(aRequestType, aData);
}

/*!
 Reads all data pending in the buffer.\n
 For Sync version this will wait until all of the data is available.\n
 @return QByteArray of results
 */
QByteArray ServiceIPCClient::readAll()
{
    return m_syncIPC->readAll();
}

/*!
 Check if an async request is already pending
 @return true if an async request is pending
 false otherwise
 */
/*bool ServiceIPCClient::requestPending()
{
    return m_asyncIPC->requestPending();
}*/

/*!
 Reads all data pending in the buffer.\n
 For Async version it will return the currently read buffer.\n
 Use only after readyRead() signal is emitted. However partial data can be read
 */
void ServiceIPCClient::handleAsyncMsg()
{
    // Read the results
    QByteArray ipcResult(m_asyncIPC->readAll());
    
    if (!ipcResult.isEmpty()) {
        emit readAsync(ipcResult);  
    }
}

/*!
 Client side interface to subscribe broadcast message
 */
void ServiceIPCClient::subscribeBroadcastMsg() 
{
    QString null;
    m_broadcastIPC->sendAsync(SUBSCRIBEBROADCASTMSG, null.toAscii());
}

/*!
 Reads all data pending in the buffer.\n
 For broadcast version it will return the currently read buffer.\n
 Use only after readyRead() signal is emitted. However partial data can be read
 */
void ServiceIPCClient::handleBroadcastMsg()
{
    // Read the results
    QByteArray ipcResult(m_broadcastIPC->readAll());
    
    if (!ipcResult.isEmpty()) {
        emit handleMessage(ipcResult);  
        subscribeBroadcastMsg(); 
    }
}

}


/*!
 \fn WRT::ServiceIPCClient::error( int aError )
 
 Signal emitted to handle any errors
 @param aError error code
 @note: For local socket implementation, the error can be interpreted 
 as QLocalSocket::LocalSocketError
 @return void
 */

/*!
 \fn WRT::ServiceIPCClient::readyRead()

 Handle when a reply has been received for async requests\n
 Emitted when the entire data packet has been received
 @return void
 */
// END OF FILE
