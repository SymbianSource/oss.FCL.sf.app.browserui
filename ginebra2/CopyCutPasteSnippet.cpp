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

#include "CopyCutPasteSnippet.h"

#define BUTTON_WIDTH 85
#define BUTTON_HEIGHT 50
#define BUTTON_PADDING 0
#define TOP_PADDING 5
#define FIRST_BUTTON_OFFSET 0
#define SECOND_BUTTON_OFFSET BUTTON_WIDTH+BUTTON_PADDING
#define THIRD_BUTTON_OFFSET SECOND_BUTTON_OFFSET+SECOND_BUTTON_OFFSET
#ifdef BROWSER_LAYOUT_TENONE
  #define URLSEARCH_CHROME_ID "TitleUrlId"
#else
  #define URLSEARCH_CHROME_ID "UrlSearchChromeId"
#endif
#define MENU_DISPLAY_DURATION 5000
#define BACKGROUND_COLOR "#FFFFDD"

namespace GVA {

    CopyCutPasteButton::CopyCutPasteButton(const QString & text, QWidget * parent)
        : QObject(parent) 
        , m_text(text)
        , m_mousePressed(false)
        , m_size(QSize())
        , m_disabled(false)
    {
        
    }
    
    void CopyCutPasteButton::mousePressEvent(QGraphicsSceneMouseEvent * event)
    {
        Q_UNUSED(event);
        m_mousePressed = true;
    }
    
    void CopyCutPasteButton::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
    {
        Q_UNUSED(event);
        if (m_mousePressed)
            emit clicked();
        m_mousePressed = false; 
    }

    void CopyCutPasteButton::render(QPainter * painter, const QPoint & targetOffset)
    {
       painter->save();
       QFont textFont = painter->font();
       textFont.setPixelSize(30);
       textFont.setBold(true);
       painter->setFont(textFont);
       if (m_mousePressed) {
           painter->setPen(Qt::white);
           painter->setBrush(Qt::black);
           painter->drawRoundedRect(QRect(targetOffset.x(), TOP_PADDING, BUTTON_WIDTH, BUTTON_HEIGHT), 10,10);
           painter->setPen(QColor(BACKGROUND_COLOR));
       } else {
           if (m_disabled)
               painter->setPen(Qt::lightGray);
           else
               painter->setPen(Qt::black);
       } 

       painter->drawText(QRect(targetOffset.x(), TOP_PADDING, BUTTON_WIDTH, BUTTON_HEIGHT), Qt::AlignCenter, m_text);
       painter->restore();
    }

    CopyCutPasteItem::CopyCutPasteItem(ChromeSnippet * snippet, ChromeWidget * chrome, QGraphicsItem * parent)
      : NativeChromeItem(snippet, parent)
      , m_editorSnippet(NULL)
    {
        Q_UNUSED(chrome);
        m_cutButton = new CopyCutPasteButton(QString("cut"));//(qtTrId("txt_browser_content_view_menu_cut"));
        m_copyButton = new CopyCutPasteButton(QString("copy"));//(qtTrId("txt_browser_content_view_menu_copy"));
        m_pasteButton = new CopyCutPasteButton(QString("paste"));//(qtTrId("txt_browser_content_view_menu_paste"));

        m_cutButton->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
        m_copyButton->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
        m_pasteButton->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
        
        connect(m_cutButton, SIGNAL(clicked()), this, SLOT(cut()));
        connect(m_copyButton, SIGNAL(clicked()), this, SLOT(copy()));
        connect(m_pasteButton, SIGNAL(clicked()), this, SLOT(paste()));
    }

    CopyCutPasteItem::~CopyCutPasteItem() 
    {
        delete m_cutButton;
        delete m_copyButton;
        delete m_pasteButton;
    }

