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
 * This class extends WebChromeContainerSnippet class 
 *
 */

#include "ActionButton.h"
#include "ActionButtonSnippet.h"
#include "ChromeRenderer.h"
#include "ChromeWidget.h"
#include "NativeChromeItem.h"
#include "TitleUrlContainerSnippet.h"
#include "UrlSearchSnippet.h"
#include "WebChromeSnippet.h"
#include "Utilities.h"

#include "QGraphicsPixmapItem"

#include "ChromeEffect.h"

#define BACKGROUND_GRADIENT_START "#000000"
#define BACKGROUND_GRADIENT_END "#333333"

#include <QDebug>

namespace GVA {

// Need to read this from the platform later
#define ICONWIDTH 65
#define ICONHEIGHT 44

TitleUrlContainerItem::TitleUrlContainerItem(ChromeSnippet * snippet, ChromeWidget * chrome, QGraphicsItem * parent)
: NativeChromeItem(snippet, parent)
, m_chrome(chrome)
{
    setObjectName("TitleUrl");
    setProperties();    
    // Create the view port widget
    m_viewPort = new QGraphicsWidget(this);
    m_viewPort->setFlags(QGraphicsItem::ItemClipsChildrenToShape);
    
    // Create title-url widget(still by the name UrlSearchItem)
    m_urlTileWidget = new GUrlSearchItem(snippet, chrome, m_viewPort); 

    QPixmap dividerPixmap;
    dividerPixmap.load(":/urlsearch/URL_search_divider.png", "PNG");
    m_dividerImage = new QGraphicsPixmapItem(dividerPixmap, m_viewPort);

    QAction * backAction = new QAction(this);
    connect(backAction, SIGNAL(triggered()), m_chrome, SIGNAL(goToBackground()));
	backAction->setObjectName("BackAction");

    m_backStepButton = new ActionButton(snippet, "BackStep", m_viewPort);
    m_backStepButton->setAction(backAction);
    m_backStepButton->setActiveOnPress(true);
    //m_backStepButton->addIcon(":/urlsearch/backstep.png", QIcon::Normal);
    m_backStepButton->addIcon(":/urlsearch/backstep_no_bg.png", QIcon::Normal);
    m_backStepButton->addIcon(":/urlsearch/backstep_pressed.png", QIcon::Active);

    // Monitor resize events.
    safe_connect(m_chrome->renderer(), SIGNAL(chromeResized()), this, SLOT(onChromeResize()));
    safe_connect(m_urlTileWidget, SIGNAL(changeEditMode(bool)), this, SLOT(changeLayout(bool)));
    safe_connect(m_urlTileWidget, SIGNAL(contextEvent(bool)), this, SIGNAL(contextEvent(bool)));
}

TitleUrlContainerItem::~TitleUrlContainerItem()
{

}

void TitleUrlContainerItem::cut()
{
    m_urlTileWidget->cut();
}

void TitleUrlContainerItem::copy()
{
    m_urlTileWidget->copy();
}

void TitleUrlContainerItem::paste()
{
    m_urlTileWidget->paste();
}

void TitleUrlContainerItem::setContextMenuStatus(bool on)
{ 
    m_urlTileWidget->setContextMenuStatus(on);
}

void TitleUrlContainerItem::setProperties() {
  m_grad.setColorAt(0, BACKGROUND_GRADIENT_START);
  m_grad.setColorAt(1, BACKGROUND_GRADIENT_END);
}

void TitleUrlContainerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget){
  Q_UNUSED(option)
  Q_UNUSED(widget)

  painter->save();

  qreal bounding_x = boundingRect().x();
  qreal bounding_y = boundingRect().y();
  qreal bounding_width = boundingRect().width();
  qreal bounding_height = boundingRect().height();


  // background
  painter->setRenderHint(QPainter::Antialiasing);
  painter->setPen(Qt::NoPen);
  m_grad.setStart(bounding_x, bounding_y + bounding_height);
  m_grad.setFinalStop(bounding_x, bounding_y);
  painter->fillRect(boundingRect(), QBrush(m_grad));
  // top border
  painter->setPen(QPen(QColor(53,53,53)));
  painter->drawLine(bounding_x, bounding_y, bounding_x + bounding_width, bounding_y);
  // bottom border
  painter->setPen(QPen(QColor(76,76,78)));
  painter->drawLine(bounding_x, bounding_y + bounding_height, bounding_x + bounding_width, bounding_y + bounding_height);
  // right border
  painter->setPen(QPen(QColor(0,0,0)));
  painter->drawLine(bounding_x + bounding_width, bounding_y, bounding_x + bounding_width, bounding_y + bounding_height);
  // left border
  painter->setPen(Qt::NoPen);
  QLinearGradient grad = QLinearGradient(bounding_x, bounding_y, bounding_x, bounding_y + bounding_height);
  grad.setColorAt(0, QColor(41,41,41));
  grad.setColorAt(0.5, QColor(83,83,84));
  grad.setColorAt(1, QColor(2,2,2));
  painter->setBrush(QBrush(grad));
  painter->drawLine(bounding_x, bounding_y, bounding_x, bounding_y + bounding_height);
  
  painter->restore();
  
  NativeChromeItem::paint(painter, option, widget);
}

