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


#ifndef serviceipcserver_p_h
#define serviceipcserver_p_h

#include <QtCore>
#include "serviceipcserver_p.h"
#include "serviceipcserver.h"
#include "sessionidtable.h"

namespace WRT {
    
    class ServiceFwIPCServer;
    class ServiceIPCSession;
    class MServiceIPCObserver;
    
    /**
     *  Private implementation interface for service framework server
     *  This class is the abstract interface for all server backends
     */
    class ServiceFwIPCServerPrivate
    {
    public:
        /**
        * Virtual destructor
        */
        virtual ~ServiceFwIPCServerPrivate() {};

    public:
    
        /**
         * Start listening for new service requests
         * @param aServerName name of the server
         * @return true if listen was successful
         */
        virtual bool listen(const QString& aServerName) = 0;
    
        /**
         * Shutdown the server and stop serving clients 
         * @return void
         */
        virtual void disconnect() = 0;
    
        /**
         * IPC server lifetime should be configurable 
         * @param aKeepServer to keep or disconnect IPC server when all clients are shutdown.
         * @return void
         */
        virtual void configIpcServerLifetime(bool aKeepServer) = 0;
        
        //Keep session handle at high level, platform independent.
        inline QHash<int, ServiceIPCSession*> getBroadcastSessions() { return m_broadcastSessions; }
        
        
        inline void appendBroadcastList(int aSessionId, ServiceIPCSession * aSession) 
        { 
            m_broadcastSessions.insert(aSessionId, aSession); 
        };
        
        inline void removeBroadcastList(int aSessionId) 
        { 
            m_broadcastSessions.remove(aSessionId); 
        };
        /*
         Release session ID from iSessionIdTable
         @param aSession
         */   
        inline void releaseSessionId(int aSessionId)
        {
            m_sessionIdTable->release(aSessionId);
        };
        
        /*
         Allocate session ID from iSessionIdTable
         @return new Session ID
         */   
        inline int allocateSessionId()
        {
            return m_sessionIdTable->allocate();
        };
    
    protected:
    
        /**
         * Get the server observer
         * @return MServiceIPCObserver* observer to this server
         */
        inline MServiceIPCObserver* Observer()
        {
            return q->m_Observer;
        };
        
        inline void startExitTimer()
        {
            return q->startTimer();
        };
        
        inline void stopExitTimer()
        {
            return q->stopTimer();
        };
           
    
    protected:
        QList<ServiceIPCSession*> m_Sessions;
        
        //use for broadcast message to clients
        QHash<int, ServiceIPCSession*> m_broadcastSessions;
        
        SessionIdTable* m_sessionIdTable;
    
    private:
        friend class ServiceFwIPCServer;
        ServiceFwIPCServer* q;
    };

}
#endif // serviceipcserver_p_h
