/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __GINEBRA_PROGRESSSNIPPET_H__
#define __GINEBRA_PROGRESSSNIPPET_H__

#include <QtGui>
#include "NativeChromeItem.h"

namespace GVA {

  class GWebContentView;

  class ProgressSnippet : public NativeChromeItem
  {
    Q_OBJECT
  public:
    ProgressSnippet(ChromeSnippet * snippet, QGraphicsItem* parent = 0);
    virtual ~ProgressSnippet();
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget);
  public slots:
    void onProgress(int progress);
    void onStart();
    void onFinished(bool ok);
  private:
    qreal m_progress;
    QColor m_color;
    GWebContentView * m_webView;
  };

} // end of namespace GVA

#endif // __GINEBRA_PROGRESSSNIPPET_H__
