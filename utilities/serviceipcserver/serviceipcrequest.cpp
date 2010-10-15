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


#include "serviceipcrequest.h"
#include "serviceipcserversession.h"
#include <clientinfo.h>

namespace WRT
{
/*!
 \class ServiceIPCRequest
 Class to encapsulate a service request
 */

/*!
 Constructor
 @param aSession session associated with this request
 @param aDataLength amount of data to be received in this request
 @param aRequestOp operaion name
 */
ServiceIPCRequest::ServiceIPCRequest(ServiceIPCSession* aSession,
                                     qint64 aDataLength,
                                     const QString& aRequestOp) 
    : QObject(NULL)
    , m_Session(aSession)
    , m_ClientInfo(NULL)
    , m_RequestOp(aRequestOp)
    , m_Datalength(aDataLength)
{

}

/*!
 Destructor
 */
ServiceIPCRequest::~ServiceIPCRequest()
{
    delete m_ClientInfo;
}

/*!
 Get the requested operation
 @return QString operation ID
 */
const QString& ServiceIPCRequest::getOperation()
{
    return m_RequestOp;
}

/*!
 Get the requested data
 @return QByteArray data for this operation
 */
const QByteArray& ServiceIPCRequest::getData()
{
    return m_RequestData;
}

/*!
 Write some data to the request
 @param aData data to write to the socket
 */
bool ServiceIPCRequest::write(const QByteArray& aData)
{
    // Do we want to buffer the writes?
    return m_Session->write(aData);
}

/*!
 Complete the request
 @return true if request completed successfully
 */
bool ServiceIPCRequest::completeRequest()
{
    return m_Session->completeRequest();
}

/*!
 Append more data when creating the request
 @arg aMoreData data to be appended to the request
 @return true if m_Datalength now equals the full length
 */
bool ServiceIPCRequest::addRequestdata(const QByteArray& aMoreData)
{
    m_RequestData.append(aMoreData);
    return (m_RequestData.length() == m_Datalength);
}

/*!
 Sets the client info.  Onwership of the object is passed in.
 @arg aClientInfo Client information
 */
void ServiceIPCRequest::setClientInfo(ClientInfo *aClientInfo)
{
    delete m_ClientInfo;
    m_ClientInfo = aClientInfo;
}

/*!
 Gets the client info.
 @return Client Information object.  NULL if none is available
 */
const ClientInfo *ServiceIPCRequest::clientInfo()
{
    return m_ClientInfo;
}
}
// END OF FILE
