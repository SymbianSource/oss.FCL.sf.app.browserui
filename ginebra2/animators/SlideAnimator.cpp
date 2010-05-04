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


#include "SlideAnimator.h"
#include "../ChromeSnippet.h"
#include <QList>
#include <QDebug>

namespace GVA {

SlideAnimator::SlideAnimator(ChromeSnippet* m_snippet) 
  : VisibilityAnimator(m_snippet)
{
  //  m_visiblePosition = m_snippet->widget()->pos();
  setDirection("up");
  m_originalTransform = m_snippet->widget()->transform();
}

SlideAnimator::~SlideAnimator()
{
}

void SlideAnimator::setDirection(const QString & direction)
{
  m_direction = direction;

  if(m_direction == "up"){
    m_xRange = 0;
    m_yRange = -m_snippet->widget()->size().height();
  }
  else if(m_direction == "down"){
    m_xRange = 0;
    m_yRange = m_snippet->widget()->size().height();
  }  
  else if(m_direction == "left"){
    m_xRange = -m_snippet->widget()->size().width();
    m_yRange = 0;
  }  
  else if(m_direction == "right"){
    m_xRange = m_snippet->widget()->size().width();
    m_yRange = 0;
  }  
}

void SlideAnimator::updateVisibility(qreal step)
{
  qDebug() << step;
  QTransform transform = m_originalTransform;
  transform.translate(step*m_xRange, step*m_yRange);
  m_snippet->widget()->setTransform(transform);
  emit moving(step);
}

}
