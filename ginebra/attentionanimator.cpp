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


#include "attentionanimator.h"
#include <QTimeLine>
// NB: These go away when plugins are implemented
#include "animations/bounceanimator.h"
#include "animations/pulseanimator.h"

AttentionAnimator::~AttentionAnimator(){
  delete m_timeLine;
}

//NB: Replace implementation with hash table populated by plugin discovery

AttentionAnimator * AttentionAnimator:: create(const QString & name, ChromeSnippet* snippet){
  if(name.compare("G_ATTENTION_BOUNCE_ANIMATOR") == 0){
    return new BounceAnimator(snippet);
  }
  if(name.compare("G_ATTENTION_PULSE_ANIMATOR") == 0){
    return new PulseAnimator(snippet);
  }
  return 0;
}


void AttentionAnimator::toggleActive(){
  if(!m_timeLine) {
    m_timeLine = new QTimeLine(1000); //Default to 1 sec
    m_timeLine->setLoopCount(3);
    m_timeLine->setCurveShape(QTimeLine::SineCurve);
    connect(m_timeLine, SIGNAL(valueChanged(qreal)),
	    this, SLOT(updateAttention(qreal)));
  }

  m_timeLine->start();
  /*
  if(m_active) {
    m_active = false;
    m_timeLine->stop();
  }
  else {
    m_active = true;
    m_timeLine->start();
    }*/
}
