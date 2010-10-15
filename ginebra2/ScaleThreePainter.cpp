#include "ScaleThreePainter.h"
#include <QtGui>

namespace GVA {

ScaleThreePainter::ScaleThreePainter(
      const QString &leftFilename,
      const QString &middleFilename,
      const QString &rightFilename)
        : m_leftFilename(leftFilename),
          m_middleFilename(middleFilename),
          m_rightFilename(rightFilename),
          m_leftPixmap(0),
          m_middlePixmap(0),
          m_rightPixmap(0),
          m_pixmapsLoaded(false)
{
}

ScaleThreePainter::~ScaleThreePainter() {
    unloadPixmaps();
}

void ScaleThreePainter::loadPixmaps() {
    Q_ASSERT(!m_pixmapsLoaded);
    m_leftPixmap = new QPixmap(m_leftFilename);
    m_middlePixmap = new QPixmap(m_middleFilename);
    m_rightPixmap = new QPixmap(m_rightFilename);
    m_pixmapsLoaded = true;
}

void ScaleThreePainter::unloadPixmaps() {
    delete m_leftPixmap; m_leftPixmap = 0;
    delete m_middlePixmap; m_middlePixmap = 0;
    delete m_rightPixmap; m_rightPixmap = 0;
    m_pixmapsLoaded = false;
}

void ScaleThreePainter::paint(QPainter* painter, const QRect &rect, QWidget* widget) {
    Q_UNUSED(widget)

    if(!m_pixmapsLoaded)
        loadPixmaps();

    //    qDebug() << "ScaleThreePainter::paint: " << rect << qMin(m_middlePixmap->height(), rect.height());

    painter->save();

    // Draw left.
    if(m_leftPixmap->height() > rect.height())
        painter->drawPixmap(0, 0, m_leftPixmap->width(), rect.height(), *m_leftPixmap);
    else
        painter->drawPixmap(0, 0, *m_leftPixmap);

    // Draw top middle -- fills in the space is between the left and right pixmaps.
    if(!m_middlePixmap->isNull()) {
        painter->drawTiledPixmap(m_leftPixmap->width(),
                                 0,
                                 rect.width() - (m_leftPixmap->width() + m_rightPixmap->width()),
                                 rect.height(),
                                 m_middlePixmap->scaledToHeight(rect.height()));
    }

    // Draw right.
    if(m_rightPixmap->height() > rect.height())
        painter->drawPixmap(rect.right() - m_rightPixmap->width() + 1, 0, m_rightPixmap->width(), rect.height(), *m_rightPixmap);
    else
        painter->drawPixmap(rect.right() - m_rightPixmap->width() + 1, 0, *m_rightPixmap);

    painter->restore();
}


}  // GVA namespace
