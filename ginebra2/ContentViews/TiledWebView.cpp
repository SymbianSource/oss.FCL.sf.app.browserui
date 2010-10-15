
#include "TiledWebView.h"

#include <QPainter>
#include <QPixmap>
#include <QStyleOptionGraphicsItem>
#include <QWebFrame>
#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsSceneResizeEvent>



const int cTileSize = 64;
const qreal cBigSideTileOverHead = 2;
const qreal cSmallSideTileOverHead = 2.5;
const int cTileUpdateTimerTick = 50;
const int cPaintIdleTimeout = cTileUpdateTimerTick * 2;
const int cTileRectRecenterTimeout = 500;
const int cTileScaleUpdateTimeout = 250;
const int cIdleTileUpdateChunkSize = 4;
const int cInPaintTileUpdateTimeout = 18;


TiledWebView::TiledWebView(QGraphicsItem* parent) : QGraphicsWebView(parent)
{
    m_tilesPool.clear();
    m_tilesField = 0;
    m_inUpdate = false;
    m_tilesRectCentered = false;
    m_tilesFrozen = false;
    m_needViewportTilesUpdate = false;
    m_needScaleCommit = false;
    m_needTilesFieldRebuild = false;
    m_lastScrollDelta = QPoint(0, 0);
#ifdef USE_ASSISTANT_ITEM
    m_assistant = new TiledWebViewAssistant();
    m_assistant->setParentItem(this);
    m_assistant->m_master = this;

    setFlag(QGraphicsItem::ItemHasNoContents, true);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
#endif

    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(updateTimeout()));
    connect(this, SIGNAL(scaleChanged()), this, SLOT(scheduleScaleUpdate()));
    m_userPaintTS.start();

}

TiledWebView::~TiledWebView()
{
    delete[] m_tilesField;
    m_tilesField = 0;
    for(int i = 0; i < m_tilesPool.count(); i++) {
        delete m_tilesPool[i];
        m_tilesPool[i] = 0;
    }
}

void TiledWebView::loadStarted()
{
//    resetTiles(QRect(QPoint(0, 0), m_tilesDim), false);
    m_needViewportTilesUpdate = true;

    startUpdateTimer();
}

void TiledWebView::setPage(QWebPage* page)
{
    bool doneOnce = false;
    
    if(!doneOnce) {
        doneOnce = true;
        page->setProperty("_q_RepaintThrottlingPreset", QVariant::fromValue(QString("Medium")));
    }
    
    page->setProperty("_q_HTMLTokenizerChunkSize", 1024);
    page->setProperty("_q_HTMLTokenizerTimeDelay", 0.750);
    resetTiles(QRect(QPoint(0, 0), m_tilesDim), false);
    m_needViewportTilesUpdate = true;

    QGraphicsWebView::setPage(page);

    QPalette pal = palette();
    pal.setColor(QPalette::ButtonText,Qt::black);
    pal.setBrush(QPalette::Base, Qt::white);
    setPalette(pal);
    page->setPalette(pal);

    connect(page, SIGNAL(repaintRequested(QRect)), this, SLOT(repaintRequested(QRect)));
    connect(page, SIGNAL(loadStarted()), this, SLOT(loadStarted()));
    m_needViewportTilesUpdate = true;
    startUpdateTimer();
}

//#define TILEPOOL_DEBUG
TiledWebView::Tile::Tile() : img(cTileSize,cTileSize), ready(0), used(0)
{
}

TiledWebView::Tile* TiledWebView::tileAt(const QPoint& p) const
{
    Q_ASSERT(p.x() < m_tilesDim.width() && p.y() < m_tilesDim.height());
    Q_ASSERT(m_tilesField);

    return m_tilesField[m_tilesDim.width() * p.y() + p.x()];
}

void TiledWebView::setTileAt(const QPoint& p,Tile* t)
{
    m_tilesField[m_tilesDim.width() * p.y() + p.x()] = t;
}

static void boundPoint(const QPoint& min, QPoint& p, const QPoint &max)
{
    p.setX(qBound(min.x(),p.x(),max.x()));
    p.setY(qBound(min.y(),p.y(),max.y()));
}

static QPoint topPoint(const QSize& s)
{
    return QPoint(s.width() - 1, s.height() - 1);
}

TiledWebView::Tile* TiledWebView::createTile(const QPoint& p)
{
    int i;
    for(i = 0; i < m_tilesPool.count(); i++)
        if(!m_tilesPool[i] || !m_tilesPool[i]->used)
            break;

    Tile* ret;
    if(i < m_tilesPool.count()) {
        if(!m_tilesPool[i]) {
            m_tilesPool[i] = new Tile();
        }
        setTileAt(p, m_tilesPool[i]);
        ret = m_tilesPool[i];
    } else {
        ret = new Tile();
        m_tilesPool.append(ret);
        setTileAt(p, ret);
    }

    ret->used = true;

#ifdef TILEPOOL_DEBUG
    checkTilesField();
#endif
    return ret;
}

