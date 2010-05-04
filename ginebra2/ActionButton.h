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


#ifndef __gva_actionbutton_h__
#define __gva_actionbutton_h__

#include "NativeChromeItem.h"
#include "ChromeSnippet.h"
#include <QIcon>
#include <QImage>

namespace GVA {

class ActionButton : public NativeChromeItem
    {
        Q_OBJECT

    public:
        ActionButton( ChromeSnippet * snippet, QGraphicsItem * parent = 0 );
        virtual ~ActionButton() {};
        void paint( QPainter * painter, const QStyleOptionGraphicsItem * opt, QWidget * widget );
        void addIcon( const QString & icon, QIcon::Mode mode = QIcon::Normal );
        void setAction ( QAction * action, QEvent::Type triggerOn = QEvent::GraphicsSceneMousePress );
	void disconnectAction();
	void setEnabled(bool);
	void setChecked(bool);
	void setInputEvent(QEvent::Type event);
    public slots:
	void onActionChanged();
	  //For testing only
	void onTriggered( bool checked );
    signals:
	void activated();
        void contextMenuEvent();
    protected:
	void mousePressEvent( QGraphicsSceneMouseEvent * ev );
	void mouseReleaseEvent( QGraphicsSceneMouseEvent * ev );
	void contextMenuEvent( QGraphicsSceneContextMenuEvent * ev );
    private:
        QAction * defaultAction();
        QAction * m_internalAction;
        QIcon m_icon;
	QEvent::Type m_triggerOn;
        bool m_active;
	QIcon::Mode m_state;
    };
} // end of namespace GVA

#endif // ICONWIDGET_H
