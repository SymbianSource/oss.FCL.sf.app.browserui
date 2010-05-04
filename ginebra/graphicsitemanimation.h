/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef GRAPHICSITEMANIMATION_H
#define GRAPHICSITEMANIMATION_H

#include <QObject>
#include <QTimeLine>
#include <assert.h>
#include "chromesnippet.h"
#include "utilities.h"

// Base class for ChromeSnippet animations.
class GraphicsItemAnimator
{
    friend class GraphicsItemAnimation;

    virtual void apply(ChromeSnippet *snippet, qreal value) = 0;
  public:
    virtual ~GraphicsItemAnimator() {}
};

// ----------------------

/** 
 * \brief Provides fade animations for ChromeSnippets.
 * 
 * \sa GraphicsItemAnimation
 */
class GraphicsItemAnimatorFade : public GraphicsItemAnimator
{
public:
    GraphicsItemAnimatorFade(ChromeSnippet *snippet, qreal opacityDelta)
      : m_opacityOriginal(snippet->opacity()),
        m_opacityDelta(opacityDelta) {
    }

    virtual void apply(ChromeSnippet *snippet, qreal value) {
        snippet->setOpacity(m_opacityOriginal + value * m_opacityDelta);
    }

private:
    qreal m_opacityOriginal;
    qreal m_opacityDelta;
};

// ----------------------

/** 
 * \brief Provides translation animations for ChromeSnippets.
 * 
 * \sa GraphicsItemAnimation
 */
class GraphicsItemAnimatorTranslate : public GraphicsItemAnimator
{
public:
    GraphicsItemAnimatorTranslate(ChromeSnippet *snippet, qreal dx, qreal dy)
      : m_posOriginal(snippet->pos()),
        m_xDelta(dx),
        m_yDelta(dy) {
    }

    virtual void apply(ChromeSnippet *snippet, qreal value) {
        snippet->setPos(m_posOriginal.x() + m_xDelta * value, m_posOriginal.y() + m_yDelta * value);
    }

private:
    QPointF m_posOriginal;
    qreal m_xDelta;
    qreal m_yDelta;
};

// ----------------------

/** 
 * \brief Provides rotation animations for ChromeSnippets.
 * 
 * \sa GraphicsItemAnimation
 */
class GraphicsItemAnimatorRotate : public GraphicsItemAnimator
{
public:
    GraphicsItemAnimatorRotate(ChromeSnippet *snippet, qreal angleDelta, qreal xCenter, qreal yCenter)
      : m_originalTransform(snippet->transform()),
        m_angleDelta(angleDelta),
        m_xCenter(xCenter),
        m_yCenter(yCenter)
    {
    }

    virtual void apply(ChromeSnippet *snippet, qreal value) {
        QTransform transform = m_originalTransform;
        transform.translate(m_xCenter, m_yCenter);
        transform.rotate(m_angleDelta * value);
        transform.translate(-m_xCenter, -m_yCenter);
        snippet->setTransform(transform);
    }

private:
    QTransform m_originalTransform;
    qreal m_angleDelta;
    qreal m_xCenter, m_yCenter;
};

// ----------------------

/**
 * \brief Basic animations for ChromeSnippets
 * 
 * The GraphicsItemAnimation class provides basic animations for ChromeSnippets.
 * 
 * Calls to translateTo(), translateBy(), fadeTo() etc. can be chained together in javascript as follows:
 *   \code window.snippets.TopChromeId.animate(500).translateBy(10,30).fadeBy(-0.5).start()\endcode
 *   
 * \sa ChromeSnippet::animate()
 * \sa GraphicsItemAnimator
 * \sa AttentionAnimator
 * \sa VisibilityAnimator
 */
class GraphicsItemAnimation : public QObject
{
    Q_OBJECT
public:
    GraphicsItemAnimation(ChromeSnippet *snippet, int duration = 500)
      : m_snippet(snippet),
        m_timeLine(duration)
    {
        setObjectName("animation");
        m_timeLine.setFrameRange(0, duration/50);
        safe_connect(&m_timeLine, SIGNAL(valueChanged(qreal)), this, SLOT(update(qreal)));
        safe_connect(&m_timeLine, SIGNAL(finished()), this, SIGNAL(finished()));
    }

    ~GraphicsItemAnimation();

public slots:
    QObject *translateTo(int x, int y)
    {
        QPointF pos = m_snippet->pos();
        m_animators.append(new GraphicsItemAnimatorTranslate(m_snippet, x - pos.x(), y - pos.y()));
        return this;
    }

    QObject *translateBy(int dx, int dy)
    {
        m_animators.append(new GraphicsItemAnimatorTranslate(m_snippet, dx, dy));
        return this;
    }

    QObject *fadeTo(qreal opacity)
    {
        m_animators.append(new GraphicsItemAnimatorFade(m_snippet, opacity - m_snippet->opacity()));
        return this;
    }

    QObject *fadeBy(qreal dOpacity)
    {
        m_animators.append(new GraphicsItemAnimatorFade(m_snippet, dOpacity));
        return this;
    }

    QObject *rotateBy(qreal angle, qreal xCenter, qreal yCenter)
    {
        m_animators.append(new GraphicsItemAnimatorRotate(m_snippet, angle, xCenter, yCenter));
        return this;
    }

    QObject *start()
    {
        m_timeLine.stop();
        m_timeLine.start();
        return this;
    }

    QObject *stop()
    {
        m_timeLine.stop();
        return this;
    }

signals:
    void finished();
    void updated(qreal value);

protected slots:
    void update(qreal value)
    {
        foreach(GraphicsItemAnimator *animator, m_animators)
        {
            animator->apply(m_snippet, value);
        }
        emit updated(value);
    }

private:
    ChromeSnippet *m_snippet;
    QTimeLine m_timeLine;
    QList<GraphicsItemAnimator *> m_animators;
};

#endif // GRAPHICSITEMANIMATION_H