QPoint TiledWebView::tileAtPoint(const QPointF& p) const
{
    QPointF tmp = mapToTileCoords(p - m_tilesRect.topLeft());
    tmp /= cTileSize;
    QPoint ret((int)tmp.x(),(int)tmp.y());
    return ret;
}

QRectF TiledWebView::tileRect(const QPoint& t) const
{
    QRectF tRect = mapToTileCoords(m_tilesRect);
    QRectF ret(tRect.topLeft() + t * cTileSize,QSizeF(cTileSize,cTileSize));

    return ret;
}

static int calcD(const QPoint p1, QPoint p2)
{
    int d1 = p2.x() - p1.x();
    int d2 = p2.y() - p1.y();
    return d1 * d1 + d2 * d2;
}
QRectF TiledWebView::viewPortRect() const
{
    return mapRectFromParent(QRectF(QPointF(0, 0),static_cast<QGraphicsWidget*>(parentItem())->size()));
}

void TiledWebView::boundTile(QPoint& t) const
{
    boundPoint(QPoint(0,0),t,topPoint(m_tilesDim));
}

QPoint TiledWebView::findTile4Update(bool inView, bool addDirty) const
{
    QRectF vpRect = viewPortRect();
    QRectF updateRect = m_tilesRect;
    if(inView)
        updateRect = updateRect.intersect(vpRect);
    QPoint found(-1, -1);
    if(updateRect.isEmpty()) return found;

    QPoint topLeft = tileAtPoint(updateRect.topLeft());
    QPoint bottomRight = tileAtPoint(updateRect.bottomRight());
    QPoint center = tileAtPoint(vpRect.center());

    bottomRight += QPoint(1, 1);
    boundTile(topLeft);
    boundTile(bottomRight);
    boundPoint(topLeft,center,bottomRight);

    int d = 1000000; // just big enough value;
    int tmpD;
    for(int j = topLeft.y(); j <= bottomRight.y(); j++)
        for(int i = topLeft.x(); i <= bottomRight.x(); i++) {
            QPoint p(i,j);
            Tile* t = tileAt(p);
            if(!t || !t->ready || (addDirty && !t->dirtyRect.isEmpty())) {
                tmpD = calcD(center, p);
                if(tmpD < d) {
                    d = tmpD;
                    found = p;
                }
            }
        }

    return found;
}

QList<QPoint> TiledWebView::findTileLine4Update(bool dirty, bool inView, bool useScrollDirection) const
{
    QRectF vpRect = viewPortRect();
    QRectF updateRect = m_tilesRect;
    if(inView)
        updateRect = updateRect.intersect(vpRect);
    QList<QPoint> ret;
    if(updateRect.isEmpty()) return ret;

    QPoint topLeft = tileAtPoint(updateRect.topLeft());
    QPoint bottomRight = tileAtPoint(updateRect.bottomRight());
    QPoint topVPLeft = tileAtPoint(vpRect.topLeft());
    QPoint bottomVPRight = tileAtPoint(vpRect.bottomRight());
    QPoint center = tileAtPoint(vpRect.center());

    bottomRight += QPoint(1, 1);
    bottomVPRight += QPoint(1, 1);
    boundTile(topLeft);
    boundTile(bottomRight);
    boundTile(topVPLeft);
    boundTile(bottomVPRight);
    boundPoint(topLeft,center,bottomRight);

    int maxCount = qAbs(topLeft.x() - center.x());
    maxCount = qMax(maxCount, qAbs(topLeft.y() - center.y()));
    maxCount = qMax(maxCount, qAbs(bottomRight.x() - center.x()));
    maxCount = qMax(maxCount, qAbs(bottomRight.y() - center.y()));

    for(int i = 0; i < maxCount; i++) {
        int j;
        if((m_lastScrollDelta.y() > 0 || !useScrollDirection) &&
           center.y() + i <= bottomRight.y())
            for(j = topVPLeft.x(); j <= bottomVPRight.x(); j++) {
                QPoint p(j, center.y() + i);
                Tile* t = tileAt(p);
                if(!t || !t->ready || (dirty && !t->dirtyRect.isEmpty())) {
                    ret += p;
                }
            }
        if(!ret.isEmpty())
            break;

        if((m_lastScrollDelta.y() < 0 || !useScrollDirection) &&
           center.y() - i >= topLeft.y())
            for(j = topVPLeft.x(); j <= bottomVPRight.x(); j++) {
                QPoint p(j, center.y() - i);
                Tile* t = tileAt(p);
                if(!t || !t->ready || (dirty && !t->dirtyRect.isEmpty())) {
                    ret += p;
                }
            }
            if(!ret.isEmpty())
                break;

            if((m_lastScrollDelta.x() > 0 || !useScrollDirection) &&
               center.x() + i <= bottomRight.x())
                for(j = topVPLeft.y(); j <= bottomVPRight.y(); j++) {
                    QPoint p(center.x() + i, j);
                    Tile* t = tileAt(p);
                    if(!t || !t->ready || (dirty && !t->dirtyRect.isEmpty())) {
                        ret += p;
                    }
                }
            if(!ret.isEmpty())
                break;

            if((m_lastScrollDelta.x() < 0  || !useScrollDirection) &&
               center.x() - i >= topLeft.x())
                for(j = topVPLeft.y(); j <= bottomVPRight.y(); j++) {
                    QPoint p(center.x() - i, j);
                Tile* t = tileAt(p);
                if(!t || !t->ready || (dirty && !t->dirtyRect.isEmpty())) {
                    ret += p;
                }
            }
            if(!ret.isEmpty())
                break;
    }

    return ret;
}

