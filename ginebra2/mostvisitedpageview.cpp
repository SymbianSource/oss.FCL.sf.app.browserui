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


#include <QtGui>

#include "mostvisitedpagestore.h"
#include "linearflowsnippet.h"
#include "mostvisitedpageview.h"
#include "webpagecontroller.h"
#include "BookmarksManager.h"

const int KLinearSnippetHeight = 120;

namespace GVA {
MostVisitedPagesWidget::MostVisitedPagesWidget(ChromeSnippet* snippet,QGraphicsWidget* parent) 
                        : m_snippet(snippet)
                        , QGraphicsWidget(parent)
                        , m_parent(parent)
                        , m_flowInterface(0)
                        , m_hideOnClose(true)
{
    setFlags(QGraphicsItem::ItemDoesntPropagateOpacityToChildren);
    setOpacity(0.5);
    m_mostVisitedPageStore = new MostVisitedPageStore();
    WebPageController* pageController = WebPageController::getSingleton();
    bool ret = connect(pageController, SIGNAL(loadFinished(const bool)), this, SLOT(onLoadFinished(const bool)));
}

MostVisitedPagesWidget::~MostVisitedPagesWidget()
{
    if (m_flowInterface)
        m_flowInterface->deleteLater();
    if (m_mostVisitedPageStore)
        delete m_mostVisitedPageStore;
    WebPageController* pageController = WebPageController::getSingleton();
    disconnect(pageController, SIGNAL(loadFinished()), this, SLOT(onLoadFinished()));
}


void MostVisitedPagesWidget::open()
{
    if (m_flowInterface)
        delete m_flowInterface;

    m_flowInterface = new GVA::LinearFlowSnippet(this);
    m_flowInterface->setZValue(m_parent->zValue() + 1);
    m_flowInterface->resize(QSize(m_parent->size().width(), KLinearSnippetHeight));
    connect(m_flowInterface, SIGNAL(mouseEvent(QEvent::Type)),this->m_snippet,SIGNAL(snippetMouseEvent(QEvent::Type)));
    
    //Initialize the page selection index
    m_selectIndex = -1;
    
    QString displayMode;

    if (m_parent->size().width() > m_parent->size().height()) {
        displayMode = "Landscape";
       
    } else {
        displayMode = "Portrait";
    }
    m_flowInterface->init(displayMode,"Most Visited Pages");

    MostVisitedPageList mvPageList = m_mostVisitedPageStore->pageList();

    for (int i = 0; i < mvPageList.size(); i++) {
        QImage *pageThumbnail = mvPageList[i]->m_pageThumbnail;
        bool removeTempThumbnail = false;
        if (!pageThumbnail) {
            removeTempThumbnail = true;
            pageThumbnail = new QImage("");
        }
        QUrl url = QUrl(mvPageList[i]->m_url);
        m_flowInterface->addSlide(*pageThumbnail, url.host());
        if (removeTempThumbnail) {
            delete pageThumbnail;
        }
    }
   
    setCenterIndex(displayMode);
    m_flowInterface->prepareStartAnimation();
    m_flowInterface->runStartAnimation();
    m_flowInterface->installEventFilter(this);
    connect(m_flowInterface, SIGNAL(ok(int)), this, SLOT(okTriggered(int)));
    connect(m_flowInterface, SIGNAL(endAnimationCplt()), this, SLOT(closeAnimationCompleted()));
}

void MostVisitedPagesWidget::close(bool hide)
{
    //This will trigger close animation after which
    //the cleanup is done in closeAnimationCompleted
    if (m_flowInterface)
        m_flowInterface->runEndAnimation();
    m_hideOnClose = hide;
}

void MostVisitedPagesWidget::updatePos(QPointF pos, qreal &toolBarHeight)
{
    QGraphicsWidget::setPos(pos);
    m_flowInterface->setPos(pos.x(), m_parent->size().height() - toolBarHeight - KLinearSnippetHeight);
}

void MostVisitedPagesWidget::resize(const QSize &size)
{
    QGraphicsWidget::resize(size);
    m_flowInterface->resize(QSize(m_parent->size().width(), KLinearSnippetHeight));
}

void MostVisitedPagesWidget::displayModeChanged(QString& newMode)
{
    m_flowInterface->displayModeChanged(newMode); 
}

void MostVisitedPagesWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->fillRect(0, 0, size().width(), size().height(), QColor(107, 109, 107));
    QGraphicsWidget::paint(painter, option, widget);
}

