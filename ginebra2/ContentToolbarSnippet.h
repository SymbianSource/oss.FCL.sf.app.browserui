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
#ifndef __GINEBRA_CONTENTTOOLBARSNIPPET_H
#define __GINEBRA_CONTENTTOOLBARSNIPPET_H

#include <QtGui>
#include "ActionButtonSnippet.h"
#include "ToolbarSnippet.h"
#include "WebChromeContainerSnippet.h"

class QTimer;
class ChromeSnippet;

namespace GVA {

  class ContentToolbarSnippet : public ToolbarSnippet
  {
    Q_OBJECT
  public:
    ContentToolbarSnippet(const QString& elementId, ChromeWidget * chrome, const QRectF& ownerArea, const QWebElement & element, QGraphicsWidget * widget);
    virtual ~ContentToolbarSnippet();

    WebChromeContainerSnippet * middleSnippet() { return m_middleSnippet;}
  Q_SIGNALS:
    void menuButtonSelected();
    void menuButtonCanceled();

  private Q_SLOTS:
    void onChromeComplete();
    void handleZoomButton();
    void handleMenuButton();
    void handleMVButton();
    void handleToggleTBButton();
    void childAdded(ChromeSnippet* child);
    void updateOwnerArea();
    void updateSize(QSize );
    void onMouseEvent( QEvent::Type type); 
    void onSnippetMouseEvent( QEvent::Type type); 
    void onHidden();
    void onShown();
    void onInactivityTimer();
    void onExternalMouse(int type, const QString & name, const QString & description);
    void onAspectChanged( ); 


  private:
    void addChild(ChromeSnippet * child) ;
    void manageChildren(ChromeItem * it);

    void addZoomButton(QAction * action, ActionButtonSnippet* button);
    void addMenuButton(QAction * action, ActionButtonSnippet* button);
    void addMostVisitedButton(QAction * action, ActionButtonSnippet* button);
    void addToggleTBButton(QAction * action, ActionButtonSnippet* button);
    void setAction(ChromeSnippet *  s);
    void connectHideShowSignals(ActionButtonSnippet * button) ;
    void addActionInfo(QString id);

    void setWidth(int);
    void resetTimer(bool start=true);
    void hideOtherPopups(QString); 



    WebChromeContainerSnippet* m_middleSnippet;
    QTimer* m_subChromeInactiveTimer;
    ChromeSnippet * m_subChromeSnippet;

  };

} // end of namespace GVA

#endif // __GINEBRA_CONTENTTOOLBARSNIPPET_H
