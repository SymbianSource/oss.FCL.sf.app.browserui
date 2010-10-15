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
INCLUDEPATH += $$PWD/Charms


# Gesture lib
   INCLUDEPATH += $$PWD/../qstmgesturelib
   INCLUDEPATH += $$PWD/../qstmgesturelib/qstmfilelogger
   LIBS += -lqstmgesturelib

contains(DEFINES, ENABLE_PERF_TRACE) {
    message("ginebra2.pro: Tracing is ON")
    LIBS += -lbrperftrace
    INCLUDEPATH += $$PWD/../internal/tests/perfTracing
}


# Common SOURCES, HEADERS from app
# include($$ROOT_DIR/app/common/platform/platform.pri)
# include($$ROOT_DIR/app/common/common.pri)

contains(br_platform_localization, no) {
    TRANSLATIONS += $$PWD/translations/browserloc.ts
}else{
    DEFINES += PLATFORM_LOCALIZATION
    TRANSLATIONS += browserLoc.ts
}

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

RESOURCES = ./qrc/ginebra2.qrc

contains(br_layout, tenone) {
    RESOURCES = ./qrc/ginebraTenone.qrc
    DEFINES += BROWSER_LAYOUT_TENONE
}

contains(br_layout, maemolayout) {
    RESOURCES = ./qrc/ginebraMaemo.qrc
}

HEADERS += \
    Application.h \
    ActionButton.h \
    ActionButtonSnippet.h \
    CachedHandler.h \
    ChromeDOM.h \
    ChromeRenderer.h \
    ChromeSnippet.h \
    LocaleDelegate.h \
    ChromeEffect.h \
    ChromeLayout.h \
    ChromeWidget.h \
    ChromeWidgetJSObject.h \
    ChromeView.h \
    ContentViews/ContentViewContextMenu.h \
    ContentViews/GWebContentViewJSObject.h \
    ContentViews/GWebContentViewWidget.h \
    ContentViews/GWebContentView.h \
    ContentViews/GContentViewTouchNavigation.h \
    ContentViews/SuperPageView.h \
    ContextMenu.h \
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
    GSuperWebPage.h \
    GraphicsItemAnimation.h \
    NativeChromeItem.h \
    SlidingWidget.h \
    Charms/ObjectCharm.h \
    Charms/ExternalEventCharm.h \
    PageSnippet.h \
    PageItem.h \
    ProgressBarItem.h \
    ProgressBarSnippet.h \
    Snippets.h \
    ScriptObjects.h \
    animators/FadeAnimator.h \
    animators/SlideAnimator.h \
    emulator/browser.h \
    ViewController.h \
    ViewStack.h \
    GWebTouchNavigation.h \
    KineticHelper.h \
    TitleItem.h \
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
    TitleUrlContainerSnippet.h \
    UrlSearchSnippet.h \
    Downloads.h \
    GAlternateFileChooser.h \
    EditorWidget.h \
    EditorSnippet.h \
    ScaleThreePainter.h \
    MostVisitedView.h \
    GridView.h \
    ScrollHelper.h \
    CopyCutPasteSnippet.h

CONFIG(maemo){
   HEADERS+= ContentViews/WindowsView.h \
             ContentViews/BrowserWindow.h
}
contains(br_layout, tenone) {
    HEADERS += ScaleNinePainter.h
}

contains(br_layout, maemolayout) {
    HEADERS += mostvisitedsnippetmaemo.h \
    ScaleNinePainter.h
}

!contains(br_layout, maemolayout) {
    HEADERS += linearflowsnippet.h \    
    emulator/ChromeConsole.h \
    mostvisitedsnippet.h
}

contains(br_tiled_backing_store, yes) {
    DEFINES += BEDROCK_TILED_BACKING_STORE
    DEFINES += OWN_BACKING_STORE
}

contains(DEFINES, OWN_BACKING_STORE) {
DEFINES += BEDROCK_TILED_BACKING_STORE

HEADERS += ContentViews/TiledWebView.h
SOURCES += ContentViews/TiledWebView.cpp
}

    HEADERS += WebGestureHelper.h
