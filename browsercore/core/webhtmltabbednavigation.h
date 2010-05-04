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


#ifndef WEBHTMLTABBEDNAVIGATION_H
#define WEBHTMLTABBEDNAVIGATION_H

#include <qobject.h>
#include <qevent.h>

class QWebPage;
namespace WRT {

    class WebHtmlTabIndexedNavigation : public QObject
    {
    public:
        WebHtmlTabIndexedNavigation(QWebPage* webPage,QObject* view);
        virtual ~WebHtmlTabIndexedNavigation();

        void install();
        void uninstall();
        void setPage( QWebPage * page){ m_webPage = page ;}

    protected:
        bool eventFilter(QObject *object, QEvent *event);
        void keyPressEvent(QKeyEvent* ev);
        void keyReleaseEvent(QKeyEvent* ev);

    private:
        QWebPage* m_webPage;
        QObject* m_view;
        int m_radioKeyPressed;
        bool m_radioGroupFocused;
    };

};

#endif
