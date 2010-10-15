#
# DownloadManager.pro
#


TEMPLATE = lib
TARGET = BrDownloadMgr
DEFINES += BUILD_DOWNLOAD_MGR_LIB
DEFINES+=QT_BEARER=1
DEFINES+=MAEMO_QT_BEARER=0
QT += core network xml

ROOT_DIR = $$PWD/../..

symbian {
    HEADERS += $$PWD/inc/drmstorage.h
    HEADERS += $$PWD/inc/wmdrmdownloadbackend.h
    SOURCES += $$PWD/src/drmstorage.cpp
}
else {
    HEADERS += $$PWD/inc/progressivedownloadserver.h
    SOURCES += $$PWD/src/progressivedownloadserver.cpp
}

INCLUDEPATH += $$PWD/inc $$PWD/../serviceipcclient
LIBS +=  -lbrserviceipcclient 

CONFIG(release, debug|release):!CONFIG(QTDIR_build){
    !unix : contains(QT_CONFIG, reduce_exports): CONFIG += hide_symbols
    unix : contains(QT_CONFIG, reduce_relocations): CONFIG += bsymbolic_functions
}
CONFIG -= warn_on
*-g++* : QMAKE_CXXFLAGS += -Wreturn-type -fno-strict-aliasing

symbian {
    TARGET.UID3 = 0x200267E1
    TARGET.VID = VID_DEFAULT
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = All -Tcb
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE

    LIBS += -lefsrv -lcaf -lcafutils 
    myIncBlock = \
    "$${LITERAL_HASH}include <bldvariant.hrh> " \
    "$${LITERAL_HASH}if (defined RD_WMDRM_DLA_ENABLERS)" \
    "SOURCE    wmdrmdownloadbackend.cpp" \
    "LIBRARY   cameseutility.lib" \
    "LIBRARY   charconv.lib" \
    "$${LITERAL_HASH}endif"

    MMP_RULES += myIncBlock
    MMP_RULES += EXPORTUNFROZEN
}

HEADERS += $$PWD/inc/downloadmanager.h \
                   $$PWD/inc/dmcommon.h \
                   $$PWD/inc/dmcommoninternal.h \
                   $$PWD/inc/download.h \
                   $$PWD/inc/dmpimpl.h \
                   $$PWD/inc/downloadbackend.h \
                   $$PWD/inc/httpdownloadbackend.h \
                   $$PWD/inc/omadownloadbackend.h \
                   $$PWD/inc/downloadcore.h \
                   $$PWD/inc/downloadcoremanager.h \
                   $$PWD/inc/downloadevent.h \
                   $$PWD/inc/downloadfactory.h \ 
                   $$PWD/inc/downloadinfo.h \
                   $$PWD/inc/omaddparser.h \
                   $$PWD/inc/downloadstore.h \ 
                   $$PWD/inc/filestorage.h \
                   $$PWD/inc/oma2downloadbackend.h \
                   $$PWD/inc/clientdownload.h \
                   $$PWD/inc/paralleldownloadmanager.h \
                   $$PWD/inc/sequentialdownloadmanager.h \ 
                   $$PWD/inc/downloadmanagerclient.h \
                   $$PWD/inc/backgrounddownloadmanager.h \
                   $$PWD/inc/backgrounddownload.h \
                   $$PWD/inc/storageutility.h

SOURCES += $$PWD/src/downloadmanager.cpp \
                   $$PWD/src/downloadcore.cpp \
                   $$PWD/src/downloadbackend.cpp \
                   $$PWD/src/httpdownloadbackend.cpp \
                   $$PWD/src/omadownloadbackend.cpp \
                   $$PWD/src/downloadcoremanager.cpp \
                   $$PWD/src/downloadevent.cpp \
                   $$PWD/src/downloadfactory.cpp \ 
                   $$PWD/src/downloadinfo.cpp \ 
                   $$PWD/src/omaddparser.cpp \ 
                   $$PWD/src/filestorage.cpp \ 
                   $$PWD/src/oma2downloadbackend.cpp \ 
                   $$PWD/src/clientdownload.cpp \ 
                   $$PWD/src/paralleldownloadmanager.cpp \ 
                   $$PWD/src/sequentialdownloadmanager.cpp \ 
                   $$PWD/src/downloadmanagerclient.cpp \ 
                   $$PWD/src/backgrounddownloadmanager.cpp \ 
                   $$PWD/src/backgrounddownload.cpp \ 
                   $$PWD/src/storageutility.cpp

