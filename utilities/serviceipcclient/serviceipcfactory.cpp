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


#include "serviceipcfactory.h"
#include "serviceipc_p.h"
#ifndef __SYMBIAN32__
#include "serviceipclocalsocket_p.h"
#endif // Q_OS_WIN32
#include "serviceipcsharedmem_p.h"

#ifdef __SYMBIAN32__
#include "serviceipcsymbian_p.h"
#endif // __SYMBIAN32__
namespace WRT
{
/*!
 \class ServiceFwIPCFactory

 Factory class for constructing the IPC backend
 */

/*!
 Constructor
 */
ServiceFwIPCFactory::ServiceFwIPCFactory()
{
}

/*!
 Destructor
 */
ServiceFwIPCFactory::~ServiceFwIPCFactory()
{
}

/*!
 Check to see if the particular IPC type is supported
 @param aIPCType type of the IPC
 @return true if the IPC type is supported, false otherwise
 */
bool ServiceFwIPCFactory::isIPCTypeSupported(TServiceIPCBackends aIPCType)
{
    bool supported(false);

#ifdef Q_OS_WIN32
    if( aIPCType == ELocalSocket )
    {
        supported = true;
    }
#elif __SYMBIAN32__
    if (aIPCType == ESymbianServer) {
        supported = true;
    }
#else
    //avoid compile warning
    aIPCType = EDefaultIPC;
    supported = false;
#endif

    return supported;
}

/*!
 Create an instance of the Service IPC backend
 @param aBackend Type of backend to create
 @return ServiceFwIPCPrivate instance
 */
ServiceFwIPCPrivate* ServiceFwIPCFactory::createBackend(TServiceIPCBackends aBackend)
{
    ServiceFwIPCPrivate* backend(NULL);

#ifndef __SYMBIAN32__
    // Local socket is default
    if( aBackend == ELocalSocket || aBackend == EDefaultIPC )
    {
        backend = new ServiceLocalSocketIPC();
    }
#else
    // Symbian server is default
    if (aBackend == ESymbianServer || aBackend == EDefaultIPC) {
        TRAP_IGNORE( backend = CServiceSymbianIPC::NewL() );
    }
#endif
    return backend;
}
}
// END OF FILE
