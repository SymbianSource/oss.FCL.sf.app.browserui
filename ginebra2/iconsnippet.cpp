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

#include "iconsnippet.h"
#include "iconwidget.h"

namespace GVA {

IconSnippet::IconSnippet(const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element) :
        ChromeSnippet(elementId, chrome, widget, element)
{

}
IconSnippet::~IconSnippet()
{

}
void IconSnippet::connectToWebpageController()
{
    static_cast<IconWidget*>(m_widget)->connectToWebpageController();
}

void IconSnippet::setIconForUrl(const QUrl& url)
{
    static_cast<IconWidget*>(m_widget)->setIconForUrl(url);
}

void IconSnippet::setDefaultImage(const QImage& image)
{
    static_cast<IconWidget*>(m_widget)->setDefaultImage(image);
}
}
