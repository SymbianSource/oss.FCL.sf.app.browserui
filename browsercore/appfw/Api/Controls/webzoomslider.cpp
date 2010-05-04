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


#include "webzoomslider_p.h"
#include "webzoomslider.h"
#include "wrtbrowsercontainer.h"
#include "WrtPageManager.h"

#define SINGLE_STEP 5

namespace WRT {

WebZoomSliderPrivate::WebZoomSliderPrivate(WrtPageManager* pageMgr, QWidget* parent) :
    m_pageMgr(pageMgr),
    m_slider(0),
    m_timer(0),
    m_widgetParent(parent),
    m_idleTimeout(0),
    m_dragging(false)
{
    Q_ASSERT(m_pageMgr);

    m_slider = new QSlider(m_widgetParent);
    m_slider->setTickPosition(QSlider::NoTicks);
    m_slider->setTracking(true);
    m_slider->setRange(WEBPAGE_ZOOM_RANGE_MIN, WEBPAGE_ZOOM_RANGE_MAX);
    m_slider->setSingleStep(SINGLE_STEP);
    m_slider->setPageStep(WEBPAGE_ZOOM_PAGE_STEP);    
}

WebZoomSliderPrivate::~WebZoomSliderPrivate()
{
}   

/*!
 * \class WebZoomSlider
 *
 * \brief A control for zooming in and out the page
 */

/*!
 * Constructor depends on page manager to select current page
 * @param pageMgr : handle to the pageMgr for this class
 * @param parent  : widget parent handle for this class
 */
WebZoomSlider::WebZoomSlider(WrtPageManager* pageMgr, QWidget* parent) : 
    QObject(parent),
    d(new WebZoomSliderPrivate(pageMgr, parent))
{
    WrtBrowserContainer *page = d->m_pageMgr->currentPage();
    int val = (page) ? page->pageZoomFactor() * 100 : 100;
    if (val < WEBPAGE_ZOOM_RANGE_MIN && val > WEBPAGE_ZOOM_RANGE_MAX)
        val = 100;
    d->m_slider->setValue(val);

    connect(d->m_slider, SIGNAL(valueChanged(int)), this, SLOT(zoom(int)));
    connect(d->m_slider, SIGNAL(sliderPressed()), this, SLOT(sliderPressed()));
    connect(d->m_slider, SIGNAL(sliderReleased()), this, SLOT(sliderReleased()));
    setGeometry(d->m_widgetParent->geometry());
}

/*!
 * Basic destructor
 */
WebZoomSlider::~WebZoomSlider()
{
    delete d;
}

/*!
  Sets the slider 3/4 of the way to the right and centers vertically
  so slider is 5% below the top of the view and 5% above the bottom of the view,
  leaving 90% for the height of the slider.
  Width and other style/sizing is done in ui's qss
  @param rect :
*/
void WebZoomSlider::setGeometry(const QRect& rect)
{
    // calc geometry but don't yet show
    d->m_slider->setOrientation(Qt::Vertical);
    d->m_slider->setFixedHeight(rect.height() * .90);
    d->m_slider->move(QPoint(rect.width() * .75 , rect.height() * .05));
}

/*!
  Show the slider and set idle timeout to time since slider last touched
*/
void WebZoomSlider::show(int idleTimeout)
{
    d->m_idleTimeout = idleTimeout;

    d->m_slider->show();

    // create timer to keep track of elapsed time since user touched slider
    d->m_timer = new QTimer(this);
    connect(d->m_timer, SIGNAL(timeout()), this, SIGNAL(idleTimeout()));
    d->m_timer->start(d->m_idleTimeout);
}

/*!
  set the zoom level on the current page according to value
  @param value : new zoom level to be set 
*/
void WebZoomSlider::zoom ( int value )
{
    WrtBrowserContainer *page =d->m_pageMgr->currentPage();
    if (page) {
        if (d->m_dragging)
            page->setPageDirtyZoomFactor(value / 100.00);
        else
            page->setPageZoomFactor(value / 100.00);
    }
    // stop timer while the slider is pressed or restart timer if slider is controlled by the keyboard
    if (d->m_dragging) {
        if (d->m_timer && d->m_timer->isActive())
            d->m_timer->stop();
    }
    else
        d->m_timer->start(d->m_idleTimeout);
}

void WebZoomSlider::sliderPressed()
{
    d->m_dragging = true;
}

void WebZoomSlider::sliderReleased()
{
    WrtBrowserContainer *page =d->m_pageMgr->currentPage();
    if (page) {
        page->setPageZoomFactor(d->m_slider->value() / 100.00);
    }
    // reset timer
    d->m_timer->start(d->m_idleTimeout);
    d->m_dragging = false;
}
/*!
  \fn void ImageView::idleTimeout();
  emitted when the slider has been idle for some time
*/


} // namespace WRT
