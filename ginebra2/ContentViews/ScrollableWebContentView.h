/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not,
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#ifndef ScrollableWebContentView_h
#define ScrollableWebContentView_h

#include "Gestures/GestureEvent.h"
#include "Gestures/GestureListener.h"
#include "Gestures/GestureRecognizer.h"
#include "ScrollableViewBase.h"
#include "webpagedata.h"
#include <QPropertyAnimation>
#include <QTimer>
#include <QWebHitTestResult>
#include "uitimer.h"

class QStm_Gesture;
class QWebPage;

namespace GVA {


#define ZOOM_ANIM_TIME    300

class ViewportMetaData;
class WebView;
class ScrollHelper;

class ScrollableWebContentView : public ScrollableViewBase, protected GestureListener {
    Q_OBJECT
public:
    ScrollableWebContentView(WebView* scrolledWidget, QGraphicsItem* parent = 0);
    ~ScrollableWebContentView();

    WebView* webView() const;
    void stepZoom(bool zoomIn);
    void toggleZoom(bool zoomIn);
    WebPageData pageDataFromViewportInfo();
    void setPageDataToViewportInfo(const WebPageData& data);
    WebPageData defaultZoomData();
    void updatePreferredContentSize();
    void setSuperPage();
    bool isOrientationChanged(QSizeF oldSize, QSizeF newSize);
    bool isChangedToLandscape(QSizeF oldSize, QSizeF newSize);
    bool isChangedToPortrait(QSizeF oldSize, QSizeF newSize);
    qreal zoomScale() const;
    void setZoomScale(qreal value, bool immediateCommit = false);
    Q_PROPERTY(qreal zoom READ zoomScale WRITE setZoomScale)

    // viewableRect is in page coordinates
    QRectF viewableRect();
    void setViewableRect(const QRectF& rect);
    Q_PROPERTY(QRectF viewableRect READ viewableRect WRITE setViewableRect)

    // page scroll position in page coordiantes
    void scrollPageTo(const QPointF& pos) { if(m_isScrolling) setScrollPosition((pos * zoomScale()).toPoint()) ; }
    QPointF pageScrollPos() const { return scrollPosition() / zoomScale(); }
    Q_PROPERTY(QPointF pageScrollPosition READ pageScrollPos WRITE scrollPageTo)

    Q_PROPERTY(QPoint widgetScrollPosition READ scrollPosition WRITE setScrollPosition)
    Q_PROPERTY(QPoint maxScrollPosition READ maximumScrollPosition);

    bool event(QEvent * e);
    bool eventFilter(QObject* o, QEvent* e);
    bool isSuperPage() { return m_isSuperPage; }
    void setPage(QWebPage* page);
    
    void setGesturesEnabled(bool value); 
    bool gesturesEnabled();
    
    // fill empty areas with white
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
Q_SIGNALS:
    void contextEventObject(QWebHitTestResult* hitTest, QPointF position);
    void updateZoomActions(bool enableZoomIn, bool enableZoomOut);
    void scaleChanged(qreal scale);
    void viewPortChanged();
    void mouseEvent(QEvent::Type type);
    void contentViewMouseEvent(QEvent::Type type);

public Q_SLOTS:
    void reset();
    void contentsSizeChanged(const QSize&);
    void pageLoadStarted();
    void pageLoadProgress(int);
    void pageLoadFinished(bool);
    void touchDownCallback();
    void hoverCallback();
    
protected:
    bool sceneEventFilter(QGraphicsItem*, QEvent*);
    void updateViewportMetaDataFromPageTag();
    QSizeF parentSize() const;

    //From GestureListener
    void handleGesture(GestureEvent*);

    //Helpers
    void handlePress(GestureEvent*);
    void handleRelease(GestureEvent*);
    void handleFlick(GestureEvent*);
    void handleDoubleTap(GestureEvent*);
    void handlePan(GestureEvent*);
    void handleLongTap(GestureEvent*);

