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


#ifndef _GINEBRA_VISIBILITY_ANIMATOR_H_
#define _GINEBRA_VISIBILITY_ANIMATOR_H_

#include <QObject>

class ChromeSnippet;
class QTimeLine;


/**
 * \brief Base class for attention visibility delegates
 * 
 * \sa ChromeSnippet::setVisibilityAnimator()
 */
class VisibilityAnimator : public QObject
{
   Q_OBJECT

 public:
  VisibilityAnimator(ChromeSnippet* snippet):m_snippet(snippet), m_visible(true), m_timeLine(0){}
  virtual ~VisibilityAnimator();
  static VisibilityAnimator * create(const QString & name, ChromeSnippet* snippet);
  void toggleVisibility();
  void setTimeline(QTimeLine * timeline){m_timeLine = timeline;}
 public slots:
  virtual void updateVisibility(qreal step) = 0;
  
 protected:
  ChromeSnippet* m_snippet;
  bool m_visible;
  QTimeLine *m_timeLine;

};

#endif
