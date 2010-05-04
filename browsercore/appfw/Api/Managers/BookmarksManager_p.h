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


#ifndef BOOKMARKS_MANAGER_P_H
#define BOOKMARKS_MANAGER_P_H

#include <QtGui/QUndoStack>
#include <browsercontentdll.h>
class QSortFilterProxyModel;

namespace WRT {
    class BookmarksManager;
    class BookmarkNode;
    class BookmarksManagerPrivate
    {
    public:
        BookmarksManagerPrivate(BookmarksManager * qq, QWidget *parent = 0);
        ~BookmarksManagerPrivate();

    public: // public actions available for this manager
        
    public:
        BookmarksManager * const q;
        //! flag to indicate was able to connect to book marks
        bool m_connectedToBookmarks;
        //! flag to indicate bookmarks are loaded from data base
        bool m_loadedBookmarks;
        //! flag to indicate history is loaded from data base
        bool m_loadedHistory;
        //! Root node for bookmark
        BookmarkNode *m_bookmarkRootNode; // owned
        //! Root node for  history
        BookmarkNode *m_historyRootNode; //  owned
        
        //! Root node for  history proxy model
        BookmarkNode *m_historyRootNodeProxy; //  owned
        int m_maxUrls;
        BrowserContent* m_bookmarkSession;
        //flag to indicate importing bookmarks
        bool m_import;

    };
}
#endif //BOOKMARKS_MANAGER_P_H
