/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#ifndef BOOKMARKS_MANAGER_P_H
#define BOOKMARKS_MANAGER_P_H

#include <QtGui/QUndoStack>
#include <browsercontentdll.h>
#include <QAction>

class ActionJSObject;

namespace WRT {
    class BookmarksManager;
    class BookmarkNode;
    class BookmarksManagerPrivate
    {
    public:
        BookmarksManagerPrivate(BookmarksManager * qq);
        ~BookmarksManagerPrivate();

    public: // public actions available for this manager
        
    public:
        BookmarksManager * const q;
        //! flag to indicate was able to connect to book marks
        bool m_connectedToBookmarks;
        //! flag to indicate bookmarks are loaded from data base
        int m_maxUrls;
        BrowserContent* m_bookmarkSession;

#ifdef Q_WS_MAEMO_5        
        //flag to indicate importing bookmarks
        bool m_import;
#endif
        
        QAction * m_actionClearHistory;
        QObject* m_actionsParent; 
        ActionJSObject *m_actionClearJSO;
    };
}
#endif //BOOKMARKS_MANAGER_P_H
