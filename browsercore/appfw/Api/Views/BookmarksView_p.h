/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#ifndef _BOOKMARKS_VIEW_P_H_
#define _BOOKMARKS_VIEW_P_H_

#include <QWidget>

namespace WRT {

    class BookmarksManager;

    class BookmarksViewPrivate
    {
    public:
        BookmarksViewPrivate(BookmarksManager* bm_mgr);
        ~BookmarksViewPrivate();

        BookmarksManager *m_bookmarksManager;  //not owned
        bool m_isActive;
    };
}

#endif //_BOOKMARKS_VIEW_P_H_
