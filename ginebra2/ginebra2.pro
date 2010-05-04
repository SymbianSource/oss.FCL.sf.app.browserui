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

TARGET = Bedrock

# Everything should be relative to ROOT_DIR (makes cut-and-paste safer).
ROOT_DIR = $$PWD/..

# Qt
QT += xml network sql webkit script

ROOT_DIR = $$PWD/..
include($$ROOT_DIR/browserui.pri)

# Common build options, library includes (browsercore etc), and Qt settings.
include($$ROOT_DIR/common/commonExternals.pri)
INCLUDEPATH += $$PWD/ContentViews


# Gesture lib
!contains(DEFINES,  NO_QSTM_GESTURE) {
   message("Including qstmgesturelib.")
   INCLUDEPATH += $$PWD/../qstmgesturelib
   INCLUDEPATH += $$PWD/../qstmgesturelib/qstmfilelogger
   LIBS += -lqstmgesturelib
   LIBPATH += $$ROOT_DIR/app/browser/qstmgesturelib/output/bin
 }

# Common SOURCES, HEADERS from app
# include($$ROOT_DIR/app/common/platform/platform.pri)
# include($$ROOT_DIR/app/common/common.pri)

TRANSLATIONS += $$PWD/translations/browserLoc.ts

# ;;; Following can presumably go away if we will also use Qt's QtWebKit
win32: { 
    # adding the WRT bin directory to LIBPATH so that we can pickup QtWebkit.lib
    CONFIG(release, debug|release):WRT_BIN_DIR = $$PWD/../../../WrtBuild/Release/bin
    CONFIG(debug, debug|release):WRT_BIN_DIR = $$PWD/../../../WrtBuild/Debug/bin
    LIBPATH += $$WRT_BIN_DIR
}

# Override some output directories set by commonExternals.pri.
# Leave DEST_DIR unchanged.
OBJECTS_DIR = $$APP_OUTPUT_DIR/ginebra2
MOC_DIR = $$APP_OUTPUT_DIR/ginebra2
RCC_DIR = $$APP_OUTPUT_DIR/ginebra2
TEMPDIR = $$APP_OUTPUT_DIR/ginebra2/build


################################################################################
#
# NEW code for ginebra2
#
################################################################################

RESOURCES = ginebra2.qrc
HEADERS = \
    Application.h \
    ActionButton.h \
    ActionButtonSnippet.h \
    CachedHandler.h \
    ChromeDOM.h \
    ChromeRenderer.h \
    ChromeSnippet.h \
    LocaleDelegate.h \
    ChromeWidget.h \
    ChromeWidgetJSObject.h \
    ChromeView.h \
    ContentViews\GWebContentViewJSObject.h \
    ContentViews\GWebContentViewWidget.h \
    ContentViews\GWebContentView.h \
    DeviceDelegate.h \
    VisibilityAnimator.h \
    PopupWebChromeItem.h \
    WebChromeItem.h \
    WebChromeSnippet.h \
    WebChromeContainerSnippet.h \
    GGraphicsWebView.h \
    GWebPage.h \
    GraphicsItemAnimation.h \
    NativeChromeItem.h \
    ScrollZoomWidget.h \
    SlidingWidget.h \
    TextEditItem.h \
    BlueChromeSnippet.h \
    GreenChromeSnippet.h \
    ProgressSnippet.h \
    Snippets.h \
    ScriptObjects.h \
    animators/FadeAnimator.h \
    animators/SlideAnimator.h \
    emulator/browser.h \
    emulator/ChromeConsole.h \
    ViewController.h \
    ViewStack.h \
    GWebTouchNavigation.h \
    KineticHelper.h \
    iconsnippet.h \
    iconwidget.h \
    ToolbarChromeItem.h \
    ContentToolbarChromeItem.h \
    UrlSearchSnippet.h \
    linearflowsnippet.h \
    mostvisitedpageview.h \
    mostvisitedsnippet.h

contains(DEFINES, ENABLE_PERF_TRACE) {    
    HEADERS += $$PWD/../internal/tests/perfTracing/wrtperftracer.h
}

contains(DEFINES, SET_DEFAULT_IAP) {     
    HEADERS += sym_iap_util.h 
}

