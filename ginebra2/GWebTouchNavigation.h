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


/* Temporary implementation of WebTouchNavigation until gestures are ready. */

#ifndef WEBTOUCHNAVIGATION_H
#define WEBTOUCHNAVIGATION_H

#include <QTime>
#include <qobject.h>
#include <qpoint.h>
#include <qevent.h>
#include <qobject.h>
#include <qtimeline.h>
#include <qwebframe.h>
#include <qwebelement.h>
#include <QWebPage>
#include "wrtBrowserDefs.h"


class QWebFrame;
class QWebPage;
class QTimer;

namespace GVA
{
    class GWebContentViewWidget;
    class ChromeWidget;

    enum Direction
    {
        DOWN,           // SOUTH 
        UP,             // NORTH 
        RIGHT,          // EAST
        LEFT,           // WEST
        BOTTOMRIGHT,    // SOUTHEAST 
        BOTTOMLEFT,     // SOUTHWEST
        TOPLEFT,        // NORTHWEST
        TOPRIGHT        // NORTHEAST
    };
    
    enum PanDirection {
        HorizontalPan,
        VerticalPan,
        RandomPan 
    };

    class DragPoint
    {
    public:
        QPoint iPoint;
        QTime  iTime;
    };

    class GWebTouchNavigation : public QObject
    {
    Q_OBJECT
    public:
        GWebTouchNavigation(QWebPage* webPage, GWebContentViewWidget* view);
        virtual ~GWebTouchNavigation();
        void install();
        void uninstall();
        void setPage( QWebPage * page);
        bool enabled() const { return m_enabled; }
        void setEnabled(bool value) { m_enabled = value; }
        void setChromeWidget(ChromeWidget* chrome) { m_chrome = chrome;}

    signals:
        void longPressEvent();// mouse long press signal    
        void focusElementChanged(wrtBrowserDefs::BrowserElementType &);// mouse long press signal
        void pageScrollPositionZero();
        
        /// Sent when this object starts panning/scrolling the page.  Can be useful for
        /// cancelling things like long-press timers which otherwise wouldn't get
        /// mouse-move events.
        void startingPanGesture(int directionHint);
        void mouseEvent(QEvent::Type type);

    protected slots:
        void scrollToEdge();
		void doubleClickTimerExpired();
		void timerControl();// local slot for controlling timer
		void pan();
		void kineticScroll();
		void BlockFocusChanged(QPoint pt);
		void onLoadStarted(); 
		void onLoadFinished(bool ok);
    protected:
        bool eventFilter(QObject *object, QEvent *event);
        void mousePressEvent(const QPoint& pos);
        void mouseMoveEvent(const QPoint& pos, const QPoint& diff);
        void mouseReleaseEvent(const QPoint& pos);
        void mouseDoubleClickEvent(const QPoint& pos);
        QWebFrame* getNextScrollableFrame(const QPoint& pos);
        void scrollFrame(const QPoint& diff);
  
        void startScrollTimer();
        void updateFlickScrollDistance();
        bool isFlick() ;
        QPoint speed() ;
        QPoint currentPos();
        QPoint previousPos();
        qreal dragTime() const;
        Direction findDirectionWithAngle(const QPoint& diff);
        bool isNear(qreal aAngleUnderTest, qreal aMinAngle, qreal aMaxAngle);
        qreal findAngle(const int& xDelta,const int& yDelta);
        int roundOff(qreal num);
        QRect findEnclosingBlock(QMouseEvent* ev);
        QWebHitTestResult getHitTestResult(QMouseEvent* ev);
        void calculateActualScrollDistance();
        void setNewScrollDistance(QPoint blockCanvasPoint, int thresholdCheckVal);
		void handleMousePressEvent(QMouseEvent* ev);
		void handleMouseReleaseEvent(QMouseEvent* ev);
        void handleDoubleClickEvent(QMouseEvent* ev);
		
        
	private:
		void highlightableElement(QMouseEvent* ev);
		QWebElement getClosestAnchorElement(QMouseEvent* ev);
		bool traverseNextNode(QWebElement parentNode,QWebElement& nextNode);
	    
		void handleHighlightChange(QMouseEvent* ev);
		bool canDehighlight(QMouseEvent* ev);
		void dehighlight(QMouseEvent* ev);
		void getFocusedElement();
		void startTimer();
        void stopTimer();

        
        void stopScrolling();        
        void startPanGesture(PanDirection);
        void panBy(const QPointF& delta);
        void scrollCurrentFrame (int dx, int dy);
        void setCurrentFrameScrollPosition (QPoint& pos);

        Qt::KeyboardModifier getEventModifier(const QPoint& pos);
        
    private:
        QWebPage* m_webPage;
        GWebContentViewWidget* m_view;
        QWebFrame* m_frame;
        ChromeWidget* m_chrome;
        bool m_scrolled;
        QPoint m_touchPosition;
        QPointF m_scrollDistance;  
        QPointF m_actualScrollDistance;
        QPointF m_prevPoint;
        QList<DragPoint> m_dragPoints;
        Direction m_flickDirection;
        QTime m_lastMoveEventTime;
		QTimer* m_doubleClickTimer;
        QMouseEvent *m_pressEvent;
        QMouseEvent *m_releaseEvent;
        QPoint m_focusedBlockPt;	
		QWebElement m_anchorElement;
		QPoint m_higlightedPos;
		bool m_ishighlighted;
		int m_offset;
		QTimer* m_longPressTimer;// long press timer
		QPoint m_initialSpeed;		
		qreal m_finalzoomfactor;
		QTimer* m_scrollTimer;
		QPoint  m_scrollDelta;
		QTimer* m_kineticTimer;
		QPointF m_kineticSpeed;
		QTime   m_actualTime;
		int     m_kineticScrollTime;
		QPoint  m_initialScrollPos;	
		bool m_isPanning;
        QTime m_delayedPressMoment;
        QPointF m_dragStartPos;
        PanDirection m_panDirection;
        QPointF m_panModeResidue;		
        bool m_enabled;
        
    bool m_isLoading;    
    };

}

#endif
