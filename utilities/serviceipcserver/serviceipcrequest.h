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


#ifndef serviceipcrequest_h
#define serviceipcrequest_h

#include <QtCore>
#include <serviceipcdefs.h>

namespace WRT {
        
    class ServiceIPCSession;
    class ClientInfo;
    
    class SFWIPCSRV_EXPORT ServiceIPCRequest : public QObject
    {
    public:
        virtual ~ServiceIPCRequest();
    
        ServiceIPCRequest(ServiceIPCSession* aSession,
                          qint64 aDataLength,
                          const QString& aRequestOp);
    
    public:
        // New function
    
        const QString& getOperation();
    
        const QByteArray& getData();
    
        bool write(const QByteArray& aData);
    
        bool completeRequest();
    
        bool addRequestdata(const QByteArray& aMoreData);

        void setClientInfo(ClientInfo *aClientInfo);

        const ClientInfo *clientInfo();
    
    private:
        ServiceIPCSession* m_Session; // not owned
        ClientInfo *m_ClientInfo;
        QString m_RequestOp;
        QByteArray m_RequestData;
        qint64 m_Datalength;
    };

}
#endif // serviceipcrequest_h