void TitleUrlContainerItem::onChromeResize() {

    QWebElement we = m_snippet->element();
    QRectF g = we.geometry();
    QGraphicsWidget::resize(g.width(), g.height());
}

void TitleUrlContainerItem::resizeEvent(QGraphicsSceneResizeEvent * event){
    QSizeF size = event->newSize();

    qreal width = size.width();
    qreal height = size.height();

    m_viewPort->setGeometry(
            0,
            0,
            width,
            height);

    m_backStepButton->setGeometry(
                width - ICONWIDTH, 
                0,
                ICONWIDTH,
                height);

    // When we first get resize event, the widget is not yet set to visible by
    // Ginebra. If the widget is not visible and for later resize events, if back-step
    // button is visible, set the width of url-title widget taking into account
    // the width of back-step button
    if ((!this->isVisible()) || m_backStepButton->isVisible()) {

        width =  width - ICONWIDTH - m_dividerImage->boundingRect().width();
    }
    m_urlTileWidget->setGeometry(0,
                0,
                width,
                height);
    
    m_dividerImage->setPos(m_urlTileWidget->rect().width() + 1, (height - m_dividerImage->boundingRect().height())/2);
}


void TitleUrlContainerItem::changeLayout(bool editMode){

    qreal width = m_viewPort->geometry().width();
    qreal height = m_viewPort->geometry().height();

    // If we are changing to edit mode, we need to hide the back-step button 
    if (editMode) {

        m_backStepButton->hide();
        m_dividerImage->hide();
    }
    else {

        // show back-step button and re-layout the widgets
        m_backStepButton->show();
        m_dividerImage->show();
        width = width - ICONWIDTH - m_dividerImage->boundingRect().width();
    }
    m_urlTileWidget->setGeometry(0,
                0,
                width,
                height);
}

QString TitleUrlContainerItem::url() const
{
    return m_urlTileWidget->url();
}

// TitleUrlContainerSnippet class
TitleUrlContainerSnippet::TitleUrlContainerSnippet(const QString & elementId, ChromeWidget * chrome,
                                                   QGraphicsWidget * widget, const QWebElement & element)
  : ChromeSnippet(elementId, chrome, widget, element)
{
}

TitleUrlContainerSnippet::~TitleUrlContainerSnippet()
{
}

TitleUrlContainerSnippet * TitleUrlContainerSnippet::instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element)
{
    TitleUrlContainerSnippet * that = new TitleUrlContainerSnippet( elementId, chrome, 0, element );
    TitleUrlContainerItem * titleUrlContainerItem = new TitleUrlContainerItem(that, chrome);
    safe_connect(titleUrlContainerItem, SIGNAL(contextEvent(bool)), that, SLOT(sendContextMenuEvent(bool)));
    that->setChromeWidget(titleUrlContainerItem);
    return that;
}

QString TitleUrlContainerSnippet::url() const
{
    TitleUrlContainerItem const *urlContainer;
    
    urlContainer = dynamic_cast<TitleUrlContainerItem const *> (constWidget());
    return urlContainer->url();
}

void TitleUrlContainerSnippet::sendContextMenuEvent(bool isContentSelected)
{
    emit contextEvent(isContentSelected, elementId());     
}

} // end of namespace GVA



