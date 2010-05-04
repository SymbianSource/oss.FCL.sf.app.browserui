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

#ifndef __g_graphicswebview_h__
#define __g_graphicswebview_h__
#include <QtGui>
#include <QGraphicsWebView>

namespace GVA {
  class GGraphicsWebView : public QGraphicsWebView
  {
    Q_OBJECT
  public:
    GGraphicsWebView(QGraphicsItem * parent = 0);
    void paint(QPainter * paint, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
  private slots:
    void onLoadStarted();
    void onLoadFinished(bool ok);
  private:
    QTime m_loadTime;
    bool m_loading;
  };
}
#endif
