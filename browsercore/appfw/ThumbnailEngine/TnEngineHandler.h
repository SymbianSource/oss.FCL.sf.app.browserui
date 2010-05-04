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


#ifndef __TNENGINEHANDLER_H__
#define __TNENGINEHANDLER_H__

#include <QTimer>
#include <QPainter>

namespace WRT {

    class TnEngineGenerator;
    class TnEngineView;
    class WrtBrowserContainer;
    class WrtPageManager;

    class TnEngineHandler : public QObject
    {
        Q_OBJECT
    public:
        ~TnEngineHandler();
        TnEngineHandler(WrtPageManager * mgr, QWidget * parent);

    public:
        void setVisible(bool visible);
        bool isVisible() const { return m_visible; }
        void setZoomOutLevel(int percent) { m_zoomOutLevel = percent; }
        int zoomOutLevel() const { return m_zoomOutLevel; }
        QRect theRect() const;
        QRect indicatorRect() const;
        TnEngineView* tnEngineView() { return m_TnEngineView; }

        void drawDocumentPart(QPainter& painter, const QRect& documentAreaToDraw);
        QRect documentViewport() const;
        void scaledPageChanged(const QRect& area, bool fullScreen, bool scroll);
        QSize documentSize() const;

    public slots:
        void documentStarted();
        void documentChanged();
        void documentCompleted(bool);
        void documentViewportMoved();

    public:
        QRect TnEngineRect() const;
        void draw(QPainter& gc, const QRect& rect) const;
        QRect containerRect() const;
        void setContainerRect(const QRect& rect);
        bool isFullScreenMode() const;
        void setFullScreenMode(bool fullScreenMode);
        void updateNow();
        QRect viewportOnDocument() const;

    public slots:

        void updateCbTimerCb();
        void updateTimerCb();
        void scrollBy(int x, int y);
        void draggingStarted();
        void draggingEnded();

    signals:
        void ok(int x, int y);

    private slots:
        void okInvoked();

    private:
        friend class TnEngineGenerator;

        void createGenerator();
        void destroyGenerator();
        void createView();
        void destroyView();
        QRect fromDocCoords(const QRect& from) const;
        QPoint fromDocCoords(const QPoint& from) const;
        QSize fromDocCoords(const QSize& from) const;
        QRect toDocCoords(const QRect& from) const;
        QPoint toDocCoords(const QPoint& from) const;
        QSize toDocCoords(const QSize& from) const;
        bool checkAndCreateBitmap(QSize size, QPixmap*& image);
        QSize calcSize() const;
        QRect calcViewportOnDocument() const;
        void documentChangedCb();
        void scrollTimerCb();
        void visibilityTimerCb();
        QRect viewportOnTnEngine() const;

        void activate();
        void deactivate();

    public:
        QWidget* widget();
        bool isDocumentComplete() const { return m_documentComplete; }

    private:
        // Page Manager
        WrtPageManager * m_pageMgr;
    

        // generator
        TnEngineGenerator* m_generator;
        // zoom level of the TnEngine in percent
        int m_zoomOutLevel;
        QRect m_viewportOnDocument;
        bool m_needsUpdate;
        QTimer* m_updateTimer;
        QTimer* m_updateCbTimer;
        bool m_documentComplete;
        QRect m_containerRect;
        bool m_fullScreenMode;
        bool m_visible;
        bool m_pageScalerUpdating;
        WrtBrowserContainer* m_activePage;
        QPoint m_savedPointPageView;
        TnEngineView* m_TnEngineView;
        qreal m_scrollX;
        qreal m_scrollY;
        bool m_dragging;

        QWidget * m_parentWidget;
    };

}
#endif

// End of File
