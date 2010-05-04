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




#include "ImageLiteView.h"
#include "ImageView_p.h"
#include "VisualFlowLite.h"

#include <QDebug>

namespace WRT {

/*!
 * \class ImageLiteView
 *
 * \brief presents the page images in the "Lite" style
 *
 * This class is responsible for....
 */

/*!
  Basic ImageLiteView constructor requires a PageManager to manage the pages
  and a parent QWidget
  @param  mgr  : WrtPageManager Handle for this class
  @param  parent : widget parent handle for this class
  @see    WrtPageManager
*/
ImageLiteView::ImageLiteView(WrtPageManager* mgr, QWidget* parent) :
    ImageView(mgr,parent)
{
}

/*!
  Basic ImageLiteView constructor requires a PageManager to manage the pages
  and a parent QGraphicsWidget
  @param  mgr  : WrtPageManager Handle for this class
  @param  parent : Graphics widget parent handle for this class
  @see    WrtPageManager
*/
ImageLiteView::ImageLiteView(WrtPageManager* mgr, QGraphicsWidget* parent) :
    ImageView(mgr,parent)
{
}

/*!
  activate the view's resources. Could be connected by client to view visibility
*/
void ImageLiteView::activate()
{
    d->m_flowInterface = new VisualFlowLite(d->m_widgetParent);
    ImageView::activate();
}

/*!
  set the history view's center index
  @param i : index to which the view's center has to be set
*/
void ImageLiteView::setCenterIndex(int i)
{
    ImageView::setCenterIndex(i);
    VisualFlowLite * vf = static_cast<VisualFlowLite*>(d->m_flowInterface);
    vf->setCenterThumbnail(i);
}

} // namespace WRT
