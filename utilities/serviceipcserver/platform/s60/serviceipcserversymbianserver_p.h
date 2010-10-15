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


#ifndef serviceipcserversymbianserver_p_h
#define serviceipcserversymbianserver_p_h

#include <e32base.h>
#include <QtCore>
#include "serviceipcserver.h"
#include "serviceipcserver_p.h"

namespace WRT {
    class MServiceIPCObserver;
    
    class CServiceSymbianServer : public CPolicyServer,
                    public ServiceFwIPCServerPrivate
    {
    public:
    
        static CServiceSymbianServer* NewL();
    
        virtual ~CServiceSymbianServer();
        
        void addSession();
        
        void closeSession();  
    
        // From ServiceFwIPCServerPrivate
    protected:
    
        bool listen(const QString& aServerName);
    
        void disconnect();
        
        void configIpcServerLifetime(bool aKeepServer);
    
        // From CPolicyServer
    protected:
    
        CSession2* NewSessionL(const TVersion& aVersion,
                               const RMessage2& aMessage) const;
    
    private:
    
        CServiceSymbianServer();
    
        void ConstructL();
    
    private:
        // data
        MServiceIPCObserver* m_observer;
        int m_sessionCount;
        bool m_keepServer;
    };
}
#endif // serviceipcserversymbianserver_p_h