!contains(DEFINES, NO_QSTM_GESTURE) {
    HEADERS += WebGestureHelper.h \
               WebTouchNavigation.h
}

 
SOURCES = \
    ActionButton.cpp \
    ActionButtonSnippet.cpp \
    Application.cpp \
    CachedHandler.cpp \
    ChromeDOM.cpp \
    ChromeRenderer.cpp \
    ChromeSnippet.cpp \
    LocaleDelegate.cpp \
    ChromeWidget.cpp \
    ChromeWidgetJSObject.cpp \
    ChromeView.cpp \
    ContentViews\GWebContentViewJSObject.cpp \
    ContentViews\GWebContentViewWidget.cpp \
    ContentViews\GWebContentView.cpp \
    DeviceDelegate.cpp \
    VisibilityAnimator.cpp \
    PopupWebChromeItem.cpp \
    WebChromeItem.cpp \
    WebChromeSnippet.cpp \
    WebChromeContainerSnippet.cpp \
    GGraphicsWebView.cpp \
    GraphicsItemAnimation.cpp \
    NativeChromeItem.cpp \
    Snippets.cpp \
    ScriptObjects.cpp \
    ScrollZoomWidget.cpp \
    SlidingWidget.cpp \
    TextEditItem.cpp \
    BlueChromeSnippet.cpp \
    GreenChromeSnippet.cpp \
    ProgressSnippet.cpp \
    animators/FadeAnimator.cpp \
    animators/SlideAnimator.cpp \
    emulator/main.cpp \
    emulator/browser.cpp \
    emulator/ChromeConsole.cpp \
    ViewController.cpp \
    ViewStack.cpp \
    GWebTouchNavigation.cpp \
    KineticHelper.cpp \
    iconsnippet.cpp \
    iconwidget.cpp \
    ToolbarChromeItem.cpp \
    ContentToolbarChromeItem.cpp \
    UrlSearchSnippet.cpp \
    mostvisitedpageview.cpp \
    linearflowsnippet.cpp \
    mostvisitedsnippet.cpp

contains(IMPORT_SUBDIRS, downloadmgr) {
    HEADERS += Downloads.h
    SOURCES += Downloads.cpp
}


contains(DEFINES, ENABLE_PERF_TRACE) {    
    SOURCES += $$PWD/../internal/tests/perfTracing/wrtperftracer.cpp
}

!contains(DEFINES, NO_QSTM_GESTURE) {
    SOURCES += WebGestureHelper.cpp \
               WebTouchNavigation.cpp 
}


    
FORMS += emulator/ui/console.ui

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
    ICON = ./browserIcon.svg
    contains(what, plat_101) {
        TARGET.UID3 = 0x10008D39
    }
    else {
        TARGET.UID3 = 0x200267DF
    }
    LIBS += -lcommdb
    LIBS += -lesock -lconnmon
