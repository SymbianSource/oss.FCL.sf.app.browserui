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


#ifndef serviceipcserversymbiansession_h
#define serviceipcserversymbiansession_h

#include <e32base.h>
#include <e32cmn.h> 
#include "serviceipcserversession.h"
#include "serviceipcserversymbianserver_p.h"

namespace WRT {
    class CServiceSymbianSession : public CSession2, public ServiceIPCSession
    {
    public:
    
        static CServiceSymbianSession* NewL(MServiceIPCObserver* aObserver);
    
        virtual ~CServiceSymbianSession();
    
        // From ServiceIPCSession
    protected:
    
        bool write(const QByteArray& aData);
    
        bool completeRequest();
    
        void close();
    
        // From CSession2
    protected:
    
        void CreateL();
        
        void ServiceL(const RMessage2& aMessage);
    
        void Disconnect(const RMessage2 &aMessage);
    
        // Service methods
    private:
    
        void handleRequestL(const RMessage2& aMessage);
    
        void handleGetBufferL(const RMessage2& aMessage);
    
        HBufC* ReadDesLC(const RMessage2& aMessage, TInt aMsgSlot);
    
        HBufC8* ReadDes8LC(const RMessage2& aMessage, TInt aMsgSlot);
    private:
    
        CServiceSymbianSession(MServiceIPCObserver* aObserver);
    
        void ConstructL();
        
        inline CServiceSymbianServer& CServiceSymbianSession::Server()
            {return *reinterpret_cast<CServiceSymbianServer*>(const_cast<CServer2*>(CSession2::Server()));}
    
        inline void releaseSessionId(int aSessionId) 
        {
            Server().releaseSessionId(aSessionId);
        };
        
        inline void appendBroadcastList(int aSessionId, ServiceIPCSession * aSession)
        {
            Server().appendBroadcastList(aSessionId, aSession);
        }; 
    private:
        // data
        RMessage2 m_message;
        QByteArray m_data;
    };
}
#endif // serviceipcserversymbiansession_h
