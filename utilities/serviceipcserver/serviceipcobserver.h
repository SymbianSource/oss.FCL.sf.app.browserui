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


#ifndef serviceipcobserver_h
#define serviceipcobserver_h

#include <QtCore>
#include "clientinfo.h"

namespace WRT {
    
    class ServiceIPCRequest;
    
    /**
     *  Interface Observer class to handle IPC related events
     */
    class MServiceIPCObserver
    {
    public:
    
        /**
         * Handle a new request 
         * @param aRequest request object, ownership NOT passed
         * @return bool true if handled
         */
        virtual bool handleRequest(ServiceIPCRequest *aRequest) = 0;
    
        /**
         * Handle any cancellation of an asynchronous request.
         * @param aRequest request object that is going to be cancelled, ownership NOT passed
         *                 Object will be cleaned up after callback returns
         * @return void
         */
        virtual void handleCancelRequest(ServiceIPCRequest *aRequest) = 0;
        
        /**
        * Handle the connection of a new client to the server
        * @param aNewClient information about the new client, including session ID
        * @return void
        */
        virtual void handleClientConnect( ClientInfo *aNewClient ) = 0;
        
        /**
        * Handle the disconnection of a client from the server. 
        * This callback is called when a client disconnects (ie closes a session)
        * @param aClient information about the client that is disconected
        * @return void
        */
        virtual void handleClientDisconnect( ClientInfo *aClient ) = 0;
    };

}
#endif // serviceipcobserver_h
