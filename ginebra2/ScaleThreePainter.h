#ifndef ScaleThreePainter_H
#define ScaleThreePainter_H

#include <QPixmap>

class QPainter;
class QString;
class QStyleOptionGraphicsItem;
class QWidget;

namespace GVA {

/*!
 * This class paints a resizable window background using separate images for its' left, middle and right areas.
 */
class ScaleThreePainter
{
public:
    ScaleThreePainter(const QString &leftFilename,
                      const QString &middleFilename,
                      const QString &rightFilename
                     );
    ~ScaleThreePainter();

    void paint(QPainter* painter, const QRect &rect, QWidget* widget);
    void unloadPixmaps();

private:
    void loadPixmaps();

private:
    QString m_leftFilename;
    QString m_middleFilename;
    QString m_rightFilename;
    QPixmap *m_leftPixmap;
    QPixmap *m_middlePixmap;
    QPixmap *m_rightPixmap;
    bool m_pixmapsLoaded;
};

}  // GVA namespace

#endif // ScaleThreePainter_H