#WebTouchNavigation.h

contains(DEFINES, BEDROCK_TILED_BACKING_STORE) {
    HEADERS += ContentViews/ScrollableWebContentView.h \
               ContentViews/ViewportMetaData.h \
               ContentViews/ViewportMetaDataParser.h \
               ContentViews/WebContentViewWidget.h \
               ContentViews/WebView.h \
               Gestures/GestureEvent.h \
               Gestures/GestureListener.h \
               Gestures/GestureRecognizer.h \
               Gestures/GestureRecognizer_p.h \
               Kinetics/KineticScrollable.h \
               Kinetics/KineticScroller.h \
               ScrollableViewBase.h                             
}

SOURCES += \
    ActionButton.cpp \
    ActionButtonSnippet.cpp \
    Application.cpp \
    CachedHandler.cpp \
    ChromeDOM.cpp \
    ChromeRenderer.cpp \
    ChromeSnippet.cpp \
    LocaleDelegate.cpp \
    ChromeEffect.cpp \
    ChromeLayout.cpp \
    ChromeWidget.cpp \
    ChromeWidgetJSObject.cpp \
    ChromeView.cpp \
    ContentViews/GWebContentViewJSObject.cpp \
    ContentViews/GWebContentViewWidget.cpp \
    ContentViews/GWebContentView.cpp \
    ContentViews/GContentViewTouchNavigation.cpp \
    ContentViews/SuperPageView.cpp \
    ContentViews/ContentViewContextMenu.cpp \
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
    GSuperWebPage.cpp \
    NativeChromeItem.cpp \
    Snippets.cpp \
    ScriptObjects.cpp \
    SlidingWidget.cpp \
    Charms/ObjectCharm.cpp \
    Charms/ExternalEventCharm.cpp \
    PageSnippet.cpp \
    PageItem.cpp \
    ProgressBarItem.cpp \
    ProgressBarSnippet.cpp \
    animators/FadeAnimator.cpp \
    animators/SlideAnimator.cpp \
    emulator/main.cpp \
    emulator/browser.cpp \
    ViewController.cpp \
    ViewStack.cpp \
    GWebTouchNavigation.cpp \
    KineticHelper.cpp \
    TitleItem.cpp \
    ToolbarChromeItem.cpp \
    ToolbarSnippet.cpp \
    ContentToolbarChromeItem.cpp \
    ContentToolbarSnippet.cpp \
    DualButtonToolbarSnippet.cpp \
    WindowToolbarSnippet.cpp \
    BookmarksToolbarSnippet.cpp \
    SettingsToolbarSnippet.cpp \
    RecentUrlToolbarSnippet.cpp \
    TitleUrlContainerSnippet.cpp \
    UrlSearchSnippet.cpp \
    Downloads.cpp \
    GAlternateFileChooser.cpp \
    EditorWidget.cpp \
    EditorSnippet.cpp \
    ScaleThreePainter.cpp \
    MostVisitedView.cpp \
    GridView.cpp \
    ScrollHelper.cpp \
    CopyCutPasteSnippet.cpp

CONFIG(maemo){
    SOURCES += ContentViews/WindowsView.cpp \
               ContentViews/BrowserWindow.cpp
}

contains(br_layout, maemolayout) {
    SOURCES += mostvisitedsnippetmaemo.cpp \
    ScaleNinePainter.cpp
}

!contains(br_layout, maemolayout) {
    SOURCES += emulator/ChromeConsole.cpp \
    linearflowsnippet.cpp \
    mostvisitedsnippet.cpp
}

contains(br_layout, tenone) {
    SOURCES += ScaleNinePainter.cpp
}

!contains(DEFINES, NO_QSTM_GESTURE) {
    SOURCES += WebGestureHelper.cpp
#    SOURCES += WebTouchNavigation.cpp
}

contains(DEFINES, BEDROCK_TILED_BACKING_STORE) {
    SOURCES += ContentViews/ScrollableWebContentView.cpp \
               ContentViews/ViewportMetaData.cpp \
               ContentViews/ViewportMetaDataParser.cpp \
               ContentViews/WebContentViewWidget.cpp \
               ContentViews/WebView.cpp \
               Gestures/GestureEvent.cpp \
               Gestures/GestureRecognizer.cpp \
               Kinetics/KineticScroller.cpp \
               ScrollableViewBase.cpp
}

