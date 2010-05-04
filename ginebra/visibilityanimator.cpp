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


#include "visibilityanimator.h"
#include <QTimeLine>
// NB: These includes go away when plugins are implemented
#include "animations/flyoutanimator.h"
#include "animations/fadeanimator.h"
#include "animations/malstromanimator.h"
#include "animations/slideanimator.h"

VisibilityAnimator::~VisibilityAnimator(){
  delete m_timeLine;
}

// NB: Replace factory implementation with hash table populated by plugin discovery

VisibilityAnimator * VisibilityAnimator:: create(const QString & name, ChromeSnippet* snippet){
  if(name.compare("G_VISIBILITY_SLIDE_ANIMATOR") == 0){
      return new SlideAnimator(snippet);
  }
  if(name.compare("G_VISIBILITY_FLYOUT_ANIMATOR") == 0){
    return new FlyoutAnimator(snippet);
  }
  if(name.compare("G_VISIBILITY_FADE_ANIMATOR") == 0){
    return new FadeAnimator(snippet);
  }
  if(name.compare("G_VISIBILITY_MALSTROM_ANIMATOR") == 0){
    return new MalstromAnimator(snippet);
  }
  return 0;
}

void VisibilityAnimator::toggleVisibility(){
 if(!m_timeLine) {
   m_timeLine = new QTimeLine(1000); //Default to 1 sec
   connect(m_timeLine, SIGNAL(valueChanged(qreal)),
	   this, SLOT(updateVisibility(qreal)));
  }
  else {
    m_timeLine->stop();
  }

 if(m_visible) {
   m_visible = false;
   m_timeLine->setDirection(QTimeLine::Forward);
 }
 else {
   m_visible = true;
   m_timeLine->setDirection(QTimeLine::Backward);
   m_timeLine->setStartFrame(m_timeLine->endFrame());
 }
 m_timeLine->start();
}
