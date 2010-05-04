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


#ifndef MOSTVISITEDSNIPPET_H
#define MOSTVISITEDSNIPPET_H

#include "ChromeSnippet.h"
#include "ChromeWidget.h"

namespace GVA {
	
class MostVisitedSnippet : public ChromeSnippet
{
    Q_OBJECT
public:
    MostVisitedSnippet(const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element);
    void toggleVisibility(bool animate);
    void hide(bool animate);
    void updateMVGeometry();
    void setWidget(QGraphicsWidget * widget);
    virtual ~MostVisitedSnippet();
    void close();
Q_SIGNALS:
    void mostVisitedSnippetCloseComplete();
private slots :
    void displayModeChanged(int newMode);
private:
    ChromeWidget *m_chrome;
};
}

#endif // MOSTVISITEDSNIPPET_H
