/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mostvisitedsnippet.h"
#include "mostvisitedpageview.h"

namespace GVA {

MostVisitedSnippet::MostVisitedSnippet(const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element) :
        ChromeSnippet(elementId, chrome, widget, element)
{
    m_chrome = chrome;
}

void MostVisitedSnippet::toggleVisibility(bool animate) 
{
    MostVisitedPagesWidget *mostVisitedPagesWidget = dynamic_cast<MostVisitedPagesWidget*>(widget());
    if (!mostVisitedPagesWidget) 
        return;

    if (!isVisible()) {
         mostVisitedPagesWidget->open();
         updateMVGeometry();
         mostVisitedPagesWidget->show();
         ChromeSnippet::toggleVisibility(animate);

    } else {
        mostVisitedPagesWidget->close();
    }
}


void MostVisitedSnippet::setWidget(QGraphicsWidget * widget)
{
    connect(m_chrome , SIGNAL(aspectChanged(int)), this, SLOT(displayModeChanged(int)));
    ChromeSnippet::setWidget(widget);   
    MostVisitedPagesWidget *mostVisitedPagesWidget = dynamic_cast<MostVisitedPagesWidget*>(widget);
    connect(mostVisitedPagesWidget, SIGNAL(closeComplete()), this, SIGNAL(mostVisitedSnippetCloseComplete()));
}

void MostVisitedSnippet::updateMVGeometry()
{
    MostVisitedPagesWidget *mostVisitedPagesWidget = dynamic_cast<MostVisitedPagesWidget*>(widget());
    qreal toolBarHeight;

    ChromeSnippet* visibleSnippet= m_chrome->getSnippet("WebViewToolbarId");
    if (visibleSnippet)
        toolBarHeight = visibleSnippet->widget()->rect().height(); 

    mostVisitedPagesWidget->resize(m_chrome->size().toSize());
    mostVisitedPagesWidget->updatePos(QPointF(0, 0), toolBarHeight);
}

void MostVisitedSnippet::displayModeChanged(int newMode)
{
    if (isVisible()) {     
        MostVisitedPagesWidget *mostVisitedPagesWidget = dynamic_cast<MostVisitedPagesWidget*>(widget());
        
        if (!mostVisitedPagesWidget) 
            return;

        updateMVGeometry();
        
        QString  displayMode = (newMode == landscape) ? "Landscape" : "Portrait";
        mostVisitedPagesWidget->displayModeChanged(displayMode);
    }
}

void MostVisitedSnippet::hide(bool animate)
 {
     MostVisitedPagesWidget *mostVisitedPagesWidget = dynamic_cast<MostVisitedPagesWidget*>(widget());
   
     if(!mostVisitedPagesWidget) 
       return;

     ChromeSnippet::hide(animate);
     mostVisitedPagesWidget->closeAnimationCompleted();
 }

void MostVisitedSnippet::close() 
{
     MostVisitedPagesWidget *mostVisitedPagesWidget = dynamic_cast<MostVisitedPagesWidget*>(widget());
   
     if(mostVisitedPagesWidget) {
       mostVisitedPagesWidget->close();
     }
}

MostVisitedSnippet::~MostVisitedSnippet()
{
    disconnect(m_chrome , SIGNAL(aspectChanged(int)), this, SLOT(displayModeChanged(int)));
}


}
