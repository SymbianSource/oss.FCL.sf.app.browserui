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


#ifndef serviceipcfactory_h
#define serviceipcfactory_h

#include <QtCore>
#include "serviceipcdefs.h"

namespace WRT {
    
    class ServiceFwIPCPrivate;
    
    class SFWIPC_EXPORT ServiceFwIPCFactory : public QObject
    {
    public:
    
        ServiceFwIPCFactory();
    
        virtual ~ServiceFwIPCFactory();
    
    public:
    
        static bool isIPCTypeSupported(TServiceIPCBackends aIPCType);
    
        static ServiceFwIPCPrivate* createBackend(TServiceIPCBackends aBackend =
                                                                     EDefaultIPC);
    
    private:
        // data
    
    };

}
#endif // serviceipcfactory_h
