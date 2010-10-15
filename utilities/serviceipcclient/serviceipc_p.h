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


#ifndef serviceipc_p_h
#define serviceipc_p_h

#include "serviceipc.h"
#include <QtCore>
#include <QByteArray>

namespace WRT {

    /**
     *  Private implementation class for IPC
     */
    class ServiceFwIPC;
    class ServiceFwIPCPrivate
    {
    public:
        /**
         * Virtual destructor
         */
        virtual ~ServiceFwIPCPrivate() {};

    
        // Virtual functions 
    
        /**
         * Connect to the server
         * @param aServerName name of the server to connect to
         * @return true if connected, false if not
         */
        virtual bool connect(const QString& aServerName) = 0;
    
        /**
         * Disconnect from the server
         */
        virtual void disconnect() = 0;
    
        /**
         * Starts the service
         * @note: refer to public API ServiceFwIPC
         */
        virtual bool startServer(const QString& aServerName,
                                 const QString& aExeName) = 0;
    
        /**
         * Send a request synchronously
         * @note: refer to public API ServiceFwIPC
         */
        virtual bool sendSync(const QString& aRequestType,
                              const QByteArray& aData) = 0;
    
        /**
         * Send a request asynchronously
         * @note: refer to public API ServiceFwIPC
         */
        virtual void sendAsync(const QString& aRequestType,
                               const QByteArray& aData) = 0;
    
        /**
         * Reads all data pending in the buffer
         * @note: refer to public API ServiceFwIPC
         */
        virtual QByteArray readAll() = 0;
    
        /**
         * Waits until data is available for reading 
         * @note: refer to public API ServiceFwIPC
         */
        virtual bool waitForRead() = 0;

        /**
         * Retrieves the session id synchronously
         * @note: refer to public API ServiceFwIPC
         */
        bool getSessionId(int& aSessionId);

        /**
         * Sets the session id
         * @note: refer to public API ServiceFwIPC
         */
        bool setSessionId(int aSessionId);

    protected:
        // TODO: Move to inl file
        inline void emitReadyRead()
        {
            if (q->m_AsyncRequestPending) {
                q->m_AsyncRequestPending = false;
                emit q->readyRead();
            }
        };
    
        inline void emitError(int aError)
        {
            emit q->error(aError);
        };
    
        inline bool asyncPending()
        {
            return q->requestPending();
        }
    private:
        friend class ServiceFwIPC;
        ServiceFwIPC* q;  // not owned
    };

}
#endif // serviceipc_p_h
