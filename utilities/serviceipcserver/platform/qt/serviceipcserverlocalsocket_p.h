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


#ifndef serviceipcserverlocalsocket_p_h
#define serviceipcserverlocalsocket_p_h

#include <QtCore>
#include "serviceipcserver_p.h"

class QLocalServer;

namespace WRT {
    
    class ServiceFwIPCServer;
    
    class ServiceFwIPCServerLocalSocket : public QObject,
                    public ServiceFwIPCServerPrivate
    {
    Q_OBJECT
    public:
    
        ServiceFwIPCServerLocalSocket(QObject* aParent);
    
        virtual ~ServiceFwIPCServerLocalSocket();
    
    // Inherited from ServiceFwIPCServerPrivate
    public:
    
        bool listen(const QString& aServerName);
    
        void disconnect();
        
        void configIpcServerLifetime(bool aKeepServer);
    
    public slots:
    
        void handleNewConnection();
    
        void handleSessionDisconnect(ServiceIPCSession* aSession);
    
    private:
        QLocalServer* m_Listener;
        QSharedMemory m_SharedMem;
        bool m_isValidSession;
        bool m_isKeepServer;
    };

}
#endif // serviceipcserverlocalsocket_p_h
