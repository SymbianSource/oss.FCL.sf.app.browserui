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


#include "serviceipc.h"
#include "serviceipc_p.h"
#include "serviceipcfactory.h"

namespace WRT
{

/*!
 \class ServiceFwIPC
 Public interface class for IPC operations
 */

/*!
 Destructor
 */
ServiceFwIPC::~ServiceFwIPC()
{
    delete d;
}

/*!
 Constructor
 @param aBackend IPC backend to use
 @param aParent Parent to this QObject
 */
ServiceFwIPC::ServiceFwIPC(QObject* aParent, TServiceIPCBackends aBackend)
    : QObject(aParent)
    , m_AsyncRequestPending(false)
{
    // Private implementation pattern
    //
    d = ServiceFwIPCFactory::createBackend(aBackend);
    d->q = this;
}

/*!
 Connect to the server
 @param aServerName name of the server to connect to
 @return true if connected, false if not
 */
bool ServiceFwIPC::connect(const QString& aServerName)
{
    return d->connect(aServerName);
}

/*!
 Disconnect from the server
 @return void
 */
void ServiceFwIPC::disconnect()
{
    d->disconnect();
}

/*!
 Starts the service
 @param aServerName name of the server
 @param aExeName executable of the server
 @return true if connected
 */
bool ServiceFwIPC::startServer(const QString& aServerName,
                               const QString& aExeName)
{
    return d->startServer(aServerName, aExeName);
}

/*!
 Send a request synchronously
 @param aRequestType name of the request
 @param aData data to send
 @return true if sent successful, otherwise false
 */
bool ServiceFwIPC::sendSync(const QString& aRequestType,
                            const QByteArray& aData)
{
#ifdef _DEBUG
    Q_ASSERT_X( aRequestType.contains(";") == false, "", "aRequestType cannot contain semicolons!" );
#endif // _DEBUG
    bool sent = d->sendSync(aRequestType, aData);
    if (sent) {
        sent = waitForRead();
    }
    return sent;
}

/*!
 Send a request asynchronously
 @param aRequestType name of the request
 @param aData data to send
 @note Errors will be emitted via errors() signal
 @return void
 */
void ServiceFwIPC::sendAsync(const QString& aRequestType,
                             const QByteArray& aData)
{
#ifdef _DEBUG
    Q_ASSERT_X( aRequestType.contains(";") == false, "", "aRequestType cannot contain semicolons!" );
#endif // _DEBUG
    d->sendAsync(aRequestType, aData);
    m_AsyncRequestPending = true;
}

/*!
 Reads all data pending in the buffer.\n
 For Sync version this will wait until all of the data is available.\n
 For Async version it will return the currently read buffer.\n
 Use only after readyRead() signal is emitted. However partial data can be read
 @return QByteArray of results
 */
QByteArray ServiceFwIPC::readAll()
{
    return d->readAll();
}

/*!
 Waits until data is available for reading
 @return true if data is available for reading
 */
bool ServiceFwIPC::waitForRead()
{
    return d->waitForRead();
}

/*!
 Check if an async request is already pending
 @return true if an async request is pending
 false otherwise
 */
bool ServiceFwIPC::requestPending()
{
    return m_AsyncRequestPending;
}

/*!
 Retrieve the session id synchronously
 @param aSessionId name of the request
 @return true if retrieved successfully, otherwise false
 */
bool ServiceFwIPC::getSessionId(int& aSessionId)
{
    QByteArray data;
    bool ret(sendSync(GETSESSIONID, data));
    if (ret)
        data = d->readAll();
    aSessionId = data.toInt();
    return ret;
}

/*!
 Set the session id synchronously
 @param aSessionId name of the request
 @return true if set successfully, otherwise false
 */
bool ServiceFwIPC::setSessionId(int aSessionId)
{
    bool ret(sendSync(SETSESSIONINFO, QByteArray::number(aSessionId)));
    if (ret)
        d->readAll(); //clean buffer
    return ret;
}

} // end of namespace

/*!
 \fn WRT::ServiceFwIPC::error( int aError )

 Signal emitted to handle any errors
 @param aError error code
 @note: For local socket implementation, the error can be interpreted
 as QLocalSocket::LocalSocketError
 @return void
 */

/*!
 \fn WRT::ServiceFwIPC::readyRead()

 Handle when a reply has been received for async requests\n
 Emitted when the entire data packet has been received
 @return void
 */
// END OF FILE
