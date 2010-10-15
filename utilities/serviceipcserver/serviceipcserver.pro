#
# ============================================================================
#  Name        : serviceipcserver.pro
#  Part of     : Service Framework / IPC Server
#  Description : Service Framework IPC
#  Version     : %version: 1 % 
#
#  Copyright © 2009 Nokia.  All rights reserved.
#  This material, including documentation and any related computer
#  programs, is protected by copyright controlled by Nokia.  All
#  rights are reserved.  Copying, including reproducing, storing,
#  adapting or translating, any or all of this material requires the
#  prior written consent of Nokia.  This material also contains
#  confidential information which may not be disclosed to others
#  without the prior written consent of Nokia.
# ============================================================================
#
TEMPLATE = lib
TARGET = brserviceipcserver
QT -= gui

ROOT_DIR = $$PWD/../..
###WRT_DIR = $$ROOT_DIR/wrt

###include($$WRT_DIR/cwrt.pri)

#Sources
HEADERS += serviceipcserverfactory.h \
           serviceipcserver.h \
           serviceipcserver_p.h \ 
           serviceipcobserver.h \
           serviceipcserversession.h \
           serviceipcrequest.h \
           sessionidtable.h

SOURCES += serviceipcserverfactory.cpp \
           serviceipcserver.cpp \
           serviceipcserversession.cpp \
           serviceipcrequest.cpp \
           sessionidtable.cpp
           
DEFINES += QT_MAKE_IPC_SERVER_DLL
###INCLUDEPATH += $$CWRT_INCLUDE

EXPORT_FILES = serviceipcserver.h \
    serviceipcobserver.h \
    serviceipcserversession.h \
    serviceipcrequest.h \
    clientinfo.h

# Symbian Specifics
symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x102829E8
    SYMBIAN_PUB_SDK = $$(SYMBIAN_PUB_SDK)
    isEmpty(SYMBIAN_PUB_SDK) {
        TARGET.VID = VID_DEFAULT
    } else {
        TARGET.VID = 0x00000000
    }
    TARGET.CAPABILITY = All -Tcb
    MMP_RULES += EXPORTUNFROZEN

    brserviceipcserverlibs.sources = brserviceipcserver.dll
    brserviceipcserverlibs.path = /sys/bin
    DEPLOYMENT += brserviceipcserverlibs

    HEADERS += ./platform/s60/serviceipcserversymbianserver_p.h \
               ./platform/s60/serviceipcserversymbiansession.h
    SOURCES += ./platform/s60/serviceipcserversymbianserver.cpp \
               ./platform/s60/serviceipcserversymbiansession.cpp

    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE ../serviceipcclient
}
else {
# Export headers on non-symbian systems
    QT +=network
    #Export directory    
###    EXPORT_DIR = $$CWRT_INCLUDE
    
    HEADERS += ./platform/qt/serviceipcserverlocalsocket_p.h \
               ./platform/qt/serviceipclocalsocketsession.h
    SOURCES += ./platform/qt/serviceipcserverlocalsocket.cpp \
               ./platform/qt/serviceipclocalsocketsession.cpp
    
    INCLUDEPATH += $$PWD/platform/qt
}

###include($$WRT_DIR/cwrt-export.pri)