QRectF TiledWebView::updateTile(const QPoint& t)
{
    m_inUpdate = true;
    Tile* tile = tileAt(t);
    if(!tile) tile = createTile(t);

    QPainter p(&(tile->img));
    QRectF tRect = mapFromTileCoords(tileRect(t));
    QRectF tDirtyRect = mapFromTileCoords(tile->dirtyRect);
    QRectF updateRect = !tile->ready || tDirtyRect.isEmpty() ? tRect : tDirtyRect;


    //if(!tile->ready)
    //    p.fillRect(0, 0, cTileSize, cTileSize, Qt::white);

    tile->ready = true;
    tile->dirtyRect = QRectF();

    p.scale(m_tilesScale,m_tilesScale);

    qreal adjust = 2 + m_tilesScale;

    // adjust rect to cover rouding errors
    updateRect.adjust(-adjust, -adjust, adjust, adjust);

    QRegion clip(updateRect.toRect());
    p.translate(-tRect.topLeft());
    p.setClipRegion(clip);
//    p.fillRect(clip.boundingRect(), Qt::white);

//    p.setRenderHint(QPainter::SmoothPixmapTransform); // cause assert now
//    p.setRenderHint(QPainter::Antialiasing);

    page()->mainFrame()->render(&p, QWebFrame::ContentsLayer, clip);

    m_inUpdate = false;
    return updateRect;
}

void TiledWebView::repaintRequested(QRect r)
{
    if(!m_tilesField) return;

    if(r.isEmpty())
        r = m_tilesRect.adjusted(-1, -1, 1, 1).toRect();

    QPoint topLeftTile = tileAtPoint(r.topLeft());
    QPoint bottomRightTile = tileAtPoint(r.bottomRight());
    boundTile(topLeftTile);
    boundTile(bottomRightTile);
    QRectF repaintClip = mapToTileCoords(r);
    bool needUpdate = false;
    QRectF vpRect = mapToTileCoords(viewPortRect());

    for(int j = topLeftTile.y(); j <= bottomRightTile.y(); j++)
        for(int i = topLeftTile.x(); i <= bottomRightTile.x(); i++) {
            QPoint t(i, j);
            QRectF clip = tileRect(t) & repaintClip;
            if(!clip.isEmpty()) {

                Tile* tile = tileAt(t);
                if(tile && tile->ready) {
                    if(!tile->dirtyRect.isEmpty())
                        clip |= tile->dirtyRect;
                    tile->dirtyRect = clip;
//                    if(vpRect.intersects(tile->dirtyRect)) {
//                        m_needViewportTilesUpdate = true;
//                    }
                }
                needUpdate = true;
            }
        }

    if(needUpdate)
        startUpdateTimer();

}

QPixmap* TiledWebView::getUnprepPixmap()
{
    static const int squareMult = 1;
    static const int cellSize = 16;
    static QPixmap pixmap(squareMult * cellSize, squareMult * cellSize);
    static bool init = false;

    if(!init) {
        init = true;
        QPainter p(&pixmap);
        p.fillRect(pixmap.rect(),Qt::lightGray);
        // p.setPen(QColor(182,242,255));
        // p.setPen(QColor(Qt::darkBlue));
        p.setPen(Qt::darkGray);
        for(int i = -1; i < squareMult + 1; i++) {
            int xoffs = i * cellSize;
            for(int j = -1; j < squareMult + 1; j++) {
                int yoffs = j * cellSize;
                p.drawLine(xoffs + 7, yoffs - 4, xoffs + 7, yoffs + 2);
                p.drawLine(xoffs + 4, yoffs + 7, xoffs + 10, yoffs + 7);
                p.drawLine(xoffs + 15, yoffs + 4, xoffs + 15, yoffs + 10);
                p.drawLine(xoffs + 12, yoffs + 15, xoffs + 18, yoffs + 15);
            }
        }
    }

    return &pixmap;
}

QSize TiledWebView::getTileFieldDim()
{
    QSizeF vpSize = static_cast<QGraphicsWidget*>(parentItem())->size();
    qreal heightMult = cBigSideTileOverHead;
    qreal widthMult = cSmallSideTileOverHead;
    if(vpSize.width() > vpSize.height())
        qSwap(widthMult, heightMult);
    return QSize((int)((vpSize.width() * widthMult + cTileSize) / cTileSize),
                 (int)((vpSize.height() * heightMult + cTileSize) / cTileSize));
}

