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


#ifndef __GINEBRA_WEBCHROMESNIPPET_H__
#define __GINEBRA_WEBCHROMESNIPPET_H__

#include <QtGui>
#include "ChromeSnippet.h"
#include <QWebElement>

namespace GVA {

  class ChromeRenderer;
  class ChromeWidget;
  class WebChromeItem;

  class WebChromeSnippet : public ChromeSnippet
  {
    Q_OBJECT
  public:
    WebChromeSnippet(const QString& elementId, ChromeWidget * chrome, const QRectF& ownerArea, const QWebElement & element);
    virtual ~WebChromeSnippet();
    WebChromeItem* item();
  public slots:
    void grabFocus();
    void updateOwnerArea();
  };

} // end of namespace GVA

#endif // __GINEBRA_WEBCHROMESNIPPET_H__
