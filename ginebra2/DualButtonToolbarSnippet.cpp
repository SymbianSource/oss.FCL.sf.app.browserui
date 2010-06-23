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

#include "DualButtonToolbarSnippet.h"
#include "ViewStack.h"
#include <QDebug>

namespace GVA {

    DualButtonToolbarSnippet::DualButtonToolbarSnippet(const QString& elementId, ChromeWidget * chrome,
                                                       const QRectF& ownerArea, const QWebElement & element, QGraphicsWidget * widget)
        : ToolbarSnippet(elementId, chrome, ownerArea, element, widget)
    {      
        m_type = "";
    }

    DualButtonToolbarSnippet::~DualButtonToolbarSnippet()
    {
    }

    /*! 
      Update the item spacing based on the new size
    */
    void DualButtonToolbarSnippet::updateSize(QSize size) {
        setChildSpacing(size.width());
        ToolbarSnippet::updateSize(size);
    }

    /*! 
      Update the item spacing based on the chrome width
    */
    void DualButtonToolbarSnippet::updateOwnerArea() {
        setChildSpacing(m_chrome->width());
        ToolbarSnippet::updateOwnerArea();
    }

    void DualButtonToolbarSnippet::setChildSpacing(int width) {
        QGraphicsLinearLayout* l = layout();

        // If all the buttons have been added, set the spacing for the last button
        if (l->count() == DUAL_TOOLBAR_NUM_BUTTONS ) {
            int spacing = width - l->itemAt(0)->geometry().width() -  l->itemAt(l->count()-1)->geometry().width();
            l->setItemSpacing(l->count()-1, spacing);
        }
        ToolbarSnippet::updateOwnerArea();
    }

    void DualButtonToolbarSnippet::onMouseEvent(QEvent::Type type) {

        //qDebug() << "DualButtonToolbarSnippet::onMouseEvent - Type:" << type;
        ActionButton* button  = static_cast<ActionButton*> ( sender());
        ActionButtonSnippet * buttonSnippet =  static_cast<ActionButtonSnippet*>( button->snippet());
 
        if (type == QEvent::GraphicsSceneMousePress ) {
            buttonSnippet->setActive(true);
        }
        else if (type == QEvent::GraphicsSceneMouseRelease ) {
            buttonSnippet->updateButtonState(false);
        }
    }
} // end of namespace GVA



