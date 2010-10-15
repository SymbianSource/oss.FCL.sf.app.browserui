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


#ifndef clientinfo_h
#define clientinfo_h

#include <QObject>
#include <QString>

namespace WRT {

    // FORWARD DECLARATIONS
    class ClientInfo
    {
    public:    
        /*!
         * Sets the client's name
         * @param aName - Client's name
         */
        inline void setName(const QString &aName) { iName = aName; }

        /*!
         * Gets the client's name
         */
        inline QString name() const { return iName; }

        /*!
         * Sets the client's process Id
         */
        inline void setProcessId(const qint64 aProcessId) { iProcessId = aProcessId; }

        /*!
         * Gets the client's process ID
         */
        inline qint64 processId() const { return iProcessId; }

        /*!
         * Sets the client's vendor ID 
         */
        inline void setVendorId(const qint64 aVendorId) { iVendorId = aVendorId; }

        /*!
         * Gets the client's vendor ID 
         */
        inline qint64 vendorId() const { return iVendorId; }

        /*!
         * Set the session ID
         */
        inline void setSessionId( qint32 aSessionId ) { iSessionId = aSessionId; }
        
        /*!
         * Get the session ID
         */ 
        inline qint32 sessionId() const { return iSessionId; }
        
    private:
        QString iName;
        qint64 iProcessId;
        qint64 iVendorId;
        qint32 iSessionId;
    };
}

#endif //clientinfo_h
