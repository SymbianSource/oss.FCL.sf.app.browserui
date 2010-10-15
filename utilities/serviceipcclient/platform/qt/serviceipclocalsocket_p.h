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


#ifndef serviceipclocalsocket_p_h
#define serviceipclocalsocket_p_h

#include <QtCore>
#include <QtNetwork>
#include "serviceipc_p.h"

namespace WRT {

    class ServiceLocalSocketIPC : public QObject, public ServiceFwIPCPrivate
    {
    Q_OBJECT
    public:
        ServiceLocalSocketIPC();
    
        virtual ~ServiceLocalSocketIPC();
    
    public:
    
        bool connect(const QString& aServerName);
    
        void disconnect();
    
        bool startServer(const QString& aServerName, const QString& aExeName);
    
        bool sendSync(const QString& aRequestType, const QByteArray& aData);
    
        void sendAsync(const QString& aRequestType, const QByteArray& aData);
    
        QByteArray readAll();
    
        bool waitForRead();
    
    private slots:
    
        void handleError(QLocalSocket::LocalSocketError socketError);
    
        void handleReadyRead();
    
    private:
        int doMapErrors( int aError );
    
    private:
        enum TBufferType {
            ENoBuffer,
            ESyncBuffer,
            EAsyncBuffer
        };
    // Member Variables
    private:
        QLocalSocket* m_Socket;
        QByteArray m_AsyncData;
        TBufferType m_BufferType;
    };

}
#endif // serviceipclocalsocket_p_h
