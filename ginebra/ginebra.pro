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

TEMPLATE = app

TARGET = obsoleteBrowser
#TARGET = ginebra

# Everything should be relative to ROOT_DIR (makes cut-and-paste safer).
#ROOT_DIR = $$PWD/..

# Common build options, library includes (browsercore etc), and Qt settings.
#include($$ROOT_DIR/common/commonExternals.pri)
include(../common/commonExternals.pri)

QT += webkit
include($$ROOT_DIR/sf/app/browserui/browserui.pri)


# Common SOURCES, HEADERS from app
include($$ROOT_DIR/sf/app/browserui/common/common.pri)

TRANSLATIONS += $$PWD/translations/browserLoc.ts

# ;;; Following can presumably go away if we will also use Qt's QtWebKit
win32: {
  # adding the WRT bin directory to LIBPATH so that we can pickup QtWebkit.lib
  CONFIG(release, debug|release):WRT_BIN_DIR=$$PWD/../../../../WrtBuild/Release/bin
  CONFIG(debug, debug|release):WRT_BIN_DIR=$$PWD/../../../../WrtBuild/Debug/bin
  LIBPATH += $$WRT_BIN_DIR
}

# Override some output directories set by commonExternals.pri.
# Leave DEST_DIR unchanged.
OBJECTS_DIR = $$APP_OUTPUT_DIR/ginebra
MOC_DIR = $$APP_OUTPUT_DIR/ginebra
RCC_DIR = $$APP_OUTPUT_DIR/ginebra
TEMPDIR = $$APP_OUTPUT_DIR/ginebra/build

################################################################################
#
# NEW code for ginebra
#
################################################################################

HEADERS = \
    attentionanimator.h \
    chromejsobject.h \
    chromerenderer.h \
    chromesnippet.h \
    chromesnippetjsobject.h \
    chromeview.h \
    LocaleDelegate.h \
    chromewidget.h \
    chromewidgetjsobject.h \
    devicedelegate.h \
    flickcharm.h \
    graphicsitemanimation.h \
    utilities.h \
    visibilityanimator.h \
    viewstack.h \
    animations/bounceanimator.h \
    animations/flyoutanimator.h \
    animations/fadeanimator.h \
    animations/malstromanimator.h \
    animations/pulseanimator.h \
    animations/slideanimator.h \
    emulator/chromeconsole.h \
    emulator/chromewindow.h 

SOURCES = \
    attentionanimator.cpp \
    chromejsobject.cpp \
    chromerenderer.cpp \
    chromesnippet.cpp \
    chromesnippetjsobject.cpp \
    chromeview.cpp \
    LocaleDelegate.cpp \
    chromewidget.cpp \
    chromewidgetjsobject.cpp \
    devicedelegate.cpp \
    flickcharm.cpp \
    graphicsitemanimation.cpp \
    visibilityanimator.cpp \
    viewstack.cpp \
    animations/bounceanimator.cpp \
    animations/fadeanimator.cpp \
    animations/flyoutanimator.cpp \
    animations/malstromanimator.cpp \
    animations/slideanimator.cpp \
    animations/pulseanimator.cpp \
    emulator/chromeconsole.cpp \
    emulator/chromewindow.cpp \
    emulator/main.cpp

FORMS += emulator/ui/console.ui

RESOURCES += $$PWD/ginebra.qrc

contains(what, devicedelegate) {
    CONFIG += mobility
    MOBILITY = bearer systeminfo
    DEFINES += QT_MOBILITY_BEARER_SYSINFO
}

