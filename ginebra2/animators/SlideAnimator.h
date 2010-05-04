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


#ifndef __GINEBRA_SLIDEANIMATOR_H__
#define __GINEBRA_SLIDEANIMATOR_H__

#include <QObject>
#include <QTransform>

#include "../VisibilityAnimator.h"

namespace GVA {
class ChromeSnippet;

class SlideAnimator : public VisibilityAnimator
{
   Q_OBJECT

 public:
  SlideAnimator(ChromeSnippet* snippet);
  virtual ~SlideAnimator();
  void setDirection(const QString& direction);
 public slots:
  virtual void updateVisibility(qreal step);
 private:
  QString m_direction;
  //QPoint m_visiblePos;
  QTransform m_originalTransform;
  qreal m_xRange;
  qreal m_yRange;
};
}
#endif
