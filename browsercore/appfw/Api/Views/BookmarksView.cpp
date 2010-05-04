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



#include "BookmarksView.h"
#include "BookmarksView_p.h"
#include "BookmarksManager.h"


namespace WRT {

BookmarksViewPrivate::BookmarksViewPrivate(BookmarksManager* bm_mgr) :
    m_bookmarksManager(bm_mgr),
    m_isActive(false)
{
    Q_ASSERT(bm_mgr);
}

BookmarksViewPrivate::~BookmarksViewPrivate() 
{
}


/*!
 * \class BookmarksView
 *
 * \brief Base class for bookmarks views
 *
 * This class is defines the slots and signals used by bookmarks views
 */

/*!
  Basic BookmarksView constructor requires a BookmarksManager and 
  a parent QWidget
  @param bm_mgr : BookmarksManager handle
  @param parent : handle to parent widget
  @see BookmarksManager
*/
BookmarksView::BookmarksView(BookmarksManager* bm_mgr, QWidget* parent) :
    d(new BookmarksViewPrivate(bm_mgr))
{
    Q_UNUSED(parent)
}

/*!
  BookmarksView destructor
*/
BookmarksView::~BookmarksView()
{
    delete d;
}

/*!
  Retrieve the BookmarksManager assigned to this view
  @return BookmarksManager Handle
  @see BookmarksManager
*/
BookmarksManager* BookmarksView::bookmarksManager()
{
    return d->m_bookmarksManager;
}

/*!
  Return the title of this view for display
  @return Title string
*/
QString BookmarksView::title() const
{
    return tr("Bookmarks");
}

/*!
  Return whether the view is active or not
*/
bool BookmarksView::isActive()
{
    return d->m_isActive;
}

BookmarksViewJSObject::BookmarksViewJSObject(BookmarksView* view, QWebFrame* webFrame, const QString& objectName)
  : ControllableViewJSObject(view, webFrame, objectName)
{
}

BookmarksViewJSObject::~BookmarksViewJSObject()
{
}

} // namespace WRT