void TiledWebView::createTileField()
{
    QRectF vpRect = viewPortRect();

    m_tilesDim = getTileFieldDim();
    m_tilesPool.reserve(m_tilesDim.width() * m_tilesDim.height());


    m_tilesField = new Tile*[m_tilesDim.width() * m_tilesDim.height()];
    memset(m_tilesField, 0, sizeof(Tile*) * m_tilesDim.width() * m_tilesDim.height());
    m_tilesScale = scale();

    adjustTilesToViewPort(true);// mapFromTileCoords(QRectF(QPointF(3, 5) * cTileSize, m_tilesDim * cTileSize));
}

QRectF TiledWebView::validateTileRect(const QRectF& rect, const QSize& dim) const
{
    QRectF ret(rect);
    QRectF vpRect = viewPortRect();
    qreal tileSize = cTileSize / m_tilesScale;

    if(ret.bottom() > size().height() + tileSize)
        ret.moveBottom(size().height() + tileSize);
    if(ret.top() < 0)
        ret.moveTop(0);

    if(ret.right() > size().width() + tileSize)
        ret.moveRight(size().width() + tileSize);
    if(ret.left() < 0)
        ret.moveLeft(0);

    if(ret.width() < vpRect.width())
        ret.setLeft(0);

    if(ret.height() < vpRect.height())
        ret.setTop(0);

    QPointF p = mapToTileCoords(ret.topLeft());
    // allign coordinates to tile boundary
    QPoint pp = (p / cTileSize).toPoint();
    p = QPointF(pp) * cTileSize;

    return mapFromTileCoords(QRectF(p, dim * cTileSize));
}

QRectF TiledWebView::adjustedTileRect(const QSize& dim) const
{
    QRectF ret(m_tilesRect);
    if(ret.isEmpty())
        ret = QRectF(QPoint(0,0),mapFromTileCoords(dim * cTileSize));
    // no repositioning and scaling and tile dropping during scaling
    if(!qFuzzyCompare(m_tilesScale,scale()))
        return ret;

    QRectF vpRect = viewPortRect();
    qreal tileSize = cTileSize / m_tilesScale;
    if(vpRect.bottom() > ret.bottom())
        ret.moveTop(vpRect.top() - tileSize);
    else if(vpRect.top() < ret.top())
        ret.moveBottom(vpRect.bottom() + tileSize);

    if(vpRect.right() > ret.right())
        ret.moveLeft(vpRect.left() - tileSize);
    else if(vpRect.left() < ret.left())
        ret.moveRight(vpRect.right() + tileSize);

    return validateTileRect(ret, dim);
}

QRectF TiledWebView::centeredTileRect(const QSize& dim) const
{
    QRectF vpRect = viewPortRect();
    QSizeF tilesSize = mapFromTileCoords(dim * cTileSize);
    QPoint centerOffset(tilesSize.width() / 2, tilesSize.height() / 2);
    QRectF centeredRect(vpRect.center() - centerOffset,tilesSize);

    return validateTileRect(centeredRect, dim);
}

void TiledWebView::adjustTilesToViewPort(bool center)
{
    QRectF newTilesRect = center ? centeredTileRect(m_tilesDim) :
                                   adjustedTileRect(m_tilesDim);
    m_tilesRectCentered = center;

    moveTilesRect(newTilesRect);
}

void TiledWebView::moveTilesRect(const QRectF& newTilesRect)
{
    QRectF trNew = mapToTileCoords(newTilesRect);
    QRectF trOld = mapToTileCoords(m_tilesRect);

    if(trNew == trOld) return;

    if(trNew.intersects(trOld)) {
        QPoint trDiff = ((trNew.topLeft() - trOld.topLeft()) / cTileSize).toPoint();
        scrollTileField(-trDiff);
    } else {
        resetTiles(QRect(QPoint(0,0), m_tilesDim), false);
#ifdef TILEPOOL_DEBUG
        checkTilesField();
#endif
    }
    m_tilesRect = newTilesRect;
}

void TiledWebView::resetTiles(const QRect& r, bool remove)
{
    for(int j = r.top(); j <= r.bottom(); j++)
        for(int i = r.left(); i <= r.right(); i++) {
            QPoint t(i, j);
            Tile* tile = tileAt(t);
            if(tile) {
                tile->ready = false;
                tile->dirtyRect = QRect();
                if(remove) {
                    tile->used = false;
                    setTileAt(t, 0);
                }
            }
        }
}

