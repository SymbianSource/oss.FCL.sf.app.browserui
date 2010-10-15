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


#ifndef serviceipclocalsocketsession_h
#define serviceipclocalsocketsession_h

#include "serviceipcserversession.h"
#include "serviceipcserverlocalsocket_p.h"

class MServiceIPCObserver;
class QLocalSocket;

namespace WRT {
    
    class MServiceIPCObserver;
    
    class LocalSocketSession : public ServiceIPCSession
    {
    Q_OBJECT
    public:
    
        virtual ~LocalSocketSession();
    
                        LocalSocketSession(QLocalSocket* aNewSocket,
                                           MServiceIPCObserver* aObserver);
    
        // From ServiceIPCSession
    
        bool write(const QByteArray& aData);
    
        bool completeRequest();
    
        void close();
        
        inline void releaseSessionId(int aSessionId) 
        {
            ((ServiceFwIPCServerLocalSocket* )parent())->releaseSessionId(aSessionId);
        };
        
        inline void appendBroadcastList(int aSessionId, ServiceIPCSession * aSession)
        {
            ((ServiceFwIPCServerLocalSocket*) parent())->appendBroadcastList(aSessionId, aSession);
        }; 
    
    public slots:
    
        void handleRequest();
    
    private slots:
    
        void handleDisconnect();
    
    private:
    
        void doCancelRequest();
    
        signals:
    
        void disconnected(ServiceIPCSession* aSession);
    
    private:
        QLocalSocket* m_socket;
    };

}

#endif // serviceipclocalsocketsession_h
