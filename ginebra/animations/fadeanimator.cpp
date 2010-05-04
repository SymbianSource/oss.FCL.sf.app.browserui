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


#include "fadeanimator.h"
#include "../chromesnippet.h"
#include <QList>

FadeAnimator::FadeAnimator(ChromeSnippet* m_snippet) 
  : VisibilityAnimator(m_snippet),
    m_minOpacity(0.1)
{
}

FadeAnimator::~FadeAnimator()
{
}

void FadeAnimator::updateVisibility(qreal step)
{
  //qDebug() << step;
  qreal o = 1.0 - step;
  if(o > m_minOpacity){
    m_snippet->setOpacity(o);
  }
}