void TiledWebView::scrollTileField(const QPoint& diff)
{
#ifdef TILEPOOL_DEBUG
    checkTilesField();
#endif
    if(qAbs(diff.x()) > m_tilesDim.width() || qAbs(diff.y()) > m_tilesDim.height())
        return;

    if(diff.x() > 0) {
        resetTiles(QRect(QPoint(m_tilesDim.width() - diff.x(),0),
                         QSize(diff.x(), m_tilesDim.height())), true);

        for(int i = m_tilesDim.width() - diff.x() - 1; i >= 0; i--) {
            int dstNum = i + diff.x();

            for(int j = 0; j < m_tilesDim.height(); j++) {
                setTileAt(dstNum, j, tileAt(i, j));
                setTileAt(i, j, 0);
            }
        }
    } else if(diff.x() < 0) {
        resetTiles(QRect(QPoint(0,0),
                         QSize(-diff.x(), m_tilesDim.height())), true);

        for(int i = -diff.x(); i < m_tilesDim.width(); i++) {
            int dstNum = i + diff.x();

            for(int j = 0; j < m_tilesDim.height(); j++) {
                setTileAt(dstNum, j, tileAt(i, j));
                setTileAt(i, j, 0);
            }
        }
    }

    if(diff.y() > 0) {
        resetTiles(QRect(QPoint(0, m_tilesDim.height() - diff.y()),
                         QSize(m_tilesDim.width(), diff.y())), true);

        for(int i = m_tilesDim.height() - diff.y() - 1; i >= 0; i--) {
            int dstNum = i + diff.y();
            Tile **srcLine = m_tilesField + m_tilesDim.width() * i;
            Tile **dstLine = m_tilesField + m_tilesDim.width() * dstNum;
            memcpy(dstLine, srcLine, m_tilesDim.width() * sizeof(Tile*));
            memset(srcLine, 0, m_tilesDim.width() * sizeof(Tile*));
        }
    } else if(diff.y() < 0) {
        resetTiles(QRect(QPoint(0, 0),
                         QSize(m_tilesDim.width(), -diff.y())), true);

        for(int i = -diff.y(); i < m_tilesDim.height(); i++) {
            int dstNum = i + diff.y();
            Tile **srcLine = m_tilesField + m_tilesDim.width() * i;
            Tile **dstLine = m_tilesField + m_tilesDim.width() * dstNum;
            memcpy(dstLine, srcLine, m_tilesDim.width() * sizeof(Tile*));
            memset(srcLine, 0, m_tilesDim.width() * sizeof(Tile*));
        }
    }
#ifdef TILEPOOL_DEBUG
    checkTilesField();
#endif
}

QList<QRectF> TiledWebView::updateViewportTiles(QList<TileSet> *updatedTiles)
{
    QList<QRectF> ret;
    // update all visible tiles
    QRectF vpRect = viewPortRect();

    QPoint topLeft = tileAtPoint(vpRect.topLeft());
    QPoint bottomRight = tileAtPoint(vpRect.bottomRight());

    bottomRight += QPoint(1, 1);
    boundTile(topLeft);
    boundTile(bottomRight);
    for(int j = topLeft.y(); j <= bottomRight.y(); j++)
        for(int i = topLeft.x(); i <= bottomRight.x(); i++) {
            QPoint t(i, j);
            Tile *tile = tileAt(t);
            if(!tile || !tile->ready || !tile->dirtyRect.isEmpty()) {
                QRectF r = updateTile(t);
                ret += r;
                if(updatedTiles)
                    *updatedTiles += TileSet(t, r);
            }
        }

    m_needViewportTilesUpdate = false;
    return ret;
}

void TiledWebView::doScaleCommit()
{
    m_needScaleCommit = false;
    if(qFuzzyCompare(m_tilesScale, scale()))
        return;

    resetTiles(QRect(QPoint(0,0), m_tilesDim), true);
#ifdef TILEPOOL_DEBUG
   checkTilesField();
#endif
    m_tilesScale = scale();
    adjustTilesToViewPort(true);
    m_needViewportTilesUpdate = true;
}

void TiledWebView::commitZoom()
{
    m_needScaleCommit = true;

    startUpdateTimer();
}

QList<QRectF> TiledWebView::updateScrollAreaTilesChunk(QList<TileSet> *updatedTiles, bool inPaint)
{
    QList<QRectF> dirtyRects;
    QList<QPoint> lst = findTileLine4Update(false, true);
    if(lst.isEmpty())
        lst = findTileLine4Update(false, false);
    if(lst.isEmpty())
        lst = findTileLine4Update(false, true, false);
//    if(!inPaint) {
    {
        if(lst.isEmpty())
            lst = findTileLine4Update(true, true);
        if(lst.isEmpty())
            lst = findTileLine4Update(true, false);
        if(lst.isEmpty())
            lst = findTileLine4Update(true, false, false);
    }

    QTime ts;
    ts.start();
    foreach(QPoint t, lst) {
        QRectF r = updateTile(t);
        dirtyRects += r;
        if(updatedTiles)
            *updatedTiles += TileSet(t, r);
        if(inPaint && ts.elapsed() > cInPaintTileUpdateTimeout)
            break;
    }

    return dirtyRects;
}


