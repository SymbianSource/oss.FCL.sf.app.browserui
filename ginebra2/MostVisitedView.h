#ifndef MOSTVISITEDVIEW_H
#define MOSTVISITEDVIEW_H

#include <QObject>
#include <QGraphicsWidget>

#include "GridView.h"
#include "Utilities.h"

class MostVisitedPage;
class QGraphicsLinearLayout;
class QGraphicsSimpleTextItem;
class QEvent;

class QStm_Gesture;

namespace GVA {

class ScaleThreePainter;

class MostVisitedViewItem : public QGraphicsWidget {
    Q_OBJECT
public:
    MostVisitedViewItem(::MostVisitedPage *mostVisitedPage, QGraphicsItem *parent);

    void activate();
    bool event(QEvent* event);
    bool eventFilter(QObject* o, QEvent* e);
    
protected:
    bool handleQStmGesture(QStm_Gesture* gesture);

signals:
    void activated();

private:
    virtual QSizeF sizeHint ( Qt::SizeHint which, const QSizeF & constraint = QSizeF() ) const  {
        Q_UNUSED(which)
        Q_UNUSED(constraint)
        #ifdef Q_WS_MAEMO_5
        return QSizeF(238, 126);
        #else
        return QSizeF(150, 105);
        #endif
    }
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);   
private:
    MostVisitedPage *m_mostVisitedPage;
};

// --------------------------------------------

class MostVisitedView : public QGraphicsWidget {
    Q_OBJECT
public:
    MostVisitedView(const QString &title = QString::null, QGraphicsItem *parent = 0);
    virtual ~MostVisitedView();
    void update(QString mode);
signals:
    void closeComplete();
    void activated();

private slots:
    void onItemActivated();

private:
    virtual void resizeEvent(QGraphicsSceneResizeEvent * event);
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    virtual void closeEvent(QCloseEvent * event);

    void updateBackgroundPixmap(const QSize &size, QWidget* widget);

private:
    QGraphicsLinearLayout *m_layout;
    QGraphicsSimpleTextItem *m_title;
    GridView *m_gridView;
    ScaleThreePainter *m_backgroundPainter;
    QPixmap *m_backgroundPixmap;
    bool m_backgroundDirty;
    QString m_displayMode;
    QGraphicsWidget *m_titleWrapper;
};

}  // namespace

#endif // MOSTVISITEDVIEW_H
