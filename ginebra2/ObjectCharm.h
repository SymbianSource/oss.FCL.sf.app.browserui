

#ifndef OBJECTCHARM_H_
#define OBJECTCHARM_H_

#include <QObject>

class QGraphicsObject;
class QGraphicsScene;
class QGraphicsItem;

namespace GVA {

// -------------------------------

/*!
 * Base class for object "charms".  Charms set an event filter on a particular
 * object and perform some action in response to the events sent to the object.
 * ObjectCharms automatically destroy themselves when the target object is
 * destroyed.
 */
class ObjectCharm : public QObject {
    Q_OBJECT
  public:
    ObjectCharm(QObject *object);

  private slots:
    virtual void onObjectDestroyed();

  protected:
    QObject *m_object;
};

// -------------------------------

/*!
 * This class emits a signal when the user clicks outside of the given graphics object.
 */
class ExternalEventCharm : public ObjectCharm {
    Q_OBJECT
  public:
    ExternalEventCharm(QGraphicsObject *object);

  signals:
    /*! This signal is emitted when the user clicks outside of the object.
     * \param type The Qt event type.
     * \param name The Qt event name.
     * \param description The Qt event description.
     */
    void externalMouseEvent(
            int type,
            const QString & name,
            const QString & description);
  private:
    void checkForExternalEvent(QObject * o, QEvent * e);
    void emitExternalEvent(QEvent * e);
    bool eventFilter(QObject *object, QEvent *event);
    QGraphicsScene *scene();
};

// -------------------------------

/*!
 * This class draws a circle in response to mouse click events on the given object.  Intended
 * for testing purposes only.
 */
class TouchCircleCharm : public ObjectCharm {
    Q_OBJECT
  public:
    TouchCircleCharm(QObject *object, QGraphicsItem *parent = 0);
    ~TouchCircleCharm();

  private slots:
    void onTimer();

  private:
    bool eventFilter(QObject *object, QEvent *event);

    class QGraphicsEllipseItem * m_item;
    class QTimer * m_timer;
};

}

#endif /* OBJECTCHARM_H_ */
