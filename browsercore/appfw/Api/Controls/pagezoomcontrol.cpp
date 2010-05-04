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


#include "webcontentview.h"
#include "pagezoomcontrol.h"
#include "pagezoomcontrol_p.h"
#include "WrtPageManager.h"
#include "wrtbrowsercontainer.h"

using namespace WRT;

const qreal KZoomStep = 0.36;
const int KSmoothZoomTimeOut = 50;
const qreal KStartStep = 0.10;
const qreal KEndStep = 0.05; 
const qreal KStepWidth = 0.01;


PageZoomControlPrivate::PageZoomControlPrivate(WrtPageManager *mgr)
                    :m_mgr(mgr),
                    m_value(0),
                    m_smoothZoomTimer(0)
{
}

PageZoomControlPrivate::~PageZoomControlPrivate()
{
    deleteTimer();
}

void PageZoomControlPrivate::smoothZoomIn()
{
    setZoom(PageZoomControlPrivate::zoomIn);
    //checking for precision upto 3 decimal points
    if ((m_value - KEndStep) <= 0.001){
        deleteTimer();
        WrtBrowserContainer* page = m_mgr->currentPage();
        WebContentWidget* view = qobject_cast<WebContentWidget*>(page->webWidget());
        view->deletePagePixmap();
        view->setPageCenterZoomFactor(view->zoomFactor() + KStepWidth);
    }else {
        m_smoothZoomTimer->start(KSmoothZoomTimeOut);
    }
}
void PageZoomControlPrivate::smoothZoomOut()
{
    setZoom(PageZoomControlPrivate::zoomOut);
    //checking for precision upto 3 decimal points
    if ((m_value - KEndStep) <= 0.001){
        deleteTimer();
        WrtBrowserContainer* page = m_mgr->currentPage();
        WebContentWidget* view = qobject_cast<WebContentWidget*>(page->webWidget());
        view->deletePagePixmap();
        view->setPageCenterZoomFactor(view->zoomFactor() - KStepWidth);
    }else {
        m_smoothZoomTimer->start(KSmoothZoomTimeOut);
    }
}

void PageZoomControlPrivate::setZoom(ZoomDirection direction)
{
    m_value = m_value - KStepWidth;
    WrtBrowserContainer * page = m_mgr->currentPage(); 
    if (page) 
    {
        WebContentWidget* view = qobject_cast<WebContentWidget*>(page->webWidget());

        if(view)
        {
            if (m_smoothZoomTimer)
                //view->setDirtyZoomFactor(view->dirtyZoomFactor() + direction * m_value);
                view->setBitmapZoom(view->zoomFactor() + static_cast<qreal>(direction) * m_value);
            else
                view->setZoomFactor(view->dirtyZoomFactor() + static_cast<qreal>(direction) * m_value);
        }
        else
            page->setPageZoomFactor(page->pageZoomFactor()+ static_cast<qreal>(direction) * m_value);
    }
//        page->setPageCenterZoomFactor(page->pageZoomFactor()+ direction * m_value);
}

void PageZoomControlPrivate::createZoomTimer(ZoomDirection direction)
{
    //clear if any exitsting timer
    deleteTimer();

    m_smoothZoomTimer = new QTimer(this);
    if (direction == PageZoomControlPrivate::zoomIn)
        connect(m_smoothZoomTimer, SIGNAL(timeout()), this, SLOT(smoothZoomIn()));
    else
        connect(m_smoothZoomTimer, SIGNAL(timeout()), this, SLOT(smoothZoomOut()));
    m_smoothZoomTimer->start(KSmoothZoomTimeOut);
}
void PageZoomControlPrivate::deleteTimer()
{
    if (m_smoothZoomTimer) {
        if(m_smoothZoomTimer->isActive())
            m_smoothZoomTimer->stop();
        delete m_smoothZoomTimer;
        m_smoothZoomTimer = NULL;
    }

    //setZoom( zoomReset );
}
PageZoomControl::PageZoomControl(WrtPageManager *mgr)
{
    d = new PageZoomControlPrivate(mgr);
}
PageZoomControl::~PageZoomControl()
{
    delete d;
}

qreal PageZoomControl::zoomLevel()
{
    qreal zoomValue = 0;
/*

    if(d->m_page)
        zoomValue = d->m_page->pageZoomFactor();

*/
    WrtBrowserContainer* page = d->m_mgr->currentPage();
    if( page )
        zoomValue = page->pageZoomFactor();

    return zoomValue;
}

qreal PageZoomControl::maxZoomLevel()
{
    WrtBrowserContainer * page = d->m_mgr->currentPage(); 
    WebContentWidget* view = qobject_cast<WebContentWidget*>(page->webWidget());
    return view->maximumScale(); 
}

qreal PageZoomControl::minZoomLevel()
{
    WrtBrowserContainer * page = d->m_mgr->currentPage(); 
    WebContentWidget* view = qobject_cast<WebContentWidget*>(page->webWidget());
    return view->minimumScale();
}

bool PageZoomControl::isUserScalable()
{
    WrtBrowserContainer * page = d->m_mgr->currentPage(); 
    WebContentWidget* view = qobject_cast<WebContentWidget*>(page->webWidget());
    return view->isUserScalable();
}

qreal PageZoomControl::zoomStep()
{
    return KZoomStep;
}

void PageZoomControl::zoomIn()
{
    if (d->m_smoothZoomTimer && d->m_smoothZoomTimer->isActive())
        return;

    if (zoomLevel() + KZoomStep <= maxZoomLevel()) {
        WrtBrowserContainer * page = d->m_mgr->currentPage(); 
        WebContentWidget* view = qobject_cast<WebContentWidget*>(page->webWidget());
        view->createPagePixmap();    	
        d->m_value =  KStartStep;
        d->createZoomTimer(PageZoomControlPrivate::zoomIn);
        emit zoomLevelChanged(zoomLevel() + KZoomStep);
    }
}
void PageZoomControl::zoomOut()
{
    if (d->m_smoothZoomTimer && d->m_smoothZoomTimer->isActive())
        return;

    if (zoomLevel() - KZoomStep >= minZoomLevel()) {
        WrtBrowserContainer * page = d->m_mgr->currentPage(); 
        WebContentWidget* view = qobject_cast<WebContentWidget*>(page->webWidget());
        view->createPagePixmap();    	
        d->m_value =  KStartStep;
        d->createZoomTimer(PageZoomControlPrivate::zoomOut);
        emit zoomLevelChanged(zoomLevel() - KZoomStep);
    }
}
