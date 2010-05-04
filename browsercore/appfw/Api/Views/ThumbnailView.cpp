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



#include "WrtPageManager.h"
#include "ThumbnailView_p.h"
#include "ThumbnailView.h"
#include "TnEngineHandler.h"
#include "TnEngineView.h"
#include "wrtbrowsercontainer.h"

#include <QDebug>

namespace WRT {
ThumbnailViewPrivate::ThumbnailViewPrivate(WrtPageManager * mgr, QWidget* parent) :
    m_widgetParent(parent),
    m_graphicsWidgetParent(0),
    m_pageManager(mgr),
    m_activePage(0),
    m_tnEngineHandler(0),
    m_tnEngineView(0),
    m_isActive(false)
{
    Q_ASSERT(m_pageManager);
    init();
}

ThumbnailViewPrivate::ThumbnailViewPrivate(WrtPageManager * mgr, QGraphicsWidget* parent) :
    m_widgetParent(0),
    m_graphicsWidgetParent(parent),
    m_pageManager(mgr),
    m_activePage(0),
    m_tnEngineHandler(0),
    m_tnEngineView(0),
    m_isActive(false)
{
    Q_ASSERT(m_pageManager);    
    init();
}

ThumbnailViewPrivate::~ThumbnailViewPrivate()
{
}

void ThumbnailViewPrivate::init()
{
    // create the view's actions
    m_actionOK = new QAction("OK",m_widgetParent);
    m_actionOK->setObjectName("OK");
    m_actionCancel = new QAction("Cancel",m_widgetParent);
    m_actionCancel->setObjectName("Cancel");    
}

/*!
 * \class ThumbnailView
 *
 * \brief The Thumbnail View
 *
 * This class provides an overview of the current page, allows the user to scroll to 
 * a different part of the page
 */

/*!
  Basic ThumbnailView constructor requires a PageManager to manage the pages
  and a parent QWidget
*/
ThumbnailView::ThumbnailView(WrtPageManager * pageMgr, QWidget* parent) :
    d(new ThumbnailViewPrivate(pageMgr, parent))
{
}

/*!
  Basic ThumbnailView constructor requires a PageManager to manage the pages
  and a parent QGraphicsWidget
*/
ThumbnailView::ThumbnailView(WrtPageManager * pageMgr, QGraphicsWidget* parent) :
    d(new ThumbnailViewPrivate(pageMgr, parent))
{    
}


ThumbnailView::~ThumbnailView()
{
    if(d->m_isActive)
        deactivate();

    if(d->m_tnEngineHandler)
        delete(d->m_tnEngineHandler);

    if(d->m_tnEngineView)
        delete(d->m_tnEngineView);

    delete d;
}

/*!
  Retrieve the WrtPageManager assigned to this view
*/
WrtPageManager* ThumbnailView::wrtPageManager()
{
    return d->m_pageManager;
}

/*!
  Return the view's OK QAction
  For invoking the view's OK
*/
QAction * ThumbnailView::getActionOK()
{
    return d->m_actionOK;
}

/*!
  Return the view's Cancel QAction
  For invoking the view's Cancel
*/
QAction * ThumbnailView::getActionCancel()
{
    return d->m_actionCancel;
}

/*!
  Return the widget handle of this view
*/
QWidget* ThumbnailView::qWidget() const
{
    return d->m_tnEngineHandler->widget();
}

/*!
  Return the title of this view for display
*/
QString ThumbnailView::title()
{
    return QString("ThumbnailView");
}

/*!
  Return whether this view is active or not
*/
bool ThumbnailView::isActive()
{
    return d->m_isActive;
}


/*! 
  Return the list of public QActions most relevant to the view's current context
  (most approptiate for contextual menus, etc.
*/
QList<QAction*> ThumbnailView::getContext()
{
    // for now, all actions valid at all times
    // but there may be some logic here to determine context
    QList<QAction*> contextList;
    contextList <<
        d->m_actionOK <<
        d->m_actionCancel;
    return contextList;
}

/*!
  activate the view's resources. Could be connected by client to view visibility
*/
void ThumbnailView::activate()
{
    Q_ASSERT(!d->m_isActive);

    d->m_tnEngineHandler = new TnEngineHandler(d->m_pageManager, d->m_widgetParent);
    Q_ASSERT(d->m_tnEngineHandler);
    d->m_tnEngineView = d->m_tnEngineHandler->tnEngineView();
    Q_ASSERT(d->m_tnEngineView);

    // connect/forward signals as appropriate
    connect(d->m_tnEngineView,SIGNAL(scrollStarted()),this,SIGNAL(scrollStarted()));
    connect(d->m_tnEngineView,SIGNAL(scrollBy(int,int)),this,SIGNAL(scrollBy(int,int)));
    connect(d->m_tnEngineView,SIGNAL(scrollEnded()),this,SIGNAL(scrollEnded()));
    connect(d->m_tnEngineHandler,SIGNAL(ok(int,int)),this,SIGNAL(ok(int,int)));
    connect(d->m_tnEngineView,SIGNAL(cancel()),this,SIGNAL(cancel()));

    //d->m_tnEngineHandler->show();
    d->m_isActive = true;

}
    
/*! 
  deactivate the view's resources. Could be connected by client to view visibility
*/
void ThumbnailView::deactivate()
{
    Q_ASSERT(d->m_isActive);

    Q_ASSERT(d->m_tnEngineHandler);
    Q_ASSERT(d->m_tnEngineView);

    // connect/forward signals as appropriate
    disconnect(d->m_tnEngineView,SIGNAL(scrollStarted()),this,SIGNAL(scrollStarted()));
    disconnect(d->m_tnEngineView,SIGNAL(scrollBy(int,int)),this,SIGNAL(scrollBy(int,int)));
    disconnect(d->m_tnEngineView,SIGNAL(scrollEnded()),this,SIGNAL(scrollEnded()));
    disconnect(d->m_tnEngineHandler,SIGNAL(ok(int,int)),this,SIGNAL(ok(int,int)));
    disconnect(d->m_tnEngineView,SIGNAL(cancel()),this,SIGNAL(cancel()));

    m_proxyWidget->setWidget(0);
    delete m_proxyWidget;
    m_proxyWidget = 0;
    delete(d->m_tnEngineHandler);
    d->m_tnEngineHandler = NULL;
    d->m_tnEngineView = NULL;

    d->m_isActive = false;
}

/*!
  scroll the ThumbnailView by the amount given the doc coordinates
*/
void ThumbnailView::scrollViewBy(int /*x*/, int /*y*/)
{
    Q_ASSERT(d->m_tnEngineHandler);
    Q_ASSERT(d->m_tnEngineView);
    d->m_tnEngineHandler->updateNow();
    d->m_tnEngineView->update();
}
    
/*!
  \fn void ThumbnailView::scrollStarted();
  emitted when scrolling on the ThumbnailView has started
*/

/*!
  \fn void ThumbnailView::scrollTo(int x, int y);
  emitted during a scrolling event
*/  

/*!  
  \fn void ThumbnailView::scrollEnded();
  emitted when scrolling has ended();
*/   
 
/*!
  \fn void ThumbnailView::ok(int x, int y);
  emitted when the ok action has occured, and the position the 
  view has currently scrolled to
*/

/*!
  \fn void ThumbnailView::cancel();
  emitted when the cancel action has occured
*/

} // namespace WRT
