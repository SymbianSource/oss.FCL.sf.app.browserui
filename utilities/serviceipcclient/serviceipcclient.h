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


#ifndef Q_SERVICEIPCCLIENT_H
#define Q_SERVICEIPCCLIENT_H

#include <QtCore>
#include "serviceipcdefs.h"

namespace WRT {
    
    class ServiceFwIPC;
    
    class SFWIPC_EXPORT ServiceIPCClient : public QObject
    {
    Q_OBJECT
    public:
        virtual ~ ServiceIPCClient();
    
        explicit ServiceIPCClient(QObject* aParent, TServiceIPCBackends aBackend = EDefaultIPC);
    
    public:
        bool connect(const QString& aServerName);
    
        void disconnect();
    
        bool startServer(const QString& aServerName, const QString& aExeName);
    
        bool sendSync(const QString& aRequestType, const QByteArray& aData);
    
        void sendAsync(const QString& aRequestType, const QByteArray& aData);
    
        QByteArray readAll();
    
        //bool requestPending();
        
        inline qint32 sessionId() {return m_sessionId;}
    
    signals:
    
        void error(int aError);
    
        void readyRead(); //keep interface for ServiceFwIPC
        
        void readAsync(QByteArray ipcResult); //read async result

        void handleMessage( QByteArray aMessage );  //send broadcast msg 
        
    private slots:
    /**
     * Handle the IPC client "readyRead" signal
     */
        void handleAsyncMsg(); 
        void handleBroadcastMsg();
        
    private:    
        void subscribeBroadcastMsg();
            
        // Member Variables
    protected:        
       // friend class ServiceFwIPC;
        // One instance of the IPC endpoint to handle async messages
        ServiceFwIPC* m_asyncIPC;
        
        // Another instance of the IPC endpoint to handle sync messages
        ServiceFwIPC* m_syncIPC;
        
        // Another instance of the IPC endpoint to handle broadcast messages
        ServiceFwIPC* m_broadcastIPC;
        
    private:
        qint32 m_sessionId;
    };

}
#endif // Q_SERVICEIPCCLIENT_H