    void setViewportWidgetGeometry(const QRectF& r);
    QRectF validateViewportRect(const QRectF& rect); // rect in page coordinates

//    void startZoomAnimToItemHotspot(const QPointF& hotspot, const QPointF& viewTargetHotspot, qreal scale, QRectF target = QRectF()); // OBS
    bool isZoomedIn() const;
    bool isZooming();

    //To handle kinetic scroller state changes
    void stateChanged(KineticScrollable::State oldState, KineticScrollable::State newState);
    void zoomToHotSpot(const QPointF& viewportHotSpot, const qreal destScale);
    void startZoomAnimationToHotSpot(const QPointF& viewportHotSpot, const qreal destScale, int animTime = ZOOM_ANIM_TIME);
    void startZoomAnimation(const QRectF& destViewRect, int animTime = ZOOM_ANIM_TIME); // destViewRect in page coordinates
    void stopZoomAnimation();
    void updateZoomEndRect();
    void resizeEvent(QGraphicsSceneResizeEvent* event);
    void adjustViewportSize(QSizeF oldSize, QSizeF newSize);
    void sendEventToWebKit(QEvent::Type type, const QPointF& scenePos, bool select = false);
    QRectF findZoomableRectForPoint(const QPointF& point); // point in view coordinates, returns rect in page coordinates
    void notifyZoomActions(qreal newScale);
    void startScrollAnimation(QPointF& scrollPos, QPointF& targetPos, 
                              int duration, QEasingCurve::Type& easingCurveType);

   
protected Q_SLOTS:
    void zoomAnimationStateChanged(QAbstractAnimation::State newState, QAbstractAnimation::State);
    void enableContentUpdates();
    void disableContentUpdates();
    void commitZoom();
    void stopScrolling();

private:
    void zoomAtPoint(QPointF touchPoint);
    bool handleQStmGesture(QStm_Gesture* gesture);
    bool doTouch(QStm_Gesture* gesture);
    bool doTap(QStm_Gesture* gesture);
    bool doMaybeTap(QStm_Gesture* gesture);
    bool doPan(QStm_Gesture* gesture);
    bool doFlick(QStm_Gesture* gesture);
    bool doDoubleTap(QStm_Gesture* gesture);
    bool doPinch(QStm_Gesture* gesture);
    bool doLongPress(QStm_Gesture* gesture);
    bool doRelease(QStm_Gesture* gesture);
    bool doLeftRight(QStm_Gesture* gesture);
    bool doUpDown(QStm_Gesture* gesture);
    qreal calcScale(int origDistance, QPointF p1, QPointF p2);
    qreal calcDistance(QPointF pt1, QPointF pt2);
    QPointF calcCenter(QPointF pt1, QPointF pt2);
    int zoomAnimationTime(bool zoomIn, qreal targetScale);
    QRectF viewportRectInPageCoord(const QPointF& viewportHotSpot, 
                                   const qreal destScale);
    void dehighlightWebElement();
    void initScrollHelper();
    QWebHitTestResult  hitTest(const QPointF& scenePos);
    bool toggleInputMethod(bool on);
    bool toggleVkb();
    bool inputMethodEnabled();
    
private:
    QRectF m_animationEndRect;
    QPropertyAnimation* m_zoomAnimator;          //Animates zooming transitions
    ViewportMetaData* m_viewportMetaData;
    QTimer m_tileUpdateEnableTimer;
    QTimer m_zoomCommitTimer;
    int m_pinchStartDistance;
    qreal m_pinchStartScale;
    bool m_isSuperPage;
    bool m_gesturesEnabled;    // bookmarks and history need this
    UiTimer* m_touchDownTimer;
    UiTimer* m_hoverTimer;
    QPointF  m_touchDownPos;
    QTime    m_pinchFinishTime;
    bool     m_isScrolling;
    bool     m_isLoading;

    ScrollHelper*   m_scrollHelper;
    
    GestureRecognizer m_gestureRecognizer;
    QTimer            m_viewportMetadataResetTimer;
    QWebHitTestResult m_hitTest;
    bool              m_isInputOn;
    bool              m_ignoreNextRelease;
}; //ScrollableWebContentView

} //namespace GVA

#endif //ScrollableWebContentView_h
