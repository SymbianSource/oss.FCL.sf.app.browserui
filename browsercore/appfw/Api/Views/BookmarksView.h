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


#ifndef _BOOKMARKS_VIEW_H_
#define _BOOKMARKS_VIEW_H_

#include <QWidget>
#include "controllableviewimpl.h"
#include "BWFGlobal.h"
namespace WRT {

    class BookmarksManager;
    class BookmarksViewPrivate;

    class BWF_EXPORT BookmarksView : public ControllableViewBase
    {
        Q_OBJECT

    public:
        BookmarksView(BookmarksManager* bm_mgr, QWidget* parent);        
        ~BookmarksView();
    
        BookmarksManager* bookmarksManager();
        
    signals:
        void openUrl(const QUrl &url);
        void close();
        
    public slots:
        virtual void activate() = 0;
        virtual void deactivate() = 0;

    public: // ControllableView
        virtual QString title() const;
        virtual bool isActive();

    protected:
        BookmarksViewPrivate * const d;

        //BookmarksManager *m_bookmarksManager;  //not owned
    };
    
    class BWF_EXPORT BookmarksViewJSObject : public ControllableViewJSObject {
           Q_OBJECT
         public:
           BookmarksViewJSObject(BookmarksView* view, QWebFrame* webFrame, const QString& objectName);
           ~BookmarksViewJSObject();

       public slots:
      
       signals:
          void done(int);

       protected:
          BookmarksView    * bookMarksView() { return static_cast<BookmarksView *>(m_contentView); }
          BookmarksView    * bookMarksViewConst() const { return static_cast<BookmarksView *>(m_contentView); }
          BookmarksManager * bookMarksManager() {
                     BookmarksView *  view =  static_cast<BookmarksView *>(m_contentView);
                     return view->bookmarksManager();
                 }
                 
       };
}

#endif //_BOOKMARKS_HANDLER_H_
