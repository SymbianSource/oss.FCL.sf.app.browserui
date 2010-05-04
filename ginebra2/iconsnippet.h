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


#ifndef ICONSNIPPET_H
#define ICONSNIPPET_H

#include "ChromeSnippet.h"
#include "ChromeWidget.h"
namespace GVA {


class IconSnippet : public ChromeSnippet
{
    Q_OBJECT
public:
    IconSnippet(const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element);
    virtual ~IconSnippet();

public slots:
    void connectToWebpageController();
    void setIconForUrl(const QUrl& url);
    void setDefaultImage(const QImage& image);

};
}

#endif // ICONSNIPPET_H
