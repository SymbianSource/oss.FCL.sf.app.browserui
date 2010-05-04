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


#ifndef _BOOKMARKS_TREE_VIEW_P_H_
#define _BOOKMARKS_TREE_VIEW_P_H_

#include <QtGui/QTreeView>
#include <QAction>

namespace WRT {
    class BookmarksTreeViewPrivate;
    class BookmarkNode;
    class BookmarksManager;
    class BookmarksModel;

    class BookmarksTreeViewPrivate
    {
    public: 
        BookmarksTreeViewPrivate(QWidget *parent);
        ~BookmarksTreeViewPrivate();
        QTreeView *m_treeView;
        QWidget *m_topLevelWidget;
        BookmarksModel *m_bookmarksModel; //not owned
        QAction * m_actionBack;
        QGraphicsProxyWidget* m_proxyWidget;
    };
}
#endif //_BOOKMARKS_TREE_VIEW_P_H_
