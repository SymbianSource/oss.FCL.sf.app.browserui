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


#include "iconwidget.h"
#include "webpagecontroller.h"
#include <QWebSettings>

namespace GVA {

IconWidget::IconWidget(ChromeSnippet * snippet, QGraphicsItem* parent)
        : NativeChromeItem(snippet, parent)
{
    m_drawingDefault = false;
    m_icon = QIcon();
    m_defaultImage = QImage(":/chrome/demochrome/Scroll.png");
    }

void IconWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget)
{
    painter->save();
    if (m_drawingDefault) {
        painter->drawImage(QRectF(0,0, geometry().width(), geometry().height()), m_defaultImage);
    }
    else {
        if (!m_icon.isNull()) {
            m_icon.paint(painter, 0, 0, geometry().width(), geometry().height());
        }
    }
    painter->restore();
}

void IconWidget::onLoadStarted()
{
    m_drawingDefault = true;
    update();
}

void IconWidget::onIconChanged()
{
    m_drawingDefault = false;
    WebPageController* pageController = WebPageController::getSingleton();
    m_icon = pageController->pageIcon();
    if (!m_icon.isNull()) {
        update();
    }
}
void IconWidget::onUrlChanged(const QUrl& url)
{
    m_icon = QWebSettings::iconForUrl(url);
    if (!m_icon.isNull()) {
        m_drawingDefault = false;
        update();
    }
}

void IconWidget::connectToWebpageController()
{
    WebPageController* pageController = WebPageController::getSingleton();
    connect(pageController, SIGNAL(loadStarted()), this, SLOT(onLoadStarted()));
    connect(pageController, SIGNAL(currentPageUrlChanged(const QUrl&)), this, SLOT(onUrlChanged(const QUrl&)));
    connect(pageController, SIGNAL(currentPageIconChanged()), this, SLOT(onIconChanged()));
}

void IconWidget::setIconForUrl(const QUrl& url) {
    m_drawingDefault = false;
    m_icon = QWebSettings::iconForUrl(url);
    update();
}
void IconWidget::setDefaultImage(const QImage& image) {
    m_drawingDefault = true;
    m_defaultImage = image;
    update();
}
}//end of name space
