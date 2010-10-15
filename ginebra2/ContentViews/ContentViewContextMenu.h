#ifndef CONTENTVIEWCONTEXTMENU_H
#define CONTENTVIEWCONTEXTMENU_H

#include <QMenu>
#include <QTimer>

#include "ContextMenu.h"

namespace GVA {

/*!
 * Context menu implementation for web content views.
 */
class ContentViewContextMenu : public ContextMenu {
    Q_OBJECT
public:
    ContentViewContextMenu(QWebHitTestResult *hitTest, QWidget *parent = 0);

    virtual void addAction(QAction *action) {
        m_menu.addAction(action);
    }

    virtual void addAction(const QString text, const QObject * receiver, const char * member, const QKeySequence & shortcut = 0) {
        m_menu.addAction(text, receiver, member, shortcut);
    }

    virtual void exec(const QPoint & p) {
        if(!m_menu.isEmpty()){
            QPoint newp(p);

            if(p.x() < 570)  // left part of the screen; depends on screen size. should be replaced by something more reliable
                newp.rx()+=20; // start menu 20 px right of point p
            else  // right part of the screen
                newp.rx()-=(20 +  m_menu.sizeHint().width()); //start menu left of point p
            m_menu.exec(newp);
        }
    }

private slots:
    void onOpenLink();
    void onShareLink();
    void onShareImage();
    void onSaveImage();
    void onViewImage();

protected:
    QMenu m_menu;
    QWebHitTestResult *m_hitTest;
};

}  // GVA namespace

#endif // CONTENTVIEWCONTEXTMENU_H
