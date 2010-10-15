/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "mostvisitedsnippet.h"
#include "MostVisitedView.h"
#include "Utilities.h"
#include "ExternalEventCharm.h"

namespace GVA {

MostVisitedSnippet::MostVisitedSnippet(const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element) :
        ChromeSnippet(elementId, chrome, widget, element),
        m_externalEventCharm(0)
{
    m_chrome = chrome;
    
}

MostVisitedSnippet::~MostVisitedSnippet()
{
    disconnect(m_chrome , SIGNAL(aspectChanged(int)), this, SLOT(displayModeChanged(int)));
    delete m_externalEventCharm;
}

MostVisitedSnippet * MostVisitedSnippet::instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element)
{
    MostVisitedSnippet* that = new MostVisitedSnippet(elementId, chrome, 0, element);
    
    that->setChromeWidget( new MostVisitedView(qtTrId("txt_browser_most_visited_title_most_visited"), chrome->layout()) );
    that->widget()->hide(); //TODO: Shouldn't be needed?
    return that;
}

void MostVisitedSnippet::toggleVisibility(bool animate)
{
    QString displayMode;
    MostVisitedView *mostVisitedPagesWidget = dynamic_cast<MostVisitedView*>(widget());
    if (!mostVisitedPagesWidget)
        return;

    if (!isVisible()) {
    		 #ifdef Q_WS_MAEMO_5
         mostVisitedPagesWidget->setGeometry(10, 10, 784, 335);
         #else
         
         displayMode = m_chrome->layout()->getDisplayMode();
         
         if (displayMode == "portrait")
         		mostVisitedPagesWidget->setGeometry(10, 20, 350, 553);
         else
         		mostVisitedPagesWidget->setGeometry(15, 10, 620, 284);
         #endif
         mostVisitedPagesWidget->update(displayMode);
         mostVisitedPagesWidget->show();
         ChromeSnippet::toggleVisibility(animate);

    } else {
        mostVisitedPagesWidget->close();
    }
}

void MostVisitedSnippet::setChromeWidget(QGraphicsWidget * widget)
{
    safe_connect(m_chrome , SIGNAL(aspectChanged(int)), this, SLOT(displayModeChanged(int)));
    ChromeSnippet::setChromeWidget(widget);
    MostVisitedView *mostVisitedPagesWidget = dynamic_cast<MostVisitedView*>(widget);
    safe_connect(mostVisitedPagesWidget, SIGNAL(closeComplete()), this, SLOT(onWidgetCloseComplete()));
//    m_externalEventCharm = new ExternalEventCharm(widget);
//    safe_connect(m_externalEventCharm, SIGNAL(externalMouseEvent(QEvent *, const QString &, const QString &)),
//                 this, SIGNAL(externalMouseEvent(QEvent *, const QString &, const QString &)));
}

void MostVisitedSnippet::displayModeChanged(int newMode)
{
	  if (isVisible())
        ChromeSnippet::toggleVisibility();
    emit mostVisitedSnippetCloseComplete();
    /*  TO DO: reenable this code.

    if (isVisible()) {
        MostVisitedView *mostVisitedView = dynamic_cast<MostVisitedView*>(widget());

        if (!mostVisitedPagesWidget)
            return;

        updateMVGeometry();

        QString  displayMode = (newMode == landscape) ? "Landscape" : "Portrait";
        mostVisitedPagesWidget->displayModeChanged(displayMode);
    }
    */
}

void MostVisitedSnippet::hide(bool animate)
 {
     MostVisitedView *mostVisitedPagesWidget = dynamic_cast<MostVisitedView*>(widget());
     if (!mostVisitedPagesWidget)
       return;

    ChromeSnippet::hide(false);    	
 }

void MostVisitedSnippet::close()
{
     MostVisitedView *mostVisitedPagesWidget = dynamic_cast<MostVisitedView*>(widget());

     if (mostVisitedPagesWidget) {
       mostVisitedPagesWidget->close();
     }     
}

void MostVisitedSnippet::onWidgetCloseComplete() {            // slot
    if (isVisible())
        ChromeSnippet::toggleVisibility();
    emit mostVisitedSnippetCloseComplete();
}




}
