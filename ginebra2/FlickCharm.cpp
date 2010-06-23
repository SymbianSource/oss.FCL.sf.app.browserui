/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Graphics Dojo project on Qt Labs.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 or 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "FlickCharm.h"

#include <QAbstractScrollArea>
#include <QApplication>
#include <QBasicTimer>
#include <QEvent>
#include <QHash>
#include <QList>
#include <QMouseEvent>
#include <QScrollBar>
#include <QWebFrame>
#include <QWebView>
#include <QGraphicsWebView>
#include <QGraphicsSceneMouseEvent>

#include <QDebug>

struct FlickData {
    typedef enum { Steady, Pressed, ManualScroll, AutoScroll, Stop } State;
    State state;
    QObject *widget;
    QPoint pressPos;
    QPoint offset;
    QPoint dragPos;
    QPoint speed;
    QList<QEvent*> ignored;
};

class FlickCharmPrivate
{
public:
    QHash<QObject*, FlickData*> flickData;
    QBasicTimer ticker;
};

FlickCharm::FlickCharm(QObject *parent): QObject(parent)
{
    d = new FlickCharmPrivate;
}

FlickCharm::~FlickCharm()
{
    delete d;
}

void FlickCharm::activateOn(QWidget *widget)
{
    QAbstractScrollArea *scrollArea = dynamic_cast<QAbstractScrollArea*>(widget);
    if (scrollArea) {
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        QWidget *viewport = scrollArea->viewport();

        viewport->installEventFilter(this);
        scrollArea->installEventFilter(this);

        d->flickData.remove(viewport);
        d->flickData[viewport] = new FlickData;
        d->flickData[viewport]->widget = widget;
        d->flickData[viewport]->state = FlickData::Steady;

        return;
    }
}

void FlickCharm::activateOn(QWebView *webView)
{
    if (webView) {
        QWebFrame *frame = webView->page()->mainFrame();
        frame->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
        frame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

        webView->installEventFilter(this);

        d->flickData.remove(webView);
        d->flickData[webView] = new FlickData;
        d->flickData[webView]->widget = webView;
        d->flickData[webView]->state = FlickData::Steady;

        return;
    }
}

void FlickCharm::activateOn(QGraphicsWebView *webView)
{
    qDebug() << "FlickCharm::activateOn";
    if (webView) {
        QWebFrame *frame = webView->page()->mainFrame();
        frame->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
        frame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

        webView->installEventFilter(this);

        d->flickData.remove(webView);
        d->flickData[webView] = new FlickData;
        d->flickData[webView]->widget = webView;
        d->flickData[webView]->state = FlickData::Steady;

        return;
    }
}

void FlickCharm::deactivateFrom(QWidget *widget)
{
    QAbstractScrollArea *scrollArea = dynamic_cast<QAbstractScrollArea*>(widget);
    if (scrollArea) {
        QWidget *viewport = scrollArea->viewport();

        viewport->removeEventFilter(this);
        scrollArea->removeEventFilter(this);

        delete d->flickData[viewport];
        d->flickData.remove(viewport);

        return;
    }

    QWebView *webView = dynamic_cast<QWebView*>(widget);
    if (webView) {
        webView->removeEventFilter(this);

        delete d->flickData[webView];
        d->flickData.remove(webView);

        return;
    }
}

static QPoint scrollOffset(QObject *widget)
{
    int x = 0, y = 0;

    QGraphicsWebView *gWebView = dynamic_cast<QGraphicsWebView*>(widget);
    if (gWebView) {
        QWebFrame *frame = gWebView->page()->mainFrame();
        x = frame->evaluateJavaScript("window.scrollX").toInt();
        y = frame->evaluateJavaScript("window.scrollY").toInt();
    }

    QAbstractScrollArea *scrollArea = dynamic_cast<QAbstractScrollArea*>(widget);
    if (scrollArea) {
        x = scrollArea->horizontalScrollBar()->value();
        y = scrollArea->verticalScrollBar()->value();
    }

    QWebView *webView = dynamic_cast<QWebView*>(widget);
    if (webView) {
        QWebFrame *frame = webView->page()->mainFrame();
        x = frame->evaluateJavaScript("window.scrollX").toInt();
        y = frame->evaluateJavaScript("window.scrollY").toInt();
    }

    return QPoint(x, y);
}

