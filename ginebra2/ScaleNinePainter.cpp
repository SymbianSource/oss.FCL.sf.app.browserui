#include "ScaleNinePainter.h"
#include <QtGui>

namespace GVA {

ScaleNinePainter::ScaleNinePainter(
      const QString &topLeftFilename,
      const QString &topMiddleFilename,
      const QString &topRightFilename,
      const QString &middleLeftFilename,
      const QString &middleMiddleFilename,
      const QString &middleRightFilename,
      const QString &bottomLeftFilename,
      const QString &bottomMiddleFilename,
      const QString &bottomRightFilename)
        : m_topLeftFilename(topLeftFilename),
          m_topMiddleFilename(topMiddleFilename),
          m_topRightFilename(topRightFilename),
          m_middleLeftFilename(middleLeftFilename),
          m_middleMiddleFilename(middleMiddleFilename),
          m_middleRightFilename(middleRightFilename),
          m_bottomLeftFilename(bottomLeftFilename),
          m_bottomMiddleFilename(bottomMiddleFilename),
          m_bottomRightFilename(bottomRightFilename),
          m_topLeftPixmap(0),
          m_topMiddlePixmap(0),
          m_topRightPixmap(0),
          m_middleLeftPixmap(0),
          m_middleMiddlePixmap(0),
          m_middleRightPixmap(0),
          m_bottomLeftPixmap(0),
          m_bottomMiddlePixmap(0),
          m_bottomRightPixmap(0),
          m_pixmapsLoaded(false)
{
}

ScaleNinePainter::~ScaleNinePainter() {
    unloadPixmaps();
}

void ScaleNinePainter::loadPixmaps() {
    Q_ASSERT(!m_pixmapsLoaded);
    m_topLeftPixmap = new QPixmap(m_topLeftFilename);
    m_topMiddlePixmap = new QPixmap(m_topMiddleFilename);
    m_topRightPixmap = new QPixmap(m_topRightFilename);
    m_middleLeftPixmap = new QPixmap(m_middleLeftFilename);
    m_middleMiddlePixmap = new QPixmap(m_middleMiddleFilename);
    m_middleRightPixmap = new QPixmap(m_middleRightFilename);
    m_bottomLeftPixmap = new QPixmap(m_bottomLeftFilename);
    m_bottomMiddlePixmap = new QPixmap(m_bottomMiddleFilename);
    m_bottomRightPixmap = new QPixmap(m_bottomRightFilename);
    m_pixmapsLoaded = true;
}

void ScaleNinePainter::unloadPixmaps() {
    delete m_topLeftPixmap; m_topLeftPixmap = 0;
    delete m_topMiddlePixmap; m_topMiddlePixmap = 0;
    delete m_topRightPixmap; m_topRightPixmap = 0;
    delete m_middleLeftPixmap; m_middleLeftPixmap = 0;
    delete m_middleMiddlePixmap; m_middleMiddlePixmap = 0;
    delete m_middleRightPixmap; m_middleRightPixmap = 0;
    delete m_bottomLeftPixmap; m_bottomLeftPixmap = 0;
    delete m_bottomMiddlePixmap; m_bottomMiddlePixmap = 0;
    delete m_bottomRightPixmap; m_bottomRightPixmap = 0;
    m_pixmapsLoaded = false;
}

void ScaleNinePainter::paint(QPainter* painter, const QRect &rect, QWidget* widget) {
    Q_UNUSED(widget)

    if(!m_pixmapsLoaded)
        loadPixmaps();

    painter->save();

    // Draw top left.
    if(m_topLeftPixmap->height() > rect.height())
        painter->drawPixmap(0, 0, m_topLeftPixmap->width(), rect.height(), *m_topLeftPixmap);
    else
        painter->drawPixmap(0, 0, *m_topLeftPixmap);

    // Draw top middle.
    if(!m_topMiddlePixmap->isNull()) {
        painter->drawTiledPixmap(m_topLeftPixmap->width(),
                                 0,
                                 rect.width() - (m_topLeftPixmap->width() + m_topRightPixmap->width()),
                                 qMin(m_topMiddlePixmap->height(), rect.height()),
                                 *m_topMiddlePixmap);
    }

    // Draw top right.
    painter->drawPixmap(rect.right() - m_topRightPixmap->width() + 1, 0, *m_topRightPixmap);

    // Draw left border.
    if(!m_middleLeftPixmap->isNull()) {
        painter->drawTiledPixmap(0,
                                 m_topLeftPixmap->height(),
                                 m_middleMiddlePixmap->width(),
                                 rect.height() - (m_topLeftPixmap->height() + m_bottomLeftPixmap->height()),
                                 *m_middleLeftPixmap);
    }

    // Draw middle.
    if(!m_middleMiddlePixmap->isNull()) {
        QRect middleRect; 
        middleRect.setLeft(m_middleLeftPixmap->isNull() ? m_topLeftPixmap->width() : m_topLeftPixmap->width());
        middleRect.setRight(rect.width() - (m_middleRightPixmap->isNull() ? m_topRightPixmap->width() : m_topRightPixmap->width()) - 1);
        middleRect.setTop(m_topMiddlePixmap->isNull() ? m_topLeftPixmap->height() : m_topMiddlePixmap->height());
        middleRect.setBottom(rect.height() - (m_bottomMiddlePixmap->isNull() ? m_bottomLeftPixmap->height() : m_bottomMiddlePixmap->height()) - 1);
        painter->drawTiledPixmap(middleRect, *m_middleMiddlePixmap);
    }

    // Draw right border.
    if(!m_middleRightPixmap->isNull()) {
        painter->drawTiledPixmap(rect.width() - m_middleRightPixmap->width(),
                                 m_topRightPixmap->height(),
                                 m_middleRightPixmap->width(),
                                 rect.height() - (m_topRightPixmap->height() + m_bottomRightPixmap->height()),
                                 *m_middleRightPixmap);
    }

    // Draw bottom row.
    if(!m_bottomLeftPixmap->isNull()) {
        painter->drawPixmap(0,
                            rect.bottom() - m_bottomLeftPixmap->height() + 1,
                            *m_bottomLeftPixmap);
    }
    if(!m_bottomMiddlePixmap->isNull()) {
        QRect bottomRect;
        bottomRect.setLeft(m_bottomLeftPixmap->width());
        bottomRect.setRight(rect.width() - m_bottomRightPixmap->width() - 1);
        bottomRect.setTop(rect.bottom() - m_bottomMiddlePixmap->height() + 1);
        bottomRect.setBottom(rect.bottom());
        painter->drawTiledPixmap(bottomRect, *m_bottomMiddlePixmap);
    }
    if(!m_bottomRightPixmap->isNull()) {
        painter->drawPixmap(rect.right() - m_bottomLeftPixmap->width() + 1,
                            rect.bottom() - m_bottomRightPixmap->height() + 1,
                            *m_bottomRightPixmap);
    }
    painter->restore();
}

}  // GVA namespace
