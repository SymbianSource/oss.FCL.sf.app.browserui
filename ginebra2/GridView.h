#ifndef GRIDVIEW_H
#define GRIDVIEW_H

#include <QGraphicsWidget>
#include <QGraphicsGridLayout>


namespace GVA {

class GridView : public QGraphicsWidget
{
public:
    explicit GridView(QGraphicsItem *parent = 0);
    ~GridView();

    void addItem(QGraphicsLayoutItem *item);

    enum LayoutMode{
        Normal,
        Vertical
    };

    void setMode(LayoutMode mode);
    LayoutMode mode() const { return m_mode; }

    void setColumnRowCounts(int columns, int rows);
    int columnCount() const { return m_columnCount; }
    int rowCount() const { return m_rowCount; }

    void clear();
    void rebuildLayout();

protected:
    QGraphicsGridLayout *m_gridLayout;
    LayoutMode m_mode;
    QList<QGraphicsLayoutItem *> m_items;
    int m_columnCount;
    int m_rowCount;
};

}  // namespace

#endif // GRIDVIEW_H
