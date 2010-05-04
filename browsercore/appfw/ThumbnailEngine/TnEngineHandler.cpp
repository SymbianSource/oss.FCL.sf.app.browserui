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


#include <QPen>
#include <QBrush>
#include <QPainter>
#include "webcontentview.h"
#include "TnEngineHandler.h"
#include "TnEngineView.h"
#include "TnEngineGenerator.h"
#include "WrtPageManager.h"
#include "wrtbrowsercontainer.h"
#include "qwebframe.h"
#include <QDebug>

namespace WRT {

const int KDefaultZoomOutPercent = 250; // 2.5x TnEngine zoom out by default

// defines a box relative to TnEngine edges. if view center goes outside this box
// the view is recentered (50%==always center)
const int KHScrollAreaPercent = 33; // horizontal distance in percent
const int KVScrollAreaPercent = 50; // verical distance in percent
const int KHScrollAreaPercentWithTouch1 = 20; // horizontal distance in percent when to scroll
const int KVScrollAreaPercentWithTouch1 = 20; // verical distance in percent when to scroll
const int KHScrollAreaPercentWithTouch2 = 25; // horizontal distance in percent amount to scroll (scroll to the right only)
const int KVScrollAreaPercentWithTouch2 = 25; // verical distance in percent amount to scroll (scroll up only)
const int KUpdateDelayComplete = 45000; // 45s
const int KUpdateDelayLoading = 7000; // 7s
const int KUpdateDelayFullScreen = 4000; // 4s
const int KUpdateCbDelayComplete = 100; // 0.1s
const int KUpdateCbDelayLoading = 1000; // 1s

TnEngineHandler::TnEngineHandler(WrtPageManager * mgr, QWidget * parent)
    : m_pageMgr(mgr),
      m_zoomOutLevel(KDefaultZoomOutPercent),
      m_needsUpdate(false),
      m_documentComplete(false),
      m_fullScreenMode(false),
      m_pageScalerUpdating(false),
      m_activePage(NULL),
      m_parentWidget(parent)
{
    m_TnEngineView = NULL;
    m_generator = NULL;
    m_updateCbTimer = NULL;
    m_updateTimer = NULL;

    activate();
}

TnEngineHandler::~TnEngineHandler()
{
    deactivate();
}

QRect TnEngineHandler::containerRect() const
{
    return m_containerRect;
}

QRect TnEngineHandler::TnEngineRect() const
{
    QSize s(calcSize());
    //center
    QPoint p((m_containerRect.width()-s.width())/2,(m_containerRect.height()-s.height())/2);
    return QRect(p,s);
}

QRect TnEngineHandler::indicatorRect() const
{
    QRect vp(documentViewport());
    QRect TnEngineVp(viewportOnDocument());
    vp.moveTo(vp.topLeft()-TnEngineVp.topLeft()); //make vp relative to TnEngineVp
    QRect res = fromDocCoords(vp); //translate vp to mmap coords
    // borders
    res.moveTo(res.topLeft()+TnEngineRect().topLeft());
    // so that view area is within the indicator
    res.adjust(1,1,-1,-1);
    // ensure it is within the bounds
    QRect mr = TnEngineRect();
    if (res.left() < mr.left()) {
        res.setLeft(mr.left());
    }
    if (res.top() < mr.top()) {
        res.setTop(mr.top());
    }
    if (res.right() > mr.right()) {
        res.setRight(mr.right());
    }
    if (res.bottom() > mr.bottom()) {
        res.setBottom(mr.bottom());
    }
    return res;
}

void TnEngineHandler::documentStarted()
{
    scaledPageChanged(theRect(), true, false);
    m_documentComplete = false;
    m_viewportOnDocument = QRect();
    m_updateTimer->stop();
    m_updateCbTimer->stop();
    m_generator->clear();
    m_needsUpdate = false;
    // keep bitmaps during loading to avoid constant realloc
    m_generator->setKeepsBitmaps(true);
}

void TnEngineHandler::documentChanged()
{
    long delay(m_documentComplete?KUpdateCbDelayComplete:KUpdateCbDelayLoading);
    m_updateCbTimer->start( delay );
}

void TnEngineHandler::documentChangedCb()
{
    m_generator->invalidate();
    if (m_updateTimer->isActive()) {
        // timer running, update when it completes
        m_needsUpdate = true;
    }
    else {
        if (documentSize().height()>5) {
            m_viewportOnDocument = calcViewportOnDocument();
            m_generator->update();
            m_needsUpdate = false;
            // don't do another update for..
            long delay(m_fullScreenMode?KUpdateDelayFullScreen:m_documentComplete?KUpdateDelayComplete:KUpdateDelayLoading);
            m_updateTimer->start(delay);
        }
    }
}

void TnEngineHandler::documentViewportMoved()
{
    m_viewportOnDocument = calcViewportOnDocument();
    m_generator->scroll();
    if (m_visible) {
        m_generator->update(true);
    }
    else {
        m_needsUpdate = true;
        if (!m_updateTimer->isActive()) {
            long delay(m_documentComplete?KUpdateDelayComplete:KUpdateDelayLoading);
            m_updateTimer->start(delay);
        }
    }
}

void TnEngineHandler::documentCompleted(bool)
{
    if( m_pageScalerUpdating ) return;
    
    // wait for a while so to make sure all images are decoded etc.
    m_needsUpdate = true;
    m_documentComplete = true;
    m_updateTimer->start(0);
    // we can delete the buffers now
    m_generator->setKeepsBitmaps(false);
    documentChanged();
}

QRect TnEngineHandler::viewportOnTnEngine() const
{
    return fromDocCoords(m_viewportOnDocument);
}


QRect TnEngineHandler::viewportOnDocument() const
{
    return m_viewportOnDocument;
}

QSize TnEngineHandler::calcSize() const
{
    QSize res, max;
    QSize mms = fromDocCoords(documentSize());
    max = m_containerRect.size();
    res.setWidth(qMin(mms.width(),max.width()));
    res.setHeight(qMin(mms.height(),max.height()));
    return res;
}

QRect TnEngineHandler::calcViewportOnDocument() const
{
    QPoint mvp(m_viewportOnDocument.topLeft());
    
    QRect docvp(documentViewport());
    QSize docs(documentSize());
    QSize ms(toDocCoords(TnEngineRect().size()));
    QPoint docc(docvp.center());
    
    if (!m_dragging) {
        // scroll if not within 1/3 center area
        // check x-direction
        if (docc.x()<mvp.x()+ms.width()*KHScrollAreaPercent/100 ||
            docc.x()>mvp.x()+ms.width()*(100-KHScrollAreaPercent)/100) {
            // far enough from the center, re-center the view
            mvp.setX(docc.x()-ms.width()/2);
            if (mvp.x()+ms.width()>docs.width()) {
                mvp.setX(docs.width()-ms.width());
            }
            if (mvp.x()<0) {
                mvp.setX(0);
            }
        }
        // y-direction
        if (docc.y()<mvp.y()+ms.height()*KVScrollAreaPercent/100 ||
            docc.y()>mvp.y()+ms.height()*(100-KVScrollAreaPercent)/100) {
            // far enough from the center, re-center the view
            mvp.setY(docc.y()-ms.height()/2);
            if (mvp.y()+ms.height()>docs.height()) {
                mvp.setY(docs.height()-ms.height());
            }
            if (mvp.y()<0) {
                mvp.setY(0);
            }
        }
        
    }
    else {
        // check x-direction
        bool moved = false;
        if (docc.x()<mvp.x()+ms.width()*KHScrollAreaPercentWithTouch1/100) {
            mvp.setX(docc.x()-ms.width()*KHScrollAreaPercentWithTouch2/100);
            moved = true;
        }
        else if (docc.x()>mvp.x()+ms.width()*(100-KHScrollAreaPercentWithTouch1)/100) {
            mvp.setX(docc.x()-ms.width()*(100-KHScrollAreaPercentWithTouch1)/100);
            moved = true;
        }
        if (moved) {
            if (mvp.x()+ms.width()>docs.width()) {
                mvp.setX(docs.width()-ms.width());
            }
            if (mvp.x()<0) {
                mvp.setX(0);
            }
        }
        // y-direction
        moved = false;
        if (docc.y()<mvp.y()+ms.height()*KVScrollAreaPercentWithTouch1/100) {
            mvp.setY(docc.y()-ms.height()*KVScrollAreaPercentWithTouch2/80);
            moved = true;
        }
        else if (docc.y()>mvp.y()+ms.height()*(100-KVScrollAreaPercentWithTouch1)/100) {
      mvp.setY(docc.y()-ms.height()*(100-KVScrollAreaPercentWithTouch1)/100);
            moved = true;
        }
        
        if (moved) {
            if (mvp.y()+ms.height()>docs.height()) {
                mvp.setY(docs.height()-ms.height());
            }
            if (mvp.y()<0) {
                mvp.setY(0);
            }
        }
    }
    return QRect(mvp,ms);
}

void TnEngineHandler::draw(QPainter& gc, const QRect& rect) const
{
    if (m_fullScreenMode) {
        QPen pen(Qt::SolidLine);
        pen.setColor(QColor(220,220,255));
        QBrush brush(Qt::SolidPattern);
        brush.setColor(QColor(220,220,255));
        gc.setPen(pen);
        gc.setBrush(brush);
        gc.drawRect(m_containerRect);
        m_generator->draw(gc, TnEngineRect());
        pen.setWidth(1);
        pen.setColor(QColor(255, 0, 0));
        gc.setPen(pen);
        gc.setBrush(Qt::NoBrush);
        gc.drawRect(indicatorRect());
    }
}

QRect TnEngineHandler::fromDocCoords(const QRect& from) const
{
    return QRect(fromDocCoords(from.topLeft()),fromDocCoords(from.size()));
}

QPoint TnEngineHandler::fromDocCoords(const QPoint& from) const
{
    QPoint res;
    res.setX(from.x()*100/m_zoomOutLevel);
    res.setY(from.y()*100/m_zoomOutLevel);
    return res;
}

QSize TnEngineHandler::fromDocCoords(const QSize& from) const
{
    QSize res;
    res.setWidth(from.width()*100/m_zoomOutLevel);
    res.setHeight(from.height()*100/m_zoomOutLevel);
    return res;
}

QPoint TnEngineHandler::toDocCoords(const QPoint& from) const
{
    QPoint res;
    res.setX(from.x()*m_zoomOutLevel/100);
    res.setY(from.y()*m_zoomOutLevel/100);
    return res;
}

QSize TnEngineHandler::toDocCoords(const QSize& from) const
{
    QSize res;
    res.setWidth(from.width()*m_zoomOutLevel/100);
    res.setHeight(from.height()*m_zoomOutLevel/100);
    return res;
}

QRect TnEngineHandler::toDocCoords(const QRect& from) const
{
  return QRect(toDocCoords(from.topLeft()),toDocCoords(from.size()));
}

bool TnEngineHandler::checkAndCreateBitmap(QSize sz, QPixmap*& image)
{
    if ( sz.width()<=0 || sz.height()<=0 ) {
        // delete bitmap if there was one
        delete image;
        image = 0;
        return false;
    }
    else {
        if ( image && sz != image->size() ) {
            // resize if different size
            QPixmap* bm = new QPixmap(image->copy(0, 0, sz.width(), sz.height()));
            delete image;
            image = bm;
        }
        else if ( !image ) {
            // create new
      QPixmap* bm = new QPixmap(sz);
          image = bm;
        }
    }
    return true;
}

void TnEngineHandler::updateCbTimerCb()
{
    m_updateCbTimer->stop();
    m_pageScalerUpdating = true;
    documentChangedCb();
    m_pageScalerUpdating = false;
}

void TnEngineHandler::updateTimerCb()
{
    m_updateTimer->stop();
    m_pageScalerUpdating = true;
    if (m_needsUpdate) {
        m_viewportOnDocument = calcViewportOnDocument();
        m_generator->update();
    }
    m_needsUpdate = false;
    m_pageScalerUpdating = false;
    m_updateTimer->stop();
}

QRect TnEngineHandler::theRect() const
{
    return m_containerRect;
}

void TnEngineHandler::setContainerRect(const QRect& rect)
{
    m_containerRect = rect;
    m_viewportOnDocument = calcViewportOnDocument();
}

bool TnEngineHandler::isFullScreenMode() const
{
    return m_fullScreenMode;
}

void TnEngineHandler::setFullScreenMode(bool fullScreenMode)
{
    m_fullScreenMode = fullScreenMode;
    m_viewportOnDocument = calcViewportOnDocument();
}

void TnEngineHandler::setVisible(bool visible)
{
    if (visible && !m_visible) {
        updateNow();
    }
    m_visible = visible;
}

void TnEngineHandler::updateNow()
{
    if (m_updateCbTimer->isActive()) {
        m_updateCbTimer->stop();
        m_updateTimer->stop();
        documentChangedCb();
    }
    else {
        m_updateTimer->stop();
        updateTimerCb();
    }
}

void TnEngineHandler::drawDocumentPart(QPainter& painter, const QRect& documentAreaToDraw)
{
    QWebFrame* frame = m_activePage->mainFrame();
    QRegion clip(documentAreaToDraw);
    painter.save();
    painter.translate(-documentAreaToDraw.x(), -documentAreaToDraw.y());
    frame->render(&painter, clip);
    painter.restore();
}

QRect TnEngineHandler::documentViewport() const
{
    QSize size = m_activePage->webWidget()->size().toSize();
    QSize offset = QSize(m_scrollX, m_scrollY);
    return QRect(offset.width(), offset.height(), size.width(), size.height());
}

void TnEngineHandler::scaledPageChanged(const QRect& area, bool /*fullScreen*/, bool /*scroll*/)
{
    if (m_TnEngineView)
        m_TnEngineView->update(area);
}

QSize TnEngineHandler::documentSize() const
{
    return m_activePage->mainFrame()->contentsSize();
}

QWidget* TnEngineHandler::widget()
{ 
    return m_TnEngineView;
}


void TnEngineHandler::activate()
{
    if (!m_TnEngineView) {
        createGenerator();
        createView();
    }
}

void TnEngineHandler::deactivate()
{
    if (m_TnEngineView) {
        destroyView();
        destroyGenerator();
    }
}

void TnEngineHandler::createGenerator()
{
    Q_ASSERT(m_pageMgr);
    m_activePage = m_pageMgr->currentPage();
    Q_ASSERT(m_activePage);
    Q_ASSERT(!m_generator);
    m_dragging = false;
    QPoint pos(0,0);
    pos = m_activePage->mainFrame()->scrollPosition();
    m_scrollX = pos.x();
    m_scrollY = pos.y();
    if (!m_generator) {
        m_generator = TnEngineGenerator::initWithTnEngine(*this);
        m_updateCbTimer = new QTimer();
        m_updateTimer = new QTimer();
    }
}

void TnEngineHandler::destroyGenerator()
{
    Q_ASSERT(m_generator);
    if (m_generator) {
        delete m_generator;
        m_generator = NULL;
    }
    if (m_updateCbTimer) {
        m_updateCbTimer->stop();
        delete m_updateCbTimer;
        m_updateCbTimer = NULL;
    }
    if (m_updateTimer) {
        m_updateTimer->stop();
        delete m_updateTimer;
        m_updateTimer = NULL;
    }
}

void TnEngineHandler::createView()
{
    Q_ASSERT(m_activePage);
    connect(m_updateCbTimer, SIGNAL(timeout()), this, SLOT(updateCbTimerCb()));
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(updateTimerCb()));
    //    connect(WebController::webController(), SIGNAL(activePageChanged()), this, SLOT(cancelTnEngineView()));
    connect(m_activePage, SIGNAL(loadStarted()), this, SLOT(documentStarted()));
    connect(m_activePage, SIGNAL(repaintRequested(const QRect&)), this, SLOT(documentChanged()));
    connect(m_activePage, SIGNAL(loadFinished(bool)), this, SLOT(documentCompleted(bool)));
    //    connect(m_activePage, SIGNAL(scrollRequested(int, int, const QRect &)), this, SLOT(documentViewportMoved()));
    Q_ASSERT(!m_TnEngineView);




