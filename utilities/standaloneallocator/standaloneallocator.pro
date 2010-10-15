# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
#
# This file is part of Qt Web Runtime.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License
# version 2.1 as published by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
#
#

# Custom memory allocator lib for Symbian only
TEMPLATE = lib
TARGET   = standaloneallocator
# We might want to change this later to project-specific output area
DESTDIR  = $$QMAKE_LIBDIR_QT
# No need for any Qt libs here, pure Symbian C++
QT       =

CONFIG  += staticlib warn_on
CONFIG  -= qt shared

symbian {
    # Note: UID only needed for ensuring that no filename generation conflicts occur
    TARGET.UID3 = 0x200267B8
    INCLUDEPATH += . $${EPOCROOT}epoc32/include
    INCLUDEPATH += . $${EPOCROOT}epoc32/include/platform
    SOURCEPATH = .
    SOURCES  =  newallocator_hook.cpp newallocator.cpp
    MMP_RULES += EXPORTUNFROZEN
    
    # This seems not to work, some hard coded libs are still added as dependency
    LIBS =
} else {
    error("$$_FILE_ is intended only for Symbian!")
}
