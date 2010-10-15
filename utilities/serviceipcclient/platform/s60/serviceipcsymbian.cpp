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


#include <e32base.h>
#include <e32cmn.h>
#include "serviceipcsymbian_p.h"

namespace WRT
{
const TInt KIPCOperation = 0;
const TInt KIPCGetBuffer = 1;
const TInt KServerMajorVersionNumber = 1;
const TInt KServerMinorVersionNumber = 0;
const TInt KServerBuildVersionNumber = 0;


/*!
 \class CServiceSymbianIPC

 Symbian Client backend for the service IPC
 */

/*!
 Constructor
 */
CServiceSymbianIPC::CServiceSymbianIPC() :
    CActive(CActive::EPriorityStandard), iDataSize(0)
{
    CActiveScheduler::Add(this);
}

/*!
 Destructor 
 */
CServiceSymbianIPC::~CServiceSymbianIPC()
{
    Cancel();
    if (iSession.Handle()) {
        iSession.Close();
    }
    delete iAsyncData;
    delete iRequestData;
}

/*!
 2nd phased constructor
 */
void CServiceSymbianIPC::ConstructL()
{
}

/*!
 Two Phased Constructor
 */
CServiceSymbianIPC* CServiceSymbianIPC::NewL()
{
    CServiceSymbianIPC* self = new (ELeave) CServiceSymbianIPC();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

/*!
 Connect to the server
 @param aServerName name of the server to connect to
 @return true if connected, false if not
 */
bool CServiceSymbianIPC::connect(const QString& aServerName)
{
    // Version informaton
    TVersion version(KServerMajorVersionNumber, 
                     KServerMinorVersionNumber, 
                     KServerBuildVersionNumber);
    TPtrC serverName(reinterpret_cast<const TUint16*> (aServerName.utf16()));
    TInt err = iSession.Connect(serverName, version);

    return (err == KErrNone);
}

/*!
 Disconnect from the server
 */
void CServiceSymbianIPC::disconnect()
{
    iSession.Close();
}

/*!
 Starts the service
 @param aServerName server name
 @param aExeName server executable name
 */
bool CServiceSymbianIPC::startServer(const QString& /*aServerName*/,
                                     const QString& aExeName)
{
    TPtrC serverName(reinterpret_cast<const TUint16*> (aExeName.utf16()));
    TInt err = iSession.StartServer(serverName);
    return (err == KErrNone);
}

/*!
 Send a request synchronously
 @param aRequestType type of request to send to the server
 @param aData data to send to the server
 */
bool CServiceSymbianIPC::sendSync(const QString& aRequestType,
                                  const QByteArray& aData)
{
    // Convert from QString to TPtr
    TPtrC request(reinterpret_cast<const TUint16*> (aRequestType.utf16()));
    TPtrC8 data(reinterpret_cast<const TUint8*> (aData.constData()), aData.length());

    // Send data, 0 is new op
    TInt err;
    TInt dataSize = 0;
    TIpcArgs args(&request, &data);
    TRAP( err, dataSize = iSession.SendReceiveL(KIPCOperation,args) );

    // map return value
    if (err == KErrNone) {
        iDataSize = dataSize;
    }
    return (err == KErrNone);
}

/*!
 Send a request asynchronously
 @param aRequestType type of request to send to the server
 @param aData data to send to the server
 */
void CServiceSymbianIPC::sendAsync(const QString& aRequestType,
                                   const QByteArray& aData)
{
    delete iRequestData;
    iRequestData = NULL;
    TPtrC request(reinterpret_cast<const TUint16*> (aRequestType.utf16()));
    iRequestData = request.Alloc();
    delete iAsyncData;
    iAsyncData = NULL;
    TPtrC8 data(reinterpret_cast<const TUint8*> (aData.constData()), aData.length());
    iAsyncData = data.Alloc();

    // Send data
    iRequestDataPtr.Set(*iRequestData);
    iAsyncDataPtr.Set(*iAsyncData);
    TIpcArgs args(&iRequestDataPtr, &iAsyncDataPtr);
    iSession.SendReceive(KIPCOperation, args, iStatus);
    SetActive();
}

/*!
 Reads all data pending in the buffer
 @return QByteArray containing the result data
 */
QByteArray CServiceSymbianIPC::readAll()
{
    QByteArray rtn;
    TRAP_IGNORE( rtn = doReadAllL() );

    return rtn;
}

/*!
 Reads all data pending in the buffer, leaves if an error occurred
 @return QByteArray containing the result data
 */
QByteArray CServiceSymbianIPC::doReadAllL()
{
    // Read the data via IPC
    //
    CBufBase* buf = CBufFlat::NewL(iDataSize);
    CleanupStack::PushL(buf);
    buf->ResizeL(iDataSize);
    TPtr8 ptr(buf->Ptr(0));
    iSession.SendReceiveL(KIPCGetBuffer, TIpcArgs(&ptr));

    QByteArray convert((char *)ptr.Ptr(), ptr.Length());

    CleanupStack::PopAndDestroy(buf);

    // Deep copy, return variable is implicitly shared
    return convert;
}

/*!
 Maps error codes from Symbian error codes to Service IPC error codes
 @param aError Symbian error code
 @return mapped error code
 */
int CServiceSymbianIPC::doMapErrors(TInt aError)
{
    int error(0);
    switch (aError) {
    case KErrNone: {
        error = 0;
        break;
    }
    case KErrPermissionDenied:
    case KErrServerTerminated: {
        error = ServiceFwIPC::EConnectionClosed;
        break;
    }
    case KErrServerBusy: {
        error = ServiceFwIPC::EConnectionError;
        break;
    }
    case KErrArgument:
    case KErrNoMemory: {
        error = ServiceFwIPC::EIPCError;
        break;
    }
    default: {
        error = ServiceFwIPC::EUnknownError;
        break;
    }
    }
    return error;
}
/*!
 Waits until data is available for reading 
 @return bool always true, no need to wait
 */
bool CServiceSymbianIPC::waitForRead()
{
    // Symbian Client-server is blocking, so no need to wait for read
    return true;
}

/*!
 Active object callback
 */
void CServiceSymbianIPC::RunL()
{
    TInt err = iStatus.Int();

    // Callback to observers
    //
    if (err >= KErrNone) {
        iDataSize = err;
        emitReadyRead();
    } else {
        emitError(doMapErrors(err));
    }

    // Cleanup async request, no need to delete if client re-requested an async op
    if( !asyncPending() ) {
        delete iRequestData;
        iRequestData = NULL;
        delete iAsyncData;
        iAsyncData = NULL;
        iRequestDataPtr.Set(KNullDesC);
        iAsyncDataPtr.Set(KNullDesC8);
    }
}

/*!
 Active object cancel
 */
void CServiceSymbianIPC::DoCancel()
{
    // We can't cancel in the IPC design.
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrCancel);
}

}
// END OF FILE
