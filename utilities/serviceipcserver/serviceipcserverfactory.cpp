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


#include "serviceipcserverfactory.h"
#ifndef __SYMBIAN32__
#include "serviceipcserverlocalsocket_p.h"
#else 
#include "serviceipcserversymbianserver_p.h"
#endif // __SYMBIAN32__
namespace WRT
{

/*!
 \class ServiceFwIPCServerFactory
 Factory class to construct IPC server backend
 */

/*!
 Destructor
 */
ServiceFwIPCServerFactory::~ServiceFwIPCServerFactory()
{
}

/*!
 Constructor
 */
ServiceFwIPCServerFactory::ServiceFwIPCServerFactory()
{
}

/*!
 Construct a backend 
 @param aBackend backend to construct
 @param aParent parent to this QObject
 */
ServiceFwIPCServerPrivate* ServiceFwIPCServerFactory::createBackend(QObject* aParent,
                                                                    TServiceIPCBackends aBackend)
{
    ServiceFwIPCServerPrivate* backend(NULL);

#ifndef __SYMBIAN32__
    // Local socket is default
    if( aBackend == ELocalSocket || aBackend == EDefaultIPC )
    {
        backend = new ServiceFwIPCServerLocalSocket( aParent );
    }
#else
    // Symbian server is default
    if (aBackend == ESymbianServer || aBackend == EDefaultIPC) {
        TRAP_IGNORE( backend = CServiceSymbianServer::NewL(); );
    }
#endif
    return backend;
}
}
// END OF FILE
