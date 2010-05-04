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



#ifndef __FLOWBASE_H__
#define __FLOWBASE_H__

#include <QImage>
#include <QWidget>
#include <QGraphicsWidget>
#include <QGraphicsItem>

class QPainter;

namespace WRT {
    /*!
     * \class FlowInterface
     *
     * \brief This file describes the common interface of the various history view engines
     * i.e., PictureFlow, VisualFlow
     *
     * This class is designed so that the Views using these engines can implement some functionality
     * in common modules, and assign the specific engine at a higher level
     */
    class FlowInterfaceBase
    {
    public:
        //! Perform init steps on flow
        virtual void init() = 0;

        //! Clear all slides
        virtual void clear() = 0;
        
        //! Add a slide to the flow
        virtual void addSlide(const QImage& image) = 0;

        //! Add a slide to the flow
        virtual void addSlide(const QImage&, const QString&) {};

        //! Set the center of the flow
        virtual void setCenterIndex(int i) = 0;

        //! Show the previous item 
        virtual void showPrevious() = 0;

        //! Show the next item
        virtual void showNext() = 0;

        //! Returns the total number of slides.
        virtual int slideCount() const = 0;

        //! Returns QImage of specified slide.
        virtual QImage slide(int) const = 0;

        //! Return the index of the current center slide
        virtual int centerIndex() const = 0;

        //! Return true if slide animation is ongoing
        virtual bool slideAnimationOngoing() const = 0;

        //! Return the central slide's rect
        virtual QRect centralRect() const = 0;

        //! Insert a slide at index position
        virtual void insert(int, const QImage&, const QString&) {}

        //! Remove a slide at index position
        virtual void removeAt(int) {}

        //! handle the display mode change
        virtual void displayModeChanged(QString&) {}

        //! prepare start-animation
        virtual void prepareStartAnimation() {}

        //! run start-animation
        virtual void runStartAnimation() {}
        
        //! run end-animation
        virtual void runEndAnimation() {}
    };

    class FlowInterface : public QWidget, public FlowInterfaceBase
    {
        Q_OBJECT
    public:
        //! Init the FlowInterface with a QWidget parent
        FlowInterface(QWidget* parent) : QWidget(parent) {}

        //! declear common signals
    signals:
        void centerIndexChanged(int index);
        void ok(int index);
        void cancel();
    };

    class GraphicsFlowInterface : public QGraphicsWidget, public FlowInterfaceBase
    {
        Q_OBJECT
    public:
        //! Init the FlowInterface with a QWidget parent
        GraphicsFlowInterface(QGraphicsItem* parent) : QGraphicsWidget(parent) {}

    signals:
        void centerIndexChanged(int index);
        void ok(int index);
        void cancel();
    };
}

#endif      // __FLOWBASE_H__
