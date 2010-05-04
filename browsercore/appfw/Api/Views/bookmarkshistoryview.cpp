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


#include <QtCore/QUrl>
#include "BookMarksHistoryView.h"
#include "BookmarksManager.h"
#include "bookmarks.h"
#include "BookmarksView_p.h"

#define COLUMN0_EXPANDED_WIDTH 165
#define COLUMN0_NORMAL_WIDTH 145

namespace WRT {

class HistoryTreeView : public QTreeView 
    {
    public:
    	HistoryTreeView(QWidget *parent = 0);
        bool viewportEvent(QEvent *event);
        void update();
    };
    

class HistoryProxyWidget : public QGraphicsProxyWidget
    {
    public:
        HistoryProxyWidget(QTreeView* wid);
        void resizeEvent ( QGraphicsSceneResizeEvent * event );
    private:
        HistoryTreeView* m_wid ;
    };


class BookmarksHistoryViewPrivate
    {
    public: 
        BookmarksHistoryViewPrivate(QWidget *parent);
        ~BookmarksHistoryViewPrivate();
        QTreeView       *m_treeView;
        HistoryModel   *m_historyModel; //not owned
        QGraphicsProxyWidget* m_proxyWidget;
    
    };

HistoryTreeView::HistoryTreeView(QWidget *parent)
         :QTreeView(parent) 
{
}
bool HistoryTreeView::viewportEvent(QEvent* event)
{
    return QTreeView::viewportEvent(event);

}
void HistoryTreeView::update()
{
    updateGeometry(); 
}

HistoryProxyWidget::HistoryProxyWidget(QTreeView* wid)
{
    m_wid = (HistoryTreeView*)wid;
}
 
void HistoryProxyWidget::resizeEvent ( QGraphicsSceneResizeEvent * event )
{
    QRectF r = geometry();
    QRect r1(r.left(),r.top(),r.width(),r.height());
    if(m_wid != NULL){
         m_wid->setGeometry(r1);
         m_wid->update();
    }
}

BookmarksHistoryViewPrivate::BookmarksHistoryViewPrivate(QWidget *parent)
{
      
      // create the tree view widget
      m_treeView = new HistoryTreeView(parent);
      m_treeView->setHeaderHidden(true);           //To keep the headers for columns hidden
      m_treeView->setAllColumnsShowFocus(true);    //To highlight the row completely when user focus on a row
      m_treeView->setAnimated(true);               //To animate when expands and collapses the folder
      m_proxyWidget = NULL;
      
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

BookmarksHistoryViewPrivate::~BookmarksHistoryViewPrivate()
{
    delete m_treeView;
}

/*!
 * \class BookmarksHistoryView
 *
 * \brief Derived class of BookmarksView. Implements tree view functionality
 *
 * This class is implements a tree view of history items
 */

/*!
 * Basic BookmarksHistoryView constructor requires a BookmarksManager and 
 * a parent QWidget
 * @param  bm_mgr : BookmarksManager Handle
 * @param  parent : handle to parent widget
 * @see BookmarksManager
 */
BookmarksHistoryView::BookmarksHistoryView(WRT::BookmarksManager* bm_mgr, QWidget *parent)
    :BookmarksView(bm_mgr, parent)
    ,dBookmarksHistoryViewPrivate(new BookmarksHistoryViewPrivate(parent))
{
    dBookmarksHistoryViewPrivate->m_historyModel = bookmarksManager()->historyModel(); //getting the history model
    dBookmarksHistoryViewPrivate->m_treeView->hide(); 
    m_jsObject = new BookmarksHistoryViewJSObject(this, 0, type());
}

/*!
 * destructor 
 */
BookmarksHistoryView::~BookmarksHistoryView()
{
    delete dBookmarksHistoryViewPrivate;
}

// SLOTS

/*!
 * opens an item in the tree. If it is a history item it will emit
 * the openUrl signal.
 * @param index : handle to modelIndex
*/
void BookmarksHistoryView::openItem(const QModelIndex &index)
{
    
    BookmarkNode *itemNode  = dBookmarksHistoryViewPrivate->m_historyModel->node(index);
    if (!itemNode)
        return;

    //open url for the history item
    if (itemNode->isBookmark())
    {
        emit openUrl(QUrl(itemNode->url));
    }
}

/*!
 * Handles when the folder is expanded
 * 
 * @param index : handle to modelIndex
*/
void BookmarksHistoryView::itemExpanded(const QModelIndex &index)
{
    BookmarkNode *itemNode  = dBookmarksHistoryViewPrivate->m_historyModel->node(index);
    if (!itemNode)
        return;

    BookmarkNode *parentNode = itemNode->parent();

    if (!parentNode)
            return;

    if (parentNode->parent()){
        dBookmarksHistoryViewPrivate->m_treeView->setColumnWidth(0,COLUMN0_EXPANDED_WIDTH); 
    }
    else{
        dBookmarksHistoryViewPrivate->m_treeView->setColumnWidth(0,COLUMN0_NORMAL_WIDTH);
    }
        
    for (int i = 0; i < parentNode->children().count(); ++i){
        BookmarkNode *childNode = parentNode->children()[i];
        if (childNode->expanded && childNode != itemNode ){
            itemNode->expanded = false;
            dBookmarksHistoryViewPrivate->m_treeView->collapse(dBookmarksHistoryViewPrivate->m_historyModel->index(childNode));
        }
    }
    
    itemNode->expanded = true;
}

/*!
 * Handles when the folder is collapsed
 * 
 * @param index : handle to modelIndex
*/
void BookmarksHistoryView::itemCollapsed(const QModelIndex &index)
{
    
    BookmarkNode *itemNode  = dBookmarksHistoryViewPrivate->m_historyModel->node(index);
    if (!itemNode)
        return;

    //set expanded false for the item which is collapsed
    itemNode->expanded = false;
}

/*! 
  activate the view's resources. Could be connected by client to view visibility
*/
void BookmarksHistoryView::activate()
{
    Q_ASSERT(!d->m_isActive);
    
    bookmarksManager()->updateHistory();
    dBookmarksHistoryViewPrivate->m_historyModel = bookmarksManager()->historyModel();
    dBookmarksHistoryViewPrivate->m_treeView->setModel(dBookmarksHistoryViewPrivate->m_historyModel); //set model for the view
    dBookmarksHistoryViewPrivate->m_treeView->setColumnWidth(0,COLUMN0_NORMAL_WIDTH);  //set the 0th column's width

    //Connect the signals
    connect(dBookmarksHistoryViewPrivate->m_treeView, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(openItem(const QModelIndex &)));
    connect(dBookmarksHistoryViewPrivate->m_treeView, SIGNAL(expanded(const QModelIndex &)),
            this, SLOT(itemExpanded(const QModelIndex &)));
    connect(dBookmarksHistoryViewPrivate->m_treeView, SIGNAL(collapsed(const QModelIndex &)),
            this, SLOT(itemCollapsed(const QModelIndex &)));

    connect(this, SIGNAL(close()), this, SLOT(deactivate()));

    BookmarkNode *historyRootNode = bookmarksManager()->history();
    
    for (int i = 0; i < historyRootNode->children().count(); ++i){
        BookmarkNode *node = historyRootNode->children()[i];
        
        //For folder set the 1st column spanned throughout
        if(node->isFolder()) {
            dBookmarksHistoryViewPrivate->m_treeView->setFirstColumnSpanned(i, QModelIndex(),true);
            //expand "Today" folder when activating the view itself
            
            if(QDateTime::currentDateTime().date().toString() == node->date.toString()) {
                dBookmarksHistoryViewPrivate->m_treeView->expand(dBookmarksHistoryViewPrivate->m_historyModel->index(node));
            }
            
            for (int j = 0; j < node->children().count(); ++j){
                BookmarkNode *subNode = node->children()[j];
            
                if(subNode->isFolder()) {
                    dBookmarksHistoryViewPrivate->m_treeView->setFirstColumnSpanned(j, 
                        dBookmarksHistoryViewPrivate->m_historyModel->index(node),true);
                }
            
            }
        }
     }

    dBookmarksHistoryViewPrivate->m_treeView->show();   //show the view
    d->m_isActive = true;
    
    emit activated();
    
    dBookmarksHistoryViewPrivate->m_treeView->setCurrentIndex ( bookmarksManager()->historyModel()->index(0,0));
    dBookmarksHistoryViewPrivate->m_treeView->setFocus();
}
    
/*!
  deactivate the view's resources. Could be connected by client to view visibility
*/
void BookmarksHistoryView::deactivate()
{
    Q_ASSERT(d->m_isActive);

    // disconnect signals
    disconnect(dBookmarksHistoryViewPrivate->m_treeView, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(openItem(const QModelIndex &)));
    disconnect(dBookmarksHistoryViewPrivate->m_treeView, SIGNAL(expanded(const QModelIndex &)),
            this, SLOT(itemExpanded(const QModelIndex &)));
    disconnect(dBookmarksHistoryViewPrivate->m_treeView, SIGNAL(collapsed(const QModelIndex &)),
            this, SLOT(itemCollided(const QModelIndex &)));

    disconnect(this, SIGNAL(close()), this, SLOT(deactivate()));

    bookmarksManager()->setHistoryView(NULL);
    dBookmarksHistoryViewPrivate->m_treeView->hide();   //hide the view
    d->m_isActive = false;
    
    emit deactivated();
}

QGraphicsWidget* BookmarksHistoryView::widget() const
{
    assert(qWidget());
    if(!dBookmarksHistoryViewPrivate->m_proxyWidget){
        dBookmarksHistoryViewPrivate->m_proxyWidget = new HistoryProxyWidget(dBookmarksHistoryViewPrivate->m_treeView);
        dBookmarksHistoryViewPrivate->m_proxyWidget->setWidget(qWidget());
    }

    return dBookmarksHistoryViewPrivate->m_proxyWidget;
}


/*!
  Return the widget handle of this view
*/
QWidget* BookmarksHistoryView::qWidget() const
{
    return dBookmarksHistoryViewPrivate->m_treeView;
}

/*!
  Return the title of this view for display
  @return Title string
*/
QString BookmarksHistoryView::title()
{
    return tr("Recent Urls");
}

/*! 
  Return the list of public QActions most relevant to the view's current context
  (most approptiate for contextual menus, etc.) (empty for now)
*/
QList<QAction*> BookmarksHistoryView::getContext()
{
    QList<QAction*> contextList;
    return contextList;
}


ControllableView* BookmarksHistoryView::createNew(QWidget *parent)
{
    return new BookmarksHistoryView(BookmarksManager::getSingleton(),parent);
}

BookmarksHistoryViewJSObject::BookmarksHistoryViewJSObject(BookmarksHistoryView* view, QWebFrame* webFrame, const QString& objectName)
  : ControllableViewJSObject(view, webFrame, objectName)
{
    connect(view,SIGNAL(activated()),this,SIGNAL(activated()));
    connect(view,SIGNAL(deactivated()),this,SIGNAL(deactivated()));
    connect(view,SIGNAL(openUrl(const QUrl &)),this,SIGNAL(openUrl()));
}

BookmarksHistoryViewJSObject::~BookmarksHistoryViewJSObject()
{
    disconnect(static_cast<BookmarksHistoryView*>(m_contentView),SIGNAL(activated()),this,SIGNAL(activated()));
    disconnect(static_cast<BookmarksHistoryView*>(m_contentView),SIGNAL(deactivated()),this,SIGNAL(deactivated()));
    disconnect(static_cast<BookmarksHistoryView*>(m_contentView),SIGNAL(openUrl(const QUrl &)),this,SIGNAL(openUrl()));
}

}
