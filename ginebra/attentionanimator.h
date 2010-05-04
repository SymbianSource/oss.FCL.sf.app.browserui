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


#ifndef _GINEBRA_ATTENTION_ANIMATOR_H_
#define _GINEBRA_ATTENTION_ANIMATOR_H_

#include <QObject>

class ChromeSnippet;
class QTimeLine;

/**
 * \brief Base class for attention animation delegates
 * 
 * \sa ChromeSnippet::setAttentionAnimator()
 */
class AttentionAnimator : public QObject
{
   Q_OBJECT

 public:
  AttentionAnimator(ChromeSnippet* snippet):m_snippet(snippet), m_active(false), m_timeLine(0){}
  static AttentionAnimator * create(const QString & name, ChromeSnippet* snippet); //Factory method
  void toggleActive(); //NB: make this a slot?
  virtual ~AttentionAnimator();
  void setTimeline(QTimeLine * timeline){m_timeLine = timeline;}
 public slots:
  virtual void updateAttention(qreal step) = 0;
  
 protected:
  ChromeSnippet* m_snippet;
  bool m_active;
  QTimeLine *m_timeLine;

};

#endif