    void CopyCutPasteItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
    {
        NativeChromeItem::paint(painter, option, widget);
        painter->save();

        painter->setPen(Qt::black);
        painter->setBrush(QColor(BACKGROUND_COLOR));

        painter->drawRoundedRect(QRect(FIRST_BUTTON_OFFSET, TOP_PADDING, THIRD_BUTTON_OFFSET + BUTTON_WIDTH, BUTTON_HEIGHT), 10,10);

        // draw split
        painter->setOpacity(0.3);
        painter->setPen(Qt::black);
        painter->drawLine(BUTTON_WIDTH, TOP_PADDING + 10, BUTTON_WIDTH, TOP_PADDING + BUTTON_HEIGHT - 10);
        painter->drawLine(SECOND_BUTTON_OFFSET + BUTTON_WIDTH, TOP_PADDING + 10, SECOND_BUTTON_OFFSET + BUTTON_WIDTH, TOP_PADDING + BUTTON_HEIGHT - 10);
        painter->setOpacity(1);
        
        // draw buttons
        m_copyButton->render(painter, QPoint(FIRST_BUTTON_OFFSET,TOP_PADDING));
        m_cutButton->render(painter, QPoint(SECOND_BUTTON_OFFSET,TOP_PADDING));
        m_pasteButton->render(painter, QPoint(THIRD_BUTTON_OFFSET,TOP_PADDING));

        painter->restore();
    }

    void CopyCutPasteItem::reset()
    {
        m_copyButton->setDisabled(true);
        m_cutButton->setDisabled(true);
        m_pasteButton->setDisabled(true);
        m_copyButton->setMousePressed(false);
        m_cutButton->setMousePressed(false);
        m_pasteButton->setMousePressed(false);
        m_editorSnippet = NULL;
    }

    void CopyCutPasteItem::cut() 
    {
        if (m_editorSnippet)
            m_editorSnippet->cut();
        m_snippet->setVisible(false);
        // set the focus back
        m_editorSnippet->grabFocus();
    }

    void CopyCutPasteItem::copy() 
    {
        if (m_editorSnippet)
            m_editorSnippet->copy();
        m_snippet->setVisible(false);
        // set the focus back
        m_editorSnippet->grabFocus();
    }

    void CopyCutPasteItem::paste() 
    {
        if (m_editorSnippet)
            m_editorSnippet->paste();
        m_snippet->setVisible(false);
        // set the focus back
        m_editorSnippet->grabFocus();
    }

    void CopyCutPasteItem::buildMenu(bool isContentSelected)
    {
        reset();
        
        m_copyButton->setDisabled(!isContentSelected);
        m_cutButton->setDisabled(!isContentSelected);

        bool canPaste = false;
        const QClipboard *clipboard = QApplication::clipboard();
        if (clipboard) {
            const QMimeData *mimeData = clipboard->mimeData();
            if (mimeData)
                canPaste = mimeData->hasText();
        }
        m_pasteButton->setDisabled(!canPaste);
    }

    void CopyCutPasteItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
    {
        int x = event->pos().x();
        if (x > 0 && x < SECOND_BUTTON_OFFSET && m_copyButton->isEnabled())
            m_copyButton->mousePressEvent(event);
        else if (x >= SECOND_BUTTON_OFFSET && x < THIRD_BUTTON_OFFSET && m_cutButton->isEnabled())
            m_cutButton->mousePressEvent(event);
        else if (x >= THIRD_BUTTON_OFFSET && m_pasteButton->isEnabled())
            m_pasteButton->mousePressEvent(event);
        update();
    }

    void CopyCutPasteItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
    {
        int x = event->pos().x();
        if (x > 0 && x < SECOND_BUTTON_OFFSET && m_copyButton->isEnabled())
            m_copyButton->mouseReleaseEvent(event);
        else if (x >= SECOND_BUTTON_OFFSET && x < THIRD_BUTTON_OFFSET && m_cutButton->isEnabled())
            m_cutButton->mouseReleaseEvent(event);
        else if (x >= THIRD_BUTTON_OFFSET && m_pasteButton->isEnabled())
            m_pasteButton->mouseReleaseEvent(event);
        // reset all the mouse
        m_copyButton->setMousePressed(false);
        m_cutButton->setMousePressed(false);
        m_pasteButton->setMousePressed(false);
        update();
    }

