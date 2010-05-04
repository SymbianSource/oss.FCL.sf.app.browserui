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


#ifndef _BOOKMARKS_TREE_VIEW_H_
#define _BOOKMARKS_TREE_VIEW_H_

#include <QtGui/QTreeView>

#include "BookmarksView.h"
#include "BWFGlobal.h"

namespace WRT {

    class BookmarksTreeViewPrivate;
    class BookmarkNode;
    class BookmarksManager;
    class BookmarksModel;
    
    class BWF_EXPORT BookmarksTreeView : public BookmarksView
    {
        Q_OBJECT
    
    public: 
        BookmarksTreeView(BookmarksManager* bm_mgr, QWidget* parent);            
        ~BookmarksTreeView();
        QAction* getActionBack();
        
    public slots:
        void openItem(const QModelIndex &index);
        void openCurrentItem();

    public slots: //BookmarksView
        void activate();
        void deactivate();
        void entryAdded(BookmarkNode *node);

    signals:
         void activated();
         void deactivated();

    public: //BookmarksView
        QList<QAction*> getContext();
        static const QString Type() { return "bookmarkTreeView"; }
        virtual QString type() const { return Type(); }
        static ControllableView *createNew(QWidget *parent);

    protected:
        QWidget* qWidget() const;
        virtual QGraphicsWidget* widget() const;
    private:
        void expandNodes(const BookmarkNode *node);
    
    private:
        BookmarksTreeViewPrivate * const dBookmarksTreeViewPrivate;
    };
    
class BWF_EXPORT BookmarksTreeViewJSObject : public BookmarksViewJSObject {
      Q_OBJECT
    public:
      BookmarksTreeViewJSObject(BookmarksTreeView* view, QWebFrame* webFrame, const QString& objectName);
       ~BookmarksTreeViewJSObject();

    public slots:
          
    signals:
       void activated();
       void deactivated();
       void openUrl();

    protected:
       BookmarksTreeView    * bookMarksView() { return static_cast<BookmarksTreeView *>(m_contentView); }
       BookmarksTreeView    * bookMarksViewConst() const { return static_cast<BookmarksTreeView *>(m_contentView); }
       BookmarksManager * bookMarksManager() {
       BookmarksTreeView *  view =  static_cast<BookmarksTreeView *>(m_contentView);
                         return view->bookmarksManager();
                     }
                   
  };

}
#endif //_BOOKMARKS_TREE_VIEW_H_