!contains(br_layout, maemolayout) {
	FORMS += emulator/ui/console.ui
}

contains(br_mobility_sysinfo, yes) {
    CONFIG += mobility
    MOBILITY += systeminfo
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

contains(br_mobility_serviceframework, yes) {
    DEFINES += QT_MOBILITY_SERVICE_FRAMEWORK
}

# Geolocation asychronous API should be support by Qt 4.7/QWebKit 2.1, but need to qulify for each platform
contains(br_geolocation, yes) {
   DEFINES += QT_GEOLOCATION
}

symbian: {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.EPOCSTACKSIZE = 0x14000
    
    lessThan(QT_VERSION, 4.6.3) {
        TARGET.EPOCHEAPSIZE = 0x20000 \
            0x10000000 \
            // \
            Min \
            128kB, \
            Max \
            256MB
        emulatorHeapSize = \
            "$${LITERAL_HASH}ifdef WINSCW" \
            "EPOCHEAPSIZE 0x20000 0x2000000 // Min 128kB, Max 32MB" \
            "$${LITERAL_HASH}endif"
        MMP_RULES += emulatorHeapSize
    } else { 
        # Set conditional Epoc Heap Size
        EHZ.WINSCW = "EPOCHEAPSIZE 0x20000 0x2000000"
        EHZ.default = "EPOCHEAPSIZE 0x20000 0x10000000"
        # Add the conditional MMP rules
        MYCONDITIONS = WINSCW
        MYVARIABLES = EHZ
        addMMPRules(MYCONDITIONS, MYVARIABLES)
    }
    
#    TARGET.CAPABILITY = All -TCB -DRM -AllFiles
    TARGET.CAPABILITY = All -TCB -DRM

    contains(browser_addon, no) {
        TARGET.UID3 = 0x10008D39
        CONFIG += hb 
        SKINICON = qtg_large_browser 
    }
    else {
        TARGET.UID3 = 0x200267DF
        ICON = ./browserIcon.svg	
    }
    TARGET.VID = VID_DEFAULT
        
    LIBS += -lcommdb
    LIBS += -lesock -lconnmon -linsock
    LIBS += -lavkon -lapparc -leikcore -lcone -lws32 -lapgrfx 
    
# QtHighway is used in TB10.1 for Application Interworking (AIW) support.
contains(br_qthighway, yes) {
    DEFINES += QTHIGHWAY
    LIBS += -lxqservice -lxqserviceutil
    CONFIG += service
    SERVICE.FILE = service_conf.xml
    
    # Browser provides service for html files.
    RSS_RULES += \
        "datatype_list = " \
        "      {" \
        "      DATATYPE" \
        "          {" \
        "          priority = EDataTypePriorityNormal;" \
        "          type = \"text/html\";" \
        "          }" \
        "      };"

        HEADERS += emulator/FileService.h
        SOURCES += emulator/FileService.cpp
}

contains(br_openurl, yes) {
    DEFINES += OPENURL
}

contains(br_fast_allocator, yes) {
    LIBS += -lstandaloneallocator.lib
}

    LIBS += -lhal -lsysutil

    contains(browser_addon, no) {
        HEADERS += emulator/BrowserMainS60.h
        SOURCES += emulator/BrowserMainS60.cpp
}

include(deployment.pri)
packageheader = "$${LITERAL_HASH}{\"NokiaBrowser\"},(0x10008D39),2,0,0,TYPE=SA,RU"
  my_deployment.pkg_prerules =  packageheader
  DEPLOYMENT += my_deployment
DEPLOYMENT.installer_header = "$${LITERAL_HASH}{\"NokiaBrowser\"},(0x10008D39),2,0,0,TYPE=SA,RU"

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
QMAKE_EXTRA_TARGETS += dox

#INCLUDEPATH += $$PWD/../../mw/bedrockProvisioning


symbian:MMP_RULES += SMPSAFE
