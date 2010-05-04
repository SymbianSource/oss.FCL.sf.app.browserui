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



#include <QDebug>

#include "WrtPageManager.h"
#include "WindowView_p.h"
#include "WindowFlowView.h"
#include "FilmstripFlow.h"

#include "qwebhistory.h"
#include "qwebframe.h"
#include "wrtbrowsercontainer.h"
#include "webpagedata.h"

namespace WRT {

ControllableView* WindowFlowView::createNew(QWidget* parent) {

    return new WindowFlowView( WRT::WrtPageManager::getSingleton(), parent, Type()); 

}

ControllableViewBase* WindowFlowView::createNew(QGraphicsWidget* parent) {

    return new WindowFlowView( WRT::WrtPageManager::getSingleton(), parent, Type());

}


/*!
 * \class WindowFlowView
 *
 * \brief presents the history of the current page in the "Flow" style
 *
 * This class implements the WindowView base class
 */

/*!
  Basic WindowFlowView constructor requires a PageManager to manage the pages
  and a parent QWidget

  @param mgr : WrtPageManager handle for this class
  @param parent : Widget parent for this class
*/
WindowFlowView::WindowFlowView(WrtPageManager* mgr, QWidget* parent,const QString& aType) :
    WindowView(mgr,parent)
,   m_type(aType)
{
    m_jsObject = new WindowViewJSObject(this, 0, type());
}

/*!
  Basic WindowFlowView constructor requires a PageManager to manage the pages
  and a parent QGraphicsWidget
  Note: This functionality is not yet tested
  @param mgr : WrtPageManager handle for this class
  @param parent : Graphics Widget parent for this class
  @see  WrtPageManager
*/
WindowFlowView::WindowFlowView(WrtPageManager* mgr, QGraphicsWidget* parent,const QString& aType) :
    WindowView(mgr,parent)
,   m_type(aType)
{
    m_jsObject = new WindowViewJSObject(this, 0, type());
}

/*!
  activate the view's resources. Could be connected by client to view visibility
*/
void WindowFlowView::activate()
{
    if (!d->m_flowInterface ) {
        d->m_flowInterface = new GraphicsFilmstripFlow(d->m_widgetParent);
        emit instantiated(this);
    }
    WindowView::activate();
}


} // namespace WRT