static void setScrollOffset(QObject *widget, const QPoint &p)
{
    QGraphicsWebView *gWebView = dynamic_cast<QGraphicsWebView*>(widget);
    if (gWebView) {
        QWebFrame *frame = gWebView->page()->mainFrame();
        if (frame)
            frame->evaluateJavaScript(QString("window.scrollTo(%1,%2);").arg(p.x()).arg(p.y()));
        return;
    }

    QAbstractScrollArea *scrollArea = dynamic_cast<QAbstractScrollArea*>(widget);
    if (scrollArea) {
        scrollArea->horizontalScrollBar()->setValue(p.x());
        scrollArea->verticalScrollBar()->setValue(p.y());
        return;
    }

    QWebView *webView = dynamic_cast<QWebView*>(widget);
    QWebFrame *frame = webView ? webView->page()->mainFrame() : 0;
    if (frame)
        frame->evaluateJavaScript(QString("window.scrollTo(%1,%2);").arg(p.x()).arg(p.y()));
}

static QPoint deaccelerate(const QPoint &speed, int a = 1, int max = 64)
{
    int x = qBound(-max, speed.x(), max);
    int y = qBound(-max, speed.y(), max);
    x = (x == 0) ? x : (x > 0) ? qMax(0, x - a) : qMin(0, x + a);
    y = (y == 0) ? y : (y > 0) ? qMax(0, y - a) : qMin(0, y + a);
    return QPoint(x, y);
}

bool FlickCharm::eventFilter(QObject *object, QEvent *event)
{
//    qDebug() << "FlickCharm::eventFilter: " << object << event;
//    if (!object->isWidgetType())
//        return false;

    QEvent::Type type = event->type();
    switch (type) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::GraphicsSceneMousePress:
        case QEvent::GraphicsSceneMouseRelease:
        case QEvent::GraphicsSceneMouseMove:
            break;
        default:
            return false;
    }

    QPoint eventPos;
    QEvent::Type eventType;
    Qt::MouseButtons eventButtons;
    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
    if (mouseEvent) {
        if (mouseEvent->modifiers() != Qt::NoModifier)
            return false;
        eventPos = mouseEvent->pos();
        eventType = mouseEvent->type();
        eventButtons = mouseEvent->buttons();
    }
    else {
        QGraphicsSceneMouseEvent *mouseEvent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
        if (mouseEvent && mouseEvent->modifiers() == Qt::NoModifier) {
            eventPos = mouseEvent->pos().toPoint();
            eventType = mouseEvent->type();
            eventButtons = mouseEvent->buttons();
        }
        else
            return false;
    }

