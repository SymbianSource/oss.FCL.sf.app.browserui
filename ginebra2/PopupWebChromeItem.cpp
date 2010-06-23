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

#include "PopupWebChromeItem.h"
#include "ChromeWidget.h"
#include "WebChromeSnippet.h"

namespace GVA {

PopupWebChromeItem::PopupWebChromeItem(
        const QRectF & ownerArea,
        ChromeWidget * chrome,
        const QWebElement & element,
        QGraphicsItem * parent,
        bool modal)
: WebChromeItem(ownerArea, chrome, element, parent),
  m_modal(modal)
{
}

PopupWebChromeItem::~PopupWebChromeItem()
{}

void PopupWebChromeItem::init(WebChromeSnippet * snippet)
{
    WebChromeItem::init(snippet);

    // Forward externalMouseEvent signals from context items.
    QObject::connect(
            this,
            SIGNAL(externalMouseEvent(int, const QString &, const QString &)),
            snippet,
            SIGNAL(externalMouseEvent(int, const QString &, const QString &)));
}

bool PopupWebChromeItem::event(QEvent * e)
{
    // Check for external events grabbed by this item.

    checkForExternalEvent(this, e);

    switch (e->type()) {
      case QEvent::Show:
        scene()->installEventFilter(this);
        if(snippet() && m_modal) {
            chrome()->emitPopupShown(snippet()->objectName());
        }
        break;
      case QEvent::Hide:
        scene()->removeEventFilter(this);
        if(snippet() && m_modal) {
            chrome()->emitPopupHidden(snippet()->objectName());
        }
        break;
      default: break;
    }

    // Let the parent class handle the event.

    return WebChromeItem::event(e);
}

bool PopupWebChromeItem::eventFilter(QObject * o, QEvent * e)
{
    // Check for external events NOT grabbed by this item.

    checkForExternalEvent(o, e);

    // Don't filter any events.

    return false;
}

void PopupWebChromeItem::checkForExternalEvent(QObject * o, QEvent * e)
{
    Q_UNUSED(o);

    // Ignore all events when this item is not showing.

    if (!isVisible()) {
        return;
    }

    // Ignore all but a few mouse press events.

    switch (e->type()) {
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseRelease:
    case QEvent::GraphicsSceneMouseDoubleClick:
// Commented out because new context menu resizes itself, don't want externalMouseEvents
// in that case.
//    case QEvent::GraphicsSceneResize:
        break;
    default:
        return;
    }

    // Check where the mouse press event occurred.
    // If it was outside this item's bounding rectangle,
    // then tell the world.

// Commented out because new context menu resizes itself, don't want externalMouseEvents
// in that case.
//    if (e->type() == QEvent::GraphicsSceneResize)
//    {
//        QGraphicsSceneResizeEvent *resizeEvent = static_cast<QGraphicsSceneResizeEvent *>(e);
//        qDebug() << "PopupWebChromeItem::checkForExternalEvent: " << resizeEvent->newSize() << resizeEvent->oldSize();
//        if (resizeEvent->newSize() != resizeEvent->oldSize())
//          emitExternalEvent(e);
//      return;
//    }

    QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent*>(e);

    QPointF eventPosition = me->scenePos();

    QRectF itemGeometry = sceneBoundingRect();

    if (!itemGeometry.contains(eventPosition)) {
        emitExternalEvent(e);
    }
}

void PopupWebChromeItem::emitExternalEvent(QEvent * e)
{
    QString description;

    QDebug stream(&description);
    stream << e;

    QString name = description;
    name.truncate(name.indexOf('('));

    emit externalMouseEvent(e->type(), name, description.trimmed());
}

} // end of namespace GVA
