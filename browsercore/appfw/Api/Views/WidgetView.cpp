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


#include "../../webkit/WebCore/platform/SharedTimer.h"

#include <QDebug>
#include <QObject>
#include <QMenu>

#include "WidgetView_p.h"
#include "WidgetView.h"
#include "WrtPageManager.h"
#include "wrtbrowsercontainer.h"


namespace WRT {

WidgetViewPrivate::WidgetViewPrivate(WrtPageManager * mgr, QWidget* parent) :
    m_pageManager(mgr),
    m_widgetParent(parent),
    m_graphicsWidgetParent(0),
    m_isActive(false)
{
    // Verify/Create Page Manager
    Q_ASSERT(m_pageManager);
}

WidgetViewPrivate::WidgetViewPrivate(WrtPageManager * mgr, QGraphicsWidget* parent) :
    m_pageManager(mgr),
    m_widgetParent(0),
    m_graphicsWidgetParent(parent),
    m_isActive(false)
{
    // Verify/Create Page Manager
    Q_ASSERT(m_pageManager);
}

WidgetViewPrivate::~WidgetViewPrivate()
{
}

/*!
 * \class WidgetView
 *
 * \brief the public view for web widget application content
 *
 * This class is responsible for....
 */

/*!
  Basic WidgetView constructor requires a PageManager to manage the pages
  and a parent QWidget
  @param mgr : WrtPageManager handle for this class
  @param parent : Widget parent for this class
  @see  WrtPageManager
*/
WidgetView::WidgetView(WrtPageManager * mgr, QWidget* parent) :
    d(new WidgetViewPrivate(mgr,parent))
{
}

/*!
  Basic WidgetView constructor requires a PageManager to manage the pages
  and a parent QGraphicsWidget
  Note: This functionality is not yet tested
  @param mgr : WrtPageManager handle for this class
  @param parent : Graphics Widget parent for this class
  @see  WrtPageManager
 
*/
WidgetView::WidgetView(WrtPageManager * mgr, QGraphicsWidget* parent) :
    d(new WidgetViewPrivate(mgr,parent))
{
}

WidgetView::~WidgetView()
{
    delete d;
}

/*! 
  Retrieve the WrtPageManager associated with this ContentView
  @return  : WrtPageManager handle for this page
*/
WrtPageManager * WidgetView::wrtPageManager()
{
    return d->m_pageManager;
}

/*!
  Show the current web widget
*/
void WidgetView::show()
{
    WrtBrowserContainer * currPage = d->m_pageManager->currentPage();
    
    if (currPage)
    {        
        currPage->webWidget()->show();
        currPage->widgetContainer()->notifyWidget(WRT::WidgetContainer::WrtWidgetOnshow);
        // start shared timers
        currPage->deferTimers(false);
    }
}

/*!
  Hide the current web widget
*/
void WidgetView::hide()
{
    WrtBrowserContainer * currPage = d->m_pageManager->currentPage();
    if (currPage)
    {
        currPage->webWidget()->hide();
        currPage->widgetContainer()->notifyWidget(WRT::WidgetContainer::WrtWidgetOnhide);
        // stop shared timers
        currPage->deferTimers(true);
    }
}

/*!
  Return the web widgets qt widget handle of this view
  @return : Widget handle for this class
*/
QWidget* WidgetView::widget()
{
    WrtBrowserContainer * currPage = d->m_pageManager->currentPage();
    Q_ASSERT(currPage);
    QWidget * webWidget = currPage->webWidget();
    return webWidget;
}

/*!
  Return the title of this view for display
  @return : Title string
*/
QString WidgetView::title()
{
    return d->m_pageManager->title();
}

/*!
  Tests if  this view is active or not
  @return true if view is active else false
*/
bool WidgetView::isActive()
{
    return d->m_isActive;
}

/*! 
  Return the list of public QActions most relevant to the view's current context
  (most approptiate for contextual menus, etc.)
  @return : list of pulicactions for this view.
*/
QList<QAction*> WidgetView::getContext()
{
    // for now, just return all actions, add logic throughout later
    QList<QAction*> contextList;
    return contextList;
}

QMenu* WidgetView::getMenu()
{
    QMenu* menu = NULL;

    WrtBrowserContainer * currPage = d->m_pageManager->currentPage();
    if (currPage)
    {
        WidgetContainer* widgetContainer = currPage->widgetContainer();
        if ( widgetContainer)
            menu = widgetContainer->menu();
    }

    return menu;
}

/*!
  activate the view's resources. Could be connected by client to view visibility
*/
void WidgetView::activate()
{
    Q_ASSERT(!d->m_isActive);

    WrtBrowserContainer * currPage = d->m_pageManager->currentPage();
    Q_ASSERT(currPage);

    QWidget * webWidget = currPage->webWidget();
    webWidget->setParent(d->m_widgetParent);
    webWidget->resize(d->m_widgetParent->size());

    d->m_isActive = true;
}

/*!
  deactivate the view's resources. Could be connected by client to view visibility
*/
void WidgetView::deactivate()
{
    Q_ASSERT(d->m_isActive);

    WrtBrowserContainer * currPage = d->m_pageManager->currentPage();
    if (currPage)
    {
        currPage->webWidget()->hide();
        currPage->widgetContainer()->notifyWidget(WRT::WidgetContainer::WrtWidgetOnhide);
    }

    d->m_isActive = false;
}

} // namespace WRT
