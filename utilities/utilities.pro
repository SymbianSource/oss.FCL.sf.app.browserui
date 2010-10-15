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

TEMPLATE = subdirs
CONFIG += ordered

# FYI, Qt 4.7.2 and above will provide a fast memory allocator for
# Symbian applications, so we may not need the standaloneallocator
# library anymore at that time.  See:
#
#   https://qtrequirements.europe.nokia.com/browse/QT-3967
#   https://qtrequirements.europe.nokia.com/browse/QT-4080
#
symbian {
    SUBDIRS += standaloneallocator
}

SUBDIRS += serviceipcserver
SUBDIRS += serviceipcclient
SUBDIRS += downloadmanager
