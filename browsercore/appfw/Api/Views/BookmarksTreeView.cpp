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


#include <QtGui/QHeaderView>
#include <QtCore/QUrl>
#include "BookmarksView_p.h"
#include "BookmarksTreeView_p.h"
#include "BookmarksTreeView.h"
#include "BookmarksManager.h"
#include "bookmarks.h"

namespace WRT {

class BookMarkTreeView : public QTreeView 
    {
    public:
        BookMarkTreeView(QWidget *parent = 0) ;
        bool viewportEvent(QEvent *event);
        void update();

    };
 
class BookmarkProxyWidget : public QGraphicsProxyWidget
    {
    public:
         BookmarkProxyWidget(QTreeView* wid);
         void resizeEvent ( QGraphicsSceneResizeEvent * event );
    private:
        BookMarkTreeView* m_wid ;
    };


BookMarkTreeView::BookMarkTreeView(QWidget *parent)
	     :QTreeView(parent) 
{
}
	
bool BookMarkTreeView::viewportEvent(QEvent* event)
{
    return QTreeView::viewportEvent(event);
}

void BookMarkTreeView::update()
{
    updateGeometry(); 
}

BookmarkProxyWidget::BookmarkProxyWidget(QTreeView* wid)
{
    m_wid = (BookMarkTreeView*)wid;
}

void BookmarkProxyWidget::resizeEvent ( QGraphicsSceneResizeEvent * event )
{
    QRectF r = geometry();
    QRect r1(r.left(),r.top(),r.width(),r.height());
    if(m_wid != NULL){
        m_wid->setGeometry(r1);
        m_wid->update();
    }
}

BookmarksTreeViewPrivate::BookmarksTreeViewPrivate(QWidget *parent)
{
    m_proxyWidget = NULL;
    // create the tree view widget
    m_treeView = new BookMarkTreeView(parent);
    
    m_actionBack = new QAction("Back", parent);
    
    m_treeView->setUniformRowHeights(true);
    m_treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_treeView->setHeaderHidden(true);
    m_treeView->setAnimated(true);
    
	//setting the style sheet for the tree view
#ifndef QT_NO_STYLE_STYLESHEET
    m_treeView->setStyleSheet( " QTreeView { \
                                      background-color : white \
                                      } \
                                      QTreeView::branch { \
                                      border-image: none; image: none \
                                      } \
                                      QTreeView::item { \
                                      height: 28px; \
                                      } \
                                      QTreeView::branch:closed:has-children:has-siblings, \
                                      QTreeView::branch:has-children:!has-siblings:closed { \
                                      border-image: none;\
                                      image: url(:/icons/collapsed.png); \
                                      } \
                                      QTreeView::branch:open:has-children:has-siblings, \
                                      QTreeView::branch:open:has-children:!has-siblings { \
                                      border-image: none;\
                                      image: url(:/icons/expanded.png);\
                                      } \
                                   ");

#endif
}


BookmarksTreeViewPrivate::~BookmarksTreeViewPrivate()
{
    delete(m_treeView);
}


/*!
 * \class BookmarksTreeView
 *
 * \brief Derived class of BookmarksView. Implements tree view functionality
 *
 * This class is implements a tree view of bookmarks
 */

/*!
 * Basic BookmarksTreeView constructor requires a BookmarksManager and 
 * a parent QWidget
 * @param  bm_mgr : BookmarksManager Handle
 * @param  parent : handle to parent widget
 * @see BookmarksManager
 */
BookmarksTreeView::BookmarksTreeView(BookmarksManager* bm_mgr, QWidget *parent) :
    BookmarksView(bm_mgr, parent),
    dBookmarksTreeViewPrivate(new BookmarksTreeViewPrivate(parent))
{
    dBookmarksTreeViewPrivate->m_bookmarksModel = bookmarksManager()->bookmarksModel();
    dBookmarksTreeViewPrivate->m_treeView->hide();
    m_jsObject = new BookmarksTreeViewJSObject(this, 0, type());
}


/*!
 * destructor 
 */
BookmarksTreeView::~BookmarksTreeView()
{
    delete dBookmarksTreeViewPrivate;
}


/*!
  Return the view's Back QAction
  @return Action*
*/
QAction * BookmarksTreeView::getActionBack()
{
    return dBookmarksTreeViewPrivate->m_actionBack;
}

// SLOTS

/*!
  opens an item in the tree. If is is a bookmark it will emit
  the \sa openUrl signal, if it is a folder, it will expand the folder
  @param index : handle to modelIndex
*/
void BookmarksTreeView::openItem(const QModelIndex &index)
{
    const BookmarkNode *node = dBookmarksTreeViewPrivate->m_bookmarksModel->node(index);
       
    if (!node)
        {
        return;
        }

    if (node->isBookmark())
        {
        emit openUrl(QUrl(node->url));
        }
    else if (node->isFolder())
        {
        expandNodes(node);
        }
}

/*!
  opens whatever the current selected item is in the tree.
*/
void BookmarksTreeView::openCurrentItem()
{
    openItem(dBookmarksTreeViewPrivate->m_treeView->currentIndex());
}

/*! 
  activate the view's resources. Could be connected by client to view visibility
*/
void BookmarksTreeView::activate()
{
    Q_ASSERT(!d->m_isActive);
    
    dBookmarksTreeViewPrivate->m_treeView->setModel(bookmarksManager()->bookmarksModel());

    dBookmarksTreeViewPrivate->m_treeView->setExpanded(dBookmarksTreeViewPrivate->m_bookmarksModel->index(0, 0), true);

    expandNodes(bookmarksManager()->bookmarks());

    bookmarksManager()->setBookMarkView(dBookmarksTreeViewPrivate->m_treeView);
    
    connect(dBookmarksTreeViewPrivate->m_treeView, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(openItem(const QModelIndex &)));

    connect(bookmarksManager(), SIGNAL(bookMarkEntryAdded(BookmarkNode *)),
                this, SLOT(entryAdded(BookmarkNode *)));

    // connect the BookmarksTreeViews signals
    connect(this, SIGNAL(close()), this, SLOT(deactivate()));

    connect(dBookmarksTreeViewPrivate->m_treeView, SIGNAL(clicked(const QModelIndex &)),
            bookmarksManager(), SLOT(updateActions()));

    dBookmarksTreeViewPrivate->m_treeView->show();

    BookmarkNode *node = bookmarksManager()->bookMarksRoot();
    QModelIndex sourceIndex = bookmarksManager()->bookmarksModel()->index(node);
    
    //Set the root for treeview
    dBookmarksTreeViewPrivate->m_treeView->setRootIndex(sourceIndex);


    d->m_isActive = true;
    
    emit activated();
    
    dBookmarksTreeViewPrivate->m_treeView->setCurrentIndex ( bookmarksManager()->bookmarksModel()->index(0,0));
    dBookmarksTreeViewPrivate->m_treeView->setFocus();        
}
    
/*!
  deactivate the view's resources. Could be connected by client to view visibility
*/
void BookmarksTreeView::deactivate()
{
    Q_ASSERT(d->m_isActive);

    // disconnect signals
    disconnect(dBookmarksTreeViewPrivate->m_treeView, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(openItem(const QModelIndex &)));

    disconnect(dBookmarksTreeViewPrivate->m_treeView, SIGNAL(clicked(const QModelIndex &)),
               bookmarksManager(), SLOT(updateActions()));

    disconnect(this, SIGNAL(close()), this, SLOT(deactivate()));

    bookmarksManager()->setBookMarkView(NULL);

    dBookmarksTreeViewPrivate->m_treeView->hide();


    d->m_isActive = false;
    
    emit deactivated();
}

/*!
  Return the widget handle of this view
*/
QWidget* BookmarksTreeView::qWidget() const
{
    return dBookmarksTreeViewPrivate->m_treeView;
}

/*! 
  Return the list of public QActions most relevant to the view's current context
  (most approptiate for contextual menus, etc.) (empty for now)
*/
QList<QAction*> BookmarksTreeView::getContext()
{
    QList<QAction*> contextList;
    return contextList;
}

void BookmarksTreeView::expandNodes(const BookmarkNode *node)
{
    for (int i = 0; i < node->children().count(); ++i) 
        {
        BookmarkNode *childNode = node->children()[i];
        if (childNode->expanded) 
            {
            QModelIndex idx = dBookmarksTreeViewPrivate->m_bookmarksModel->index(childNode);
            dBookmarksTreeViewPrivate->m_treeView->setExpanded(idx, true);
            expandNodes(childNode);
            }
        }
}


ControllableView* BookmarksTreeView::createNew(QWidget *parent)
{
    return new BookmarksTreeView(BookmarksManager::getSingleton(),parent);
}


void BookmarksTreeView::entryAdded(BookmarkNode *node)
 {
    QModelIndex index = bookmarksManager()->bookmarksModel()->index(node);
    dBookmarksTreeViewPrivate->m_treeView->setCurrentIndex ( index );
    
 }

