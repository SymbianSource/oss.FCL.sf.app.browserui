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


#include "serviceipcserversymbianserver_p.h"
#include "serviceipcserversymbiansession.h"
#include <clientinfo.h>

namespace WRT
{
// Server Security Policy
const TUint KServerRangeCount = 2;
const TInt KServerRanges[KServerRangeCount] = { 
                0, //range is [0-1)
                2 //range is [2-KMaxTInt] 
                };
const TUint8 KSeverElementsIndex[KServerRangeCount] = { 
                0,
                CPolicyServer::ENotSupported };

const CPolicyServer::TPolicyElement KServerPolicyElements[] = { {
                _INIT_SECURITY_POLICY_C1(ECapabilityWriteDeviceData),
                CPolicyServer::EFailClient } };

const CPolicyServer::TPolicy KServerPolicy = {
                CPolicyServer::EAlwaysPass, //specifies all connect attempts should pass
                KServerRangeCount, KServerRanges, KSeverElementsIndex,
                KServerPolicyElements };

enum
{
    EServerPriority = CActive::EPriorityStandard
};

// ======== MEMBER FUNCTIONS ========

/*!
 \class CServiceSymbianServer
 Symbian client server implementation
 */

/*!
 Constructor
 */
CServiceSymbianServer::CServiceSymbianServer() :
    CPolicyServer(EServerPriority, KServerPolicy), m_keepServer(false)
{
    m_sessionIdTable = new SessionIdTable();
}

/*!
 2nd phased constructor
 */
void CServiceSymbianServer::ConstructL()
{
}

/*!
 Two phased constructor
 */
CServiceSymbianServer* CServiceSymbianServer::NewL()
{
    CServiceSymbianServer* self = new (ELeave) CServiceSymbianServer;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

/*!
 Destructor
 */
CServiceSymbianServer::~CServiceSymbianServer()
{
    delete m_sessionIdTable;
}


/*!
 A new session is being created \n
 Cancel the shutdown timer if it was running
*/
void CServiceSymbianServer::addSession()

{
    ++m_sessionCount;
    stopExitTimer();
}

/*!
 A session is being destroyed \n
 Start the shutdown timer if it is the last session.
*/
void CServiceSymbianServer::closeSession()
{
    if ((--m_sessionCount==0) && (!m_keepServer))
    {
        startExitTimer();
    }
}

/*!
 Start listening for new service requests
 @param aServerName name of the server
 */
bool CServiceSymbianServer::listen(const QString& aServerName)
{
    bool listening(true);

    // Needs to be here because Q_Ptr isonly set after constructor of public
    m_observer = Observer();
    TPtrC serverName(reinterpret_cast<const TUint16*> (aServerName.utf16()));
    TRAPD( err, StartL(serverName) );
    if (err != KErrNone) {
        listening = false;
    }
    // Complete the server rendezvous that th client started
    RProcess::Rendezvous(KErrNone);
    return listening;
}

/*!
 Shutdown the server and stop serving clients 
 */
void CServiceSymbianServer::disconnect()
{
    // Symbian Servers do not have disconnect, 
    // the process has to exit
}

/*!
 A session is being destroyed \n
 Start the shutdown timer if it is the last session and aKeepServer is trues.
 @param aKeepLife to keep or disconnect IPC server when all clients are shutdown. 
*/
void CServiceSymbianServer::configIpcServerLifetime(bool aKeepServer)
{
    if ((m_keepServer) && (m_sessionCount==0) && (!aKeepServer))
    {
        startExitTimer();
    }
    m_keepServer = aKeepServer;
}

/*!
 Create a new session, derived from CPolicyServer
 @param aVersion version of the server
 @param aMessage message object
 */
CSession2* CServiceSymbianServer::NewSessionL(const TVersion& aVersion,
                                              const RMessage2& aMessage) const
{
    TVersion v(1, 0, 0);
    if (!User::QueryVersionSupported(v, aVersion)) {
        User::Leave(KErrNotSupported);
    }

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
    client->setSessionId(m_sessionIdTable->allocate());
    // Create a new Symbian Session for the client
    CServiceSymbianSession * session = CServiceSymbianSession::NewL(m_observer);
    session->setClientInfo(client);
    return session;
}
}

// END OF FILE
