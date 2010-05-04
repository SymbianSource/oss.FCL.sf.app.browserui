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


#include "NativeChromeItem.h"
#include "ChromeSnippet.h"

namespace GVA {
  
  NativeChromeItem::NativeChromeItem(ChromeSnippet * snippet, QGraphicsItem* parent)
    : QGraphicsWidget(parent), 
      m_snippet(snippet)
  {
    //Set opacity from element CSS
    QString CSSOpacity = m_snippet->element().styleProperty("opacity", QWebElement::ComputedStyle);
    setOpacity(CSSOpacity.toFloat());
  }

  NativeChromeItem::~NativeChromeItem()
  {
  }

  void NativeChromeItem::CSSToQColor(QString cssColor, QColor & color){
    QStringList vals = cssColor.remove("rgb(").remove(")").split(", ");
    //qDebug() << vals[0] << ":" << vals[1] << ":" << vals[2];
    color.setRgb(vals[0].toInt(), vals[1].toInt(), vals[2].toInt());
  }
  
} // end of namespace GVA
