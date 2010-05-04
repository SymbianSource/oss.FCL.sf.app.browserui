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


#include "FadeAnimator.h"
#include "../ChromeSnippet.h"
#include <QList>
#include <QDebug>

namespace GVA {

FadeAnimator::FadeAnimator(ChromeSnippet* m_snippet) 
  : VisibilityAnimator(m_snippet),
    m_minOpacity(0.1)
{
  // If the snippet is currently hidden, it will be shown
  // before the animator starts. It should therefore show
  // with minimum opacity.

  if(m_snippet->widget() && !m_snippet->widget()->isVisible())
    m_snippet->widget()->setOpacity(m_minOpacity);
}

FadeAnimator::~FadeAnimator()
{
}

void FadeAnimator::updateVisibility(qreal step)
{
  qreal o = 1.0 - step;
  m_snippet->widget()->setOpacity((o > m_minOpacity)?o:m_minOpacity);
      
  qDebug() << step;
}

}
