#ifndef TILEDWEBVIEW_H
#define TILEDWEBVIEW_H

#include <QGraphicsWebView>

#include <QTime>
#include <QTimer>

#ifdef USE_OWN_TILED_CACHE
#define USE_TILED_CACHE
#endif

//#define USE_ASSISTANT_ITEM

#ifdef USE_ASSISTANT_ITEM
class TiledWebView;
class TiledWebViewAssistant : public QGraphicsWidget {
public:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* options, QWidget* widget);
    TiledWebView* m_master;
};
#endif

class TiledWebView : public QGraphicsWebView
{
    Q_OBJECT
protected:
    TiledWebView(QGraphicsItem* parent = 0);
    virtual ~TiledWebView();

    struct Tile {
        Tile();
        QPixmap img;
        QRectF dirtyRect; // in tile coordinates
        bool ready;
        bool used;
    };

    struct TileSet {
        TileSet(QPoint aT, QRectF aR) { t = aT; r = aR; }
        TileSet(const TileSet &o) { t = o.t; r = o.r; }
        QPoint t;
        QRectF r;
    };

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* options, QWidget* widget);

    Tile* createTile(const QPoint& p);
    Tile* tileAt(const QPoint& p) const;
    Tile* tileAt(int x, int y) const { return tileAt(QPoint(x, y)); }
    void setTileAt(const QPoint& p,Tile* t);
    void setTileAt(int x, int y,Tile* t) { setTileAt(QPoint(x, y), t); }
    QPoint tileAtPoint(const QPointF& p) const;
    QRectF tileRect(const QPoint& t) const; // return is in tile coords
    QRectF mapToTileCoords(const QRectF& rect) const
        { return QRectF(mapToTileCoords(rect.topLeft()),mapToTileCoords(rect.size())); }

    QPointF mapToTileCoords(const QPointF& p) const     { return p * m_tilesScale; }
    QSizeF mapToTileCoords(const QSizeF& s) const     { return s * m_tilesScale; }

    QRectF mapFromTileCoords(const QRectF& rect) const
        { return QRectF(mapFromTileCoords(rect.topLeft()),mapFromTileCoords(rect.size())); }

    QPointF mapFromTileCoords(const QPointF& p) const     { return p / m_tilesScale; }
    QSizeF mapFromTileCoords(const QSizeF& s) const     { return s / m_tilesScale; }

    QRectF viewPortRect() const;
    QPoint findTile4Update(bool inView, bool addDirty = false) const;
    QList<QPoint> findTileLine4Update(bool dirty, bool inView, bool useScrollDirection = true) const;
    void boundTile(QPoint& t) const;
    QRectF updateTile(const QPoint& t);
    void paintTile(QPainter* painter, const QPoint& t, const QRectF& dirtyRect, QRegion& dirtyRegion);
    void createTileField();


    QRectF validateTileRect(const QRectF& rect, const QSize& dim) const;
    QRectF adjustedTileRect(const QSize& dim) const;
    QRectF centeredTileRect(const QSize& dim) const;
    void adjustTilesToViewPort(bool center = false);
    void moveTilesRect(const QRectF& newTilesRect);
    void resetTiles(const QRect& r, bool remove);
    void scrollTileField(const QPoint& diff);
    static QPixmap* getUnprepPixmap();
    void checkTilesField();
    void setPage(QWebPage* page);
    QList<QRectF> updateViewportTiles(QList<TileSet> *updatedTiles = 0);
    QList<QRectF> updateScrollAreaTilesChunk(QList<TileSet> *updatedTiles = 0, bool inPaint = 0);
    void doScaleCommit();
    void doTilesFieldRebuild();
    void updateSceneRects(const QList<QRectF>& dirtyTiles);
#ifdef USE_ASSISTANT_ITEM
    void resizeEvent(QGraphicsSceneResizeEvent *event);
#endif
    QSize getTileFieldDim();

    QVector<Tile*> m_tilesPool;
    Tile** m_tilesField;
    QSize  m_tilesDim;
    QRectF m_tilesRect;
    qreal  m_tilesScale;
    bool   m_inUpdate;
    bool   m_tilesRectCentered;
    QTime  m_userPaintTS;
    QTimer m_updateTimer;
    bool   m_tilesFrozen;
    bool   m_needViewportTilesUpdate;
    bool   m_needScaleCommit;
    bool   m_needTilesFieldRebuild;
    QPoint m_lastScrollDelta;
#ifdef USE_ASSISTANT_ITEM
    TiledWebViewAssistant* m_assistant;
#endif // USE_ASSISTANT_ITEM

private slots:
    void repaintRequested(QRect);
    void updateTimeout();
    void scheduleScaleUpdate();
    void startUpdateTimer();
    void stopUpdateTimer();
    void loadStarted();

public slots:
    void viewportUpdated();
    void commitZoom();
    void setTiledBackingStoreFrozen(bool frozen);
    void userActivity();
    void viewScrolled(QPoint& scrollPos, QPoint& delta);

  friend class TiledWebViewAssistant;
};

#endif // TILEDWEBVIEW_H
