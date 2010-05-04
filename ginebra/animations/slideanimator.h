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

#ifndef _GINEBRA_SLIDE_ANIMATOR_H_
#define _GINEBRA_SLIDE_ANIMATOR_H_

#include <QObject>
#include "../visibilityanimator.h"

class ChromeSnippet;
class QPainterPath;

class SlideAnimator : public VisibilityAnimator
{
   Q_OBJECT
 enum direction {
     slideUp,
     slideDown,
     slideLeft,
     slideRight
   };
 public:
  SlideAnimator(ChromeSnippet* snippet);
  virtual ~SlideAnimator();
 public slots:
  virtual void updateVisibility(qreal step);
 private:
  ChromeSnippet * m_snippet;
  direction m_direction;
  qreal m_visiblePosition;
  qreal m_invisiblePosition;
  qreal m_moveBy;
};

#endif
