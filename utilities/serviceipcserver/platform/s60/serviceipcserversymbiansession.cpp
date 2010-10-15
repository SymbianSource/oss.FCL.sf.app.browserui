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


#include <clientinfo.h>
#include "serviceipcobserver.h"
#include "serviceipcrequest.h"
#include "serviceipcserversymbiansession.h"

namespace WRT
{

// Constants for the IPC operation id
const TInt KIPCNewOperation = 0;
const TInt KIPCGetData = 1;

/*!
 \class CServiceSymbianSession
 Symbian Session class
 */

/*!
 Constructor
 @param aObserver observer to the server
 */
CServiceSymbianSession::CServiceSymbianSession(MServiceIPCObserver* aObserver) :
    ServiceIPCSession(aObserver)
{
}

/*!
 2nd phased constructor
 */
void CServiceSymbianSession::ConstructL()
{
}

void CServiceSymbianSession::CreateL()
{
    Server().addSession(); 
}

/*!
 Two-Phased Constructor
 @param aObserver observer to the server
 */
CServiceSymbianSession* CServiceSymbianSession::NewL(MServiceIPCObserver* aObserver)
{
    CServiceSymbianSession* self =
                    new (ELeave) CServiceSymbianSession(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

/*!
 Destructor
 */
CServiceSymbianSession::~CServiceSymbianSession()
{
    delete m_curRequest;
    m_curRequest = NULL;
    if (m_clientInfo) {
        delete m_clientInfo;
        m_clientInfo = NULL;
    }
}

/*!
 Write some data in response to a request
 @param aData some data to write as response
 @return bool if write was successful
 */
bool CServiceSymbianSession::write(const QByteArray& aData)
{
    // Implicitly shared
    m_data = aData;
    return false;
}

/*!
 Complete a Request
 @return bool if request completed 
 */
bool CServiceSymbianSession::completeRequest()
{

    m_message.Complete(m_data.count());
    delete m_curRequest;
    m_curRequest = NULL;
    return true;
}

/*!
 Close a session and gracefully shutdown
 */
void CServiceSymbianSession::close()
{
    // Symbian doesn't really do anything
}

/*!
 From CSession2
 Service request
 @param aMessage message object
 */
void CServiceSymbianSession::ServiceL(const RMessage2& aMessage)
{
    // Default ServiceErrorL() will complete the message if this method leaves
    TInt operation(aMessage.Function());
    switch (operation) {
    case KIPCNewOperation: {
        handleRequestL(aMessage);
        break;
    }
    case KIPCGetData: {
        handleGetBufferL(aMessage);
        break;
    }
    default: {
        aMessage.Complete(KErrNotFound);
        break;
    }
    }
}

/*!
 From CSession2
 Handle any disconnection from the client
 @param aMessage message Object
 */
void CServiceSymbianSession::Disconnect(const RMessage2 &/*aMessage*/)
{
    if (m_appendToBList) {
        Server().removeBroadcastList(m_clientInfo->sessionId());  
    }
    m_observer->handleClientDisconnect(m_clientInfo);
    Server().releaseSessionId(m_clientInfo->sessionId()); //release sessionId
    if (m_curRequest) {
        completeRequest();
    }
    Server().closeSession();
}

/*!
 Handle an IPC request
 @param aMessage message Object
 */
void CServiceSymbianSession::handleRequestL(const RMessage2& aMessage)
{
    // Store the message
    m_message = aMessage;

    // Convert from Symbian to QT
    HBufC* request = ReadDesLC(aMessage, 0);
    HBufC8* data = ReadDes8LC(aMessage, 1);

    // Shallow copy only, we want a deep copy
    QString d = QString::fromUtf16(request->Ptr(), request->Length());
    QString operation;
    operation += d;

    QByteArray convertData( reinterpret_cast<const char*>(data->Ptr()), data->Length() );

    // New request
    Q_ASSERT(!m_curRequest);
    m_curRequest = new ServiceIPCRequest(this, 0, operation);
    m_data.clear();

    // Get client info
    ClientInfo *client = new ClientInfo();
    TSecureId sid(aMessage.Identity());
    client->setProcessId(sid.iId);
    client->setVendorId(aMessage.VendorId().iId);
    RThread clientThread;
    aMessage.ClientL(clientThread);
    RProcess clientProc;
    clientThread.Process(clientProc);
    client->setName(QString::fromUtf16(clientProc.Name().Ptr(), 
                                       clientProc.Name().Length()));
    client->setSessionId(m_clientInfo->sessionId());
    // Add data and callback to the observer
    // 
    m_curRequest->addRequestdata(convertData);
    m_curRequest->setClientInfo(client); // ownership passed
    //m_observer->handleRequest(m_curRequest);
    handleReq();

    CleanupStack::PopAndDestroy(2, request);
}

/*!
 Handle getting the result buffer
 */
void CServiceSymbianSession::handleGetBufferL(const RMessage2& aMessage)
{
    TPtrC8 data(reinterpret_cast<const TUint8*> (m_data.constData()), m_data.count());
    aMessage.Write(0, data);
    aMessage.Complete(KErrNone);
}

/*!
 Read a 16 bit descriptor from the message
 @param aMessage message to read from, 
 @param aMsgSlot slot to read from
 */
HBufC* CServiceSymbianSession::ReadDesLC(const RMessage2& aMessage,
                                         TInt aMsgSlot)
{
    TInt length = aMessage.GetDesLengthL(aMsgSlot);
    HBufC* des = HBufC::NewLC(length);
    TPtr ptr = des->Des();
    aMessage.ReadL(aMsgSlot, ptr);
    return des;
}

/*!
 Read a 8 bit descriptor from the message
 @param aMessage message to read from, 
 @param aMsgSlot slot to read from
 */
HBufC8* CServiceSymbianSession::ReadDes8LC(const RMessage2& aMessage,
                                           TInt aMsgSlot)
{
    TInt length = aMessage.GetDesLengthL(aMsgSlot);
    HBufC8* des = HBufC8::NewLC(length); 
    TPtr8 ptr = des->Des();
    aMessage.ReadL(aMsgSlot, ptr);
    return des;
}
}

// END OF FILE



