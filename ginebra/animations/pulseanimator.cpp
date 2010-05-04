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


#include "pulseanimator.h"
#include "../chromesnippet.h"
#include "QTransform"

PulseAnimator::PulseAnimator(ChromeSnippet* m_snippet)
  : AttentionAnimator(m_snippet)
{
    m_timeLine = new QTimeLine(1500); //Default to 1 sec
    m_timeLine->setLoopCount(5);
    m_timeLine->setCurveShape(QTimeLine::SineCurve);
    connect(m_timeLine, SIGNAL(valueChanged(qreal)),
	    this, SLOT(updateAttention(qreal)));

}

PulseAnimator::~PulseAnimator()
{
}

void PulseAnimator::updateAttention(qreal step)
{
#ifdef Q_OS_SYMBIAN
  if(step < 0.1) step = 1.0;
  m_snippet->setOpacity(step);
#else
  QTransform t, m, b;
  step = step/2+ 0.5;
  if(step < 0.55) step = 1.0;
  m.translate(18,18);
  t.scale(step, step);
  b.translate(-18,-18);
  m_snippet->setTransform(b*t*m);
#endif
}
