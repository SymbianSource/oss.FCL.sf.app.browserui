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
#include "serviceipcsharedmem_p.h"

namespace WRT
{

/*!
 \class ServiceSharedMemIPC

 Shared memory backend for the service IPC
 @note: THIS CLASS IS NOT FINISHED
 */

/*!
 Constructor
 */
ServiceSharedMemIPC::ServiceSharedMemIPC()
{
}

/*!
 Destructor
 */
ServiceSharedMemIPC::~ServiceSharedMemIPC()
{
}

/*!
 Connect to the server
 */
bool ServiceSharedMemIPC::connect(const QString& /*aServerName*/)
{
    return false;
}

/*!
 Disconnect from the server
 */
void ServiceSharedMemIPC::disconnect()
{
}

/*!
 Start the server
 */
bool ServiceSharedMemIPC::startServer(const QString& /*aServerName*/,
                                      const QString& /*aExeName*/)
{
    return false;
}

/*!
 Send data synchronously
 */
bool ServiceSharedMemIPC::sendSync(const QString& /*aRequestType*/,
                                   const QByteArray& /*aData*/)
{
    return false;
}

/*!
 Send data asynchronously
 */
void ServiceSharedMemIPC::sendAsync(const QString& /*aRequestType*/,
                                    const QByteArray& /*aData*/)
{
}

/*!
 Read all data from the buffer
 */
QByteArray ServiceSharedMemIPC::readAll()
{
    return QByteArray();
}

/*!
 Wait for data to be available in the buffer
 */
bool ServiceSharedMemIPC::waitForRead()
{
    return false;
}
}
// END OF FILE