void TiledWebView::updateTimeout()
{
    if(m_tilesFrozen) return;
    if(m_inUpdate) return;

    int elapsed = m_userPaintTS.elapsed();
    QList<QRectF> dirtyTiles;

    if(m_needTilesFieldRebuild) {
        doTilesFieldRebuild();
    } else if(m_needScaleCommit) {
        doScaleCommit();
    } else if(m_needViewportTilesUpdate) {
/*  just do nothing, because it will update tiles below
        dirtyTiles += updateViewportTiles(); */
    } else if(elapsed < cPaintIdleTimeout) {
//        updateSceneRects(updateScrollAreaTilesChunk);

/*        QList<QRectF> rects = updateViewportTiles();
        foreach(QRectF r, rects)
            update(r); */
        return;
    }

    if(elapsed > cTileScaleUpdateTimeout && !qFuzzyCompare(m_tilesScale, scale())) {
         doScaleCommit();
    }
//    else if(elapsed > cTileRectRecenterTimeout && !m_tilesRectCentered)
//        adjustTilesToViewPort(true);

    dirtyTiles += updateViewportTiles();
    if(dirtyTiles.isEmpty())
        dirtyTiles = updateScrollAreaTilesChunk();

    if(dirtyTiles.isEmpty())
        for(int i = 0; i < cIdleTileUpdateChunkSize; i++) {
            // 1st try to paint not ready tiles in view
            QPoint oneDirtyTile = findTile4Update(true);
            // 2nd update dirty tiles in view
            if(oneDirtyTile.x() < 0) oneDirtyTile = findTile4Update(true, true);
            // 3rd try to paint not ready tiles everywhere else
            if(oneDirtyTile.x() < 0) oneDirtyTile = findTile4Update(false);
            // 4th update all other dirty tiles
            if(oneDirtyTile.x() < 0) oneDirtyTile = findTile4Update(false, true);
            if(oneDirtyTile.x() >= 0)
                dirtyTiles += updateTile(oneDirtyTile);
            else if(/*m_tilesRectCentered && */qFuzzyCompare(m_tilesScale, scale())) {
                stopUpdateTimer();
                break;
            }
        }

    m_inUpdate = true;

    updateSceneRects(dirtyTiles);

    m_inUpdate = false;
    
    // restart timer if some of update flags was set during recursive calls from webkit render
    if(m_needTilesFieldRebuild || m_needScaleCommit || m_needViewportTilesUpdate)
        startUpdateTimer();
}

void TiledWebView::updateSceneRects(const QList<QRectF>& dirtyTiles)
{
    QGraphicsScene* s = scene();
    QList<QGraphicsView*> gvList = s->views();

    QRectF vpRect = viewPortRect();

    foreach(QGraphicsView* v, gvList) {
        QRegion reg;
        foreach(QRectF r, dirtyTiles) {
            r = r.intersected(vpRect);
            update(r);
            r = mapRectToScene(r);
            r = v->mapFromScene(r).boundingRect();
            reg += r.toRect();
        }
//        v->repaint(reg);
    }
}

void TiledWebView::scheduleScaleUpdate()
{
    if(!m_tilesField) return;

    startUpdateTimer();
}

void TiledWebView::checkTilesField()
{
    int usedCount1 = 0;
    for(int j = 0; j < m_tilesDim.height(); j++)
        for(int i = 0; i < m_tilesDim.width(); i++) {
            Tile* t = tileAt(i, j);
            if(t) {
                Q_ASSERT(t->used);
                usedCount1++;
            }
        }

    int usedCount2 = 0;
    for(int i = 0; i < m_tilesPool.count(); i++)
        if(m_tilesPool[i] && m_tilesPool[i]->used)
            usedCount2++;

    Q_ASSERT(usedCount1 == usedCount2);
}

