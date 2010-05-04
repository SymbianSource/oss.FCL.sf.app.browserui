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


#ifndef ICONWIDGET_H
#define ICONWIDGET_H

#include "NativeChromeItem.h"
#include <QUrl>
#include <QIcon>
#include <QImage>

namespace GVA {

    class IconWidget : public NativeChromeItem
    {
        Q_OBJECT

    public:
        IconWidget(ChromeSnippet * snippet, QGraphicsItem* parent = 0);
        virtual ~IconWidget() {};
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget);

    public slots:
        void onLoadStarted();
        void onIconChanged();
        void onUrlChanged(const QUrl& url);
        void setIconForUrl(const QUrl& url);
        void setDefaultImage(const QImage& image);
        void connectToWebpageController();

    private:
        bool m_drawingDefault;
        QImage m_defaultImage;
        QIcon m_icon;
    };
} // end of namespace GVA

#endif // ICONWIDGET_H
