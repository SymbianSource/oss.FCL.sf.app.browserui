#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, version 2.1 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not,
# see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
#
# Description:
#

TEMPLATE = app

TARGET = NokiaBrowser

include($$PWD/../flags.pri)

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
   #LIBPATH += $$ROOT_DIR/app/browser/qstmgesturelib/output/bin
 }

contains(DEFINES, ENABLE_PERF_TRACE) {
    message("ginebra2.pro: Tracing is ON")
    LIBS += -lbrperftrace
    INCLUDEPATH += $$PWD/../internal/tests/perfTracing
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
    ChromeEffect.h \
    ChromeWidget.h \
    ChromeWidgetJSObject.h \
    ChromeView.h \
    ContentViews\GWebContentViewJSObject.h \
    ContentViews\GWebContentViewWidget.h \
    ContentViews\GWebContentView.h \
    ContentViews\GContentViewTouchNavigation.h \
    DeviceDelegate.h \
    NetworkDelegate.h \
    VisibilityAnimator.h \
    ChromeItem.h \
    PopupWebChromeItem.h \
    WebChromeItem.h \
    WebChromeSnippet.h \
    WebChromeContainerSnippet.h \
    GGraphicsWebView.h \
    GWebPage.h \
    GraphicsItemAnimation.h \
    NativeChromeItem.h \
    SlidingWidget.h \
    ObjectCharm.h \
    PageSnippet.h \
    PageItem.h \
    ProgressBarItem.h \
    ProgressBarSnippet.h \
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
    Toolbar.h \
    ToolbarChromeItem.h \
    ToolbarSnippet.h \
    ContentToolbarChromeItem.h \
    ContentToolbarSnippet.h \
    DualButtonToolbarSnippet.h \
    WindowToolbarSnippet.h \
    BookmarksToolbarSnippet.h \
    SettingsToolbarSnippet.h \
    RecentUrlToolbarSnippet.h \
    UrlSearchSnippet.h \
    Downloads.h \
    GAlternateFileChooser.h \
    linearflowsnippet.h \
    mostvisitedpageview.h \
    mostvisitedsnippet.h \
    EditorWidget.h \
    EditorSnippet.h


symbian: {
  contains(br_default_iap, yes) {
    DEFINES += SET_DEFAULT_IAP
    HEADERS += sym_iap_util.h
  }
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
    ChromeEffect.cpp \
    ChromeWidget.cpp \
    ChromeWidgetJSObject.cpp \
    ChromeView.cpp \
    ContentViews\GWebContentViewJSObject.cpp \
    ContentViews\GWebContentViewWidget.cpp \
    ContentViews\GWebContentView.cpp \
    ContentViews\GContentViewTouchNavigation.cpp \
    DeviceDelegate.cpp \
    NetworkDelegate.cpp \
    VisibilityAnimator.cpp \
    ChromeItem.cpp \
    PopupWebChromeItem.cpp \
    WebChromeItem.cpp \
    WebChromeSnippet.cpp \
    WebChromeContainerSnippet.cpp \
    GGraphicsWebView.cpp \
    GraphicsItemAnimation.cpp \
    GWebPage.cpp \
    NativeChromeItem.cpp \
    Snippets.cpp \
    ScriptObjects.cpp \
    SlidingWidget.cpp \
    ObjectCharm.cpp \
    PageSnippet.cpp \
    PageItem.cpp \
    ProgressBarItem.cpp \
    ProgressBarSnippet.cpp \
    animators/FadeAnimator.cpp \
    animators/SlideAnimator.cpp \
    emulator/main.cpp \
    emulator/browser.cpp \
    emulator/ChromeConsole.cpp \
    ViewController.cpp \
    ViewStack.cpp \
    GWebTouchNavigation.cpp \
    KineticHelper.cpp \
    ToolbarChromeItem.cpp \
    ToolbarSnippet.cpp \
    ContentToolbarChromeItem.cpp \
    ContentToolbarSnippet.cpp \
    DualButtonToolbarSnippet.cpp \
    WindowToolbarSnippet.cpp \
    BookmarksToolbarSnippet.cpp \
    SettingsToolbarSnippet.cpp \
    RecentUrlToolbarSnippet.cpp \
    UrlSearchSnippet.cpp \
    Downloads.cpp \
    GAlternateFileChooser.cpp \
    linearflowsnippet.cpp \
    mostvisitedpageview.cpp \
    mostvisitedsnippet.cpp \
    EditorWidget.cpp \
    EditorSnippet.cpp

!contains(DEFINES, NO_QSTM_GESTURE) {
    SOURCES += WebGestureHelper.cpp \
               WebTouchNavigation.cpp
}

FORMS += emulator/ui/console.ui

contains(br_mobility_sysinfo, yes) {
    CONFIG += mobility
    MOBILITY = systeminfo
    DEFINES += QT_MOBILITY_SYSINFO
}

contains(br_mobility_sysinfo, yes) {
    HEADERS += SystemDeviceImpl.h \
               SystemNetworkImpl.h
    SOURCES += SystemDeviceImpl.cpp \
               SystemNetworkImpl.cpp
}

contains(br_mobility_bearer, yes) {
    DEFINES += QT_MOBILITY_BEARER_MANAGEMENT
}

contains(br_orbit_ui, yes) {
    INCLUDEPATH += /epoc32/include/mw/hb/hbcore \
                   /epoc32/include/mw/hb/hbwidgets
    LIBS += -lHbCore -lHbWidgets
    DEFINES += ORBIT_UI
}

symbian: {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.EPOCSTACKSIZE = 0x14000
    contains(br_increased_heap,yes){
    TARGET.EPOCHEAPSIZE = 0x20000 \
        0x4000000 \
        // \
        Min \
        128kB, \
        Max \
        64MB

    emulatorHeapSize = \
        "$${LITERAL_HASH}ifdef WINSCW" \
        "EPOCHEAPSIZE 0x20000 0x2000000 // Min 128kB, Max 32MB" \
        "$${LITERAL_HASH}endif"

    MMP_RULES += emulatorHeapSize
    }else{
    TARGET.EPOCHEAPSIZE = 0x20000 \
        0x2000000 \
        // \
        Min \
        128kB, \
        Max \
        32MB
    }

    TARGET.CAPABILITY = All -TCB -DRM -AllFiles
    ICON = ./browserIcon.svg
    contains(browser_addon, no) {
        TARGET.UID3 = 0x10008D39
    }
    else {
        TARGET.UID3 = 0x200267DF
    }
    LIBS += -lcommdb
    LIBS += -lesock -lconnmon -linsock
contains(browser_addon, no) {
    DEFINES += PLAT_101
}

contains(br_fast_allocator, yes) {
    LIBS += -lstandaloneallocator.lib
}

    LIBS += -lhal -lsysutil

    chrome.sources = ./chrome/*.htm \
                     ./chrome/*.js \
                     ./chrome/*.css
    chrome.path = ./chrome
    DEPLOYMENT += chrome

    # for all chromes
    globaljs.sources =  ./chrome/js/*.htm \
                        ./chrome/js/*.js \
                        ./chrome/js/*.css
    globaljs.path = ./chrome/js
    CHROME_DEPLOYS += globaljs

    # localpages
    localpages.sources =    ./chrome/localpages/*.htm* \
                            ./chrome/localpages/*.js \
                            ./chrome/localpages/*.css \
                            ./chrome/localpages/*.jpg \
                            ./chrome/localpages/*.png
    localpages.path = ./chrome/localpages
    CHROME_DEPLOYS += localpages

!contains(DEFINES, NO_QSTM_GESTURE) {
    qstmgesturelib.sources = qstmgesturelib.dll
    qstmgesturelib.path = /sys/bin
    DEPLOYMENT += qstmgesturelib
}

contains(DEFINES, ENABLE_PERF_TRACE) {
    brperftrace.sources = brperftrace.dll
    brperftrace.path = /sys/bin
    DEPLOYMENT += brperftrace
}

    chromehtml.sources =    ./chrome/bedrockchrome/*.htm* \
                            ./chrome/bedrockchrome/*.js \
                            ./chrome/bedrockchrome/*.css
    chromehtml.path = ./chrome/bedrockchrome
    BEDROCKCHROME_DEPLOYS += chromehtml

    globaljsthp.sources =   ./chrome/js/3rdparty/*.htm* \
                            ./chrome/js/3rdparty/*.js \
                            ./chrome/js/3rdparty/*.css
    globaljsthp.path = ./chrome/js/3rdparty
    CHROME_DEPLOYS += globaljsthp

    globaljsjui.sources =   ./chrome/js/3rdparty/jquery-ui/*.htm* \
                            ./chrome/js/3rdparty/jquery-ui/*.js \
                            ./chrome/js/3rdparty/jquery-ui/*.css
    globaljsjui.path = ./chrome/js/3rdparty/jquery-ui
    CHROME_DEPLOYS += globaljsjui

    statusbar.sources = ./chrome/bedrockchrome/statusbar.snippet/*.htm* \
                        ./chrome/bedrockchrome/statusbar.snippet/*.js \
                        ./chrome/bedrockchrome/statusbar.snippet/*.css
    statusbar.path = ./chrome/bedrockchrome/statusbar.snippet
    BEDROCKCHROME_DEPLOYS += statusbar

    statusbaricons.sources = ./chrome/bedrockchrome/statusbar.snippet/icons/*.png
    statusbaricons.path = ./chrome/bedrockchrome/statusbar.snippet/icons
    BEDROCKCHROME_DEPLOYS += statusbaricons

    statusbariconsbattery.sources = ./chrome/bedrockchrome/statusbar.snippet/icons/battery/*.png
    statusbariconsbattery.path = ./chrome/bedrockchrome/statusbar.snippet/icons/battery
    BEDROCKCHROME_DEPLOYS += statusbariconsbattery

    statusbariconssignal.sources = ./chrome/bedrockchrome/statusbar.snippet/icons/signal/*.png
    statusbariconssignal.path = ./chrome/bedrockchrome/statusbar.snippet/icons/signal
    BEDROCKCHROME_DEPLOYS += statusbariconssignal

    toolbar.sources =   ./chrome/bedrockchrome/toolbar.snippet/*.htm* \
                        ./chrome/bedrockchrome/toolbar.snippet/*.js \
                        ./chrome/bedrockchrome/toolbar.snippet/*.css
    toolbar.path = ./chrome/bedrockchrome/toolbar.snippet
    BEDROCKCHROME_DEPLOYS += toolbar

    # Toolbar icon deployment not necessary as they are in resources
    #toolbaricons.sources = ./chrome/bedrockchrome/toolbar.snippet/icons/*.png
    #toolbaricons.path = ./chrome/bedrockchrome/toolbar.snippet/icons
    #BEDROCKCHROME_DEPLOYS += toolbaricons

    download.sources =  ./chrome/bedrockchrome/download.snippet/*.htm* \
                        ./chrome/bedrockchrome/download.snippet/*.js \
                        ./chrome/bedrockchrome/download.snippet/*.css
    download.path = ./chrome/bedrockchrome/download.snippet
    BEDROCKCHROME_DEPLOYS += download

    downloadicons.sources = ./chrome/bedrockchrome/download.snippet/icons/*.png
    downloadicons.path = ./chrome/bedrockchrome/download.snippet/icons
    BEDROCKCHROME_DEPLOYS += downloadicons

    contextmenu.sources =   ./chrome/bedrockchrome/contextmenu.snippet/*.htm* \
                            ./chrome/bedrockchrome/contextmenu.snippet/*.js \
                            ./chrome/bedrockchrome/contextmenu.snippet/*.css
    contextmenu.path = ./chrome/bedrockchrome/contextmenu.snippet
    BEDROCKCHROME_DEPLOYS += contextmenu

    contexticons.sources = ./chrome/bedrockchrome/contextmenu.snippet/icons/*.png
    contexticons.path = ./chrome/bedrockchrome/contextmenu.snippet/icons
    BEDROCKCHROME_DEPLOYS += contexticons

    urlsearch.sources = ./chrome/bedrockchrome/urlsearch.snippet/*.htm* \
                        ./chrome/bedrockchrome/urlsearch.snippet/*.js \
                        ./chrome/bedrockchrome/urlsearch.snippet/*.css
    urlsearch.path = ./chrome/bedrockchrome/urlsearch.snippet
    BEDROCKCHROME_DEPLOYS += urlsearch

    urlsearchicons.sources = ./chrome/bedrockchrome/urlsearch.snippet/icons/*.png
    urlsearchicons.path = ./chrome/bedrockchrome/urlsearch.snippet/icons
    BEDROCKCHROME_DEPLOYS += urlsearchicons

    suggests.sources =  ./chrome/bedrockchrome/suggests.snippet/*.htm* \
                        ./chrome/bedrockchrome/suggests.snippet/*.js \
                        ./chrome/bedrockchrome/suggests.snippet/*.css
    suggests.path = ./chrome/bedrockchrome/suggests.snippet
    BEDROCKCHROME_DEPLOYS += suggests

    suggestsicons.sources = ./chrome/bedrockchrome/suggests.snippet/icons/*.png
    suggestsicons.path = ./chrome/bedrockchrome/suggests.snippet/icons
    BEDROCKCHROME_DEPLOYS += suggestsicons

    windowcount.sources =   ./chrome/bedrockchrome/windowcount.snippet/*.htm* \
                            ./chrome/bedrockchrome/windowcount.snippet/*.js \
                            ./chrome/bedrockchrome/windowcount.snippet/*.css
    windowcount.path = ./chrome/bedrockchrome/windowcount.snippet
    BEDROCKCHROME_DEPLOYS += windowcount

    networkstatus.sources = ./chrome/bedrockchrome/networkstatus.snippet/*.htm* \
                            ./chrome/bedrockchrome/networkstatus.snippet/*.js \
                            ./chrome/bedrockchrome/networkstatus.snippet/*.css
    networkstatus.path = ./chrome/bedrockchrome/networkstatus.snippet
    BEDROCKCHROME_DEPLOYS += networkstatus

    networkstatusicons.sources = ./chrome/bedrockchrome/networkstatus.snippet/icons/*.png
    networkstatusicons.path = ./chrome/bedrockchrome/networkstatus.snippet/icons
    BEDROCKCHROME_DEPLOYS += networkstatusicons

    windowcounticons.sources = ./chrome/bedrockchrome/windowcount.snippet/icons/*.png
    windowcounticons.path = ./chrome/bedrockchrome/windowcount.snippet/icons
    BEDROCKCHROME_DEPLOYS += windowcounticons

    zoombar.sources =   ./chrome/bedrockchrome/zoombar.snippet/*.htm* \
                        ./chrome/bedrockchrome/zoombar.snippet/*.js \
                        ./chrome/bedrockchrome/zoombar.snippet/*.css
    zoombar.path = ./chrome/bedrockchrome/zoombar.snippet
    BEDROCKCHROME_DEPLOYS += zoombar

    zoombaricons.sources = ./chrome/bedrockchrome/zoombar.snippet/icons/*.png
    zoombaricons.path = ./chrome/bedrockchrome/zoombar.snippet/icons
    BEDROCKCHROME_DEPLOYS += zoombaricons

    bookmarkview.sources =  ./chrome/bedrockchrome/bookmarkview.superpage/*.htm* \
                            ./chrome/bedrockchrome/bookmarkview.superpage/*.js \
                            ./chrome/bedrockchrome/bookmarkview.superpage/*.css
    bookmarkview.path = ./chrome/bedrockchrome/bookmarkview.superpage
    BEDROCKCHROME_DEPLOYS += bookmarkview

    bookmarkviewicons.sources = ./chrome/bedrockchrome/bookmarkview.superpage/icons/*.png \
                                ./chrome/bedrockchrome/bookmarkview.superpage/icons/*.gif \
                                ./chrome/bedrockchrome/bookmarkview.superpage/icons/*.db
    bookmarkviewicons.path = ./chrome/bedrockchrome/bookmarkview.superpage/icons
    BEDROCKCHROME_DEPLOYS += bookmarkviewicons

    historyview.sources =   ./chrome/bedrockchrome/historyview.superpage/*.htm* \
                            ./chrome/bedrockchrome/historyview.superpage/*.js \
                            ./chrome/bedrockchrome/historyview.superpage/*.css
    historyview.path = ./chrome/bedrockchrome/historyview.superpage
    BEDROCKCHROME_DEPLOYS += historyview

    historyviewicons.sources = ./chrome/bedrockchrome/historyview.superpage/icons/*.png
    historyviewicons.path = ./chrome/bedrockchrome/historyview.superpage/icons
    BEDROCKCHROME_DEPLOYS += historyviewicons

    settingsview.sources =  ./chrome/bedrockchrome/settingsview.superpage/*.htm* \
                            ./chrome/bedrockchrome/settingsview.superpage/*.js \
                            ./chrome/bedrockchrome/settingsview.superpage/*.css
    settingsview.path = ./chrome/bedrockchrome/settingsview.superpage
    BEDROCKCHROME_DEPLOYS += settingsview

    settingsviewicons.sources = ./chrome/bedrockchrome/settingsview.superpage/icons/*.png \
                                ./chrome/bedrockchrome/settingsview.superpage/icons/*.gif \
                                ./chrome/bedrockchrome/settingsview.superpage/icons/*.db
    settingsviewicons.path = ./chrome/bedrockchrome/settingsview.superpage/icons
    BEDROCKCHROME_DEPLOYS += settingsviewicons


     # Deploy common chrome files.
     DEPLOYMENT += $$CHROME_DEPLOYS

     # Deploy bedrock chrome files.
     DEPLOYMENT += $$BEDROCKCHROME_DEPLOYS

    contains(browser_addon, no) {
        HEADERS += emulator/BrowserMainS60.h
        SOURCES += emulator/BrowserMainS60.cpp
}
}

#unix: {
    # Create symbolic link to executable.
#    QMAKE_POST_LINK += ln -sf $$DESTDIR/$$TARGET $$PWD/$$TARGET;
#}


##################################
# Generate documentation
# #################################
dox.target = docs
dox.commands = doxygen ./doc/Doxyfile
dox.depends = $$SOURCES $$HEADERS
QMAKE_EXTRA_UNIX_TARGETS += dox

#INCLUDEPATH += $$PWD/../../mw/bedrockProvisioning
