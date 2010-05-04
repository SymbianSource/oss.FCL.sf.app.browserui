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


#ifndef __GINEBRA_WEBCHROMECONTAINERSNIPPET_H__
#define __GINEBRA_WEBCHROMECONTAINERSNIPPET_H__

#include <QtGui>
#include "ChromeSnippet.h"

namespace GVA {

  class ChromeWidget;

  class WebChromeContainerSnippet : public ChromeSnippet
  {
    Q_OBJECT
  public:
    WebChromeContainerSnippet(const QString& elementId, ChromeWidget * chrome, const QRectF& ownerArea, const QWebElement & element, QGraphicsWidget * widget);
    virtual ~WebChromeContainerSnippet();
    void addChild(ChromeSnippet * child);
    QGraphicsLinearLayout* layout() { return m_layout;}

  public slots:
    void setLayoutHeight(int height);

  protected slots:
    void positionChildren();
  private slots:
    void updateOwnerArea();
  private:

    QRectF m_ownerArea;
    qreal m_layoutHeight;
    QGraphicsLinearLayout * m_layout;
  };

} // end of namespace GVA

#endif // __GINEBRA_WEBCHROMECONTAINERSNIPPET_H__