symbian: { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x20000 \
        0x2000000 \
        // \
        Min \
        128kB, \
        Max \
        32MB
    TARGET.CAPABILITY = All -TCB -DRM -AllFiles 
    
    TARGET.UID3 = 0x200267F0
    CONFIG += link_prl
    LIBS += -lcommdb
    LIBS += -lesock -lconnmon

contains(what, plat_101 ) {
    DEFINES += PLAT_101
} else {
    LIBS += -lstandaloneallocator.lib 
}

    LIBS += -lhal 

    # localpages
    localpages.sources = ./chrome/localpages/*.htm* \
                         ./chrome/localpages/*/js \
                         ./chrome/localpages/*.css \
                         ./chrome/localpages/*.png
    localpages.path = /data/Others/chrome/localpages
    CHROME_DEPLOYS += localpages

    # for all chromes
    globaljs.sources = ./chrome/js/*.html* \
                       ./chrome/js/*.js \
                       ./chrome/js/*.css 
    globaljs.path = /data/Others/chrome/js
    CHROME_DEPLOYS += globaljs

    # for deploying bedrock chrome
    chromehtml.sources = ./chrome/bedrockchrome/*.html* \
                         ./chrome/bedrockchrome/*.js \
                         ./chrome/bedrockchrome/*.css
    chromehtml.path = /data/Others/chrome/bedrockchrome
    BEDROCKCHROME_DEPLOYS += chromehtml
    
    globaljsthp.sources =   ./chrome/js/3rdparty/*.html* \
                            ./chrome/js/3rdparty/*.js \
                            ./chrome/js/3rdparty/*.css
    globaljsthp.path = /data/Others/chrome/js/3rdparty
    CHROME_DEPLOYS += globaljsthp
    
    statusbar.sources = ./chrome/bedrockchrome/statusbar.snippet/*.html* \
                        ./chrome/bedrockchrome/statusbar.snippet/*.js \
                        ./chrome/bedrockchrome/statusbar.snippet/*.css
    statusbar.path = /data/Others/chrome/bedrockchrome/statusbar.snippet
    BEDROCKCHROME_DEPLOYS += statusbar

    statusbaricons.sources = ./chrome/bedrockchrome/statusbar.snippet/icons/*.png
    statusbaricons.path = /data/Others/chrome/bedrockchrome/statusbar.snippet/icons
    BEDROCKCHROME_DEPLOYS += statusbaricons

    statusbariconsbattery.sources = ./chrome/bedrockchrome/statusbar.snippet/icons/battery/*.png
    statusbariconsbattery.path = /data/Others/chrome/bedrockchrome/statusbar.snippet/icons/battery
    BEDROCKCHROME_DEPLOYS += statusbariconsbattery

    statusbariconssignal.sources = ./chrome/bedrockchrome/statusbar.snippet/icons/signal/*.png
    statusbariconssignal.path = /data/Others/chrome/bedrockchrome/statusbar.snippet/icons/signal
    BEDROCKCHROME_DEPLOYS += statusbariconssignal

    toolbar.sources =   ./chrome/bedrockchrome/toolbar2.snippet/*.html* \
                        ./chrome/bedrockchrome/toolbar2.snippet/*.js \
                        ./chrome/bedrockchrome/toolbar2.snippet/*.css
    toolbar.path = /data/Others/chrome/bedrockchrome/toolbar2.snippet
    BEDROCKCHROME_DEPLOYS += toolbar

    toolbaricons.sources = ./chrome/bedrockchrome/toolbar2.snippet/icons/*.png
    toolbaricons.path = /data/Others/chrome/bedrockchrome/toolbar2.snippet/icons
    BEDROCKCHROME_DEPLOYS += toolbaricons

    toolbarfjicons.sources = ./chrome/bedrockchrome/toolbar2.snippet/fjicons/*.png
    toolbarfjicons.path = /data/Others/chrome/bedrockchrome/toolbar2.snippet/fjicons
    BEDROCKCHROME_DEPLOYS += toolbarfjicons

    contextmenu.sources =   ./chrome/bedrockchrome/contextmenu.snippet/*.html* \
                            ./chrome/bedrockchrome/contextmenu.snippet/*.js \
                            ./chrome/bedrockchrome/contextmenu.snippet/*.css
    contextmenu.path = /data/Others/chrome/bedrockchrome/contextmenu.snippet
    BEDROCKCHROME_DEPLOYS += contextmenu

    download.sources =  ./chrome/bedrockchrome/download.snippet/*.html* \
                        ./chrome/bedrockchrome/download.snippet/*.js \
                        ./chrome/bedrockchrome/download.snippet/*.css
    download.path = /data/Others/chrome/bedrockchrome/download.snippet
    BEDROCKCHROME_DEPLOYS += download
    
    urlsearch.sources = ./chrome/bedrockchrome/urlsearch.snippet/*.html* \
                        ./chrome/bedrockchrome/urlsearch.snippet/*.js \
                        ./chrome/bedrockchrome/urlsearch.snippet/*.css
    urlsearch.path = /data/Others/chrome/bedrockchrome/urlsearch.snippet
    BEDROCKCHROME_DEPLOYS += urlsearch

    suggests.sources =  ./chrome/bedrockchrome/suggests.snippet/*.html* \
                        ./chrome/bedrockchrome/suggests.snippet/*.js \
                        ./chrome/bedrockchrome/suggests.snippet/*.css
    suggests.path = /data/Others/chrome/bedrockchrome/suggests.snippet
    BEDROCKCHROME_DEPLOYS += suggests

    urlsearchicons.sources = ./chrome/bedrockchrome/urlsearch.snippet/icons/*.png
    urlsearchicons.path = /data/Others/chrome/bedrockchrome/urlsearch.snippet/icons
    BEDROCKCHROME_DEPLOYS += urlsearchicons
 
    windowcount.sources =   ./chrome/bedrockchrome/windowcount.snippet/*.html* \
                            ./chrome/bedrockchrome/windowcount.snippet/*.js \
                            ./chrome/bedrockchrome/windowcount.snippet/*.css
    windowcount.path = /data/Others/chrome/bedrockchrome/windowcount.snippet
    BEDROCKCHROME_DEPLOYS += windowcount

    windowcounticons.sources = ./chrome/bedrockchrome/windowcount.snippet/icons/*.png
    windowcounticons.path = /data/Others/chrome/bedrockchrome/windowcount.snippet/icons
    BEDROCKCHROME_DEPLOYS += windowcounticons

  
    # Deploy common chrome files.
    DEPLOYMENT += $$CHROME_DEPLOYS
    
    # Deploy bedrock chrome files.
    DEPLOYMENT += $$BEDROCKCHROME_DEPLOYS
    
    # Deploy demo chrome files.
    #DEPLOYMENT += $$DEMOCHROME_DEPLOYS
}

unix: !symbian: {
    CURRENT_DIR = $$PWD
    CURRENT_CHROME = $$CURRENT_DIR/chrome
    CHROME_DEST = $$DESTDIR/chrome
    system(ln -s $$CURRENT_CHROME $$CHROME_DEST)
}

##################################
# Generate documentation
##################################
dox.target = docs
dox.commands = doxygen ./doc/doxyfile
dox.depends = $$SOURCES $$HEADERS
QMAKE_EXTRA_UNIX_TARGETS += dox
