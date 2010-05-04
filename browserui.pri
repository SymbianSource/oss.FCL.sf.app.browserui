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

# Build using default nestwork access mgr
#DEFINES+=NO_NETWORK_ACCESS_MANAGER

# Build without page resizing on load 
#DEFINES+=NO_RESIZE_ON_LOAD

# Build without gestures
DEFINES += NO_QSTM_GESTURE

# Build without chrome
#DEFINES += __gva_no_chrome__

# Enable setting Default IAP 
symbian: {  
 !contains(what, plat_101) { 
        # Enable setting Default IAP 
        DEFINES += SET_DEFAULT_IAP 
    } 
}

# Enable perfTracer
#DEFINES += ENABLE_PERF_TRACE

isEmpty(_BROWSERUI_PRI_INCLUDED_): {
    
    _BROWSERUI_PRI_INCLUDED_=1

    symbian {
        CONFIG += debug_and_release
        MMP_RULES += EXPORTUNFROZEN
    }
   
    CONFIG(debug, debug|release) {
        SUBDIRPART = Debug
    } else {
        SUBDIRPART = Release
    }
    
    # Figure out the root of where stuff should go (this could be done via configure)
    
    symbian {
        OUTPUT_DIR = $$PWD
    } else {
        OUTPUT_DIR = $$PWD/../../..
    }    
    OBJECTS_DIR = $$OUTPUT_DIR/WrtBuild/$$SUBDIRPART/$$TARGET/tmp
    MOC_DIR = $$OUTPUT_DIR/WrtBuild/$$SUBDIRPART/$$TARGET/tmp
    RCC_DIR = $$OUTPUT_DIR/WrtBuild/$$SUBDIRPART/$$TARGET/tmp
    UI_DIR = $$OUTPUT_DIR/WrtBuild/$$SUBDIRPART/$$TARGET/tmp
    TEMPDIR= $$OUTPUT_DIR/WrtBuild/$$SUBDIRPART/$$TARGET/build
    DESTDIR = $$OUTPUT_DIR/WrtBuild/$$SUBDIRPART/bin
    QMAKE_RPATHDIR = $$DESTDIR $$QMAKE_RPATHDIR
    QMAKE_LIBDIR = $$DESTDIR $$QMAKE_LIBDIR
    
    # Add the output dirs to the link path too
    LIBS += -L$$DESTDIR
    
    DEPENDPATH += .
    INCLUDEPATH += .

    QMAKE_CXXFLAGS_DEBUG += -g
    
    CONFIG(gcov)   {
       LIBS +=   -lgcov
       QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    }
}

