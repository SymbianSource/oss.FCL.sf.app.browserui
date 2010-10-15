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


#ifndef serviceipcserversession_h
#define serviceipcserversession_h

#include <QtCore>
#include "clientinfo.h"

namespace WRT {
    
    class MServiceIPCObserver;
    class ServiceIPCRequest;
    
    class ServiceIPCSession : public QObject
    {
    Q_OBJECT
    public:
    
        ServiceIPCSession(MServiceIPCObserver* aObserver);
    
        virtual ~ServiceIPCSession();
    
        virtual bool write(const QByteArray& aData) = 0;
    
        virtual bool completeRequest() = 0;
    
        virtual void close() = 0;
        
        virtual void releaseSessionId(int aSessionId) = 0;
        
        virtual void appendBroadcastList(int aSessionId, ServiceIPCSession * aSession) = 0; 
    
        inline void setClientInfo(ClientInfo* aClientInfo) 
        {
            m_clientInfo = aClientInfo;
        };        
        inline bool getReadyToSend() const
        {
            return m_readyToSend;
        };
        inline void setReadyToSend(bool readyToSend) 
        {
            m_readyToSend = readyToSend;
        };       
        inline void appendMessageList(const QByteArray& aMessage) 
        {
            m_messageList.append(aMessage);
        };       
        inline bool messageListIsEmpty() const
        {
            return m_messageList.isEmpty();
        };                 
    protected:
        void handleReq();
    protected:
        // data
        bool m_isPendingRequest;
        MServiceIPCObserver* m_observer;
        ServiceIPCRequest* m_curRequest;
        ClientInfo* m_clientInfo;
        bool m_appendToBList;
    private:
        QList<QByteArray> m_messageList;
        bool m_readyToSend;
    };

}
#endif // Q_SERVICE_IPC_SESSION_H
