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


#include "flyoutanimator.h"
#include <QPainterPath>
#include "../chromesnippet.h"
#include <QList>

FlyoutAnimator::FlyoutAnimator(ChromeSnippet* m_snippet) 
  : VisibilityAnimator(m_snippet)
{
  m_path = new QPainterPath();
  m_path->moveTo(0,0);
  m_path->quadTo(m_snippet->ownerArea().width()*2,-m_snippet->ownerArea().height(), 500, -400); //QUAD RIGHT
}

FlyoutAnimator::~FlyoutAnimator()
{
  delete m_path;
}

void FlyoutAnimator::setPath(QPainterPath * path)
{
  delete m_path;
  m_path = path;
}

void FlyoutAnimator::updateVisibility(qreal start)
{
  
  qreal pathPercent = start;
  QList<QGraphicsItem*> snippets = m_snippet->childItems();
  if(snippets.size() > 0){
    for(int i = 0; i < snippets.size(); i++){
      if(pathPercent < 1.0){
        qreal xoffset = m_path->pointAtPercent(pathPercent).x();
        qreal yoffset = m_path->pointAtPercent(pathPercent).y();
	snippets[i]->setPos(xoffset, yoffset);
        pathPercent += 0.05;
      }
    }
    if(start == 0){
      m_snippet->updateChildGeometries(); // Make sure we go back to original state
    }
  } else {
    QTransform transform;
    transform.translate(m_path->pointAtPercent(start).x(), m_path->pointAtPercent(start).y());
    m_snippet->setTransform(transform);
  }
 
}