 QGraphicsWidget* BookmarksTreeView::widget() const
{
    assert(qWidget());
    if(!dBookmarksTreeViewPrivate->m_proxyWidget)
    {
        dBookmarksTreeViewPrivate->m_proxyWidget = new BookmarkProxyWidget(dBookmarksTreeViewPrivate->m_treeView);
        dBookmarksTreeViewPrivate->m_proxyWidget->setWidget(qWidget());
    }

    return dBookmarksTreeViewPrivate->m_proxyWidget;
}

BookmarksTreeViewJSObject::BookmarksTreeViewJSObject(BookmarksTreeView* view, QWebFrame* webFrame, const QString& objectName)
  : BookmarksViewJSObject(view, webFrame, objectName)
{
    connect(view,SIGNAL(activated()),this,SIGNAL(activated()));
    connect(view,SIGNAL(deactivated()),this,SIGNAL(deactivated()));
    connect(view,SIGNAL(openUrl(const QUrl &)),this,SIGNAL(openUrl()));
}

BookmarksTreeViewJSObject::~BookmarksTreeViewJSObject()
{
    BookmarksTreeView *  view =  static_cast<BookmarksTreeView *>(m_contentView);
    disconnect(view,SIGNAL(activated()),this,SIGNAL(activated()));
    disconnect(view,SIGNAL(deactivated()),this,SIGNAL(deactivated()));
    disconnect(view,SIGNAL(openUrl(const QUrl &)),this,SIGNAL(openUrl()));
}


} // namespace WRT