void MostVisitedPagesWidget::setCenterIndex(QString displayMode)
{
    //display beginning of the film strip first
    m_flowInterface->setCenterIndex((displayMode == "Portrait") ? 1 : 2);
}

void MostVisitedPagesWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    close();
}

void MostVisitedPagesWidget::okTriggered(int index)
{
    Q_ASSERT(m_flowInterface);
    m_selectIndex = index;
    close();    
}

void MostVisitedPagesWidget::closeAnimationCompleted()
{
    if (!m_flowInterface)
        return;

    QGraphicsWidget::hide();
    disconnect(m_flowInterface, SIGNAL(ok(int)), this, SLOT(okTriggered(int)));
    disconnect(m_flowInterface, SIGNAL(endAnimationCplt()), this, SLOT(closeAnimationCompleted()));
    disconnect(m_flowInterface, SIGNAL(mouseEvent(QEvent::Type)),this->m_snippet,SIGNAL(snippetMouseEvent(QEvent::Type)));

    m_flowInterface->removeEventFilter(this);
    m_flowInterface->deleteLater();
    m_flowInterface = NULL;
    
    if (m_selectIndex != -1)
        static_cast<ChromeWidget*>(m_parent)->loadUrlToCurrentPage(m_mostVisitedPageStore->pageAt(m_selectIndex)->pageUrl());

    if (m_snippet->isVisible() && m_hideOnClose)
        m_snippet->ChromeSnippet::toggleVisibility();

    m_hideOnClose = false;
    emit closeComplete();
}

void MostVisitedPagesWidget::updateMVGeometry()
{
    qreal toolBarHeight;

    ChromeSnippet* visibleSnippet= static_cast<ChromeWidget*>(m_parent)->getSnippet("WebViewToolbarId");
    if (visibleSnippet)
        toolBarHeight = visibleSnippet->widget()->geometry().height(); 

    resize(m_parent->size().toSize());
    updatePos(QPointF(0, 0), toolBarHeight);
}

 void MostVisitedPagesWidget::updateMVStore(QWebPage *page)
  {
    Q_ASSERT(page);
    Q_ASSERT(!page->mainFrame()->url().isEmpty());

    QImage* pageThumbnail = 0;
    QUrl pageUrl = page->mainFrame()->url();
    int pageRank = 0;
    
    //check if page exits in store along with its thumbnail
    if (!m_mostVisitedPageStore->contains(pageUrl.toString(), true)) {
        QSize thumbnailSize(200, 200);
        pageThumbnail = new QImage(thumbnailSize.width(), thumbnailSize.height(), QImage::Format_RGB32); 
        QPainter painter(pageThumbnail);
        qreal webcoreScale = page->mainFrame()->zoomFactor();
        painter.scale(1.f / webcoreScale, 1.f / webcoreScale);
        painter.fillRect(0, 0, size().width(), size().height(), QColor(255, 255, 255));
        page->mainFrame()->render(&painter, QWebFrame::AllLayers, QRegion(0, 0, thumbnailSize.width(), thumbnailSize.height()));
    }

    //if it is a new page to the store, get its rank from history
    //FIX ME : need to optimize this code
    pageRank = WRT::BookmarksManager::getSingleton()->getPageRank(pageUrl.toString());
    m_mostVisitedPageStore->pageAccessed(pageUrl, pageThumbnail, pageRank);
  }

 void MostVisitedPagesWidget::onLoadFinished(const bool ok)
 {
     if (ok) {
        WRT::WrtBrowserContainer * activePage = WebPageController::getSingleton()->currentPage();
        updateMVStore(activePage);
     }
 }

} // endof namespace GVA

