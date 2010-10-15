#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QGraphicsWidget>

class QWebHitTestResult;

namespace GVA {

/*!
 * Abstract base class for context menus.
 */
class ContextMenu : public QObject
{
    Q_OBJECT
public:
    ContextMenu(QObject *parent = 0) : QObject(parent) {}
    virtual void addAction(QAction *action) = 0;
    virtual void addAction(const QString text, const QObject * receiver, const char * member, const QKeySequence & shortcut = 0) = 0;
    virtual void exec(const QPoint & p) = 0;
};

}  // GVA namespace

#endif // CONTEXTMENU_H
