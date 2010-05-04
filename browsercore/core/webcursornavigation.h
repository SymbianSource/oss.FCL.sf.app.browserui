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


#ifndef WEBCURSORNAVIGATION_H
#define WEBCURSORNAVIGATION_H


#include <qobject.h>
#include <qevent.h>
#include <qtimer.h>
#include <qpixmap.h>

class QWebPage;

namespace WRT {

    class WebCursorNavigation : public QObject
    {
    Q_OBJECT
    public:
        WebCursorNavigation(QWebPage* webPage,QObject* view);
        virtual ~WebCursorNavigation();

        void install();
        void uninstall();
        void setPage( QWebPage * page){ m_webPage = page ;}

    signals:
        void pageScrollPositionZero();

    protected slots:
        void keypressTimeout();
        void scrollTimeout();

    protected:
        void keyPressEvent(QKeyEvent* ev);
        void keyReleaseEvent(QKeyEvent* ev);
        bool isContentEditable();
        bool eventFilter(QObject *object, QEvent *event);
        bool scroll(int direction);
        void moveCursor(int direction);
        int getNearestEdge(int scrollRange,int direction);


    private:
        QWebPage* m_webPage;
        QObject* m_view;
        QPoint m_cursorPosition;
        int m_flipcounter;
        int m_direction;
        int m_lastdirection;
        QTimer m_keypressTimer;
        QTimer m_scrollTimer;

    };
};
 
#endif
