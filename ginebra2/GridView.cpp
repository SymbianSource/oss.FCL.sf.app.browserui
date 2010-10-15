#include "GridView.h"

#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QPainter>

namespace GVA {

GridView::GridView(QGraphicsItem *parent) :
    QGraphicsWidget(parent),
    m_gridLayout(new QGraphicsGridLayout(this)),
    m_columnCount(2),
    m_rowCount(2)
{
    #ifdef Q_WS_MAEMO_5
    m_gridLayout->setSpacing(16);
    m_gridLayout->setContentsMargins(0, 0, 0 ,0);
    #endif
    
    setLayout(m_gridLayout);
    palette().setColor(QPalette::Normal, QPalette::Window, QColor(0, 44, 0));
}

GridView::~GridView() {
    clear();
}

void GridView::setColumnRowCounts(int columns, int rows) {
    m_columnCount = columns;
    m_rowCount = rows;
    
    #ifndef Q_WS_MAEMO_5  
    if (m_columnCount == 2) // potrait mode
    {
    	m_gridLayout->setVerticalSpacing(49);
    	m_gridLayout->setHorizontalSpacing(14);
  	}
  	else
  	{
  		m_gridLayout->setVerticalSpacing(13);
    	m_gridLayout->setHorizontalSpacing(42);
  	}
    m_gridLayout->setContentsMargins(0, 0, 0 ,0);
    #endif
    
}

void GridView::rebuildLayout() {
    int column = 0;
    int row = 0;
    foreach(QGraphicsLayoutItem *item, m_items) {
        if(column == m_columnCount) {
            row++;
            column = 0;
        }
        //qDebug() << __PRETTY_FUNCTION__ << column << row;
        m_gridLayout->addItem(item, row, column);
        column++;
    }
    m_gridLayout->activate();
}

void GridView::addItem(QGraphicsLayoutItem *item) {
    m_items.append(item);
}

void GridView::clear() {
    while (!m_items.isEmpty())
        delete m_items.takeFirst();
}


}  // namespace

