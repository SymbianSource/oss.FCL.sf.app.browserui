/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not,
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
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
    virtual void addChild(ChromeSnippet * child);
    QGraphicsLinearLayout* layout() { return m_layout;}
    QRectF ownerArea() { return m_ownerArea;}

  Q_SIGNALS:
    void childAdded(ChromeSnippet * );
  public slots:
    void setLayoutHeight(int height);
    void setLayoutWidth(int width, bool update = false);
    virtual void updateSize(QSize );
    virtual void updateOwnerArea();

  protected slots:
    void positionChildren();

  private:
    void setOwnerArea();

    QRectF m_ownerArea;
    qreal m_layoutHeight;
    qreal m_layoutWidth;
    QGraphicsLinearLayout * m_layout;
  };

} // end of namespace GVA

#endif // __GINEBRA_WEBCHROMECONTAINERSNIPPET_H__
