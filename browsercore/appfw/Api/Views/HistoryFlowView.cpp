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
#include "HistoryView_p.h"
#include "HistoryFlowView.h"
#include "PictureFlow.h"


#include "qwebhistory.h"
#include "qwebframe.h"
#include "wrtbrowsercontainer.h"
#include "webpagedata.h"

namespace WRT {

/*!
 * \class HistoryFlowView
 *
 * \brief presents the history of the current page in the "Flow" style
 *
 * This class implements the HistoryView base class
 */

/*!
  Basic HistoryFlowView constructor requires a PageManager to manage the pages
  and a parent QWidget
  @param mgr : WrtPageManager handle for this class
  @param parent : Widget parent for this class
  @see  WrtPageManager
*/
HistoryFlowView::HistoryFlowView(WrtPageManager* mgr, QWidget* parent) :
    HistoryView(mgr,parent)
{
    m_jsObject = new HistoryViewJSObject(this, 0, type());
}

/*!
  Basic HistoryFlowView constructor requires a PageManager to manage the pages
  and a parent QGraphicsWidget
  Note: This functionality is not yet tested
  @param mgr : WrtPageManager handle for this class
  @param parent : Graphics Widget parent for this class
  @see  WrtPageManager
  
*/
HistoryFlowView::HistoryFlowView(WrtPageManager* mgr, QGraphicsWidget* parent) :
    HistoryView(mgr,parent)
{
}

/*!
  activate the view's resources. Could be connected by client to view visibility
*/
void HistoryFlowView::activate()
{
    d->m_flowInterface = new GraphicsPictureFlow(d->m_widgetParent);
    HistoryView::activate();
}

/*! 
  set the history view's center index
  @param i   : history index to be set
*/
void HistoryFlowView::setCenterIndex(int i)
{    
    HistoryView::setCenterIndex(i);
    GraphicsPictureFlow * pf = static_cast<GraphicsPictureFlow*>(d->m_flowInterface);
    pf->showSlide(i);
}

/*!
  get the history view's current slide image
  @return QImage
*/

QImage HistoryFlowView::getCurrentSlide()
{
    GraphicsPictureFlow * pf = static_cast<GraphicsPictureFlow*>(d->m_flowInterface);
    qDebug()<<pf->centerIndex();
    return pf->slide(pf->centerIndex());
}

ControllableView* HistoryFlowView::createNew(QWidget *parent)
{
    return new HistoryFlowView(WrtPageManager::getSingleton(),parent);
}

} // namespace WRT
