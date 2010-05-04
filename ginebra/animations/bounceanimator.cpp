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


#include "bounceanimator.h"
#include "../chromesnippet.h"
#include "QTransform"

BounceAnimator::BounceAnimator(ChromeSnippet* m_snippet)
  : AttentionAnimator(m_snippet)
{
}

BounceAnimator::~BounceAnimator()
{
}

void BounceAnimator::updateAttention(qreal step)
{
  
  qreal dy = (step -0.5)*10.0;
  //qDebug() << "Bounce%: " << step;
  m_snippet->moveBy(0.0, -dy);
  
}

