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

#ifndef _GINEBRA_FLYOUT_ANIMATOR_H_
#define _GINEBRA_FLYOUT_ANIMATOR_H_

#include <QObject>
#include "../visibilityanimator.h"

class ChromeSnippet;
class QPainterPath;

class FlyoutAnimator : public VisibilityAnimator
{
   Q_OBJECT

 public:
  FlyoutAnimator(ChromeSnippet* snippet);
  virtual ~FlyoutAnimator();
  void setPath(QPainterPath* path);
 public slots:
  virtual void updateVisibility(qreal step);
  
 private:
  QPainterPath* m_path;

};

#endif
