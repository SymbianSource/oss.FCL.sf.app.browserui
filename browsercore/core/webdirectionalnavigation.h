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


#ifndef WEBDIRECTIONALNAVIGATION_H
#define WEBDIRECTIONALNAVIGATION_H

#include <qobject.h>
#include <qevent.h>
#include <qvector.h>
#include <qpixmap.h>
#include <qwebelement.h>

class QWebPage;

namespace WRT {

    class WebDirectionalNavigation : public QObject
    {
    Q_OBJECT
    public:
        WebDirectionalNavigation(QWebPage* webPage,QObject* view);
        virtual ~WebDirectionalNavigation();

        void install();
        void uninstall();
        void setPage( QWebPage * page){ m_webPage = page ;}

    signals:
        void pageScrollPositionZero();

    protected:
        bool eventFilter(QObject *object, QEvent *event);
        void initiallayoutCompleted();
        double calculateElementDistance(int direction, const QRect& possibleRect);
        bool nextElementInDirection(int direction);
        void scrollFrame(int direction);
        void scrollFrameOneDirection (int direction, int distance);
        void scrollCurrentFrame (int dx, int dy);
        void setCurrentFrameScrollPosition (QPoint& pos);

    protected:
        QWebPage* m_webPage;
        QObject* m_view;
        QWebElement m_webElement;
		QString m_webElementStyle; 
        QPoint m_focusPoint;
    };

};

#endif
