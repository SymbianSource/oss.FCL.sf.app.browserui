#include <QtCore>
#include "ScrollHelper.h"
#include "qstmfilelogger.h"


#define SCROLL_TIMEOUT   16

namespace GVA
{
static const int MaxFlickDurartion = 1500;
static const int MidFlickDurartion = 800;
static const int MinFlickDurartion = 500;
static const qreal MaxFlickSpeed = 2.0;
static const qreal MidFlickSpeed = 1.2;
static const qreal MinFlickSpeed = 0.5;
static const qreal MaxFlickInViewportUnits = 1.0;
static const qreal MidFlickInViewportUnits = 0.8;
static const qreal MinFlickInViewportUnits = 0.2;
static const qreal DefaultDecel = 0.005;

ScrollHelper::ScrollHelper(QObject* scrolledWidget) : QObject(),
                                  m_scrolledWidget(scrolledWidget),
                                  m_minFlickDuration(MinFlickDurartion),
                                  m_maxFlickDuration(MaxFlickDurartion),
                                  m_midFlickDuration(MidFlickDurartion),
                                  m_scrollDelta(QPointF()),
                                  m_minFlickSpeed(MinFlickSpeed),
                                  m_maxFlickSpeed(MaxFlickSpeed),
                                  m_midFlickSpeed(MidFlickSpeed),
                                  m_maxFlickInViewportUnits(MaxFlickInViewportUnits),
                                  m_minFlickInViewportUnits(MinFlickInViewportUnits),
                                  m_midFlickInViewportUnits(MidFlickInViewportUnits),
                                  m_viewportSize(QSize()),
                                  m_lockToY(false),
                                  m_lockToX(false),
                                  m_decel(DefaultDecel)
                                  
{
    m_scrollAnimation = new QPropertyAnimation(scrolledWidget, "widgetScrollPosition");
    connect(m_scrollAnimation, SIGNAL(finished()), this, SLOT(stopScroll()));
    connect(m_scrollAnimation, SIGNAL(stateChanged(QAbstractAnimation::State,QAbstractAnimation::State)),
            this, SLOT(scrollAnimationStateChanged(QAbstractAnimation::State,QAbstractAnimation::State)));
    m_scrollTimer = UiTimer::New();    
    m_scrollTimer->setPriority(50);   
    m_scrollTimer->setTimerCallback("scrollTimerCallback");
    m_easingCurve = new QEasingCurve(QEasingCurve::OutCubic);
    m_easingCurveOvershoot= new QEasingCurve(QEasingCurve::OutBack);

}

ScrollHelper::~ScrollHelper()
{
    m_scrollTimer->stop();
    delete m_scrollTimer;
    delete m_easingCurve;
    delete m_easingCurveOvershoot;
}


void ScrollHelper::setFlickDurationLimits(int minDuration, int midDuration, int maxDuration)
{
    m_minFlickDuration = minDuration;
    m_midFlickDuration = midDuration;
    m_maxFlickDuration = maxDuration;
}

void ScrollHelper::setFlickSpeedLimits(qreal minSpeed, qreal midSpeed, qreal maxSpeed)
{
    m_minFlickSpeed = minSpeed;
    m_midFlickSpeed = midSpeed;
    m_maxFlickSpeed = maxSpeed;
}

void ScrollHelper::setFlickLimits(qreal minFlick, qreal midFlick, qreal maxFlick)
{
    m_minFlickInViewportUnits = minFlick;
    m_midFlickInViewportUnits = midFlick;
    m_maxFlickInViewportUnits = maxFlick;
}


void ScrollHelper::setDeceleration(qreal decel)
{
    m_decel = decel;
}

qreal ScrollHelper::getDeceleration()
{
    return m_decel;
}


void ScrollHelper::scrollTimerCallback()
{
    m_scrollTimer->stop();
    if (m_scrollState != ScrollHelper::ActiveState) {
        return;
    }
    qstmDebug() << "ScrollHelper::scrollTimerCallback. m_scrollDelta: " << m_scrollDelta <<
            ", m_scrollMode: " << m_scrollMode << "\n";
    
    QPointF speed = m_kineticSpeed;
    if (m_scrollMode == ScrollHelper::KineticScrollMode) {
        m_scrollDuration += SCROLL_TIMEOUT;
        qreal progress = (qreal)m_scrollDuration / m_scrollTotalDuration;
        if (progress > 0.0 && progress < 1.0) {
            qreal val = m_curEasingCurve->valueForProgress(progress);
            QPointF tp = m_startScrollPos + val * (m_targetScrollPos - m_startScrollPos);
            m_curScrollPos = tp;
            setScrollPos(tp);
            m_scrollTimer->start(SCROLL_TIMEOUT, this);
        }
        else {
            stopScroll();
        }
    }
    else if (m_scrollMode == ScrollHelper::PanScrollMode) {
        doScroll(m_scrollDelta);
        m_scrollDelta = QPointF();
        m_scrollTimer->start(SCROLL_TIMEOUT, this);
    }
}


QPointF ScrollHelper::speedForNextInterval(const QPointF& initSpeed, long timeInterval, const QPointF& decel)
{
    QPointF speed = initSpeed;
    if (speed.y() < 0) {
        speed.ry() += (decel.y() * timeInterval);
        speed.ry() = qMin(qreal(0.0), speed.y());
    }
    else if (speed.y() > 0) {
        speed.ry() -= (decel.y() * timeInterval);
        speed.ry() = qMax(qreal(0.0), speed.y());
    }
    if (speed.x() < 0) {
        speed.rx() += (decel.x() * timeInterval);
        speed.rx() = qMin(qreal(0.0), speed.x());
    }
    else if (speed.x() > 0) {
        speed.rx() -= (decel.x() * timeInterval);
        speed.rx() = qMax(qreal(0.0), speed.x());
    }
    return speed;
}


void ScrollHelper::scroll(QPointF& delta)
{
    if ( delta.x() == 0 && delta.y() == 0)  return;

    if (m_scrollState == ScrollHelper::IdleState) {
        //m_lockToX = (delta.y() == 0);
        //m_lockToY = (delta.x() == 0);

        doScroll(delta);
        m_scrollDelta = QPointF();
        m_scrollTimer->setSingleShot(true);
        m_scrollTimer->start(SCROLL_TIMEOUT, this);
    }
    else {
        if (m_lockToX) {
            delta.ry() = 0.0;
        }
        if (m_lockToY) {
            delta.rx() = 0.0;
        }
        m_scrollDelta += delta;
    }
    m_scrollMode = ScrollHelper::PanScrollMode;
    m_scrollState = ScrollHelper::ActiveState;
}

bool ScrollHelper::isScrolling()
{
    return m_scrollState == ScrollHelper::ActiveState;
}

void ScrollHelper::doScroll(QPointF& delta)
{
    QPointF scrollPos = getScrollPos();
    QPointF targetPos;
    targetPos = scrollPos + delta;
    setScrollPos(targetPos);
}

QPointF ScrollHelper::getScrollPos()
{
    return m_scrolledWidget->property("widgetScrollPosition").toPointF();
}

void  ScrollHelper::setScrollPos(QPointF& pos)
{
     m_scrolledWidget->setProperty("widgetScrollPosition", pos.toPoint());
}

QPointF ScrollHelper::getMaxScrollPos()
{
    return m_scrolledWidget->property("maxScrollPosition").toPointF();
}


QSizeF ScrollHelper::viewportSize()
{
    return m_viewportSize;
}

void ScrollHelper::setViewportSize(const QSizeF& size)
{
    m_viewportSize = size;
}

void ScrollHelper::stopScrollNoSignal()
{
    m_scrollTimer->stop();
    reset();    
}

void ScrollHelper::stopScroll()
{     
    stopScrollNoSignal();
    emit scrollFinished();
}


void ScrollHelper::reset()
{
    m_scrollState = ScrollHelper::IdleState;
    m_scrollMode = ScrollHelper::ReadyMode;
    m_scrollDelta = QPointF();
    m_lockToY = false;
    m_lockToX = false;
}

void ScrollHelper::panFromOvershoot()
{
    stopScroll();
    QPointF scrollPos = getScrollPos();
    QPointF targetScrollPos = scrollPos;
    QEasingCurve::Type easingCurveType = QEasingCurve::OutCubic;
    if (clampScrollPosition(targetScrollPos)) {
        easingCurveType = QEasingCurve::OutBack;
        m_scrollMode = ScrollHelper::PanScrollMode;
        startScrollAnimation(scrollPos, targetScrollPos, 300, easingCurveType);
    }
}

bool ScrollHelper::clampScrollPosition(QPointF& scrollPos)
{
    QPointF maxScrollPos = getMaxScrollPos();
    QPointF origPos = scrollPos;
    scrollPos.ry() = qBound(qreal(0.0), origPos.y(), maxScrollPos.y());
    scrollPos.rx() = qBound(qreal(0.0), origPos.x(), maxScrollPos.x());
    bool clampY = (scrollPos.y() != origPos.y()) &&
                  (scrollPos.y() == qreal(0.0) || scrollPos.y() == maxScrollPos.y());

    bool clampX = (scrollPos.x() != origPos.x()) &&
                   (scrollPos.x() == qreal(0.0) || scrollPos.x() == maxScrollPos.x());
    return (clampX || clampY);
}


void ScrollHelper::startScrollAnimation(QPointF& scrollPos, QPointF& targetPos,
                                        int duration, QEasingCurve::Type& easingCurveType)
{
    if (scrollPos != targetPos) {
        m_scrollAnimation->setStartValue(scrollPos);
        m_scrollAnimation->setEndValue(targetPos);
        m_scrollAnimation->setDuration(duration);
        m_scrollAnimation->setEasingCurve(easingCurveType);
        m_scrollAnimation->start();
        m_scrollState = ScrollHelper::ActiveState;
    }
}

void ScrollHelper::scrollAnimationStateChanged(QAbstractAnimation::State newState,
                                               QAbstractAnimation::State)
{
    switch (newState) {
        case QAbstractAnimation::Stopped:
            reset();
            break;
        case QAbstractAnimation::Running:
            break;
        default:
            break;
    }
}


int ScrollHelper::calcScrollDuration(const QPointF& speed, const QPointF& delta)
{
    QPointF v1 = speed;
    int flickDuration = sqrt(delta.x() * delta.x() + delta.y() * delta.y()) /
                        sqrt(v1.x() * v1.x() + v1.y() * v1.y());
    flickDuration = qBound(m_minFlickDuration, flickDuration, m_maxFlickDuration);
    return flickDuration;
}


int ScrollHelper::calcScrollDuration(qreal motionFactor)
{
    int duration = m_midFlickDuration;
    if (motionFactor == m_maxFlickInViewportUnits) {
         duration = m_maxFlickDuration;
    }
    else if (motionFactor == m_minFlickInViewportUnits) {
        duration = m_minFlickDuration;
    }
    return duration;
}

void ScrollHelper::kineticScroll(QPointF& speed)
{
    if (m_lockToY) {
        speed.rx() = 0.0;
    }
    if (m_lockToX) {
        speed.ry() = 0.0;
    }
    
    //doScroll(m_scrollDelta);
    //stopScroll();
    
    QEasingCurve::Type easingCurveType = QEasingCurve::InOutCubic;
    
    QPointF scrollPos = getScrollPos() + m_scrollDelta;
    int flickDuration = 0;
    QPointF targetPos = calcTargetScrollPosAndDuration(speed, scrollPos, flickDuration);
    QPointF delta = targetPos - scrollPos;

    m_curEasingCurve = m_easingCurve;
    if (clampScrollPosition(targetPos)) {
        easingCurveType = QEasingCurve::OutBack;
        m_curEasingCurve = m_easingCurveOvershoot;
    }
    m_scrollMode = ScrollHelper::KineticScrollMode;
    qstmDebug() << "ScrollHelper::kineticScroll. initial speed: " << speed <<
            ", delta: " << delta <<
            ", flickDuration: " << flickDuration <<
            ", m_scrollDelta: " << m_scrollDelta << "\n";
    
    m_targetScrollPos = targetPos;
    m_kineticSpeed = speed;
    m_curScrollPos = scrollPos;
    m_startScrollPos = scrollPos;
    m_scrollTotalDuration = flickDuration;
    m_scrollDuration = 0; 
    m_lockToX = false;
    m_lockToY = false;
    //m_decelVec = calcTargetScrollPosAndDuration(speed, delta, flickDuration);

    //startScrollAnimation(scrollPos, targetPos, flickDuration, easingCurveType);
}

QPointF ScrollHelper::calcDeceleration(const QPointF& initSpeed, const QPointF& distance, long time)
{
    return 2 * (initSpeed * time - distance) / (time * time);
}

QPointF ScrollHelper::calcTargetScrollPosAndDuration(const QPointF& speed, const QPointF& scrollPos, int& duration)
{
    QSizeF  vpSize = viewportSize();
    QPointF targetPos = scrollPos;
    if (vpSize.isValid()) {
        qreal kX = 0.0;
        qreal kY = 0.0;
        if (speed.x() != 0.0) {
            kX = calcMotionFactor(speed.x());
        }

        if (speed.y() != 0.0) {
            kY = calcMotionFactor(speed.y());
        }

        targetPos.rx() -= kX * vpSize.width();
        targetPos.ry() -= kY * vpSize.height();
        duration = calcScrollDuration(qMax(qAbs(kX), qAbs(kY)));
    }
    return targetPos;
}


qreal ScrollHelper::calcMotionFactor(const qreal speed)
{
    qreal k = 0.0;
    qreal absSpeed = qAbs(speed);
    if (absSpeed >= m_maxFlickSpeed) {
        k = m_maxFlickInViewportUnits;
    }
    else if (absSpeed < m_maxFlickSpeed && absSpeed >= m_midFlickSpeed) {
        k = m_midFlickInViewportUnits;
    }
    else {
        k = m_minFlickInViewportUnits;
    }
    if (speed < 0) {
        k = -k;
    }
    return k;
}


}
