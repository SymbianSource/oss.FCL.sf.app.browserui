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

# Everything should be relative to ROOT_DIR (makes cut-and-paste safer).
ROOT_DIR = $$PWD/../../../..


#
# set lib dirs and output dirs depending on debug or not
symbian {
    CONFIG(release, debug|release) {
        APP_OUTPUT_DIR=$$PWD/../WrtBuild/Release
    }
    CONFIG(debug, debug|release) {
        APP_OUTPUT_DIR=$$PWD/../WrtBuild/Debug
    }
} else {
    CONFIG(release, debug|release) {
        APP_OUTPUT_DIR=$$ROOT_DIR/WrtBuild/Release
    }
    CONFIG(debug, debug|release) {
        APP_OUTPUT_DIR=$$ROOT_DIR/WrtBuild/Debug
    }
}
OBJECTS_DIR = $$APP_OUTPUT_DIR/browser/tmp
DESTDIR = $$APP_OUTPUT_DIR/bin
DLLDIR = $$APP_OUTPUT_DIR/bin
MOC_DIR=$$APP_OUTPUT_DIR/browser/tmp
RCC_DIR=$$APP_OUTPUT_DIR/browser/tmp
TEMPDIR=$$APP_OUTPUT_DIR/browser/build

# use the stylesheets for demo UIs!
!contains(DEFINES, WRT_USE_STYLE_SHEET=.): DEFINES += WRT_USE_STYLE_SHEET=1

# no app bundle (for OSX)
CONFIG -= app_bundle

# create QMAKE_RPATHDIR, LIBDIR
QMAKE_RPATHDIR = \
    $$DLLDIR \
    $$QMAKE_RPATHDIR

QMAKE_LIBDIR = \
    $$DLLDIR \
    $$QMAKE_RPATHDIR

# Import pre-built binary components.
include($$ROOT_DIR/import/import.pri)

# BedrockProvisioning
INCLUDEPATH += $$ROOT_DIR/sf/app/browserui/bedrockProvisioning
LIBS += -lBedrockProvisioning

# Bookmarks
#LIBS += -lBookMarksClient
LIBS += -lbrowsercontentdll

# Browsercore
BROWSERCORE_DIR = $$ROOT_DIR/sf/app/browserui/browsercore
INCLUDEPATH += $$BROWSERCORE_DIR/core
INCLUDEPATH += $$BROWSERCORE_DIR/appfw/Common


include($$BROWSERCORE_DIR/appfw/Api/Api.pri)
LIBS += -lBrowserCore

# Qt
QT += xml network sql webkit
