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


#ifndef C_SERVICEIPC_SYMBIAH_H
#define C_SERVICEIPC_SYMBIAH_H

#include <e32base.h>
#include <serviceipc_p.h>
#include "serviceipcsymbiansession.h"

namespace WRT {
    
    class CServiceSymbianIPC : public CActive, public ServiceFwIPCPrivate
    {
    
    public:
    
        virtual ~CServiceSymbianIPC();
    
        static CServiceSymbianIPC* NewL();
    
        // From ServiceFwIPCPrivate
    protected:
    
        bool connect(const QString& aServerName);
    
        void disconnect();
    
        bool startServer(const QString& aServerName, const QString& aExeName);
    
        bool sendSync(const QString& aRequestType, const QByteArray& aData);
    
        void sendAsync(const QString& aRequestType, const QByteArray& aData);
    
        QByteArray readAll();
    
        bool waitForRead();
    
        // From CActive
    protected:
    
        void RunL();
    
        void DoCancel();
    
    private:
    
        QByteArray doReadAllL();
    
        int doMapErrors( TInt aError );
        
    private:
    
        CServiceSymbianIPC();
    
        void ConstructL();
    
    private:
        RServiceIPCSession iSession;
        HBufC8* iAsyncData; // member variable to keep alive any data for async ops
        TPtrC8 iAsyncDataPtr;
        HBufC* iRequestData;
        TPtrC iRequestDataPtr;
        TInt iDataSize;
    };
}
#endif // C_SERVICEIPC_SYMBIAH_H
