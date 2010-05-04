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

#include "slideanimator.h"
#include "../chromesnippet.h"
#include <QList>

SlideAnimator::SlideAnimator(ChromeSnippet* snippet) 
  : VisibilityAnimator(snippet),
    m_snippet(snippet),
    m_direction(slideUp)
{
  QPointF pos = snippet->pos();
  if(snippet->anchor() == "AnchorBottom"){
    m_direction = slideDown;
    m_visiblePosition = pos.y();
    m_invisiblePosition = m_visiblePosition + snippet->geometry().height() + snippet->anchorOffset();
  }
  else if(snippet->anchor() == "AnchorLeft"){
    m_direction = slideLeft;
    m_visiblePosition = pos.x();
    m_invisiblePosition = m_visiblePosition - snippet->geometry().width() - snippet->anchorOffset();  
  }
  else if (snippet->anchor() == "AnchorRight"){
    m_direction = slideRight;
    m_visiblePosition = pos.x();
    m_invisiblePosition = m_visiblePosition + snippet->geometry().width() + snippet->anchorOffset();
  }
  else {
    m_visiblePosition = pos.y();
    m_invisiblePosition = m_visiblePosition - snippet->geometry().height() - snippet->anchorOffset();
  }
  m_moveBy = m_invisiblePosition - m_visiblePosition;
}

SlideAnimator::~SlideAnimator()
{
}

void SlideAnimator::updateVisibility(qreal step)
{
  qreal p = step*m_moveBy + m_visiblePosition;
  if((m_direction == slideDown) || (m_direction == slideUp)){
    if(step < 0.05){
      m_snippet->setPos(m_snippet->pos().x(), m_visiblePosition);
    }
    else if(step > 0.95){
       m_snippet->setPos(m_snippet->pos().x(), m_invisiblePosition);
     }
    else{
       m_snippet->setPos(m_snippet->pos().x(), p);
    }
  }
  else {
    if(step < 0.05){
      m_snippet->setPos(m_visiblePosition, m_snippet->pos().y());
    }
    else if(step > 0.95){
      m_snippet->setPos(m_invisiblePosition, m_snippet->pos().y());
    }
    else{
      m_snippet->setPos(p,m_snippet->pos().y());
    }
  }
}
