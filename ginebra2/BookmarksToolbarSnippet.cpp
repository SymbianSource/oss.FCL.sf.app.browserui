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
 * This class extends ToolbarSnippet class to hold the
 * windows view toolbar buttons
 *
 */

#include "BookmarksToolbarSnippet.h"
#include "ViewStack.h"
#include "GWebContentView.h"
#include "BookmarksManager.h"
#include <QDebug>

namespace GVA {

    BookmarksToolbarSnippet::BookmarksToolbarSnippet(const QString& elementId, ChromeWidget * chrome,
                                                     const QRectF& ownerArea, const QWebElement & element, QGraphicsWidget * widget)
        : DualButtonToolbarSnippet(elementId, chrome, ownerArea, element, widget),
          m_action1(0), m_action2(0)
    {      
    }

    BookmarksToolbarSnippet::~BookmarksToolbarSnippet()
    {
        if(m_action1)
            delete m_action1;
        if(m_action2)
            delete m_action2;
    }

    void BookmarksToolbarSnippet::addChild(ChromeSnippet * child) {
        WebChromeContainerSnippet * s =  dynamic_cast<WebChromeContainerSnippet* >(child);
        if (!s) {
            ToolbarActions_t* t = new ToolbarActions_t();
            if (child->elementId() == "BookmarksBackButton" ) {
                t->actionId = BOOKMARKS_VIEW_ACTION_BACK;
                t->actionName = BOOKMARKS_TOOLBAR_BACK;
                t->activeImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_back.png";
                t->disabledImg = "";
                t->selectedImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_back_pressed.png";
            }
            else if (child->elementId() == "BookmarksAddButton" ) {
                t->actionId = BOOKMARKS_VIEW_ACTION_ADD;
                t->actionName = BOOKMARKS_TOOLBAR_ADD;
                t->activeImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_add.png";
                t->disabledImg = "";
                t->selectedImg = ":/chrome/bedrockchrome/toolbar.snippet/icons/icon_add_pressed.png";
            }
            t->id = child->elementId();
            m_actionInfo.append(t);
        }

        WebChromeContainerSnippet::addChild(child);
    }

    void BookmarksToolbarSnippet::setAction(ChromeSnippet * s) {
        ToolbarSnippet::setAction(s);

        ActionButtonSnippet * button  = static_cast<ActionButtonSnippet*> (s);
        int index = getIndex(s);

        if (index != -1 ) {
            ToolbarActions_t * t = m_actionInfo.at(index);
            QAction * action =  button->getDefaultAction();
            if (t->actionId == BOOKMARKS_VIEW_ACTION_BACK ) {
                if( !action ) {
                    action = new QAction(0);
                    button->setDefaultAction(action);
                    m_action1 = action;
                }
                connect(action, SIGNAL(triggered()), this, SLOT(handleBackButton()));
            }
            else if (t->actionId == BOOKMARKS_VIEW_ACTION_ADD) {
                if( !action ) {
                    action = new QAction(0);
                    button->setDefaultAction(action);
                    m_action2 = action;
                }
                connect(action, SIGNAL(triggered()), this, SIGNAL(addBookmarkSelected()));
            }
        }
    }

    void BookmarksToolbarSnippet::handleBackButton() {
        GWebContentView* webView = static_cast<GWebContentView*> (chrome()->getView("WebView"));
        if(webView) {
            webView->showNormalPage();
            webView->setGesturesEnabled(true);
        }

        ViewStack::getSingleton()->switchView( TOOLBAR_WEB_VIEW, TOOLBAR_BOOKMARKS_VIEW );
    }

} // end of namespace GVA


