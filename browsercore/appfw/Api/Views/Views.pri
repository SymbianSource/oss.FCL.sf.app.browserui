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

HEADERS += \
    $$PWD/webcontentview.h \
    $$PWD/BookmarksView_p.h \
    $$PWD/BookmarksView.h \
    $$PWD/controllableview.h \
    $$PWD/controllableviewimpl.h \
    $$PWD/controllableviewjsobject.h \
    $$PWD/HistoryView_p.h \
    $$PWD/HistoryView.h \
    $$PWD/HistoryFlowView.h \   
    $$PWD/WindowFlowView.h \
    $$PWD/WindowView.h \
    $$PWD/WindowView_p.h \
    $$PWD/mostvisitedpagestore.h 

SOURCES += \
    $$PWD/controllableviewimpl.cpp \
    $$PWD/controllableviewjsobject.cpp \
    $$PWD/webcontentview.cpp \
    $$PWD/BookmarksView.cpp \
    $$PWD/HistoryView.cpp \
    $$PWD/HistoryFlowView.cpp \
    $$PWD/WindowFlowView.cpp \
    $$PWD/WindowView.cpp \
    $$PWD/mostvisitedpagestore.cpp 

RESOURCES += \
    $$PWD/views.qrc