    CopyCutPasteSnippet::CopyCutPasteSnippet( const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element )
    : ChromeSnippet( elementId, chrome, widget, element )
    , m_timer(new QTimer(this))
    , m_externalEventCharm(NULL)
    {
        connectAll();
    }

    void CopyCutPasteSnippet::setChromeWidget(QGraphicsWidget * widget)
    {
        ChromeSnippet::setChromeWidget(widget);
    }

    CopyCutPasteSnippet * CopyCutPasteSnippet::instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element)
    {
        CopyCutPasteSnippet* that = new CopyCutPasteSnippet(elementId, chrome, 0, element);
        CopyCutPasteItem * item = new CopyCutPasteItem(that, chrome);
        that->setChromeWidget(item);
        return that;
    }

    void CopyCutPasteSnippet::connectAll()
    {
        connect(m_chrome, SIGNAL(chromeComplete()), this, SLOT(onChromeComplete()));
        connect(m_chrome, SIGNAL(aspectChanged(int)), this, SLOT(onAspectChanged(int)));
        connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    }

    void CopyCutPasteSnippet::onChromeComplete()
    {
        ChromeSnippet* urlSnippet = m_chrome->getSnippet(URLSEARCH_CHROME_ID);
        if (urlSnippet)
            connect(urlSnippet, SIGNAL(contextEvent(bool, QString)), this, SLOT(onContextEvent(bool, QString)));

         m_externalEventCharm = new ExternalEventCharm(copyCutPasteItem());

         connect(m_externalEventCharm,
                    SIGNAL(externalMouseEvent(QEvent *, const QString &, const QString &)),
                    this,
                    SLOT(onExternalMouseEvent(QEvent *, const QString &, const QString &)));
    }
    
    void CopyCutPasteSnippet::onAspectChanged(int aspect)
    {
        Q_UNUSED(aspect);
        if (isVisible()) {
            ChromeSnippet* editorSnippet = copyCutPasteItem()->editorSnippet();
            if (!editorSnippet)
                return;
            int x = (m_chrome->viewSize().width() - (BUTTON_WIDTH + THIRD_BUTTON_OFFSET)) / 2;
            int y = editorSnippet->widget()->size().height() + 1;
            anchorTo(URLSEARCH_CHROME_ID, x, y);
            copyCutPasteItem()->update();
        }
    }

    void CopyCutPasteSnippet::onContextEvent(bool isContentSelected, QString snippetId)
    {
        ChromeSnippet* editorSnippet = m_chrome->getSnippet(snippetId);
        if (!editorSnippet)
            return;
        int x = (m_chrome->viewSize().width() - (BUTTON_WIDTH + THIRD_BUTTON_OFFSET)) / 2;
        int y = editorSnippet->widget()->size().height() + 1;
        anchorTo(URLSEARCH_CHROME_ID, x, y);
        copyCutPasteItem()->buildMenu(isContentSelected);
        copyCutPasteItem()->setEditorSnippet(editorSnippet);
        copyCutPasteItem()->update();
        setVisible(true);
        m_timer->start(MENU_DISPLAY_DURATION);
    }

    void CopyCutPasteSnippet::onTimeout()
    {
        setVisible(false);
    }

    void  CopyCutPasteSnippet::onExternalMouseEvent(QEvent * ev, const QString & name, const QString & description)
    {
        Q_UNUSED(ev);
        Q_UNUSED(description);
        if (name == "MouseClick")
            setVisible(false);
    }

    void CopyCutPasteSnippet::setVisible(bool visiblity, bool animate)
    {
        // let snippet know menu shows or hides
        if (copyCutPasteItem()->editorSnippet())
            copyCutPasteItem()->editorSnippet()->setContextMenuStatus(visiblity); 
        ChromeSnippet::setVisible(visiblity, animate);
    }

    CopyCutPasteItem * CopyCutPasteSnippet::copyCutPasteItem()
    {
        return qobject_cast<CopyCutPasteItem*>(m_widget);
    }
}
