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


QT += webkit

TEMPLATE = lib
TARGET = BrowserCore
DEFINES += BUILDING_BROWSER_CORE
DEFINES += BUILDING_BWF_CORE

NETWORK_DEBUG: DEFINES+=NETWORK_DEBUG

################################################################################

# establish relative root dir
ROOT_DIR = $$PWD/../..

QT += webkit
include($$ROOT_DIR/browserui.pri)

isEmpty(WRT_OUTPUT_DIR) {
    symbian {
        CONFIG(release, debug|release):WRT_OUTPUT_DIR=$$PWD/../../WrtBuild/Release
        CONFIG(debug, debug|release):WRT_OUTPUT_DIR=$$PWD/../../WrtBuild/Debug
    } 
    else {
        CONFIG(release, debug|release):WRT_OUTPUT_DIR=$$ROOT_DIR/../../../WrtBuild/Release
        CONFIG(debug, debug|release):WRT_OUTPUT_DIR=$$ROOT_DIR/../../../WrtBuild/Debug
    }
}

LIBS += -lBedrockProvisioning -lbrowsercontentdll

RESOURCES += $$PWD/../browsercore.qrc

symbian: {
    SYMBIAN_PUB_SDK = $$(SYMBIAN_PUB_SDK)
    isEmpty(SYMBIAN_PUB_SDK) {
        LIBS += -lsisregistryclient -letelmm
    } else {
        DEFINES += SYMBIAN_PUB_SDK
    }
    #LIBS += -llibpthread -letel -lsysutil -lWrtTelService -lsendui -letext -lcommonengine -lcone -lefsrv 
    LIBS += -llibpthread -letel -lsysutil -lsendui -letext -lcommonengine -lcommonui -lcone -lefsrv -lServiceHandler -lapmime -lapparc
    
    isEmpty(SYMBIAN_PUB_SDK) {
    LIBS +=  \
        -laiwdialdata
		}

    AIWResource = \
        "START RESOURCE WrtTelService.rss" \
        "HEADER" \
        "TARGETPATH resource/apps" \
        "END"
    MMP_RULES += AIWResource 


    browsercorelibs.sources = browsercore.dll

    browsercorelibs.path = /sys/bin

    #browsercoreresources.sources = /epoc32/data/z/resource/apps/WrtTelService.rsc
    #browsercoreresources.path = /resource/apps

    DEPLOYMENT += browsercorelibs 
    #browsercoreresources
}

UTILITIES_DIR = $$ROOT_DIR/utilities


CONFIG += \
    building-libs \
    depend_includepath \
    dll

contains(what, plat_101 ) {
    CONFIG += mobility
    MOBILITY = bearer
    DEFINES += QT_MOBILITY_BEARER_MANAGEMENT
}

contains(QT_CONFIG, embedded): CONFIG += embedded

!CONFIG(QTDIR_build) {
     OBJECTS_DIR = $$WRT_OUTPUT_DIR/browsercore/tmp
     DESTDIR = $$WRT_OUTPUT_DIR/bin
     MOC_DIR=$$WRT_OUTPUT_DIR/browsercore/tmp
     RCC_DIR=$$WRT_OUTPUT_DIR/browsercore/tmp
     TEMPDIR=$$WRT_OUTPUT_DIR/browsercore/build
     QMAKE_LIBDIR=$$DESTDIR $$QMAKE_LIBDIR
}

CONFIG(release, debug|release):!CONFIG(QTDIR_build){
    contains(QT_CONFIG, reduce_exports): CONFIG += hide_symbols
    unix : contains(QT_CONFIG, reduce_relocations): CONFIG += bsymbolic_functions
}

CONFIG -= warn_on
*-g++* : QMAKE_CXXFLAGS += -Wreturn-type -fno-strict-aliasing

CONFIG(gcov)   {
   LIBS +=   -lgcov
   QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
}

# Pick up 3rdparty libraries from INCLUDE/LIB just like with MSVC
win32-g++ {
    TMPPATH = $$quote($$(INCLUDE))
    QMAKE_INCDIR_POST += $$split(TMPPATH,";")
    TMPPATH = $$quote($$(LIB))
    QMAKE_LIBDIR_POST += $$split(TMPPATH,";")
}

symbian: {
    DEFINES += NO_IOSTREAM
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000 // Min 128kB, Max 32MB
    DEFINES += _WCHAR_T_DECLARED
    QMAKE_CXXFLAGS.CW = -O1 -wchar_t on
    TARGET.CAPABILITY = All -TCB -DRM -AllFiles 
    TARGET.UID3 = 0x200267BB
    TARGET.VID = VID_DEFAULT
}


# Import pre-built binary components.
include($$PWD/../../../../../import/import.pri)


#
# SOURCE and HEADER lists
#

include(core.pri)
include($$PWD/../appfw/appfw-includepath.pri)

# TEMP until appfw is its own dll
DEFINES += BUILDING_BWF_CORE
include(../appfw/appfw.pri)

QT += network
QT += webkit

#
# INCLUDE PATHS
#
INCLUDEPATH += \
    $$PWD \
    $$PWD/network \
    $$PWD/../../bedrockProvisioning \
    #FIXME_10.1 fix path below
    $$PWD/../../../../mw/browser/bookmarksengine/browsercontentdll/inc
symbian: {
    INCLUDEPATH +=  $$PWD $$MW_LAYER_SYSTEMINCLUDE $$APP_LAYER_SYSTEMINCLUDE
#   INCLUDEPATH += /epoc32/include/oem/tgfw
}