contains(what, plat_101 ) {
    DEFINES += PLAT_101
} else {
    LIBS += -lstandaloneallocator.lib 
}

    LIBS += -lhal -lsysutil

    chrome.sources = ./chrome/*.htm \
                     ./chrome/*.js \
                     ./chrome/*.css
    chrome.path = /data/Others/ginebra2/chrome
    DEPLOYMENT += chrome

    # for all chromes
    globaljs.sources =  ./chrome/js/*.htm \
                        ./chrome/js/*.js \
                        ./chrome/js/*.css 
    globaljs.path = /data/Others/ginebra2/chrome/js
    CHROME_DEPLOYS += globaljs

    # localpages
    localpages.sources =    ./chrome/localpages/*.htm* \
                            ./chrome/localpages/*.js \
                            ./chrome/localpages/*.css \
                            ./chrome/localpages/*.jpg \
                            ./chrome/localpages/*.png 
    localpages.path = /data/Others/ginebra2/chrome/localpages
    CHROME_DEPLOYS += localpages

!contains(DEFINES, NO_QSTM_GESTURE) {    
    qstmgesturelib.sources = qstmgesturelib.dll
    qstmgesturelib.path = /sys/bin
    DEPLOYMENT += qstmgesturelib
}
    
    chromehtml.sources =    ./chrome/bedrockchrome/*.htm* \
                            ./chrome/bedrockchrome/*.js \
                            ./chrome/bedrockchrome/*.css
    chromehtml.path = /data/Others/ginebra2/chrome/bedrockchrome
    BEDROCKCHROME_DEPLOYS += chromehtml
    
    globaljsthp.sources =   ./chrome/js/3rdparty/*.htm* \
                            ./chrome/js/3rdparty/*.js \
                            ./chrome/js/3rdparty/*.css
    globaljsthp.path = /data/Others/ginebra2/chrome/js/3rdparty
    CHROME_DEPLOYS += globaljsthp
    
    globaljsjui.sources =   ./chrome/js/3rdparty/jquery-ui/*.htm* \
                            ./chrome/js/3rdparty/jquery-ui/*.js \
                            ./chrome/js/3rdparty/jquery-ui/*.css
    globaljsjui.path = /data/Others/ginebra2/chrome/js/3rdparty/jquery-ui
    CHROME_DEPLOYS += globaljsjui

    statusbar.sources = ./chrome/bedrockchrome/statusbar.snippet/*.htm* \
                        ./chrome/bedrockchrome/statusbar.snippet/*.js \
                        ./chrome/bedrockchrome/statusbar.snippet/*.css
    statusbar.path = /data/Others/ginebra2/chrome/bedrockchrome/statusbar.snippet
    BEDROCKCHROME_DEPLOYS += statusbar

    statusbaricons.sources = ./chrome/bedrockchrome/statusbar.snippet/icons/*.png
    statusbaricons.path = /data/Others/ginebra2/chrome/bedrockchrome/statusbar.snippet/icons
    BEDROCKCHROME_DEPLOYS += statusbaricons

    statusbariconsbattery.sources = ./chrome/bedrockchrome/statusbar.snippet/icons/battery/*.png
    statusbariconsbattery.path = /data/Others/ginebra2/chrome/bedrockchrome/statusbar.snippet/icons/battery
    BEDROCKCHROME_DEPLOYS += statusbariconsbattery

    statusbariconssignal.sources = ./chrome/bedrockchrome/statusbar.snippet/icons/signal/*.png
    statusbariconssignal.path = /data/Others/ginebra2/chrome/bedrockchrome/statusbar.snippet/icons/signal
    BEDROCKCHROME_DEPLOYS += statusbariconssignal

    toolbar.sources =   ./chrome/bedrockchrome/toolbar.snippet/*.htm* \
                        ./chrome/bedrockchrome/toolbar.snippet/*.js \
                        ./chrome/bedrockchrome/toolbar.snippet/*.css
    toolbar.path = /data/Others/ginebra2/chrome/bedrockchrome/toolbar.snippet
    BEDROCKCHROME_DEPLOYS += toolbar

    toolbaricons.sources = ./chrome/bedrockchrome/toolbar.snippet/icons/*.png
    toolbaricons.path = /data/Others/ginebra2/chrome/bedrockchrome/toolbar.snippet/icons
    BEDROCKCHROME_DEPLOYS += toolbaricons

    download.sources =  ./chrome/bedrockchrome/download.snippet/*.htm* \
                        ./chrome/bedrockchrome/download.snippet/*.js \
                        ./chrome/bedrockchrome/download.snippet/*.css
    download.path = /data/Others/ginebra2/chrome/bedrockchrome/download.snippet
    BEDROCKCHROME_DEPLOYS += download

    downloadicons.sources = ./chrome/bedrockchrome/download.snippet/icons/*.png
    downloadicons.path = /data/Others/ginebra2/chrome/bedrockchrome/download.snippet/icons
    BEDROCKCHROME_DEPLOYS += downloadicons

    contextmenu.sources =   ./chrome/bedrockchrome/contextmenu.snippet/*.htm* \
                            ./chrome/bedrockchrome/contextmenu.snippet/*.js \
                            ./chrome/bedrockchrome/contextmenu.snippet/*.css
    contextmenu.path = /data/Others/ginebra2/chrome/bedrockchrome/contextmenu.snippet
    BEDROCKCHROME_DEPLOYS += contextmenu
    
    contexticons.sources = ./chrome/bedrockchrome/contextmenu.snippet/icons/*.png
    contexticons.path = /data/Others/ginebra2/chrome/bedrockchrome/contextmenu.snippet/icons
    BEDROCKCHROME_DEPLOYS += contexticons

    urlsearch.sources = ./chrome/bedrockchrome/urlsearch.snippet/*.htm* \
                        ./chrome/bedrockchrome/urlsearch.snippet/*.js \
                        ./chrome/bedrockchrome/urlsearch.snippet/*.css
    urlsearch.path = /data/Others/ginebra2/chrome/bedrockchrome/urlsearch.snippet
    BEDROCKCHROME_DEPLOYS += urlsearch

    urlsearchicons.sources = ./chrome/bedrockchrome/urlsearch.snippet/icons/*.png
    urlsearchicons.path = /data/Others/ginebra2/chrome/bedrockchrome/urlsearch.snippet/icons
    BEDROCKCHROME_DEPLOYS += urlsearchicons
 
    suggests.sources =  ./chrome/bedrockchrome/suggests.snippet/*.htm* \
                        ./chrome/bedrockchrome/suggests.snippet/*.js \
                        ./chrome/bedrockchrome/suggests.snippet/*.css
    suggests.path = /data/Others/ginebra2/chrome/bedrockchrome/suggests.snippet
    BEDROCKCHROME_DEPLOYS += suggests

    windowcount.sources =   ./chrome/bedrockchrome/windowcount.snippet/*.htm* \
                            ./chrome/bedrockchrome/windowcount.snippet/*.js \
                            ./chrome/bedrockchrome/windowcount.snippet/*.css
    windowcount.path = /data/Others/ginebra2/chrome/bedrockchrome/windowcount.snippet
    BEDROCKCHROME_DEPLOYS += windowcount

    networkstatus.sources = ./chrome/bedrockchrome/networkstatus.snippet/*.htm* \
                            ./chrome/bedrockchrome/networkstatus.snippet/*.js \
                            ./chrome/bedrockchrome/networkstatus.snippet/*.css
    networkstatus.path = /data/Others/ginebra2/chrome/bedrockchrome/networkstatus.snippet
    BEDROCKCHROME_DEPLOYS += networkstatus

    windowcounticons.sources = ./chrome/bedrockchrome/windowcount.snippet/icons/*.png
    windowcounticons.path = /data/Others/ginebra2/chrome/bedrockchrome/windowcount.snippet/icons
    BEDROCKCHROME_DEPLOYS += windowcounticons

    zoombar.sources =   ./chrome/bedrockchrome/zoombar.snippet/*.htm* \
                        ./chrome/bedrockchrome/zoombar.snippet/*.js \
                        ./chrome/bedrockchrome/zoombar.snippet/*.css
    zoombar.path = /data/Others/ginebra2/chrome/bedrockchrome/zoombar.snippet
    BEDROCKCHROME_DEPLOYS += zoombar

    zoombaricons.sources = ./chrome/bedrockchrome/zoombar.snippet/icons/*.png
    zoombaricons.path = /data/Others/ginebra2/chrome/bedrockchrome/zoombar.snippet/icons
    BEDROCKCHROME_DEPLOYS += zoombaricons
    
    bookmarkview.sources =  ./chrome/bedrockchrome/bookmarkview.superpage/*.htm* \
                            ./chrome/bedrockchrome/bookmarkview.superpage/*.js \
                            ./chrome/bedrockchrome/bookmarkview.superpage/*.css
    bookmarkview.path = /data/Others/ginebra2/chrome/bedrockchrome/bookmarkview.superpage
    BEDROCKCHROME_DEPLOYS += bookmarkview

    bookmarkviewicons.sources = ./chrome/bedrockchrome/bookmarkview.superpage/icons/*.png \
                                ./chrome/bedrockchrome/bookmarkview.superpage/icons/*.gif \
                                ./chrome/bedrockchrome/bookmarkview.superpage/icons/*.db
    bookmarkviewicons.path = /data/Others/ginebra2/chrome/bedrockchrome/bookmarkview.superpage/icons
    BEDROCKCHROME_DEPLOYS += bookmarkviewicons

    historyview.sources =   ./chrome/bedrockchrome/historyview.superpage/*.htm* \
                            ./chrome/bedrockchrome/historyview.superpage/*.js \
                            ./chrome/bedrockchrome/historyview.superpage/*.css
    historyview.path = /data/Others/ginebra2/chrome/bedrockchrome/historyview.superpage
    BEDROCKCHROME_DEPLOYS += historyview

    historyviewicons.sources = ./chrome/bedrockchrome/historyview.superpage/icons/*.png
    historyviewicons.path = /data/Others/ginebra2/chrome/bedrockchrome/historyview.superpage/icons
    BEDROCKCHROME_DEPLOYS += historyviewicons
    
    settingsview.sources =  ./chrome/bedrockchrome/settingsview.superpage/*.htm* \
                            ./chrome/bedrockchrome/settingsview.superpage/*.js \
                            ./chrome/bedrockchrome/settingsview.superpage/*.css
    settingsview.path = /data/Others/ginebra2/chrome/bedrockchrome/settingsview.superpage
    BEDROCKCHROME_DEPLOYS += settingsview

    settingsviewicons.sources = ./chrome/bedrockchrome/settingsview.superpage/icons/*.png \
                                ./chrome/bedrockchrome/settingsview.superpage/icons/*.gif \
                                ./chrome/bedrockchrome/settingsview.superpage/icons/*.db
    settingsviewicons.path = /data/Others/ginebra2/chrome/bedrockchrome/settingsview.superpage/icons
    BEDROCKCHROME_DEPLOYS += settingsviewicons


     # Deploy common chrome files.
     DEPLOYMENT += $$CHROME_DEPLOYS
    
     # Deploy bedrock chrome files.
     DEPLOYMENT += $$BEDROCKCHROME_DEPLOYS
}

#unix: {
	# Create symbolic link to executable.
#    QMAKE_POST_LINK += ln -sf $$DESTDIR/$$TARGET $$PWD/$$TARGET;
#}


##################################
# Generate documentation
# #################################
dox.target = docs
dox.commands = doxygen ./doc/doxyfile
dox.depends = $$SOURCES $$HEADERS
QMAKE_EXTRA_UNIX_TARGETS += dox

#INCLUDEPATH += $$PWD/../../mw/bedrockProvisioning
