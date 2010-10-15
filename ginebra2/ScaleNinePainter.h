#ifndef SCALENINEPAINTER_H
#define SCALENINEPAINTER_H

#include <QPixmap>

class QPainter;
class QString;
class QStyleOptionGraphicsItem;
class QWidget;

namespace GVA {

/*!
 * This class paints a resizable window background using separate images for its' four corners,
 * four edges and middle area.
 */
class ScaleNinePainter
{
public:
    ScaleNinePainter(const QString &topLeftFilename,
                     const QString &topMiddleFilename,
                     const QString &topRightFilename,
                     const QString &middleLeftFilename = QString::null,
                     const QString &middleMiddleFilename = QString::null,
                     const QString &middleRightFilename = QString::null,
                     const QString &bottomLeftFilename = QString::null,
                     const QString &bottomMiddleFilename = QString::null,
                     const QString &bottomRightFilename = QString::null
                     );
    ~ScaleNinePainter();
    void paint(QPainter* painter, const QRect &rect, QWidget* widget);
    void unloadPixmaps();

private:
    void loadPixmaps();

private:
    QString m_topLeftFilename;
    QString m_topMiddleFilename;
    QString m_topRightFilename;
    QString m_middleLeftFilename;
    QString m_middleMiddleFilename;
    QString m_middleRightFilename;
    QString m_bottomLeftFilename;
    QString m_bottomMiddleFilename;
    QString m_bottomRightFilename;
    QPixmap *m_topLeftPixmap;
    QPixmap *m_topMiddlePixmap;
    QPixmap *m_topRightPixmap;
    QPixmap *m_middleLeftPixmap;
    QPixmap *m_middleMiddlePixmap;
    QPixmap *m_middleRightPixmap;
    QPixmap *m_bottomLeftPixmap;
    QPixmap *m_bottomMiddlePixmap;
    QPixmap *m_bottomRightPixmap;
    bool m_pixmapsLoaded;
};

}  // GVA namespace

#endif // SCALENINEPAINTER_H
