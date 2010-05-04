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
    $$PWD/network/networkerrorreply.h \
    $$PWD/network/webcookiejar.h \
    $$PWD/network/webnetworkaccessmanager.h \
    $$PWD/network/SchemeHandlerBr.h \
    $$PWD/actionjsobject.h \
    $$PWD/browserpagefactory.h \
    $$PWD/brtglobal.h \
    $$PWD/scriptobjects.h \
    $$PWD/webpagedata.h \
    $$PWD/wrtbrowsercontainer_p.h \
    $$PWD/wrtbrowsercontainer.h \
    $$PWD/webcursornavigation.h \
    $$PWD/webdirectionalnavigation.h \
    $$PWD/webhtmltabbednavigation.h \
    $$PWD/webnavigation.h \
    $$PWD/webtouchnavigation.h \
    $$PWD/wrtpage.h

SOURCES += \
    $$PWD/network/networkerrorreply.cpp \
    $$PWD/network/webcookiejar.cpp \
    $$PWD/network/webnetworkaccessmanager.cpp \
    $$PWD/network/SchemeHandlerBr.cpp \
    $$PWD/actionjsobject.cpp \
    $$PWD/wrtbrowsercontainer.cpp \
    $$PWD/webcursornavigation.cpp \
    $$PWD/webdirectionalnavigation.cpp \
    $$PWD/webhtmltabbednavigation.cpp \
    $$PWD/webnavigation.cpp \
    $$PWD/webtouchnavigation.cpp \
    $$PWD/wrtpage.cpp
    
contains(what, plat_101 ) {
HEADERS += \
    $$PWD/network/WebNetworkConnectionManager.h \
    $$PWD/network/WebNetworkSession.h
SOURCES += \
    $$PWD/network/WebNetworkConnectionManager.cpp \
    $$PWD/network/WebNetworkSession.cpp \
}

symbian: {
SOURCES += $$PWD/network/WrtTelServiceBr.cpp 
HEADERS += \
   $$PWD/network/WrtTelServiceBr.h \
   $$PWD/network/WrtTelServiceBr_p.h 

}