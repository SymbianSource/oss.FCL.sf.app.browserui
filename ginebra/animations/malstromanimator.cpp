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


#include "malstromanimator.h"
#include <QPainterPath>
#include "../chromesnippet.h"
#include <QList>

MalstromAnimator::MalstromAnimator(ChromeSnippet* m_snippet) 
  : VisibilityAnimator(m_snippet)
{
  m_path = new QPainterPath();
  m_path->moveTo(0,0);
  //m_path->lineTo(m_snippet->ownerArea().width()*2, 0); //LINE RIGHT
  //m_path->quadTo(m_snippet->ownerArea().width()*2,-m_snippet->ownerArea().height(), 500, -400); //QUAD RIGHT
  m_path->arcTo(0.0, 0.0, 300.0, -200.0, 0.0, 60.0);
}

MalstromAnimator::~MalstromAnimator()
{
  delete m_path;
}

void MalstromAnimator::setPath(QPainterPath * path)
{
  delete m_path;
  m_path = path;
}

void MalstromAnimator::updateVisibility(qreal start)
{
  
  qreal pathPercent = start;
  QList<QGraphicsItem*> snippets = m_snippet->childItems();
  if(snippets.size() > 0){
    for(int i = 0; i < snippets.size(); i++){
      if(pathPercent < 1.0){
	int xoffset = (int)m_path->pointAtPercent(pathPercent).x();
	int yoffset = (int)m_path->pointAtPercent(pathPercent).y();
	snippets[i]->setPos(xoffset, yoffset);
        QTransform t;
        t.scale(1.0-start, 1.0-start);
        snippets[i]->setTransform(t);
	pathPercent += 0.1;
        if(pathPercent > 0.9)
	  snippets[i]->setOpacity(0.0);
        else
          snippets[i]->setOpacity(1.0);
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
