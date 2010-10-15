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

#ifndef DOWNLOADMGRPIMPL_H
#define DOWNLOADMGRPIMPL_H

#define DM_DECLARE_PRIVATE(Class) \
        private: \
                inline Class##Private* priv_func() { return reinterpret_cast<Class##Private *>(priv_ptr); } \
                inline const Class##Private* priv_func() const { return reinterpret_cast<const Class##Private *>(priv_ptr); } \
                friend class Class##Private; \
                void* priv_ptr;

#define DM_DECLARE_PUBLIC(Class) \
        public: \
                inline Class* pub_func() { return static_cast<Class *>(pub_ptr); } \
                inline const Class* pub_func() const { return static_cast<const Class *>(pub_ptr); } \
        private: \
                friend class Class; \
                void* pub_ptr;

#define DM_PRIVATE(Class) Class##Private * const priv = priv_func()
#define DM_PUBLIC(Class) Class * const pub = pub_func()

#define DM_INITIALIZE(Class) \
                priv_ptr = new Class##Private(); \
                DM_PRIVATE(Class); \
                priv->pub_ptr = this;

#define DM_UNINITIALIZE(Class) \
                DM_PRIVATE(Class); \
                delete priv;

#endif // DOWNLOADMGRPIMPL_H

