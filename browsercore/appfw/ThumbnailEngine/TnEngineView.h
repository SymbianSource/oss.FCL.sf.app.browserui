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


#ifndef __TNENGINEVIEW_H__
#define __TNENGINEVIEW_H__

#include <QWidget>

namespace WRT {

class TnEngineHandler;

class TnEngineView : public QWidget
    {
    Q_OBJECT

    public:
        static TnEngineView* initiWithParentAndTnEngineHandler(QWidget* parent, TnEngineHandler* handler);
        virtual ~TnEngineView();

    protected:
        virtual void paintEvent(QPaintEvent* e);
        virtual void keyPressEvent(QKeyEvent* ev);
        virtual void resizeEvent (QResizeEvent* event);
        virtual void mouseMoveEvent(QMouseEvent*);
        virtual void mousePressEvent(QMouseEvent*);
        virtual void mouseReleaseEvent(QMouseEvent*);

    signals:
        void scrollStarted();  
        void scrollBy(int x, int y);
        void scrollEnded();
        void ok();
        void cancel();

    private:
        TnEngineView(QWidget* parent, TnEngineHandler* handler);
        void initview();

    private:    // Data
        TnEngineHandler* m_TnEngineHandler;
        QPoint m_currPoint;
    };
}
#endif // __TNENGINEVIEW_H__

