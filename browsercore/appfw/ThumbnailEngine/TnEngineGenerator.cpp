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


#include <QPainter>
#include "qdebug.h"

#include "TnEngineGenerator.h"
#include "TnEngineHandler.h"

const int KUnscaledBitmapSize = 1024*128; // pixels, = 128kB
#if defined Q_WS_MAC || defined Q_WS_WIN || defined Q_WS_X11
const int KBufferBitmapSize = 1024*2048;// pixels, = 2MB
#else
const int KBufferBitmapSize = 1024*374;// pixels, = 374kB
#endif
const int KMaxDocWidth = 1200; // limit the maximum width the TnEngine covers
const int KExtraUpdateHeightTop = 100; // hq update this much pixels outsize the view
const int KExtraUpdateHeightBottom = 300; // hq update this much pixels outsize the view
const int KExtraLQUpdateHeight = 0; // lq update this much pixels outsize the view

namespace WRT {

TnEngineGenerator::TnEngineGenerator(TnEngineHandler& TnEngine)
: m_TnEngine(&TnEngine),
m_validLQRegion(QRegion()), m_validHQRegion(QRegion()), m_bufferPos(0, 0), m_docSize(0, 0), m_keepsBitmaps(false)
{
    m_unscaledBitmap = 0;
    m_bufferBitmap = 0;
    m_asyncUpdateStarted = 0;
}

void TnEngineGenerator::init()
{
    m_scaler = TnEngineScaler::initWithCallback(*this);
    m_asyncUpdateStarted = new QTimer;
    connect(m_asyncUpdateStarted, SIGNAL(timeout()), this, SLOT(startAsyncBufferUpdate()));
}

TnEngineGenerator* TnEngineGenerator::initWithTnEngine(TnEngineHandler& TnEngine)
{
    TnEngineGenerator* self = new TnEngineGenerator(TnEngine);
    self->init();
    return self;
}

TnEngineGenerator::~TnEngineGenerator()
{
#ifdef __OOM__
    delete iOOMCollector;
#endif    
    deleteUnscaledBitmap();
    deleteBufferBitmap();
    delete m_scaler;
    delete m_asyncUpdateStarted;
    m_validLQRegion = QRegion();
    m_validHQRegion = QRegion();
}

void TnEngineGenerator::scalingCompleted(QPixmap& result, const QRect& targetRect)
{
    if (!m_bufferBitmap) {
        return;
    }
    QRect target(targetRect);
    QRect bufrect(bufferRect());
    // maybe the buffer has been scrolled out while scaling was going on?
    if (target.intersects(bufrect)) {
        // update the valid reqion
        m_validHQRegion += (target);
        m_validHQRegion = m_validHQRegion.intersected (bufrect);
        
        // blit the newly scaled area to correct position in buffer
        target.moveTo(target.topLeft()-m_bufferPos);
        QPainter painter(m_bufferBitmap);
        painter.drawPixmap(target.topLeft(), result);
    }
    // search for next stripe to update
    bool more = startAsyncBufferUpdate();
    
    // delete unscaled bitmap if nothing more to do
    if (!more && !m_keepsBitmaps) {
        deleteUnscaledBitmap();
    }
    
    // signal the update
    m_TnEngine->scaledPageChanged(m_TnEngine->theRect(), !more /*aReady*/, false);
}


void TnEngineGenerator::invalidate()
{
    QSize docSize(m_TnEngine->documentSize());
    // if doc width changes, assume larger changes and invalidate low quality buffer too
    if (docSize.width()!=m_docSize.width() || (!m_TnEngine->isDocumentComplete() && m_TnEngine->isFullScreenMode()))
    {
        m_validLQRegion = QRegion();
    }
    m_docSize = docSize;
    // otherwise only invalidate high quality areas to avoid lq<->hq flicker
    m_validHQRegion = QRegion();
}


void TnEngineGenerator::update(bool scrolling)
{
    bool changes = fastBufferUpdate();
    if (changes && m_scaler->isActive()) {
        // fastBufferUpdate uses the same bitmap, scaling op is no longer valid, have to cancel
        m_scaler->cancel();
    }
    // if scaler is already active no need to restart it
    if (m_TnEngine->isDocumentComplete() && !m_asyncUpdateStarted->isActive()
#ifdef __OOM__
        && !iOOMCollector->IsCollecting()
#endif        
        ) {
        m_asyncUpdateStarted->start(0);
    }
    if (changes) {
        m_TnEngine->scaledPageChanged(m_TnEngine->theRect(), true, scrolling);
    }
}


void TnEngineGenerator::scroll()
{
    calcBufferPosition();
}


void TnEngineGenerator::clear()
{
    m_scaler->cancel();
    m_validLQRegion = QRegion();
    m_validHQRegion = QRegion();
    if (m_bufferBitmap) {
        delete m_bufferBitmap;
        m_bufferBitmap = NULL;
    }
    if (m_unscaledBitmap) {
        delete m_unscaledBitmap;
    m_unscaledBitmap = NULL;
    }
    m_bufferPos = QPoint(0,0);
    m_docSize = QSize(0,0);
}


void TnEngineGenerator::calcBufferPosition()
{
    if (!m_bufferBitmap) {
        return;
    }
    QRect vpr(m_TnEngine->viewportOnTnEngine());
    QPoint vpc(vpr.center());
    QRect bufrect(bufferRect());
    QSize bufsize(bufrect.size());
    QPoint newPos(bufrect.topLeft());
    QSize mmdocSize(m_TnEngine->fromDocCoords(m_docSize));
    
    // check if view is outside center 1/3 of the buffer
    if (vpc.y()<m_bufferPos.y()+bufsize.height()/3 ||
        vpc.y()>m_bufferPos.y()+bufsize.height()*2/3 ) {
        // recalc new pos
        newPos.setY(vpc.y() - bufsize.height()/2);
        if (newPos.y()+bufsize.height()>mmdocSize.height())
            newPos.setY(mmdocSize.height()-bufsize.height());
        if (newPos.y()<0)
            newPos.setY(0);
    }
    
    // scroll the buffer if needed
    int scrollAmount = newPos.y()-m_bufferPos.y();
    if (scrollAmount!=0) {
        // check if we need to move bitmap
        if (scrollAmount>0 && scrollAmount<bufsize.height()) {
            // scroll buffer down
            QRect from (QPoint(0,scrollAmount), QSize(bufsize.width(), bufsize.height()-scrollAmount));
            // copy area that remains in buffer to a new position
            QPainter painter(m_bufferBitmap);
            painter.drawPixmap(QPoint(0,0), *m_bufferBitmap, from);
        }
        else if (scrollAmount<0 && (-scrollAmount)<bufsize.height()) {
            // scroll buffer up
            QRect from (QPoint(0,0), QSize(bufsize.width(), bufsize.height()+scrollAmount));
            // copy area that remains in buffer to a new position
            QPainter painter(m_bufferBitmap);
            painter.drawPixmap(QPoint(0,-scrollAmount), *m_bufferBitmap, from);
        }
        // invalidate the areas that are out from the buffer due to scrolling
        m_validLQRegion = m_validLQRegion.intersected(bufrect);
        m_validHQRegion = m_validHQRegion.intersected(bufrect);
    }
    m_bufferPos = newPos;
}

bool TnEngineGenerator::fastBufferUpdate()
{
    if (!checkAndCreateBitmaps()) {
        return false;
    }
    
    bool changes(false);
    
    QRect vp(m_TnEngine->viewportOnTnEngine());
    QRect bufrect(bufferRect());
    QSize unscaledSize(m_unscaledBitmap->size());
    
    QSize targetSize(m_TnEngine->fromDocCoords(unscaledSize));
    
    // divide to unscaled bitmap height stripes
    int ypos = ((vp.top() - KExtraLQUpdateHeight)/targetSize.height())*targetSize.height();
    int yend(vp.bottom() + KExtraLQUpdateHeight);
    
    if (ypos<0) {
        ypos = 0;
    }
    if (yend>bufrect.bottom()) {
        yend = bufrect.bottom();
    }
    // loop over the view area, checking if this stripe needs an update
    while (ypos<yend) {
        QRect target(QPoint(0,ypos),targetSize);
        QRect from(m_TnEngine->toDocCoords(target).topLeft(),unscaledSize);
        
        // check if this area is already valid
        QRegion tempR;
        tempR += (target);
        tempR = tempR.intersected(bufrect);
        tempR = tempR.subtracted (m_validLQRegion);
        tempR = tempR.subtracted (m_validHQRegion);
        
        if (!tempR.isEmpty()) {
            //  mark area valid
            m_validLQRegion += target;
            
            m_validLQRegion = m_validLQRegion.intersected(bufrect);
            
            // if not, get the bitmap from client
            QPainter unscaledPainter(m_unscaledBitmap);
            m_TnEngine->drawDocumentPart(unscaledPainter, from);
            // scale down
            target.moveTo(target.topLeft()-m_bufferPos);
            QPainter painter(m_bufferBitmap);
            painter.drawPixmap(target,*m_unscaledBitmap,QRect(QPoint(0,0), unscaledSize));
            changes = true;
        }
        tempR = QRegion();
        
        ypos += targetSize.height();
    }
    return changes;
}

bool TnEngineGenerator::startAsyncBufferUpdate()
{
    m_asyncUpdateStarted->stop();
    
    if (!checkAndCreateBitmaps()) {
        return false;
    }
    
    QRect vp(m_TnEngine->viewportOnTnEngine());
    QRect bufrect(bufferRect());
    QSize unscaledSize(m_unscaledBitmap->size());
    
    QSize targetSize(m_TnEngine->fromDocCoords(unscaledSize));
    
    // divide to unscaled bitmap height stripes
    int ypos(((vp.top() - KExtraUpdateHeightTop)/targetSize.height())*targetSize.height());
    int yend(vp.bottom() + KExtraUpdateHeightBottom);
    if (ypos<bufrect.top()) {
        ypos = bufrect.top();
    }
    if (yend>bufrect.bottom()) {
        yend = bufrect.bottom();
    }
    // loop over the view area, searching for a stripe that needs an update
    while (ypos<yend) {
        // update this area
        QRect target(QPoint(0,ypos),targetSize);
        // from here
        QRect from(m_TnEngine->toDocCoords(target).topLeft(),unscaledSize);
        
        // check if this area is already valid in high quality region
        QRegion tempR;
        tempR += (target);
        tempR = tempR.intersected(bufrect);
        tempR = tempR.subtracted(m_validHQRegion);
        if (!tempR.isEmpty()) {
            tempR = QRegion();
            // if not, get the bitmap from client
            QPainter painter(m_unscaledBitmap);
            m_TnEngine->drawDocumentPart(painter, from);
            // scale asynchronously
            m_scaler->startScaling(*m_unscaledBitmap,target);
            // update started, get out
            return true;
        }
        tempR = QRegion();
        
        ypos += targetSize.height();
    }
    // nothing to do
    return false;
}

bool TnEngineGenerator::checkAndCreateBitmaps()
{
#ifdef __OOM__
    if( iOOMCollector->IsCollecting() ) return false;
#endif
    
    QSize docSize(m_docSize);
    // minmap won't cover ridiculously wide document fully in horizontal
    // direction to avoid stripes from getting too wide/low
    if (docSize.width() > KMaxDocWidth) {
        docSize.setWidth(KMaxDocWidth);
    }
    QSize mmdocSize(m_TnEngine->fromDocCoords(docSize));
    QSize bufsize(0,0);
    QSize unscaledsize(0,0);
    if (mmdocSize.width()>0 && docSize.width()>0) {
      bufsize = QSize(mmdocSize.width(), qMin(mmdocSize.height(),KBufferBitmapSize/mmdocSize.width()));
      unscaledsize = QSize(docSize.width(), qMin(docSize.height(),KUnscaledBitmapSize/docSize.width()));
        if (!m_unscaledBitmap || unscaledsize != m_unscaledBitmap->size()) {
            // cancel scaling since we might delete the bitmap
            m_scaler->cancel();
        }
    }
    
    if (m_TnEngine->checkAndCreateBitmap(bufsize,m_bufferBitmap)) {
        m_TnEngine->checkAndCreateBitmap(unscaledsize,m_unscaledBitmap);
    }
    if( !m_bufferBitmap || !m_unscaledBitmap)
    {
        deleteUnscaledBitmap();
        deleteBufferBitmap();
    }
    return ( m_bufferBitmap!=0 ) && ( m_unscaledBitmap!=0 );
}

void TnEngineGenerator::deleteUnscaledBitmap()
{
    // stop scaling
    m_scaler->cancel();
    delete m_unscaledBitmap;
    m_unscaledBitmap = 0;
}

void TnEngineGenerator::deleteBufferBitmap()
{
    delete m_bufferBitmap;
    m_bufferBitmap = 0;
}

QRect TnEngineGenerator::bufferRect() const
{
    return QRect(m_bufferPos, m_bufferBitmap->size());
}

void TnEngineGenerator::setKeepsBitmaps(bool keepsBitmaps)
{
    m_keepsBitmaps = keepsBitmaps;
    // delete bitmap if no scaling active
    if (!m_keepsBitmaps && !m_scaler->isActive()) {
        deleteUnscaledBitmap();
    }
}

bool TnEngineGenerator::keepsBitmaps() const
{
    return m_keepsBitmaps;
}

void TnEngineGenerator::draw(QPainter& gc, const QRect& to) const
{
    if (!m_bufferBitmap) {
        return;
    }
    
    QRect vp(m_TnEngine->viewportOnTnEngine());
    
    QRect from(vp.topLeft()-m_bufferPos, to.size());
    
    gc.drawPixmap(to.topLeft(), *m_bufferBitmap, from);
}

}

