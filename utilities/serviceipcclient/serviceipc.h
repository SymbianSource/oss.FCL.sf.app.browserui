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


#ifndef serviceipc_h
#define serviceipc_h

#include "serviceipcdefs.h"
#include <QtCore>

namespace WRT {
    
    class ServiceFwIPCPrivate;
    
    class SFWIPC_EXPORT ServiceFwIPC : public QObject
    {
    Q_OBJECT
    public:

        //! ServiceFwIPC::ServiceIPCErrors
        /*!
         This enum describes the errors that may be returned by the IPC.
         */
        enum ServiceIPCErrors
            {
            EConnectionError = -5000,               /*!< Error in IPC Connection */
            EConnectionClosed,                      /*!< IPC Connection is closed */
            EServerNotFound,                        /*!< Can not find server */
            EIPCError,                              /*!< Known IPC error defined by SDK */
            EUnknownError                           /*!< Unknown IPC error */
            };

    public:
        virtual ~ServiceFwIPC();
    
        explicit ServiceFwIPC(QObject* aParent, TServiceIPCBackends aBackend = EDefaultIPC);
    
    public:
        bool connect(const QString& aServerName);
    
        void disconnect();
    
        bool startServer(const QString& aServerName, const QString& aExeName);
    
        bool sendSync(const QString& aRequestType, const QByteArray& aData);
    
        void sendAsync(const QString& aRequestType, const QByteArray& aData);
    
        QByteArray readAll();
    
        bool requestPending();
    
        bool getSessionId(int& aSessionId);

        bool setSessionId(int aSessionId);
    private:
        bool waitForRead();
    
        signals:
    
        void error(int aError);
    
        void readyRead();
    
        // Member Variables
    protected:
        bool m_AsyncRequestPending;

        friend class ServiceFwIPCPrivate;
        ServiceFwIPCPrivate* d;
    };

}
#endif // serviceipc_h
