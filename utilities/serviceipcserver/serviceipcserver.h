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


#ifndef serviceipcserver_h
#define serviceipcserver_h

#include <QtCore>
#include <serviceipcdefs.h>

namespace WRT {
    
    // Forward Declarations
    class ServiceFwIPCServerPrivate;
    class MServiceIPCObserver;
    
    class SFWIPCSRV_EXPORT ServiceFwIPCServer : public QObject
    {
	 Q_OBJECT
    public:
    
        ServiceFwIPCServer(MServiceIPCObserver* aObserver,
                           QObject* aParent,
                           TServiceIPCBackends aBackend = EDefaultIPC);
    
        virtual ~ServiceFwIPCServer();
    
    // APIs provided
    public:
    
        bool listen(const QString& aServerName);
    
        void disconnect();
        
        void configIpcServerLifetime(bool aKeepServer);
   
        /**
        * Send a broadcast message to all clients connected to this server
        * @param aMessage the contents of the message
        */ 
        void broadcast( const QByteArray& aMessage );
        
        /**
        * Send a message to a particular client connected to this server
        * @param aSessionId, id of a session to broadcast to
        * @param aMessage the contents of the message
        */ 
        void sendMessage( qint32 aSessionId, QByteArray& aMessage );
                             
             
    signals:
        void handleExit();
        
    private:
        void startTimer();
        
        void stopTimer();
        
    protected:
        // data
        MServiceIPCObserver* m_Observer;
        
    private:
        friend class ServiceFwIPCServerPrivate;
        ServiceFwIPCServerPrivate* d;
        QTimer *m_Timer;
        
    };

}
#endif // serviceipcserver_h
