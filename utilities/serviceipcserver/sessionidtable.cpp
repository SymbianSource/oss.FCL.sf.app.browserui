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


#include "sessionidtable.h"
#include <QDebug>

/*!
 \class SessionIdTable
 Assign unique session id for each client session
 */
 
namespace WRT
{
const int KSessionIdNotDefined = -1;
/*!
 Default constructor
 */
SessionIdTable::SessionIdTable()
{
    for ( int i = 0; i < SessionIdTableSize; i++ ) {
        m_data[i]  = false;
    }
}

/*!
 Reserve a session id.
 @return int unique session id.
 */
int SessionIdTable::allocate()
{
    int sessionId = KSessionIdNotDefined;

    // Search the table for free index
    for ( int i = 0; i < SessionIdTableSize; ++i ) {
        if ( !m_data[i] ) {
            m_data[i]  = true;
            sessionId = i;
            break;
        }
    }

    if( sessionId == KSessionIdNotDefined ){
        //all session id being used??
    }

    return sessionId;
}

/*!
 Release a session id.
 @param aSessionId session id.
 */ 
void SessionIdTable::release( int aSessionId ) {
    if( !m_data[aSessionId] ) {
        qDebug() << "release sessionid " << aSessionId; 
    } 
    m_data[aSessionId] = false;
}
}

// End of file
