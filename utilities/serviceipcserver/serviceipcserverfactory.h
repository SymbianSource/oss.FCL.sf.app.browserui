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


#ifndef serviceipcserverfactory_h
#define serviceipcserverfactory_h

#include <QtCore>
#include <serviceipcdefs.h>

namespace WRT {
    
    // FORWARD DECLARATIONS
    class ServiceFwIPCServerPrivate;
    
    class ServiceFwIPCServerFactory
    {
    
    public:
    
        virtual ~ServiceFwIPCServerFactory();
    
        ServiceFwIPCServerFactory();
    
    public:
    
        static ServiceFwIPCServerPrivate* createBackend(QObject* aParent,
                                                        TServiceIPCBackends aBackend);
    
    };

}
#endif // serviceipcserverfactory_h
