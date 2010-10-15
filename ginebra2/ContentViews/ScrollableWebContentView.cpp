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

#include "ScrollableWebContentView.h"
#include "Kinetics/KineticScroller.h"
#include "ScrollableViewBase.h"
#include "ViewportMetaDataParser.h"
#include "WebView.h"
#include "qstmgestureevent.h"
#include "qstmfilelogger.h"
#include "bedrockprovisioning.h"
#include "ScrollHelper.h"

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsWebView>
#include <QStyleOptionGraphicsItem>
#include <QWebElement>
#include <QWebHitTestResult>
#include <QWebPage>
#include <QWebHistory>
#include <QInputContext>

#include "Gestures/GestureRecognizer.h"
#ifdef ORBIT_UI
#include <hbinputmethod.h>
#endif // ORBIT_UI

namespace GVA {
//Kinetic scroll constants
static const int ScrollsPerSecond = 30;
static const int MinimumScrollVelocity = 10;
static const qreal AxisLockThreshold = .8;

//Zooming constants
const int ZoomAnimationDuration = 600;   //ms. Zooming transition duration
const int MaxZoomAnimationDuration = 2000;   //ms. Zooming transition duration
static const qreal ZoomStep = .5;               //Incremental zoom step
#ifdef Q_WS_MAEMO_5
const int TileUpdateEnableDelay = 10;         //Wait duration before tiling updates are enabled.
#else
const int TileUpdateEnableDelay = 150;         //Wait duration before tiling updates are enabled.
#endif
static const int MinDoubleClickZoomTargetWidth = 100;     //Target block width for applying double tap zoom
static const int ZoomCommitDuration = 60;                //Timeout before commiting zoom
static const qreal ZoomableContentMinWidth = 300.;

static const qreal InvalidCoord = 1e10;
static const int TouchDownTimeout = 200;
static const int HoverTimeout = 100;

#undef USE_KINETIC_SCROLLER
using namespace qstmGesture;

ScrollableWebContentView::ScrollableWebContentView(WebView* scrolledWidget, QGraphicsItem* parent)
    : ScrollableViewBase(parent)
    , m_gestureRecognizer(this)
    , m_isInputOn(false)
    , m_ignoreNextRelease(false)
{
    m_viewportMetaData = new ViewportMetaData();

    scrolledWidget->installEventFilter(this);
    
    //Kinetic scroller settings
    //Sets the number of scrolls (frames) per second to sps.
    m_kineticScroller->setScrollsPerSecond(ScrollsPerSecond);
    //For elastic scroll in page edges
    m_kineticScroller->setOvershootPolicy(KineticScroller::OvershootWhenScrollable);

    setWidget(scrolledWidget);

    m_tileUpdateEnableTimer.setSingleShot(true);
    connect(&m_tileUpdateEnableTimer, SIGNAL(timeout()), this, SLOT(enableContentUpdates()));
    m_zoomCommitTimer.setSingleShot(true);
    connect(&m_zoomCommitTimer, SIGNAL(timeout()),this,SLOT(commitZoom()));

    //Setup zooming animator
    m_zoomAnimator = new QPropertyAnimation(this, "viewableRect");
    //m_zoomAnimator->setDuration(ZoomAnimationDuration);
    connect(m_zoomAnimator, SIGNAL(stateChanged(QAbstractAnimation::State,QAbstractAnimation::State)), 
            this, SLOT(zoomAnimationStateChanged(QAbstractAnimation::State,QAbstractAnimation::State)));
    
    m_isSuperPage = false;
    m_gesturesEnabled = true;
    m_isLoading = false;
    m_touchDownTimer = UiTimer::New();
    m_touchDownTimer->setTimerCallback((char*)"touchDownCallback");
    
    m_hoverTimer = UiTimer::New();
    m_hoverTimer->setTimerCallback((char*)"hoverCallback");
        
    m_touchDownTimer->setPriority(150);
    m_pinchFinishTime = QTime::currentTime();
    // ScrollHelper
    m_scrollHelper = new ScrollHelper(this);
    initScrollHelper();
        
    //Gesture settings
    //For detecting scroll direction
    m_gestureRecognizer.setAxisLockThreshold(AxisLockThreshold);
    //To enable touch and drag scrolling
    m_gestureRecognizer.setMinimumVelocity(MinimumScrollVelocity);

    grabGesture(QStm_Gesture::assignedType());
    connect(this, SIGNAL(viewScrolled(QPoint&, QPoint&)), scrolledWidget, SLOT(viewScrolled(QPoint&, QPoint&)));
    installEventFilter(this);
}

void ScrollableWebContentView::initScrollHelper()
{
    qreal decel = BedrockSettings->value("KineticDeceleration").toDouble();
    m_scrollHelper->setDeceleration(decel);

    qreal maxFlickKoef = BedrockSettings->value("MaxFlickInViewportUnits").toDouble();
    qreal minFlickKoef = BedrockSettings->value("MinFlickInViewportUnits").toDouble();
    qreal midFlickKoef = BedrockSettings->value("MidFlickInViewportUnits").toDouble();
    m_scrollHelper->setFlickLimits(minFlickKoef, midFlickKoef, maxFlickKoef);

    qreal maxFlickSpeed = BedrockSettings->value("MaxFlickSpeed").toDouble();
    qreal minFlickSpeed = BedrockSettings->value("MinFlickSpeed").toDouble();
    qreal midFlickSpeed = BedrockSettings->value("MidFlickSpeed").toDouble();
    m_scrollHelper->setFlickSpeedLimits(minFlickSpeed, midFlickSpeed, maxFlickSpeed);

    int maxFlickDuration = BedrockSettings->value("MaxFlickDuration").toInt();
    int minFlickDuration = BedrockSettings->value("MinFlickDuration").toInt();
    int midFlickDuration = BedrockSettings->value("MidFlickDuration").toInt();
    m_scrollHelper->setFlickDurationLimits(minFlickDuration, midFlickDuration, maxFlickDuration);
    
    connect(m_scrollHelper, SIGNAL(scrollFinished()), this, SLOT(stopScrolling()));
    
    m_gesturesEnabled = true;
}

ScrollableWebContentView::~ScrollableWebContentView()
{
    delete m_viewportMetaData;
    if(m_zoomAnimator) {
        m_zoomAnimator->stop();
        delete m_zoomAnimator;
    }
    
    m_touchDownTimer->stop();
    delete m_touchDownTimer;
    
    m_hoverTimer->stop();
    delete m_hoverTimer;

}

void ScrollableWebContentView::stepZoom(bool zoomIn)
{
    //If viewport metadata has user scalable false.
    //Do not zoom.
    if (!m_viewportMetaData->m_userScalable)
        return;

    if (isZooming()) {
        stopZoomAnimation();
        return;
    }
    
    qreal scale = 1;
    scale += ZoomStep;

    if (!zoomIn)
        scale = 1/scale;

    qreal curScale =  scrollWidget()->scale();
    qreal destScale = curScale * scale;

    if (zoomIn && (destScale > m_viewportMetaData->m_maximumScale))
        destScale = m_viewportMetaData->m_maximumScale;
    else if (!zoomIn && (destScale < m_viewportMetaData->m_minimumScale))
        destScale = m_viewportMetaData->m_minimumScale;

    if(destScale == curScale)
        return;

    //Screen center
    //QPointF zoomHotSpot(size().width()/2, size().height()/2); //center zoom looks ugly in some cases
    QPointF zoomHotSpot(0, 0);
    
    startZoomAnimationToHotSpot(zoomHotSpot, destScale);
}


int ScrollableWebContentView::zoomAnimationTime(bool zoomIn, qreal targetScale)
{
    qreal curScale = zoomScale(); 
    qreal scaleRatio = zoomIn ? (targetScale / curScale) :
                            1.2 * (curScale / targetScale);
    qreal fullScaleRatio = (m_viewportMetaData->m_maximumScale / m_viewportMetaData->m_minimumScale);
    return MaxZoomAnimationDuration * scaleRatio / fullScaleRatio;
}

void ScrollableWebContentView::toggleZoom(bool zoomIn)
{
    m_scrollHelper->stopScrollNoSignal();
    if (isZooming()) {
        stopZoomAnimation();
        commitZoom();
        return;
    }
    qreal scale = zoomIn ? m_viewportMetaData->m_maximumScale : m_viewportMetaData->m_minimumScale;
    int t = zoomAnimationTime(zoomIn, scale);
    QPointF zoomHotSpot(0, 0);
    startZoomAnimationToHotSpot(zoomHotSpot, scale, t);
}

QRectF ScrollableWebContentView::viewportRectInPageCoord(const QPointF& viewportHotSpot, 
                                                         const qreal destScale)
{
    QSizeF vpSize = size();
    QSizeF contentSize = webView()->size();
    QSizeF scaleVpSize(vpSize.width() / destScale, vpSize.height() / destScale);
    QPointF contentHotSpot = webView()->mapFromParent(viewportHotSpot);
    QPointF scaledHotSpot = viewportHotSpot / destScale; 
    QRectF destViewRect( contentHotSpot - scaledHotSpot, scaleVpSize);
    return destViewRect;
}

void ScrollableWebContentView::startZoomAnimationToHotSpot(const QPointF& viewportHotSpot, 
                                                           const qreal destScale, int animTime)
{
    QRectF destViewRect = viewportRectInPageCoord(viewportHotSpot, destScale);
    startZoomAnimation(destViewRect, animTime);
}

void ScrollableWebContentView::zoomToHotSpot(const QPointF& viewportHotSpot, const qreal destScale)
{
    QRectF destViewRect = viewportRectInPageCoord(viewportHotSpot, destScale);
    destViewRect = validateViewportRect(destViewRect);
    setViewableRect(destViewRect);
}

WebPageData ScrollableWebContentView::pageDataFromViewportInfo()
{
    if(!m_viewportMetaData->m_isValid) return WebPageData();

    // No viewport data saving or restoring for superpages
    if(isSuperPage()) return WebPageData();

    // invalidate viewport meta data after saving to history
    // cannot do it earlier because loadStarted event comes earlier than save to history
    if(m_isLoading)
        m_viewportMetaData->m_isValid = false;

    bool fitToScreen = qFuzzyCompare(m_viewportMetaData->m_width / webView()->size().width(), zoomScale());

    return WebPageData(m_viewportMetaData->m_maximumScale, m_viewportMetaData->m_minimumScale,
                       m_viewportMetaData->m_userScalable, m_viewportMetaData->m_initialScale,
                       geometry(), webView()->geometry(), scrollWidget()->scale(), size(),
                       m_viewportMetaData->m_specifiedData.m_width, m_viewportMetaData->m_specifiedData.m_height,
                       fitToScreen);
}

QSizeF ScrollableWebContentView::parentSize() const
{
    return static_cast<QGraphicsWidget*>(parentItem())->size();
}

void ScrollableWebContentView::setPageDataToViewportInfo(const WebPageData& data)
{
    if(!data.isValid()) return;

    // No viewport data saving or restoring for superpages
    if(isSuperPage()) return;

    m_viewportMetaData->m_initialScale = data.initialScale;
    m_viewportMetaData->m_minimumScale = data.minScale;
    m_viewportMetaData->m_maximumScale = data.maxScale;
    m_viewportMetaData->m_userScalable = data.userScalable;
    m_viewportMetaData->m_specifiedData.m_width = data.specifiedWidth;
    m_viewportMetaData->m_specifiedData.m_height = data.specifiedHeight;
    m_viewportMetaData->m_isValid = true;
    m_viewportMetaData->m_width = data.viewportSize.width();
    m_viewportMetaData->m_height = data.viewportSize.height();


    if(m_viewportMetaData->m_width < 0 ||
       m_viewportMetaData->m_width < 0) {
        m_viewportMetaData->m_isValid = false;
        reset();
        return;
    }

    QSizeF newSize = parentSize();
    if(newSize.isEmpty())
        newSize = data.viewportSize;

    disableContentUpdates();
    //    updateViewportMetaDataFromPageTag();


    QSizeF vpSz(m_viewportMetaData->m_width, m_viewportMetaData->m_height);
    if(vpSz.isEmpty())
        vpSz = newSize;

    // sometimes on load from history webpage doesn't resize webView
    // set correct size of webView here
    webView()->setGeometry(QRectF( QPointF(0, 0),
                                   QSizeF(webView()->page()->mainFrame()->contentsSize())));

    qreal sc = data.scale; //qBound(m_viewportMetaData->m_minimumScale,data.scale,m_viewportMetaData->m_maximumScale);
    webView()->setScale(sc);

//    qreal fitToScreenScale = data.viewportSize.width() / webView()->size().width();
//    bool isFitToScreen = qFuzzyCompare(zoomScale(), fitToScreenScale);
    adjustViewportSize(data.viewportSize, newSize);
    m_viewportMetaData->adjustZoomValues(webView()->size());
    if(data.fitToScreen && newSize.width() != vpSz.width()) {
        qreal fitToScreenScale = size().width() / webView()->size().width();
        fitToScreenScale = qBound(m_viewportMetaData->m_minimumScale,
                                  fitToScreenScale,m_viewportMetaData->m_minimumScale);
        webView()->setScale(fitToScreenScale);
    }
//    else
//       updatePreferredContentSize();

    if (newSize.width() != m_viewportMetaData->m_width ||
        newSize.height() != m_viewportMetaData->m_height) {
        //setGeometry(0, 0, m_viewportMetaData->m_width, m_viewportMetaData->m_height);
        m_scrollHelper->setViewportSize(size());
    }


    if (data.webViewRect.isValid()) {
        QPointF webViewPos = data.webViewRect.topLeft();
        qreal newSc = webView()->scale(); // might be adjust by fitToScreen
        webViewPos = webViewPos / sc * newSc; // recalc pos if scale changed by fitToScreen
        QSizeF ss = webView()->size() * newSc;
        if(!ss.isEmpty()) {
            webViewPos.setX(qBound((qreal)m_viewportMetaData->m_width - (qreal)ss.width(), (qreal)webViewPos.x(), (qreal)0.0));
            webViewPos.setY(qBound((qreal)m_viewportMetaData->m_height - (qreal)ss.height(), (qreal)webViewPos.y(), (qreal)0.0));
            if(ss.width() < m_viewportMetaData->m_width) webViewPos.setX(0);
            if(ss.height() < m_viewportMetaData->m_height) webViewPos.setY(0);
        }
        setScrollWidgetPos(webViewPos);
    }
    enableContentUpdates();
//    updatePreferredContentSize();
    

    // emit scrolled event to hide/show url bar

    QPoint p(0, 0);
    if(!m_isLoading) {
        p = scrollPosition();
    }
    QPoint d(0, 0);
    emit viewScrolled(p, d);

}

WebPageData ScrollableWebContentView::defaultZoomData()
{
    WebPageData data;

    data.magic = 0;
    data.initialScale = m_viewportMetaData->m_initialScale;
    data.minScale = m_viewportMetaData->m_minimumScale;
    data.maxScale = m_viewportMetaData->m_maximumScale;
    data.userScalable = m_viewportMetaData->m_userScalable;

    data.scale = 1.0;
    data.rect = rect();
    data.webViewRect = webView()->rect();
    data.viewportSize = QSizeF(m_viewportMetaData->m_width, m_viewportMetaData->m_height);

    return data;
}

void ScrollableWebContentView::updatePreferredContentSize()
{
#ifdef VIEWPORT_ALWAYS_ALLOW_ZOOMING
    // Don't call updatePreferredContentSize() if we've over-ridden user-scalable because it
    // resets the content size and zoom factor.
    if(m_viewportMetaData->m_userScalableOverRidden)
        return;
#endif
    QSize s = m_viewportMetaData->getSpecifiedSize();
/*    if (!isSuperPage()) {
        prefferedHeight = qMax(m_viewportMetaData->m_width, m_viewportMetaData->m_height);
        prefferedWidth = m_viewportMetaData->m_width;        
        if(m_viewportMetaData->m_width * 1.5 < prefferedHeight) {
            // If the screen sides ratio is less than 3:2, than vertical width is
            // too narrow for normal page layout, but setting preffered width to the biggest side
            // makes page too scaled. Because of this set the page to the average of 2 viewport sides.
            prefferedWidth = (m_viewportMetaData->m_width + prefferedHeight) / 2;
        }
    }

//    if(m_viewportMetaData->m_specifiedData.)
*/
    webView()->page()->setPreferredContentsSize(s);
}
                                                                                                                                                                                                                                                                
void ScrollableWebContentView::setPage(QWebPage* page)
{
    m_isSuperPage = false;
    m_gesturesEnabled = true;
    webView()->setPage(page);
}

void ScrollableWebContentView::setSuperPage()
{
    m_isSuperPage = true;
    m_viewportMetaData->m_initialScale = 1.;
    m_viewportMetaData->m_minimumScale = 1.;
    m_viewportMetaData->m_maximumScale = 1.;
    m_viewportMetaData->m_specifiedData.m_width = "device-width";
    m_viewportMetaData->m_specifiedData.m_height = "device-height";
    m_viewportMetaData->m_userScalable = false;

    disableContentUpdates();
    webView()->setScale(1.);
    //QSize contentSize = (webView()->size() * zoomScale()).toSize();
    //QRect webViewRect(0, 0, size().width(), contentSize.height());
    //webView()->setGeometry(webViewRect);
    setScrollWidgetPos(QPointF(0, 0));

    m_viewportMetaData->m_width = size().width();
    m_viewportMetaData->m_height = size().height();
    m_viewportMetaData->m_isValid = true;
    enableContentUpdates();
#ifdef VIEWPORT_ALWAYS_ALLOW_ZOOMING
    updatePreferredContentSize();
    //viewportWidget()->updatePreferredContentSize(QSize(m_viewportMetaData->m_width
     //                                                  , m_viewportMetaData->m_height));
#else
    updatePreferredContentSize();
#endif
}

void ScrollableWebContentView::updateViewportMetaDataFromPageTag()
{
    QWebPage* page = webView()->page();
    if (!page)
        return;

    QWebFrame* frame = page->mainFrame();
    QMap<QString, QString> metaData = frame->metaData();
    QString viewportTag = metaData.value("viewport");

    QRect clientRect = geometry().toAlignedRect();
    ViewportMetaDataParser parser(clientRect);
    parser.parse(viewportTag, *m_viewportMetaData);

    m_viewportMetaData->adjustZoomValues(webView()->size());
}

void ScrollableWebContentView::reset()
{
    // TODO: INVESTIGATE: In the case of multiple windows loading pages simultaneously, it is possible
    // to be calling this slot on a signal from a frame that is not
    // the frame of the page saved here. It might be better to use 'sender' instead of
    // page->mainFrame() to get the metaData so that we use the meta data of the corresponding
    // frame

    QWebPage* page = webView()->page();
    if (!page)
        return;
//    if(m_viewportMetaData->m_isValid) return;

    //Initialize viewport metadata
    m_viewportMetaData->reset();

    disableContentUpdates();

    webView()->setScale(1);

    // sometimes on load from history webpage doesn't resize webView
    // set correct size of webView here
    webView()->setGeometry(QRectF( QPointF(0, 0),
                                   QSizeF(webView()->page()->mainFrame()->contentsSize())));

    updateViewportMetaDataFromPageTag();

    //setViewportWidgetGeometry(QRectF(QPointF(),
    //                                 QSize(m_viewportMetaData->m_width, m_viewportMetaData->m_height)));

    static const QPoint nullP(0,0);
    setScrollPosition(nullP,nullP);

    QSizeF sz(m_viewportMetaData->m_width, m_viewportMetaData->m_height);
    adjustViewportSize(sz, size());

    qreal initScale = m_viewportMetaData->m_initialScale > 0 ? m_viewportMetaData->m_initialScale :
                      m_viewportMetaData->m_width / webView()->size().width();
    // m_viewportMetaData->m_initialScale = m_viewportMetaData->m_width / webView()->size().width();
    webView()->setScale(initScale);

    enableContentUpdates();

    m_scrollHelper->setViewportSize(size());

    // Update corrected viewport data back to webpage metadata
    emit viewPortChanged();
}

void ScrollableWebContentView::contentsSizeChanged(const QSize& newContentSize)
{
    m_viewportMetaData->adjustZoomValues(newContentSize);
    qreal sc = zoomScale();
    sc = qBound(m_viewportMetaData->m_minimumScale, sc, m_viewportMetaData->m_maximumScale);
    if(!qFuzzyCompare(sc, zoomScale()))
        webView()->setScale(sc);
    emit viewPortChanged();
}

void ScrollableWebContentView::pageLoadStarted()
{
    m_isLoading = true;
//    m_viewportMetaData->m_isValid = false;
}

void ScrollableWebContentView::pageLoadProgress(int progress)
{
}

void ScrollableWebContentView::pageLoadFinished(bool ok)
{
    Q_UNUSED(ok);
    m_isLoading = false;
    if(!m_viewportMetaData->m_isValid)
        m_viewportMetadataResetTimer.singleShot(0,this,SLOT(reset()));
    else {
        QSize contentSize = scrollWidget()->size().toSize();
        m_viewportMetaData->adjustZoomValues(contentSize);
    }

    // report scroll position to hide url bar if necessary
    QPoint p = scrollPosition();
    QPoint d(0, 0);
    emit viewScrolled(p, d);
    
    webView()->update(); // invalidate the view to force tiles update
}


void ScrollableWebContentView::zoomAtPoint(QPointF touchPoint)
{
    QRectF target;

    //Get the focusable element rect from current touch position
    if(isZoomedIn()) {
        startZoomAnimationToHotSpot(touchPoint,size().width() / webView()->size().width());
        return;
    }

    //Pass all events to recognizer
    QRectF zoomRect = findZoomableRectForPoint(touchPoint);

    if (!zoomRect.isValid()) {
        //FIX ME: Add an event ignore animation
        return;
    }

    startZoomAnimation(zoomRect);
}


void ScrollableWebContentView::setViewportWidgetGeometry(const QRectF& r)
{
    if(r != geometry()) {
        setGeometry(r);
        emit viewPortChanged();
    }
}


bool ScrollableWebContentView::isZoomedIn() const
{
    qreal vpWidth = size().width();
    qreal scaledContentWidth = scrollWidget()->size().width() * zoomScale(); 
    qreal diff = scaledContentWidth - vpWidth; 
    return  diff > 0.01f;
}

void ScrollableWebContentView::stateChanged(KineticScrollable::State oldState,
                                            KineticScrollable::State newState)
{
    ScrollableViewBase::stateChanged(oldState, newState);

    switch(newState) {
        case KineticScrollable::Pushing :
        case KineticScrollable::AutoScrolling :
            m_tileUpdateEnableTimer.stop();
            //disableContentUpdates();
        break;
        case KineticScrollable::Inactive :
            m_tileUpdateEnableTimer.start(TileUpdateEnableDelay);
        break;
    }
}

QRectF ScrollableWebContentView::validateViewportRect(const QRectF& rect)
{
    QRectF ret(rect);

    if(ret.right() > webView()->size().width())
        ret.moveLeft(webView()->size().width() - ret.width());
    if(ret.bottom() > webView()->size().height())
        ret.moveTop(webView()->size().height() - ret.height());
    if(ret.x() < 0) ret.moveLeft(0);
    if(ret.y() < 0) ret.moveTop(0);
//    if(ret.width() > webView()->size().width()) ret.setWidth(webView()->size().width());
    if(ret.width() > webView()->size().width())
        ret.moveLeft(0); // do not center! ret.moveLeft(webView()->size().width() / 2 - ret.width() / 2);
    if(ret.height() > webView()->size().height())
        ret.moveTop(0); // do not center! ret.moveTop(webView()->size().height() / 2 - ret.height() / 2);

    return ret;
}

QRectF ScrollableWebContentView::viewableRect()
{
    return webView()->mapRectFromParent(geometry());
}

void ScrollableWebContentView::setViewableRect(const QRectF& rect)
{
    qreal scale = size().width() / rect.width(); 
    qstmDebug() << "setViewableRect: rect: " << rect << ", scale: " << scale << "\n";
    setZoomScale(scale, false);
    m_isScrolling = true;
    scrollPageTo(rect.topLeft());
    m_isScrolling = false;
}

void ScrollableWebContentView::startZoomAnimation(const QRectF& destViewRect, int animTime)
{
    if (webView()->geometry().isValid()) {
        m_zoomAnimator->setDuration(animTime);
        m_zoomAnimator->setStartValue(webView()->mapRectFromParent(geometry()));
        m_animationEndRect = validateViewportRect(destViewRect);
        m_zoomAnimator->setEndValue(m_animationEndRect);
        m_zoomAnimator->start();
    }
}

void ScrollableWebContentView::stopZoomAnimation()
{
    m_zoomAnimator->stop();
}

void ScrollableWebContentView::zoomAnimationStateChanged(QAbstractAnimation::State newState,QAbstractAnimation::State)
{
    switch (newState) {
    case QAbstractAnimation::Stopped:
        commitZoom();
        break;
    case QAbstractAnimation::Running:
        disableContentUpdates();
        break;
    default:
        break;
    }
}

void ScrollableWebContentView::resizeEvent(QGraphicsSceneResizeEvent* event)
{
    //Ignore resize when chrome is being still setup
    if (event->oldSize().width()) {

        qreal fitToScreenScale = event->oldSize().width() / webView()->size().width();
        bool isFitToScreen = qFuzzyCompare(zoomScale(), fitToScreenScale);
        QGraphicsWidget::resizeEvent(event);
        adjustViewportSize(event->oldSize(), event->newSize());
        if (isFitToScreen && !isSuperPage()) {
            QPointF docViewTopLeft(webView()->mapFromParent(QPointF(0,0)));
            fitToScreenScale = size().width() / webView()->size().width();
            zoomToHotSpot(docViewTopLeft, fitToScreenScale);
            m_zoomCommitTimer.start(ZoomCommitDuration);
        } else {
            QRectF viewRect = mapRectToItem(webView(), QRectF(QPointF(0,0), size()));
            QRectF validRect = validateViewportRect(viewRect);
            if(validRect != viewRect)
                setViewableRect(validRect);
        }
        m_scrollHelper->setViewportSize(size());
    }

#ifdef OWN_BACKING_STORE
    webView()->viewportUpdated();
#endif // OWN_BACKING_STORE
}

bool  ScrollableWebContentView::isChangedToPortrait(QSizeF oldSize, QSizeF newSize)
{
    return (oldSize.width() > oldSize.height()) && 
           (newSize.width() < newSize.height());
}

bool  ScrollableWebContentView::isChangedToLandscape(QSizeF oldSize, QSizeF newSize)
{
    return (oldSize.width() < oldSize.height()) && 
           (newSize.width() > newSize.height());
    emit mouseEvent(QEvent::GraphicsSceneMouseRelease);
}

bool ScrollableWebContentView::isOrientationChanged(QSizeF oldSize, QSizeF newSize)
{
    return isChangedToPortrait(oldSize, newSize) || 
           isChangedToLandscape(oldSize, newSize);
}


void ScrollableWebContentView::adjustViewportSize(QSizeF oldSize, QSizeF newSize)
{
    if(newSize.isNull()) return;

    if (isOrientationChanged(oldSize, newSize)) {
        m_viewportMetaData->orientationChanged(oldSize);
    }
    m_viewportMetaData->adjustViewportData(newSize);
    updatePreferredContentSize();
    return;    
}

void ScrollableWebContentView::sendEventToWebKit(QEvent::Type type, const QPointF& scenePos, bool select)
{
    QGraphicsSceneMouseEvent event(type);
    qstmSetGraphicsSceneMouseEvent(scenePos, webView(), event, select);
    webView()->page()->event(&event);
}

void ScrollableWebContentView::disableContentUpdates()
{
    if (m_zoomCommitTimer.isActive()) {
        m_zoomCommitTimer.stop();
    }
    webView()->setTiledBackingStoreFrozen(true);
}

void ScrollableWebContentView::enableContentUpdates()
{
    webView()->setTiledBackingStoreFrozen(false);
}

void ScrollableWebContentView::commitZoom()
{         
    m_zoomCommitTimer.stop();
    notifyZoomActions(zoomScale());
    enableContentUpdates();
    emit scaleChanged(zoomScale());
}

WebView* ScrollableWebContentView::webView() const
{
    return static_cast<WebView*>(scrollWidget());
}

void ScrollableWebContentView::setZoomScale(qreal value, bool immediateCommit)
{
    value = qBound(m_viewportMetaData->m_minimumScale, value, m_viewportMetaData->m_maximumScale);
    qreal curZoomScale = zoomScale();

    if (qFuzzyCompare(value, curZoomScale)) {
        notifyZoomActions(curZoomScale);
        return;
    }

    if (!immediateCommit)
        disableContentUpdates();

    webView()->setScale(value);

    if (immediateCommit)
        commitZoom();
//    else
//        m_zoomCommitTimer.start(ZoomCommitDuration);
}

qreal ScrollableWebContentView::zoomScale() const
{
    if (!webView())
        return 1.;

    return webView()->scale();
}

QRectF ScrollableWebContentView::findZoomableRectForPoint(const QPointF& point)
{
    QPointF zoomPoint = webView()->mapFromParent(point);

    QWebHitTestResult hitResult = webView()->page()->mainFrame()->hitTestContent(zoomPoint.toPoint());
    QWebElement targetElement = hitResult.enclosingBlockElement();

    while (!targetElement.isNull() && targetElement.geometry().width() < MinDoubleClickZoomTargetWidth)
        targetElement = targetElement.parent();

    if (!targetElement.isNull()) {
        QRectF elementRect = targetElement.geometry();
        qreal overMinWidth = elementRect.width() - ZoomableContentMinWidth;
        if (overMinWidth < 0)
            elementRect.adjust(overMinWidth / 2, 0, -overMinWidth / 2, 0);
        qreal destScale = size().width() / elementRect.width();
        QPointF rectPoint(elementRect.x(),zoomPoint.y() - point.y() / destScale);
        return QRectF(rectPoint, elementRect.size());
    }
    return QRectF();
}

void ScrollableWebContentView::notifyZoomActions(qreal newScale)
{
    bool enableZoomIn = false;
    bool enableZoomOut = false;

    if (m_viewportMetaData->m_userScalable) {

        if (newScale > m_viewportMetaData->m_minimumScale)
            enableZoomOut = true;
        else
            enableZoomOut = false;

        if (newScale < m_viewportMetaData->m_maximumScale)
            enableZoomIn = true;
        else
            enableZoomIn = false;
    }

    emit updateZoomActions(enableZoomIn, enableZoomOut);
}



bool ScrollableWebContentView::eventFilter(QObject* o, QEvent* e)
{
    if (o != scrollWidget()) return false;
    bool ret = false;

    if (m_gesturesEnabled) {
            ret = QStm_GestureEventFilter::instance()->eventFilter(o, e);
        }

    // Superpages should never receive contextmenu events
    if (isSuperPage() && (e->type() == QEvent::GraphicsSceneContextMenu || e->type() == QEvent::ContextMenu))
        ret = true;
    
    return ret;
}

bool ScrollableWebContentView::event(QEvent * e) 
{
    if (e->type() == QEvent::Gesture && m_gesturesEnabled) {
          QStm_Gesture* gesture = getQStmGesture(e);
          if (gesture) {
              bool ret = handleQStmGesture(gesture);
              if (gesture->getGestureStmType() == QStmTouchGestureType)
                  e->accept();
              return ret;
          }
    }
    return QGraphicsWidget::event(e);
}



bool  ScrollableWebContentView::handleQStmGesture(QStm_Gesture* gesture)
{
#ifdef OWN_BACKING_STORE
    // Signal tiling to minimize tile update activity while user does something
    webView()->userActivity();
#endif
    QStm_GestureType type = gesture->getGestureStmType();
    bool ret = false;
    switch (type) {
        case QStmTapGestureType:
        {
            ret = doTap(gesture);
            break;
        }

        case QStmMaybeTapGestureType:
        {
            ret = doMaybeTap(gesture);
            break;
        }

        case QStmReleaseGestureType:
        {
            ret = doRelease(gesture);
            break;
        }
        case QStmLeftRightGestureType:
        {
            ret = doLeftRight(gesture);
            break;
        }
        case QStmUpDownGestureType:
        {
            ret = doUpDown(gesture);
            break;
        }
        case QStmPanGestureType:
        {
            ret = doPan(gesture);
            break;
        }
        case QStmFlickGestureType:
        {
            ret = doFlick(gesture);
            break;
        }
        case QStmDoubleTapGestureType:
        {
            ret = doDoubleTap(gesture);
            break;
        }
        case QStmTouchGestureType:
        {
            ret = doTouch(gesture);
            break;
        }
        case QStmPinchGestureType:
        {
            ret = doPinch(gesture);  
            break;
        }
        case QStmLongPressGestureType:
        {
            ret = doLongPress(gesture);
            break;
        }
        
        default: 
        {
            ret = true;
        }
    }
    
    return ret;
}

QWebHitTestResult  ScrollableWebContentView::hitTest(const QPointF& scenePos)
{
    QPointF contextPt = webView()->mapFromScene(scenePos);
    QWebPage* page = webView()->page();
    return page->currentFrame()->hitTestContent(contextPt.toPoint()); 
}



bool ScrollableWebContentView::toggleVkb()
{
    bool inputEnabled = false;
    QInputContext *ic = qApp->inputContext();
    if (m_hitTest.isContentEditable()) {
        QEvent sipe(QEvent::RequestSoftwareInputPanel);
        ic->filterEvent(&sipe);
        inputEnabled = true;
    }
    else {
        QEvent sipe(QEvent::CloseSoftwareInputPanel);
        ic->filterEvent(&sipe);

    }
    m_isInputOn = inputEnabled;
    return inputEnabled;
}

bool  ScrollableWebContentView::toggleInputMethod(bool on)
{
    QGraphicsView* gv = qstmGetGraphicsView(webView());
    bool oldInputEnabled = false;
    if (gv != NULL) {
        gv->testAttribute(Qt::WA_InputMethodEnabled);
        gv->setAttribute(Qt::WA_InputMethodEnabled, on);
    }
    m_isInputOn = on;
    return oldInputEnabled;
}

bool  ScrollableWebContentView::inputMethodEnabled()
{
#ifdef ORBIT_UI
    HbInputMethod* im = HbInputMethod::activeInputMethod();
    
    QGraphicsView* gv = qstmGetGraphicsView(webView());
    bool enabled = false;
    if (gv) {
        enabled = gv->testAttribute(Qt::WA_InputMethodEnabled);
    }
    return enabled; 
#else
    return false;
#endif // ORBIT_UI
}

bool  ScrollableWebContentView::doLongPress(QStm_Gesture* gesture)
{
    bool willHandle = m_gesturesEnabled && !isSuperPage();
    if (willHandle) {
//        QWebPage* page = webView()->page();
        QPoint gpos = gesture->position();
        QPointF pos = qstmMapToScene(gpos, this);
//        QPointF contextPt = webView()->mapFromScene(pos);
        //QWebHitTestResult result = page->currentFrame()->hitTestContent(contextPt.toPoint());
    //Notify context menu observers
        emit contextEventObject(&m_hitTest, pos);
        m_ignoreNextRelease = true;
    }
    return willHandle;
}



bool ScrollableWebContentView::doTouch(QStm_Gesture* gesture)
{
    bool willHandle = m_gesturesEnabled;
    m_scrollHelper->stopScrollNoSignal();
    m_touchDownPos = gesture->scenePosition(this);
    if (!isSuperPage()) {
        m_hitTest = hitTest(m_touchDownPos);
        //toggleInputMethod(false);
        qreal scale = zoomScale();

        m_hoverTimer->stop();
        m_hoverTimer->setSingleShot(true);
        
        m_touchDownTimer->stop();
        m_touchDownTimer->setSingleShot(true);
                
        m_touchDownTimer->start(TouchDownTimeout, this);
        m_hoverTimer->start(HoverTimeout, this);
    }
    else {
        sendEventToWebKit(QEvent::GraphicsSceneMousePress, m_touchDownPos);
    }
    return willHandle;
}

void ScrollableWebContentView::touchDownCallback()
{
    m_touchDownTimer->stop();
    if (m_gesturesEnabled) {
        sendEventToWebKit(QEvent::GraphicsSceneMousePress, m_touchDownPos);
    }
}

void ScrollableWebContentView::hoverCallback()
{
    m_hoverTimer->stop();
    if (m_gesturesEnabled && !isSuperPage()) {
        sendEventToWebKit(QEvent::GraphicsSceneMouseMove, m_touchDownPos);
    }
}


bool ScrollableWebContentView::doTap(QStm_Gesture* gesture)
{
    bool willHandle = m_gesturesEnabled;
    if (willHandle && !isSuperPage()) {
        QPointF pos = gesture->scenePosition(this);
        bool hasInputMethod = toggleVkb();
        sendEventToWebKit(QEvent::GraphicsSceneMouseRelease, pos);
        emit contentViewMouseEvent(QEvent::GraphicsSceneMouseRelease);
    }
    return willHandle;
}

bool ScrollableWebContentView::doMaybeTap(QStm_Gesture* gesture)
{
    bool willHandle = m_gesturesEnabled;
    if (willHandle && isSuperPage()) {
        QPointF pos = gesture->scenePosition(this);
        sendEventToWebKit(QEvent::GraphicsSceneMouseRelease, pos);
    }
    return willHandle;
}

#ifdef USE_KINETIC_SCROLLER
bool ScrollableWebContentView::doPan(QStm_Gesture* gesture)
{
    if (m_touchDownTimer->isActive()) {
        m_touchDownTimer->stop();
    }
    
    QStm_GestureType type = gesture->getGestureStmType();
    bool willHandle = m_gesturesEnabled;
    
    if (willHandle) {
        QPoint scrollPos = ScrollableViewBase::scrollPosition();
        QPoint delta = gesture->getLengthAndDirection();
        delta.ry() = -delta.y();
        m_kineticScroller->doPan(delta);
    }
    return willHandle;
}
#else


bool ScrollableWebContentView::doLeftRight(QStm_Gesture* gesture)
{
    bool ret = true;
    if (m_hitTest.isContentEditable() && m_isInputOn) {
        QPointF pos = gesture->scenePosition(this);
        sendEventToWebKit(QEvent::GraphicsSceneMouseMove, pos, true);
    }
    else {
        ret = doPan(gesture);
    }
    return ret;
}


bool ScrollableWebContentView::doUpDown(QStm_Gesture* gesture)
{
    bool ret = true;
    if (m_hitTest.isContentEditable() && m_isInputOn) {
        QPointF pos = gesture->scenePosition(this);
        sendEventToWebKit(QEvent::GraphicsSceneMouseMove, pos, true);
    }
    else {
        ret = doPan(gesture);
    }
    return ret;
}



bool ScrollableWebContentView::doPan(QStm_Gesture* gesture)
{
    bool willHandle = m_gesturesEnabled;
    if (m_hitTest.isContentEditable() && m_isInputOn) {
        QPointF pos = gesture->scenePosition(this);
        sendEventToWebKit(QEvent::GraphicsSceneMouseMove, pos, true);
    }
    else {
        dehighlightWebElement();
        if (willHandle) {
            if (gesture->gestureState() != Qt::GestureFinished) {
                QPointF delta = gesture->sceneLengthAndDirection(this);

                if (isSuperPage()) {
                    delta.rx() = 0.0;
                }
                else {
                    //delta.rx() = -delta.x();
                }
            
                //disableContentUpdates();
                qstmDebug() << "doPan: gesture timestamp: " << gesture->timestamp().toString("hh:mm:ss.zzz") <<
                        ", delta: " << delta << "\n";

                m_scrollHelper->scroll(delta);
            }
        }
    }
    return willHandle;
}
#endif //USE_KINETIC_SCROLLER


bool ScrollableWebContentView::doRelease(QStm_Gesture* gesture)
{
    bool willHandle = m_gesturesEnabled;
    if (willHandle) {
        if (m_ignoreNextRelease) {
            m_ignoreNextRelease = false;
        }
        else if (m_scrollHelper->isScrolling()) {
            m_scrollHelper->panFromOvershoot();
//            enableContentUpdates();
        }
        else {
            /*
             * on tap we send mouseRelease
             * assumption here is that we can get
             * either tap or release gesture but not both.
             */
            doTap(gesture);
        }
    }
    return willHandle;
}

#ifdef USE_KINETIC_SCROLLER
bool ScrollableWebContentView::doFlick(QStm_Gesture* gesture)
{
    int direction = gesture->getDirection();
    QStm_GestureType type = gesture->getGestureStmType();
    bool willHandle = m_gesturesEnabled;
    if (willHandle ) {          
        m_kineticScroller->doFlick(90 * gesture->getSpeedVec());
    }
}

#else
bool ScrollableWebContentView::doFlick(QStm_Gesture* gesture)
{    
    bool willHandle = m_gesturesEnabled;
    int afterPinch = m_pinchFinishTime.elapsed();
    if (willHandle && afterPinch > 100 && m_scrollHelper->isScrolling()) {
        dehighlightWebElement();
        int direction = gesture->sceneDirection(this);
        QPointF v = gesture->sceneSpeedVec(this);
        QPointF vOrig = v;
        qstmDebug() << "doFlick: timestamp: " << gesture->timestamp().toString("hh:mm:ss.zzz") <<
                ", v: " << v << "\n";
        if (v.x() != 0.0 || v.y() != 0.0) {
            if (direction == EEast || direction == EWest) {
                v.ry() = 0.0;
            }
            if (direction == ENorth || direction == ESouth) {
                v.rx() = 0.0;
            }
            m_scrollHelper->kineticScroll(v);
            //enableContentUpdates();
            //m_tileUpdateEnableTimer.start(TileUpdateEnableDelay);
        }
        else if (afterPinch <= 500) {
            qstmDebug() << "doFlick: Flick is too soon after pinch\n";
        }
    }
    return willHandle;
}
#endif //USE_KINETIC_SCROLLER

bool ScrollableWebContentView::doDoubleTap(QStm_Gesture* gesture)
{
    if (!m_gesturesEnabled || !m_viewportMetaData->m_userScalable) {
        m_touchDownTimer->stop();
        m_hoverTimer->stop();
        return m_gesturesEnabled;    
    }
    dehighlightWebElement();
    QPointF pos = gesture->scenePosition(this);
    pos = mapFromScene(pos);
    qstmDebug() << "doDoubleTap: zoom at pos: " << pos << "\n";
    zoomAtPoint(pos);
    
    return m_gesturesEnabled;
}

#define square(x) (x)*(x)

qreal ScrollableWebContentView::calcScale(int origDistance, QPointF p1, QPointF p2)
{
    qreal d1f = origDistance;
    qreal dist = calcDistance(p1, p2);
    return (dist/ d1f);
}

qreal ScrollableWebContentView::calcDistance(QPointF pt1, QPointF pt2)
{
    return (sqrt((double)square(pt2.x() - pt1.x()) + square(pt2.y() - pt1.y())));
}

QPointF ScrollableWebContentView::calcCenter(QPointF pt1, QPointF pt2)
{
    return((pt1 + pt2) / 2);
}

bool ScrollableWebContentView::doPinch(QStm_Gesture* gesture)
{
    m_scrollHelper->stopScrollNoSignal();
    dehighlightWebElement();
    m_pinchFinishTime.restart();
    if (!m_gesturesEnabled || !m_viewportMetaData->m_userScalable)
        return m_gesturesEnabled;

    // Normally, "details" contains the distance between the two touched points
    // It's null when the pinch starts (which in effect is zero)
    // When we start pinch, we don't zoom because there is no delta
    QPointF p1 = gesture->scenePosition(this);
    QPointF p2 = gesture->scenePosition2(this);
    if (gesture->gestureState() == Qt::GestureFinished) {
        commitZoom();        
        return m_gesturesEnabled;
    }

    if (!gesture->getDetails()) {
        m_pinchStartDistance = calcDistance(p1, p2);
        m_pinchStartScale = zoomScale();
    }
    else {
        qreal scale = calcScale(m_pinchStartDistance, p1, p2);
        qreal newScale = qBound(m_viewportMetaData->m_minimumScale, 
                                scale * m_pinchStartScale,
                                m_viewportMetaData->m_maximumScale);
        // Don't allow zooming beyond the min/max but still call the zoom animation (coz the hotspot could be moving and we want to pan with it)
        //zoomToHotSpot(calcCenter(p1, p2), newScale);
        if (!qFuzzyCompare(zoomScale(), newScale)) {
            startZoomAnimationToHotSpot(calcCenter(p1, p2), newScale, ZoomAnimationDuration);
        }
    }
    return m_gesturesEnabled;
}

bool ScrollableWebContentView::isZooming()
{
    return m_zoomAnimator->state() == QAbstractAnimation::Running;     
}

void ScrollableWebContentView::setGesturesEnabled(bool value) 
{ 
    if (!value && m_touchDownTimer->isActive()) {
        m_touchDownTimer->stop();
    }

    m_gesturesEnabled = value;
}

bool ScrollableWebContentView::gesturesEnabled() 
{ 
    return m_gesturesEnabled; 
}


void ScrollableWebContentView::dehighlightWebElement()
{
    m_touchDownTimer->stop();
    m_hoverTimer->stop();
    
    QSizeF contentSize = webView()->size();
    QPointF dummyPosF(-contentSize.width() - 2.0, -contentSize.height( ) - 2.0);
    QPoint dummyPos(-1, -1);
    QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseRelease);
    event.setScenePos(dummyPos);
    event.setPos(dummyPos);
    event.setButton(Qt::LeftButton);

