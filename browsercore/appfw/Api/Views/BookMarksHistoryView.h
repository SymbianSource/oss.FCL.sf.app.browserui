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


#ifndef _BOOKMARKS_HISTORY_VIEW_H_
#define _BOOKMARKS_HISTORY_VIEW_H_

#include <QtGui/QTreeView>
#include <QtGui/QWidget>

#include "BookmarksView.h"


namespace WRT {

    class BookmarksHistoryViewPrivate;
    class BookmarkNode;
    class BookmarksManager;
        
class BWF_EXPORT  BookmarksHistoryView : public BookmarksView
    {
        Q_OBJECT
    public: 
        BookmarksHistoryView(BookmarksManager* bm_mgr, QWidget* parent);            
        ~BookmarksHistoryView();
        QString title();
        static ControllableView *createNew(QWidget *parent);
   
    signals:
            void activated();
            void deactivated();
            
    public slots:
        void openItem(const QModelIndex &index);
        void itemExpanded(const QModelIndex &index);
        void itemCollapsed(const QModelIndex &index);

    public slots: //BookmarksView
        void activate();
        void deactivate();

    public: //BookmarksView
        QList<QAction*> getContext();
        static const QString Type() { return "bookmarkHistoryView"; }
        virtual QString type() const { return Type(); }

    protected:
        QWidget* qWidget() const;
        virtual QGraphicsWidget* widget() const;
    private:
        BookmarksHistoryViewPrivate * const dBookmarksHistoryViewPrivate;
        
    };



class BWF_EXPORT BookmarksHistoryViewJSObject : public ControllableViewJSObject {
           Q_OBJECT
 public: 
        BookmarksHistoryViewJSObject(BookmarksHistoryView* view, QWebFrame* webFrame, const QString& objectName);
       ~BookmarksHistoryViewJSObject();

      
       public slots:
       
       signals:
          void done(int);
          void activated();
          void deactivated();
          void openUrl();

       protected:
       BookmarksHistoryView * bookMarksHistoryView() { return static_cast<BookmarksHistoryView *>(m_contentView); }
       BookmarksHistoryView * bookMarksHistoryViewConst() const { return static_cast<BookmarksHistoryView *>(m_contentView); }
                    
       };
}
#endif //_BOOKMARKS_HISTORY_VIEW_H_
