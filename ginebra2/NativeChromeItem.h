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


#ifndef __GINEBRA_NATIVECHROMEITEM_H__
#define __GINEBRA_NATIVECHROMEITEM_H__

#include <QtGui>
#include <QWebElement>

namespace GVA {

class ChromeSnippet;

  class NativeChromeItem : public QGraphicsWidget
  {
  public:
    NativeChromeItem(ChromeSnippet * snippet,  QGraphicsItem* parent=0);
    virtual ~NativeChromeItem();
    static void CSSToQColor(QString cssColor, QColor & color);
  protected:
    ChromeSnippet * m_snippet;
  };

} // end of namespace GVA

#endif // __GINEBRA_NATIVECHROMEITEM_H__