    bool hasInputMethod = toggleInputMethod(false);
    webView()->page()->event(&event);
    toggleInputMethod(hasInputMethod);
    //sendEventToWebKit(QEvent::GraphicsSceneMouseRelease, dummyPos);    
}


void ScrollableWebContentView::stopScrolling()
{
    m_isScrolling = false;
    //enableContentUpdates();
}



bool ScrollableWebContentView::sceneEventFilter(QGraphicsItem* item, QEvent* event)
{
    Q_UNUSED(item);

    bool handled = false;
#ifdef USE_KINETIC_SCROLLER    
    if (!isVisible())
        return handled;

    //Pass all events to recognizer
    handled  = m_gestureRecognizer.mouseEventFilter(static_cast<QGraphicsSceneMouseEvent *>(event));
#endif
    return handled;
}


void ScrollableWebContentView::handleGesture(GestureEvent* gestureEvent)
{
    switch (gestureEvent->type()) {
    case GestureEvent::Touch:
        handlePress(gestureEvent);
        break;
    case GestureEvent::Release:
        handleRelease(gestureEvent);
        break;
    case GestureEvent::Pan:
        handlePan(gestureEvent);
        break;
    case GestureEvent::Flick:
        handleFlick(gestureEvent);
        break;
    case GestureEvent::DoubleTap:
        handleDoubleTap(gestureEvent);
        break;
    case GestureEvent::LongTap:
        handleLongTap(gestureEvent);
        break;
    default:
        break;
    }

}