    //m_TnEngineView = TnEngineView::initiWithParentAndTnEngineHandler(m_activePage->webWidget(), this);
    m_TnEngineView = TnEngineView::initiWithParentAndTnEngineHandler(m_parentWidget, this);
    m_TnEngineView->show();
    m_savedPointPageView = documentViewport().topLeft();
    //    connect(m_activePage, SIGNAL(scrollRequested(int, int, const QRect &)), m_TnEngineView, SLOT(update()));
    connect(m_TnEngineView, SIGNAL(scrollBy(int, int)), this, SLOT(scrollBy(int, int)));
    connect(m_TnEngineView, SIGNAL(scrollStarted()), this, SLOT(draggingStarted()));
    connect(m_TnEngineView, SIGNAL(scrollEnded()), this, SLOT(draggingEnded()));
    connect(m_TnEngineView, SIGNAL(ok()), this, SLOT(okInvoked()));
    // get initial thumbnail
    documentStarted();
    documentChanged();
    documentCompleted(true);
}

void TnEngineHandler::destroyView()
{
    Q_ASSERT(m_activePage);
    disconnect(m_updateCbTimer, SIGNAL(timeout()), this, SLOT(updateCbTimerCb()));
    disconnect(m_updateTimer, SIGNAL(timeout()), this, SLOT(updateTimerCb()));
    //    disconnect(WebController::webController(), SIGNAL(activePageChanged()), this, SLOT(cancelTnEngineView()));
    disconnect(m_activePage, SIGNAL(loadStarted()), this, SLOT(documentStarted()));
    disconnect(m_activePage, SIGNAL(repaintRequested(const QRect&)), this, SLOT(documentChanged()));
    disconnect(m_activePage, SIGNAL(loadFinished(bool)), this, SLOT(documentCompleted(bool)));
    //    disconnect(m_activePage, SIGNAL(scrollRequested(int, int, const QRect &)), this, SLOT(documentViewportMoved()));
    Q_ASSERT(m_TnEngineView);
    //    disconnect(m_activePage, SIGNAL(scrollRequested(int, int, const QRect &)), m_TnEngineView, SLOT(update()));
    disconnect(m_TnEngineView, SIGNAL(scrollBy(int, int)), this, SLOT(scrollBy(int, int)));
    disconnect(m_TnEngineView, SIGNAL(scrollStarted()), this, SLOT(draggingStarted()));
    disconnect(m_TnEngineView, SIGNAL(scrollEnded()), this, SLOT(draggingEnded()));
    disconnect(m_TnEngineView, SIGNAL(ok()), this, SLOT(okInvoked()));
    m_TnEngineView->hide();
    m_TnEngineView->deleteLater();
    m_TnEngineView = NULL;
    m_visible = false;
}

void TnEngineHandler::scrollBy(int x, int y)
{
    WebContentWidget* canvas = static_cast<WebContentWidget*>(m_activePage->webWidget());
    qreal w = documentSize().width() - canvas->size().width();
    qreal h = documentSize().height() - canvas->size().height();

    qreal newX = m_scrollX + x;
    qreal newY = m_scrollY + y;
    newX = newX > 0 ? newX : 0;
    newX = newX < w ? newX : w;
    newY = newY > 0 ? newY : 0;
    newY = newY < h ? newY : h;
    m_scrollX = newX;
    m_scrollY = newY;
    if (m_generator)
        documentViewportMoved();
    if (m_TnEngineView)
        m_TnEngineView->update();
}

void TnEngineHandler::draggingStarted()
{
    m_dragging = true;
}

void TnEngineHandler::draggingEnded()
{
    m_dragging = false;
}

void TnEngineHandler::okInvoked()
{
    QPoint pos(0,0);
    pos = m_activePage->mainFrame()->scrollPosition();
    
    emit ok(m_scrollX - pos.x(), m_scrollY - pos.y());
}


}
//  End of File
