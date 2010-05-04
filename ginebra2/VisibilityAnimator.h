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


#ifndef __GINEBRA_VISIBILITYANIMATOR_H__
#define __GINEBRA_VISIBILITYANIMATOR_H__

#include <QObject>

class QTimeLine;

namespace GVA {
class ChromeSnippet;

/**
 * \brief Base class for visibility animation delegates
 * 
 * \sa ChromeSnippet::setVisibilityAnimator()
 */

class VisibilityAnimator : public QObject
{
   Q_OBJECT

 public:
  VisibilityAnimator(ChromeSnippet* snippet) : m_snippet(snippet), m_timeLine(0), m_visible(true){}
  virtual ~VisibilityAnimator();
  static VisibilityAnimator * create(const QString & name, ChromeSnippet* snippet);
  void setVisible(bool visible, bool animate = true);
  void setTimeline(QTimeLine * timeline){m_timeLine = timeline;}
 public slots:
  virtual void updateVisibility(qreal step) = 0;
 private slots:
  void onFinished();
 signals:
  void started(bool visible);
  void finished(bool visible);
  void moving(qreal delta);
 protected:
  ChromeSnippet* m_snippet;
  QTimeLine *m_timeLine;
  bool m_visible;
};

}
#endif
