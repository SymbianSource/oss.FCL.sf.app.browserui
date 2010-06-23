/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not,
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/
#include <QtGui>
#include "ChromeItem.h"
#include "ChromeSnippet.h"

namespace GVA {
    class LinearFlowSnippet;
}

namespace GVA {
class WRT::WrtBrowserContainer;

class  MostVisitedPagesWidget : public ChromeItem
{
    Q_OBJECT
public :
    //construction and destruction
    MostVisitedPagesWidget(ChromeSnippet* snippet,QGraphicsWidget* parent);
    ~MostVisitedPagesWidget();

    void open();

    void updatePos(QPointF pos, qreal &toolBarHeight);
    void resize(const QSize &size);
    void displayModeChanged(QString& newMode);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setCenterIndex(QString displayMode);
    void updateMVGeometry();
    void updateMVStore(WRT::WrtBrowserContainer *page);

Q_SIGNALS:
    void closeComplete();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

public slots:
    void close(bool hide=true);
    void okTriggered(int index);
    void closeAnimationCompleted();
    void onLoadFinished(const bool ok);
    void clearMVStore();
private :
    MostVisitedPageStore* m_mostVisitedPageStore;
    QGraphicsWidget* m_parent;
    GVA::LinearFlowSnippet *m_flowInterface;
    int m_selectIndex;
    bool m_hideOnClose;
};
}
