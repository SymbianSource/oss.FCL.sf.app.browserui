#
# ============================================================================
#  Name        : serviceipc.pro
#  Part of     : Service Framework / IPC
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
TARGET = brserviceipcclient

QT -=gui

ROOT_DIR = $$PWD/../..

#Sources
HEADERS += serviceipcdefs.h \
           serviceipc_p.h \
           serviceipc.h \ 
           serviceipcfactory.h \
           serviceipcsharedmem_p.h \
           serviceipcclient.h
SOURCES += serviceipc.cpp \ 
           serviceipcfactory.cpp \
           serviceipcsharedmem.cpp \
           serviceipcclient.cpp

DEFINES += QT_MAKE_IPC_DLL
DEFINES+=QT_BEARER=1
DEFINES+=MAEMO_QT_BEARER=0
###INCLUDEPATH += $$CWRT_INCLUDE 


### EXPORT_FILES = serviceipcdefs.h serviceipc.h serviceipcclient.h

# Symbian Specifics
symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x200267E0
    SYMBIAN_PUB_SDK = $$(SYMBIAN_PUB_SDK)
    isEmpty(SYMBIAN_PUB_SDK) {
        TARGET.VID = VID_DEFAULT
    } else {
        TARGET.VID = 0x00000000
    }
    TARGET.CAPABILITY = All -Tcb
    MMP_RULES += EXPORTUNFROZEN

    brserviceipcclientlibs.sources = brserviceipcclient.dll
    brserviceipcclientlibs.path = /sys/bin
    DEPLOYMENT += brserviceipcclientlibs

    HEADERS += ./platform/s60/serviceipcsymbian_p.h \
               ./platform/s60/serviceipcsymbiansession.h 
    SOURCES += ./platform/s60/serviceipcsymbian.cpp \
               ./platform/s60/serviceipcsymbiansession.cpp
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
}
else {
    QT +=network
    HEADERS += ./platform/qt/serviceipclocalsocket_p.h
    SOURCES += ./platform/qt/serviceipclocalsocket.cpp
    INCLUDEPATH += $$PWD/platform/qt
    
    # Export headers on non-symbian systems
###    EXPORT_DIR = $$CWRT_INCLUDE
}

###include($$WRT_DIR/cwrt-export.pri)
