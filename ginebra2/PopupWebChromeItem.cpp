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


#include "PopupWebChromeItem.h"
#include "ChromeWidget.h"
#include "WebChromeSnippet.h"

namespace GVA {

PopupWebChromeItem::PopupWebChromeItem(
        const QRectF & ownerArea,
        ChromeWidget * chrome,
        const QWebElement & element,
        QGraphicsItem * parent)
: WebChromeItem(ownerArea, chrome, element, parent)
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

    switch(e->type()) {
      case QEvent::Show:
        scene()->installEventFilter(this);
        break;
      case QEvent::Hide:
        scene()->removeEventFilter(this);
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
    case QEvent::GraphicsSceneResize:
        break;
    default:
        return;
    }

    // Check where the mouse press event occurred.
    // If it was outside this item's bounding rectangle,
    // then tell the world.

    if(e->type() == QEvent::GraphicsSceneResize)
    {
    	emitExternalEvent(e);
    	return;
    }
    
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
