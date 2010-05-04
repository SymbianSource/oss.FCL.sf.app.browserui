#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#

HEADERS += \
    $$PWD/addbookmarkdialog.h \
    $$PWD/autosaver.h \
    $$PWD/bookmarks.h \
    $$PWD/editbookmarkdialog.h \
    $$PWD/editfolderdialog.h \
    $$PWD/modelmenu.h \
    $$PWD/UnicodeFile.h \
    $$PWD/xbel.h

SOURCES += \
    $$PWD/autosaver.cpp \
    $$PWD/bookmarks.cpp \
    $$PWD/modelmenu.cpp \
    $$PWD/HtmlTxtParser.cpp \
    $$PWD/UnicodeFile.cpp \  
    $$PWD/BookmarkDumpMain.cpp \  
    $$PWD/xbel.cpp

symbian: {   
LIBS +=  -lFavouritesEngine  
}