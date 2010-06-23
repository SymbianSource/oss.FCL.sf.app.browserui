
#include <QtGui>
#include <QObject>
#include <QDebug>
#include <QApplication>
#include <QTimer>

#include "ObjectCharm.h"

namespace GVA {

// ----------------------------
// ObjectCharm
// ----------------------------

ObjectCharm::ObjectCharm(QObject *object)
  : m_object(object)
{
    m_object->installEventFilter(this);
    connect(m_object, SIGNAL(destroyed()), this, SLOT(onObjectDestroyed()));
}

void ObjectCharm::onObjectDestroyed() {
    deleteLater();
}

// ----------------------------
// ExternalEventCharm
// ----------------------------

inline QGraphicsScene *ExternalEventCharm::scene() {
    QGraphicsObject *item = static_cast<QGraphicsObject*>(m_object);
    return item->scene();
}

ExternalEventCharm::ExternalEventCharm(QGraphicsObject *object)
  : ObjectCharm(object)
{
}

bool ExternalEventCharm::eventFilter(QObject *object, QEvent *event) {
    //qDebug() << "ExternalEventCharm::eventFilter: " << object << event;
    // Check for external events grabbed by this item.

    checkForExternalEvent(this, event);

    switch (event->type()) {
      case QEvent::Show:
      {
          scene()->installEventFilter(this);
          break;
      }
      case QEvent::Hide:
      {
          scene()->removeEventFilter(this);
          break;
      }
      default: break;
    }

    // standard event processing
    return object->eventFilter(object, event);
}

void ExternalEventCharm::checkForExternalEvent(QObject * o, QEvent * e)
{
    Q_UNUSED(o);

    // Ignore all but a few mouse press events.

    switch (e->type()) {
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseRelease:
    case QEvent::GraphicsSceneMouseDoubleClick:
        break;
    default:
        return;
    }

    QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent*>(e);

    QPointF eventPosition = me->scenePos();

    QGraphicsObject *item = static_cast<QGraphicsObject*>(m_object);

    QRectF itemGeometry = item->sceneBoundingRect();

    if (!itemGeometry.contains(eventPosition)) {
        emitExternalEvent(e);
    }
}

void ExternalEventCharm::emitExternalEvent(QEvent * e)
{
    QString description;

    QDebug stream(&description);
    stream << e;

    QString name = description;
    name.truncate(name.indexOf('('));

    emit externalMouseEvent(e->type(), name, description.trimmed());
}

// ----------------------------
// TouchCircleCharm
// ----------------------------

TouchCircleCharm::TouchCircleCharm(QObject *object, QGraphicsItem *parent)
    : ObjectCharm(object),
      m_timer(new QTimer())
{
    m_item = new QGraphicsEllipseItem(QRect(0,0,20,20), parent);
    m_item->hide();
    m_item->setPen(QPen(QColor(Qt::black)));
    m_item->setBrush(QBrush(QColor(200, 0, 0)));
    m_item->setZValue(100);  // make sure it isn't obscured

    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}

TouchCircleCharm::~TouchCircleCharm() {
    delete m_item;
    delete m_timer;
}

bool TouchCircleCharm::eventFilter(QObject *object, QEvent *event) {
    switch (event->type()) {
        case QEvent::GraphicsSceneMousePress: {
            // Show the item.
            QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent*>(event);
            m_item->setPos(me->scenePos().x() - m_item->rect().width()/2,
                           me->scenePos().y() - m_item->rect().height()/2);

            // Change the color to indicate click.
            m_item->setBrush(QBrush(QColor(0, 0, 200)));

            m_item->show();
            m_timer->stop();
            break;
        }
        case QEvent::GraphicsSceneMouseMove: {
            if(m_item->isVisible()) {
                // Move it to new position.
                QGraphicsSceneMouseEvent * me = static_cast<QGraphicsSceneMouseEvent*>(event);
                m_item->setPos(me->scenePos().x() - m_item->rect().width()/2,
                               me->scenePos().y() - m_item->rect().height()/2);

                // Change the color to indicate drag.
                m_item->setBrush(QBrush(QColor(200, 0, 0)));
            }
            break;
        }
        case QEvent::GraphicsSceneMouseDoubleClick: {
            if(m_item->isVisible()) {
                // Change the color to indicate double click.
                m_item->setBrush(QBrush(QColor(0, 200, 0)));

                m_timer->stop();
            }
            break;
        }
        case QEvent::GraphicsSceneMouseRelease: {
            if(m_item->isVisible()) {
                // Start timer to hide it.
                m_timer->start(2000);
            }
            break;
        }
    }
    return object->eventFilter(object, event);
}

void TouchCircleCharm::onTimer() {   // slot
    m_item->hide();
}

}   // namespace GVA
