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

TEMPLATE = lib
TARGET = qstmgesturelib
INCLUDEPATH += .
INCLUDEPATH += qstmfilelogger
INCLUDEPATH += recognisers

ROOT_DIR = $$PWD/../..
include($$ROOT_DIR/browserui.pri)

QT += core 

HEADERS += qstmgestureevent.h \
    recognisers/qstmtouchgesturerecogniser.h \
    qstmgestureapi.h \
    recognisers/qstmzoomgesturerecogniser.h \
    recognisers/qstmupdowngesturerecogniser.h \
    recognisers/qstmunknowngesturerecogniser.h \
    recognisers/qstmtapgesturerecogniser.h \
    recognisers/qstmreleasegesturerecogniser.h \
    recognisers/qstmpinchgesturerecogniser.h \
    recognisers/qstmpangesturerecogniser.h \
    recognisers/qstmlongpressgesturerecogniser.h \
    recognisers/qstmleftrightgesturerecogniser.h \
    recognisers/qstmhoveringgesturerecogniser.h \
    recognisers/qstmflickgesturerecogniser.h \
    recognisers/qstmedgescrollgesturerecogniser.h \
    qstmgestureengine.h \
    qstmgesturelistener_if.h \
    qstmgenericsimplegesture.h \
    qstmgestureengine_if.h \
    qstmgesture_if.h \
    qstmutils.h \
    qstmuievent_if.h \
    qstmgesturedefs.h \
    qstmcallbacktimer.h \
    qstmstatemachine.h \
    qstmuievent.h \
    qstmstatemachine_v2.h \
    qstmtimerinterface.h \
    qstmstateengineconfig.h \
    qstmstateengine.h \
    qstmfilelogger/qstmfilelogger.h

SOURCES += recognisers/qstmtouchgesturerecogniser.cpp \
    qstmgestureapi.cpp \
    recognisers/qstmzoomgesturerecogniser.cpp \
    recognisers/qstmupdowngesturerecogniser.cpp \
    recognisers/qstmunknowngesturerecogniser.cpp \
    recognisers/qstmtapgesturerecogniser.cpp \
    recognisers/qstmreleasegesturerecogniser.cpp \
    recognisers/qstmpinchgesturerecogniser.cpp \
    recognisers/qstmpangesturerecogniser.cpp \
    recognisers/qstmlongpressgesturerecogniser.cpp \
    recognisers/qstmleftrightgesturerecogniser.cpp \
    recognisers/qstmhoveringgesturerecogniser.cpp \
    recognisers/qstmflickgesturerecogniser.cpp \
    recognisers/qstmedgescrollgesturerecogniser.cpp \
    qstmgestureengine.cpp \
    qstmgenericsimplegesture.cpp \
    qstmutils.cpp \
    qstmstateengine.cpp \
    qstmstateengineconfig.cpp \
    qstmstatemachine.cpp \
    qstmuievent.cpp \
    qstmuieventsender.cpp \
    qstmfilelogger/qstmfilelogger.cpp \
    qstmgestureevent.cpp



FORMS += 
RESOURCES += 

DEFINES += QSTMGESTURELIB

CONFIG += dll

#isEmpty(BEDROCK_OUTPUT_DIR): {
#    CONFIG(release, debug|release):BEDROCKSETTINGSLIB_OUTPUT_DIR=$$PWD/../../../../WrtBuild/Release
#    CONFIG(debug, debug|release):BEDROCKSETTINGSLIB_OUTPUT_DIR=$$PWD/../../../../WrtBuild/Debug
#} else {
#    BEDROCKSETTINGSLIB_OUTPUT_DIR = $$BEDROCK_OUTPUT_DIR
#}

GESTLIB_OUTPUT_DIR = $$ROOT_DIR/app/browser/qstmgesturelib/output
OBJECTS_DIR = $$GESTLIB_OUTPUT_DIR/qstmgesturelib/tmp
DESTDIR = $$GESTLIB_OUTPUT_DIR/bin
MOC_DIR = $$GESTLIB_OUTPUT_DIR/qstmgesturelib/tmp
RCC_DIR = $$GESTLIB_OUTPUT_DIR/qstmgesturelib/tmp
TEMPDIR = $$GESTLIB_OUTPUT_DIR/qstmgesturelib/build




symbian: {
#INCLUDEPATH += /epoc32/include/oem/tgfw
#EXPORT_DIR += /epoc32/include/oem/tgfw
#EXPORT_FILES += qstmgestureapi.h \
#                qstmgesturelistener_if.h \
#                qstmgestureengine_if.h \
#                qstmgesture_if.h \
#                qstmutils.h \
#                qstmuievent_if.h \
#                qstmgesturedefs.h \
#                qstmgestureevent.h \
#                qstmfilelogger/qstmfilelogger.h



#        for(exp_file, EXPORT_FILES) {
#            BLD_INF_RULES.prj_exports += "$${exp_file}   $${EXPORT_DIR}/$$basename(exp_file)" 
#        }

    #FIXME: These capabilities are way overkill
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = All -TCB -DRM -AllFiles 
    # NOTE: This is an not an OFFICIAL UID3
    TARGET.UID3 = 0x200267D7
    TARGET.VID = VID_DEFAULT
    
    LIBS += -lcone

    qstmgesturelib.sources = qstmgesturelib.dll
    qstmgesturelib.path = /sys/bin
    DEPLOYMENT += qstmgesturelib
}
