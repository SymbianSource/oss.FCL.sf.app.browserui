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


#ifndef __gva_actionbuttonsnippet_h__
#define __gva_actionbuttonsnippet_h__

#include "ChromeSnippet.h"
#include "ChromeWidget.h"
#include "ActionButton.h"

namespace GVA {


  class ActionButtonSnippet : public ChromeSnippet
  {
    Q_OBJECT
    public:
       ActionButtonSnippet(const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element);
       virtual ~ActionButtonSnippet() {;}

    public slots:
       void setIcon( const QString & icon );
       void setDisabledIcon( const QString & icon );
       void setSelectedIcon( const QString & icon );
       void setActiveIcon( const QString & icon );
       void connectAction ( const QString & action, const QString & view, const QString & inputEvent = "Down" );
       void setEnabled( bool enabled );
       void setLatched( bool latched );
       void setInputEvent( const QString & inputEvent );
    signals:
       void activated();
       void contextMenuEvent();
  };
}

#endif // ICONSNIPPET_H