void TiledWebView::doTilesFieldRebuild()
{
    QSize oldDim = m_tilesDim;
    QSize newDim = getTileFieldDim();
    
    if(!qFuzzyCompare(m_tilesScale, scale())) {
        resetTiles(QRect(QPoint(0,0), m_tilesDim), true);
        m_tilesScale = scale();
        
        QRectF newRect = adjustedTileRect(newDim);
            
        Tile** newField = new Tile*[newDim.width() * newDim.height()];
        memset(newField, 0, sizeof(Tile*) * newDim.width() * newDim.height());
        delete[] m_tilesField;
        m_tilesField = newField;
        m_tilesDim = newDim;
        m_tilesRect = newRect;

        m_needViewportTilesUpdate = true;

    } else { 
        Tile** oldField = m_tilesField;
        QRectF newRect = adjustedTileRect(newDim);
        
        QRectF trNew = mapToTileCoords(newRect);
        QRectF trOld = mapToTileCoords(m_tilesRect);
    
        if(trNew != trOld) {
    
            Tile** newField = new Tile*[newDim.width() * newDim.height()];
            memset(newField, 0, sizeof(Tile*) * newDim.width() * newDim.height());
            QRectF trCommon = trNew.intersect(trOld);
    
            if(!trCommon.isEmpty()) {
                QSize copySize = (trCommon.size() / cTileSize).toSize();
                QPoint oldOffs = ((trCommon.topLeft() - trOld.topLeft()) / cTileSize).toPoint();
                QPoint newOffs = ((trCommon.topLeft() - trNew.topLeft()) / cTileSize).toPoint();
                if(trNew.size().width() - newOffs.x() < copySize.width())
                    copySize.setWidth(trNew.size().width() - newOffs.x());
                if(trNew.size().height() - newOffs.y() < copySize.height())
                    copySize.setHeight(trNew.size().height() - newOffs.y());
                if(trOld.size().width() - oldOffs.x() < copySize.width())
                    copySize.setWidth(trOld.size().width() - oldOffs.x());
                if(trOld.size().height() - oldOffs.y() < copySize.height())
                    copySize.setHeight(trOld.size().height() - oldOffs.y());
    
                for(int j = 0; j < copySize.height(); j++)
                    for(int i = 0; i < copySize.width(); i++) {
                    QPoint cPoint(i, j);
                    QPoint oldPos = cPoint + oldOffs;
                    Tile *tile = tileAt(oldPos);
                    setTileAt(oldPos, 0);
                    QPoint newPos = cPoint + newOffs;
                    Q_ASSERT(newPos.x() >= 0 && newPos.y() >=0 &&
                             newPos.x() < newDim.width() && newPos.y() < newDim.height());
                    *(newField + newPos.y() * newDim.width() + newPos.x()) = tile;
                }
            }
    
            // release remaining tiles in old field
            resetTiles(QRect(QPoint(0,0), m_tilesDim), true);
    
            delete[] m_tilesField;
            m_tilesField = newField;
            m_tilesDim = newDim;
            m_tilesRect = newRect;
    
            int newTileCount = m_tilesDim.height() * m_tilesDim.width();
            while(m_tilesPool.count() > newTileCount) {
                bool deleted = false;
                for(int i = m_tilesPool.count() - 1; i >= 0; i--) {
                    Tile* tile = m_tilesPool[i];
                    if(!tile->used) {
                        deleted = true;
                        if(tile) delete tile;
                        m_tilesPool.remove(i);
                        break;
                    }
                }
                Q_ASSERT(deleted);
            }

            m_needViewportTilesUpdate = true;
        }
    }

    m_needTilesFieldRebuild = false;
}

void TiledWebView::viewportUpdated()
{
    if(!m_tilesField) {
        createTileField();
        commitZoom();
    }
    else {
        m_needTilesFieldRebuild = true;
        startUpdateTimer();
    }
}

// #define DRAW_TILE_BOUNDS
void TiledWebView::paintTile(QPainter* painter, const QPoint& t, const QRectF& clipRect, QRegion& dirtyRegion)
{
    QRectF tRectOrig = tileRect(t);
    qreal adjust = 1; // + m_tilesScale;
    QRectF tRect = tRectOrig.adjusted(-adjust, -adjust, adjust, adjust);
    QRectF drawRect = clipRect.intersected(tRect);
    //painter->drawPixmap(tRectOrig,tileAt(t)->img, tRectOrig.translated(-tRectOrig.topLeft()));
    painter->drawPixmap(tRectOrig.topLeft(),tileAt(t)->img);
#ifdef DRAW_TILE_BOUNDS
    painter->setPen(Qt::red);
    painter->drawRect(tileRect(t));
#endif // DRAW_TILE_BOUNDS
    dirtyRegion = dirtyRegion.subtract(QRegion(mapFromTileCoords(drawRect).toRect()));
}

