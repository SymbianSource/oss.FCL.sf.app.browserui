/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/

#include <QtGui>
#include "ChromeSnippet.h"

namespace GVA {
    class LinearFlowSnippet;
}

namespace GVA {

class  MostVisitedPagesWidget : public QGraphicsWidget 
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
    void updateMVStore(QWebPage *page);

Q_SIGNALS:
    void closeComplete();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

public slots:
    void close(bool hide=true);
    void okTriggered(int index);
    void closeAnimationCompleted();
    void onLoadFinished(const bool ok);

private :
    MostVisitedPageStore* m_mostVisitedPageStore;            
    QGraphicsWidget* m_parent;
    GVA::LinearFlowSnippet *m_flowInterface;
    int m_selectIndex;
    ChromeSnippet* m_snippet;
    bool m_hideOnClose;
};
}