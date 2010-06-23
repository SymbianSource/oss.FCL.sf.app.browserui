#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
# All rights reserved.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, version 2.1 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, 
# see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
#
# Description:
#

HEADERS += \
    $$PWD/BookmarksManager_p.h \
    $$PWD/BookmarksManager.h \
    $$PWD/secureuicontroller_p.h \
    $$PWD/secureuicontroller.h \
    $$PWD/LoadController.h \
    $$PWD/viewcontroller.h \
    $$PWD/webpagecontroller.h \
    $$PWD/webpagecontroller_p.h  \
    $$PWD/downloadcontroller_p.h \
    $$PWD/downloadcontroller.h \
    $$PWD/contentagent_p.h \
    $$PWD/contentagent.h \
    $$PWD/lowmemoryhandler.h \
    $$PWD/lowmemoryhandler_p.h

SOURCES += \
    $$PWD/BookmarksManager.cpp \
    $$PWD/secureuicontroller.cpp \
    $$PWD/LoadController.cpp \
    $$PWD/viewcontroller.cpp \
    $$PWD/webpagecontroller.cpp \
    $$PWD/downloadcontroller.cpp \
    $$PWD/contentagent.cpp \
    $$PWD/lowmemoryhandler.cpp
 
symbian: { 
SOURCES += \
   $$PWD/lowmemoryhandler_p_s60.cpp
}
!symbian: {
SOURCES += \
   $$PWD/lowmemoryhandler_p.cpp
}
