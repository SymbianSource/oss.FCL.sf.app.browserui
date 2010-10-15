
#ifndef SCROLLHELPER_H_
#define SCROLLHELPER_H_

#include <QPropertyAnimation>
#include <QRectF>
#include <QPoint>
#include <QTime>
#include <QTimer>
#include "uitimer.h"

namespace GVA {
class ScrollHelper: public QObject
{
    Q_OBJECT
    
public:
    enum ScrollMode {
        ReadyMode = 0,
        PanScrollMode,
        KineticScrollMode
    };
    
    enum ScrollState {
        IdleState = 0,   // no scrolling
        ActiveState, // scroll poins added
        WaitState    // no new scroll points
    };
    
    enum ScrollProperty {
        Geometry,
        Position,
        Velocity
    };
    
    ScrollHelper(QObject* scrolledWidget);
    ~ScrollHelper();
    void scroll(QPointF& delta);
    
    bool isScrolling();
    void panFromOvershoot();
    void setFlickDurationLimits(int minDuration, int midDuration, int maxDuration);
    void setFlickSpeedLimits(qreal minSpeed, qreal midSpeed, qreal maxSpeed);
    void setDeceleration(qreal decel);
    qreal getDeceleration();
    void kineticScroll(QPointF& speed);
    QSizeF viewportSize();
    void setViewportSize(const QSizeF& size);
    void setFlickLimits(qreal minFlick, qreal midFlick, qreal maxFlick);
    void stopScrollNoSignal();

public slots:
    void stopScroll();    
    void scrollTimerCallback();
    void scrollAnimationStateChanged(QAbstractAnimation::State newState,
                                     QAbstractAnimation::State);
signals:
    void scrollFinished();

private:
    void reset();
    void doScroll(QPointF& delta);
    QPointF getScrollPos();
    QPointF getMaxScrollPos();
    void setScrollPos(QPointF& pos);
    bool clampScrollPosition(QPointF& scrollPos);
    void startScrollAnimation(QPointF& scrollPos, QPointF& targetPos,
                              int duration, QEasingCurve::Type& easingCurveType);
    int calcScrollDuration(const QPointF& speed, const QPointF& delta);
    int calcScrollDuration(qreal motionFactor);
    QPointF calcTargetScrollPosAndDuration(const QPointF& speed,
                                           const QPointF& scrollPos, int& duration);
    qreal calcMotionFactor(const qreal speed);
    QPointF calcDeceleration(const QPointF& initSpeed, const QPointF& distance, long time);
    QPointF speedForNextInterval(const QPointF& initSpeed, long timeInterval, const QPointF& decel);
private:
    ScrollMode          m_scrollMode;
    ScrollState         m_scrollState;
    QPointF             m_scrollDelta;
    QPointF             m_curScrollPos;
    QPropertyAnimation* m_scrollAnimation;
    UiTimer*            m_scrollTimer;
    QObject*            m_scrolledWidget;
    int                 m_minFlickDuration;
    int                 m_maxFlickDuration;
    int                 m_midFlickDuration;
    qreal               m_minFlickSpeed;
    qreal               m_maxFlickSpeed;
    qreal               m_midFlickSpeed;
    qreal               m_decel;
    QPointF             m_decelVec;
    QSizeF              m_viewportSize;
    qreal               m_maxFlickInViewportUnits;
    qreal               m_minFlickInViewportUnits;
    qreal               m_midFlickInViewportUnits;
    bool                m_lockToY;
    bool                m_lockToX;
    QPointF             m_targetScrollPos;
    QPointF             m_startScrollPos;
    QPointF             m_kineticSpeed;
    long                m_scrollTotalDuration;
    long                m_scrollDuration;
    QEasingCurve::Type  m_easingCurveType;
    QEasingCurve*       m_easingCurve;
    QEasingCurve*       m_easingCurveOvershoot;
    QEasingCurve*       m_curEasingCurve;
};
}
#endif /* SCROLLHELPER_H_ */