void ScrollableWebContentView::handlePress(GestureEvent* gestureEvent)
{
    m_kineticScroller->stop();
    QPointF pos = gestureEvent->position();
    sendEventToWebKit(QEvent::GraphicsSceneMousePress, pos);
}

void ScrollableWebContentView::handleRelease(GestureEvent* gestureEvent)
{
    //Cache release event to send on release
    QPointF pos = gestureEvent->position();
    sendEventToWebKit(QEvent::GraphicsSceneMouseRelease, pos);
}

void ScrollableWebContentView::handleDoubleTap(GestureEvent* gestureEvent)
{
    if (!m_viewportMetaData->m_userScalable)
        return;

    QRectF target;

    //Get the focussable element rect from current touch position
    QPointF touchPoint = mapFromScene(gestureEvent->position());

    if(isZoomedIn()) {
        startZoomAnimationToHotSpot(touchPoint,size().width() / webView()->size().width());
            return;
    }

    QRectF zoomRect = findZoomableRectForPoint(touchPoint);

    if (!zoomRect.isValid()) {
        //FIX ME: Add an event ignore animation
        return;
    }

    startZoomAnimation(zoomRect);
}

void ScrollableWebContentView::handlePan(GestureEvent* gestureEvent)
{
    QPoint scrollPos = ScrollableViewBase::scrollPosition();
    m_kineticScroller->doPan(gestureEvent->delta());
}

void ScrollableWebContentView::handleFlick(GestureEvent* gestureEvent)
{
    QPoint scrollPos = ScrollableViewBase::scrollPosition();
    m_kineticScroller->doFlick(gestureEvent->velocity());
}

void ScrollableWebContentView::handleLongTap(GestureEvent* gestureEvent)
{
    QWebPage* page = webView()->page();
    QPointF contextPt = webView()->mapFromScene(gestureEvent->position());
    QWebHitTestResult result = page->currentFrame()->hitTestContent(contextPt.toPoint());
    
    //Notify context menu observers
    emit contextEventObject(&result, gestureEvent->position());
}

void ScrollableWebContentView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    QRectF fillRect = option ? option->exposedRect : QRectF(QPoint(0, 0), size());

    painter->fillRect(fillRect, Qt::white);
}


} //namespace GVA
