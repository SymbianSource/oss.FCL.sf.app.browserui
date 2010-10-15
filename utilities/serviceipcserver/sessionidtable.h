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


#ifndef SESSIONIDTABLE_H
#define SESSIONIDTABLE_H

namespace WRT
{
// CONSTANTS

const int SessionIdTableSize = 65536;

// CLASS DECLARATION

/*!
* Class for acquiring session Ids.
*
*/
class SessionIdTable
{

    public:

        SessionIdTable();
        
        int allocate();

        void release( int aSessionId );

    private: // Data

        bool m_data[SessionIdTableSize];

};
}
#endif // SESSIONIDTABLE_H
// End of file