void TiledWebView::paint(QPainter* painter, const QStyleOptionGraphicsItem* options, QWidget* widget)
{

    if(!m_tilesField) {
        QGraphicsWebView::paint(painter, options, widget );
        return;
    }

    painter->save();
    QRectF clipRect = viewPortRect().adjusted(-1, -1, 1, 1);
    if(options && !options->exposedRect.isEmpty())
        clipRect &= options->exposedRect;

    QList<QRectF> updatedTileRects;
    QList<TileSet> updatedTiles;
    if(!m_inUpdate && !m_tilesFrozen) {
        QList<QRectF> lst;
        if(m_userPaintTS.elapsed() > cPaintIdleTimeout || m_needViewportTilesUpdate) {
            lst = updateViewportTiles(&updatedTiles);
            m_needViewportTilesUpdate = false;
        } else
            lst = updateScrollAreaTilesChunk(&updatedTiles, true);
        QRectF vpRect =  viewPortRect().adjusted(-1, -1, 1, 1);
        foreach(QRectF r, lst) {
            r = r.intersected(vpRect);
            if(!r.isEmpty()) {
                if(clipRect.contains(r)) {
                    // do nothing, it will be updated in any case
                } else if(r.contains(clipRect)) {
                    clipRect = r;
                } else if(clipRect.intersects(r)) {
                    clipRect = clipRect.unite(r);
                } else {
                    updatedTileRects += r;
                }
            }
        }
    }

    painter->setBackgroundMode(Qt::OpaqueMode);

    painter->setClipRect(clipRect, Qt::IntersectClip);

    QRectF tileClipRect = mapToTileCoords(clipRect);
    QRegion dirtyRgn(clipRect.toRect());
    QPoint topLeftTile = tileAtPoint(clipRect.topLeft());
    QPoint rightBottomTile = tileAtPoint(clipRect.bottomRight());

    boundPoint(QPoint(0,0),topLeftTile,topPoint(m_tilesDim));
    boundPoint(QPoint(0,0),rightBottomTile,topPoint(m_tilesDim));

    qreal sc = scale();
    QPointF p = pos() / sc;

    QRectF scr(clipRect.topLeft() + p, clipRect.size() * sc);

    QRegion notReadyClip(scr.toRect()); //.adjusted(-2, -2, 2, 2).toRect());
    for(int j = topLeftTile.y(); j <= rightBottomTile.y(); j++)
        for(int i = topLeftTile.x(); i <= rightBottomTile.x(); i++) {
            QPoint t(i,j);
            if(tileAt(t) && tileAt(t)->ready) {
                QRectF r = mapFromTileCoords(tileRect(t));
                r = QRectF(r.topLeft() * sc, r.size() * sc);
                r.translate(pos());
                notReadyClip = notReadyClip.subtract(r.toRect());
//               painter->setPen(Qt::red);
//                painter->drawRect(r);
            }
        }

//    painter->setPen(Qt::red);
//    painter->drawRect(scr.adjusted(10,10,-10,-10));

    QVector<QRect> rList = notReadyClip.rects();
    if(!rList.isEmpty()) {
        painter->save();
        painter->translate(-p);
        painter->scale(1/sc, 1/sc);
        foreach(QRect r, rList) {
            painter->fillRect(r, QBrush(*getUnprepPixmap()));
            //        painter->setPen(Qt::red);
            //        painter->drawRect(r.adjusted(10, 10, -10, -10));
        }
        painter->restore();
    }

    painter->scale(1 / m_tilesScale, 1 / m_tilesScale);

    for(int j = topLeftTile.y(); j <= rightBottomTile.y(); j++)
        for(int i = topLeftTile.x(); i <= rightBottomTile.x(); i++) {
            QPoint t(i,j);
            if(tileAt(t) && tileAt(t)->ready)
                paintTile(painter, t, tileClipRect, dirtyRgn);
        }
    QRect clippedRectTiles(topLeftTile,rightBottomTile);
    foreach(TileSet ts, updatedTiles)
        if(!clippedRectTiles.contains(ts.t))
            paintTile(painter, ts.t, ts.r, dirtyRgn);

//    if(!m_inUpdate)
//        m_userPaintTS.start();

    if(!m_tilesFrozen && !m_tilesRect.contains(viewPortRect()) && qFuzzyCompare(scale(),m_tilesScale)) {
        adjustTilesToViewPort();
        startUpdateTimer();
    }
/*    painter->setPen(Qt::red);
    painter->drawLine(0, 0, 100, 100);
*/
    painter->restore();
}

void TiledWebView::setTiledBackingStoreFrozen(bool frozen)
{
    m_tilesFrozen = frozen;
    if(frozen) {
        if(m_updateTimer.isActive())
            stopUpdateTimer();
    } else {
        if(!qFuzzyCompare(scale(),m_tilesScale))
            commitZoom();
        else {
            // m_needViewportTilesUpdate = true;
            m_tilesRectCentered = false;
        }

        startUpdateTimer();
    }
}

void TiledWebView::startUpdateTimer()
{
    if(!m_updateTimer.isActive() && !m_tilesFrozen) {
        m_updateTimer.start(cTileUpdateTimerTick);
    }
}

void TiledWebView::stopUpdateTimer()
{
    m_updateTimer.stop();
}

void TiledWebView::userActivity()
{
    m_userPaintTS.start();
}

void TiledWebView::viewScrolled(QPoint& scrollPos, QPoint& delta)
{
    m_lastScrollDelta = delta;

    userActivity();
    if(!m_tilesField) return;

    QRectF ret(m_tilesRect);
    if(ret.isEmpty())
        ret = QRectF(QPoint(0,0),mapFromTileCoords(m_tilesDim * cTileSize));
    // no repositioning and scaling and tile dropping during scaling
    if(!qFuzzyCompare(m_tilesScale,scale()))
        return;

    QRectF vpRect = viewPortRect();
    qreal tileSize = cTileSize / m_tilesScale;
    vpRect.adjust(-tileSize, -tileSize, tileSize, tileSize);
    if(vpRect.bottom() > ret.bottom() && delta.y() > 0)
        ret.moveTop(vpRect.top() - tileSize);
    else if(vpRect.top() < ret.top() && delta.y() < 0)
        ret.moveBottom(vpRect.bottom() + tileSize);

    if(vpRect.right() > ret.right() && delta.x() > 0)
        ret.moveLeft(vpRect.left() - tileSize);
    else if(vpRect.left() < ret.left() & delta.x() < 0)
        ret.moveRight(vpRect.right() + tileSize);

    ret = validateTileRect(ret, m_tilesDim);

    moveTilesRect(ret);
}

#ifdef USE_ASSISTANT_ITEM

void TiledWebView::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    m_assistant->setGeometry(QRectF(QPoint(0,0), event->newSize()));
}

void TiledWebViewAssistant::paint(QPainter* painter, const QStyleOptionGraphicsItem* options, QWidget* widget)
{
    m_master->paint(painter, options, widget);
}
#endif