//    QWidget *viewport = dynamic_cast<QWidget*>(object);
    FlickData *data = d->flickData.value(object);
    if (!data || data->ignored.removeAll(event)) {
        return false;
    }

    bool consumed = false;
    switch (data->state) {

    case FlickData::Steady:
        if (eventType == QEvent::MouseButtonPress ||
            eventType == QEvent::GraphicsSceneMousePress)
            if (eventButtons == Qt::LeftButton) {
                consumed = true;
                data->state = FlickData::Pressed;
                data->pressPos = eventPos;
                data->offset = scrollOffset(data->widget);
            }
        break;

    case FlickData::Pressed:
        if (eventType == QEvent::MouseButtonRelease) {
            consumed = true;
            data->state = FlickData::Steady;

            QMouseEvent *event1 = new QMouseEvent(QEvent::MouseButtonPress,
                                                  data->pressPos, Qt::LeftButton,
                                                  Qt::LeftButton, Qt::NoModifier);
            QMouseEvent *event2 = new QMouseEvent(*mouseEvent);

            data->ignored << event1;
            data->ignored << event2;
            QApplication::postEvent(object, event1);
            QApplication::postEvent(object, event2);
        }
        else if (eventType == QEvent::GraphicsSceneMouseRelease) {
            consumed = true;
            data->state = FlickData::Steady;

            QGraphicsSceneMouseEvent *origMouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
            QGraphicsSceneMouseEvent *event1 =
                    new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMousePress);
            event1->setPos(origMouseEvent->pos());
            event1->setScenePos(origMouseEvent->scenePos());
            event1->setLastPos(origMouseEvent->lastPos());
            event1->setLastScenePos(origMouseEvent->lastScenePos());
            event1->setButtons(origMouseEvent->buttons());
            event1->setButton(origMouseEvent->button());
            event1->setModifiers(origMouseEvent->modifiers());

            QGraphicsSceneMouseEvent *event2 = new QGraphicsSceneMouseEvent(origMouseEvent->type());
            event2->setPos(origMouseEvent->pos());
            event2->setScenePos(origMouseEvent->scenePos());
            event2->setLastPos(origMouseEvent->lastPos());
            event2->setLastScenePos(origMouseEvent->lastScenePos());
            event2->setButtons(origMouseEvent->buttons());
            event2->setButton(origMouseEvent->button());
            event2->setModifiers(origMouseEvent->modifiers());

            data->ignored << event1;
            data->ignored << event2;
            QApplication::postEvent(object, event1);
            QApplication::postEvent(object, event2);
        }
        if (eventType == QEvent::MouseMove ||
            eventType == QEvent::GraphicsSceneMouseMove) {
            consumed = true;
            data->state = FlickData::ManualScroll;
            data->dragPos = QCursor::pos();
            if (!d->ticker.isActive())
                d->ticker.start(20, this);
        }
        break;

    case FlickData::ManualScroll:
        if (eventType == QEvent::MouseMove ||
            eventType == QEvent::GraphicsSceneMouseMove) {
            consumed = true;
            QPoint delta = eventPos - data->pressPos;
            setScrollOffset(data->widget, data->offset - delta);
        }
        if (eventType == QEvent::MouseButtonRelease ||
            eventType == QEvent::GraphicsSceneMouseRelease) {
            consumed = true;
            data->state = FlickData::AutoScroll;
        }
        break;

    case FlickData::AutoScroll:
        if (eventType == QEvent::MouseButtonPress ||
            eventType == QEvent::GraphicsSceneMousePress) {
            consumed = true;
            data->state = FlickData::Stop;
            data->speed = QPoint(0, 0);
            data->pressPos = eventPos;
            data->offset = scrollOffset(data->widget);
        }
        if (eventType == QEvent::MouseButtonRelease ||
            eventType == QEvent::GraphicsSceneMouseRelease) {
            consumed = true;
            data->state = FlickData::Steady;
            data->speed = QPoint(0, 0);
        }
        break;

    case FlickData::Stop:
        if (eventType == QEvent::MouseButtonRelease ||
            eventType == QEvent::GraphicsSceneMouseRelease) {
            consumed = true;
            data->state = FlickData::Steady;
        }
        if (eventType == QEvent::MouseMove ||
            eventType == QEvent::GraphicsSceneMouseMove) {
            consumed = true;
            data->state = FlickData::ManualScroll;
            data->dragPos = QCursor::pos();
            if (!d->ticker.isActive())
                d->ticker.start(20, this);
        }
        break;

    default:
        break;
    }

    return consumed;
}

void FlickCharm::timerEvent(QTimerEvent *event)
{
    int count = 0;
    QHashIterator<QObject*, FlickData*> item(d->flickData);
    while (item.hasNext()) {
        item.next();
        FlickData *data = item.value();

        if (data->state == FlickData::ManualScroll) {
            count++;
            data->speed = QCursor::pos() - data->dragPos;
            data->dragPos = QCursor::pos();
        }

        if (data->state == FlickData::AutoScroll) {
            count++;
            data->speed = deaccelerate(data->speed);
            QPoint p = scrollOffset(data->widget);
            setScrollOffset(data->widget, p - data->speed);
            if (data->speed == QPoint(0, 0))
                data->state = FlickData::Steady;
        }
    }

    if (!count)
        d->ticker.stop();

    QObject::timerEvent(event);
}
