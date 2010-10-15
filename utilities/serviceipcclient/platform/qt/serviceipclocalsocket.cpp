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


#include <QByteArray>
#include "serviceipclocalsocket_p.h"

namespace WRT
{
// CONSTANTS
const char* REQUEST_COMPLETE_TOKEN = ";ROK";
const int REQUEST_COMPLETE_TOKEN_LENGTH = 4;
const char REQUEST_DELIMITER_TOKEN = ';';
/*!
 \class ServiceLocalSocketIPC
 QLocalSocket based IPC client-side backend
 */

/*!
 Constructor
 */
ServiceLocalSocketIPC::ServiceLocalSocketIPC() : m_BufferType( ENoBuffer )
{
    m_Socket = new QLocalSocket();
    QObject::connect(m_Socket, SIGNAL( error( QLocalSocket::LocalSocketError ) ),
    this, SLOT( handleError( QLocalSocket::LocalSocketError ) ) );
}

/*!
 Destructor
 */

ServiceLocalSocketIPC::~ServiceLocalSocketIPC()
{
    delete m_Socket;
}

/*!
 Connect to the server
 @param aServerName name of the server to connect to
 @return true if connected, false otherwise
 */
bool ServiceLocalSocketIPC::connect(const QString& aServerName)
{
    bool rtn;
    m_Socket->connectToServer(aServerName);
    rtn = m_Socket->waitForConnected();
    return rtn;
}

/*!
 Disconnect from the server
 */
void ServiceLocalSocketIPC::disconnect()
{
    m_Socket->close();
}

/*!
 Starts the service
 @param aServerName name of the server to connect to
 @param aExeName name of the server executable
 @return true if server started, false otherwise
 */
bool ServiceLocalSocketIPC::startServer(const QString& aServerName,
                                        const QString& aExeName)
{
    bool started(true);

    //Semaphore with 1 initial count,
    //Use system semaphore to ensure no contention exists across multiple processes
    //According to QT documentation for System Semaphores on Windows, 
    //the semaphore is automatically cleaned up if a process crashes, thus preventing deadlock
    //
    QSystemSemaphore funcSem(aServerName + FUNCTIONSEM, 1);
    funcSem.acquire();

    // Shared chunk to check if the server has been started or not
    QSharedMemory sharedMem(aServerName);
    char* data(NULL);
    bool attached = sharedMem.attach();
    if (attached) {
        data = (char*) sharedMem.data();
    }

    // Shared memory not created or the flag was not set properly
    if (!attached || strcmp(data, SERVERNOTSTARTED) == 0) {
        // Create the server wait semaphore. When the server has listened, 
        // it will signaled after listen has started
        //
        QSystemSemaphore sem(aServerName + SERVERSEM, 0);

        // Start the server, since this function is mutex'ed by the global semaphore 
        // only 1 process/thread can reach here
        //
        started = QProcess::startDetached(aExeName);

        // Wait until the server signals
        if (started) {
            sem.acquire();
        }
    }

    // Test if server started successfully
#if _DEBUG
    attached = sharedMem.attach();
    if( attached )
    {
        data = (char*)sharedMem.data();
        if( strcmp( data, SERVERSTARTED ) == 0 )
        {
            qDebug() << "Server Started Successfully";
        }
    }
#endif // _DEBUG
    // Free shared memory
    sharedMem.detach();

    // Release the function semaphore
    funcSem.release(1);
    return started;
}

/*!
 Send a request synchronously
 @param aRequestType type of request, toAscii() will be called to serialize the data
 @param aData aData data to send to the server
 @return true if data is sent, false otherwise
 */
bool ServiceLocalSocketIPC::sendSync(const QString& aRequestType,
                                     const QByteArray& aData)
{
    QByteArray data;
    data.setNum(aData.length());
    data.append(REQUEST_DELIMITER_TOKEN);
    data.append(aRequestType.toAscii());
    data.append(REQUEST_DELIMITER_TOKEN);
    data.append(aData);
    int count = m_Socket->write(data);
    m_Socket->flush();
    m_BufferType = ESyncBuffer;
    return (count > 0);
}

/*!
 Send a request asynchronously
 @param aRequestType type of request, toAscii() will be called to serialize the data
 @param aData data to send to the server
 */
void ServiceLocalSocketIPC::sendAsync(const QString& aRequestType,
                                      const QByteArray& aData)
{
    QByteArray data;
    data.setNum(aData.length());
    data.append(REQUEST_DELIMITER_TOKEN);
    data.append(aRequestType.toAscii());
    data.append(REQUEST_DELIMITER_TOKEN);
    data.append(aData);
    m_Socket->write(data);

    // Connect the signal and reset aync data buffer
    m_AsyncData.clear();
    QObject::connect(m_Socket, SIGNAL( readyRead() ),
    this, SLOT( handleReadyRead() ) );
    m_BufferType = EAsyncBuffer;
}

/*!
 Reads all data pending in the buffer
 @return QByteArray data that has been read
 */
QByteArray ServiceLocalSocketIPC::readAll()
{
    QByteArray result;

    // If asynchronous read all data from the socket 
    //
    if ( m_BufferType == ESyncBuffer ) {
        // Wait for all data to be completed before returning
        //
        bool done = false;
        do {
            result.append(m_Socket->readAll());
            if (result.right(REQUEST_COMPLETE_TOKEN_LENGTH)
                            == REQUEST_COMPLETE_TOKEN) {
                // Chop the end token
                result.chop(REQUEST_COMPLETE_TOKEN_LENGTH);
                done = true;
            }
        } while (done == false);
    }
    // If async, return the internal databuffer
    else if( m_BufferType == EAsyncBuffer ){
        // Should be just a d-ptr copy
        result = m_AsyncData;
        QObject::disconnect(m_Socket, SIGNAL( readyRead() ),
        this, SLOT( handleReadyRead() ) );
    }
    m_BufferType = ENoBuffer;

    return result;
}

/*!
 Waits until data is available for reading 
 @return bool true if data can be read
 */
bool ServiceLocalSocketIPC::waitForRead()
{
    return m_Socket->waitForReadyRead();
}

/*!
 Handle any socket errors
 @param socketError error
 */
void ServiceLocalSocketIPC::handleError(QLocalSocket::LocalSocketError aSocketError)
{
    // Use base class to send this
    emitError(doMapErrors(aSocketError));
}

/*!
 Handle when data is ready to be read
 */
void ServiceLocalSocketIPC::handleReadyRead()
{
    m_AsyncData.append(m_Socket->readAll());
    if (m_AsyncData.right(REQUEST_COMPLETE_TOKEN_LENGTH)
                    == REQUEST_COMPLETE_TOKEN) {
        // Chop the end token
        m_AsyncData.chop(REQUEST_COMPLETE_TOKEN_LENGTH);

        // Use base class to send signal when all the data has been assembled
        emitReadyRead();
    }
}

int ServiceLocalSocketIPC::doMapErrors( int aError )
{
    int error(0);
    
    // Map QT Local Socket error codes to custom error codes
    //
    switch( aError ) {
    case QLocalSocket::ConnectionError:
    case QLocalSocket::ConnectionRefusedError: {
        error = ServiceFwIPC::EConnectionError;
        break;
        }
    case QLocalSocket::PeerClosedError: {
        error = ServiceFwIPC::EConnectionClosed;
        break;
        } 
    case QLocalSocket::ServerNotFoundError: {
        error = ServiceFwIPC::EServerNotFound;
        break;
        } 
    case QLocalSocket::SocketAccessError:
    case QLocalSocket::SocketResourceError:
    case QLocalSocket::SocketTimeoutError:
    case QLocalSocket::DatagramTooLargeError:
    case QLocalSocket::UnsupportedSocketOperationError: {
        error = ServiceFwIPC::EIPCError;
        break;
        }    
    case QLocalSocket::UnknownSocketError: {
        error = ServiceFwIPC::EUnknownError;
        break;
        }
    }
    return error;
}

}
// END OF FILE
