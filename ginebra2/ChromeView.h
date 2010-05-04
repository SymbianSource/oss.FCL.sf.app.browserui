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


#ifndef __GINEBRA_CHROMEVIEW_H__
#define __GINEBRA_CHROMEVIEW_H__
#include <QtGui>

namespace GVA {

class ChromeWidget;

class ChromeView : public QGraphicsView
{
    Q_OBJECT
 public:
  ChromeView(ChromeWidget * chrome, QWidget * parent = 0 );
  ~ChromeView();
  void resizeEvent(QResizeEvent * ev);
  bool event(QEvent* event);
  void scrollContentsBy(int dx, int dy);
  //protected:
  //bool eventFilter(QObject * obj, QEvent * ev);
 
#ifdef Q_OS_SYMBIAN
#ifdef SET_DEFAULT_IAP
  public slots:
  void setDefaultIap();
#endif 
#endif

 private:
  QGraphicsWidget * m_topWidget;
};
} 
#endif //__GINEBRA_CHROMEVIEW_H__
