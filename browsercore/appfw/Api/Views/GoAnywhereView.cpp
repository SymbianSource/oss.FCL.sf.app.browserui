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



#include "GoAnywhereView.h"
#include "GoAnywhereViewJSObject.h"
#include "GoAnywhereView_p.h"
#include "wrtbrowsercontainer.h"
#include "WrtPageManager.h"
#include "qwebframe.h"
#include "wrtpage.h"

#include "qwebhistory.h"
#include "qwebframe.h"
#include "wrtbrowsercontainer.h"
#include "webpagedata.h"
#include "HistoryFlowView.h"

#include <QButtonGroup>
#include <QGroupBox>
#include <QDebug>
#include <QFileInfo>
#include <QGroupBox>
#include <QHash>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QTabBar>


namespace WRT {
//QString GoAnywhereView::type() const 
//{ return d->m_type; }

GoAnywhereViewPrivate::GoAnywhereViewPrivate(WrtPageManager * pageMgr,
                                         QWidget* parent,
                                         GoAnywhereView* view,
                                         const QString& aType) :
    m_widgetParent(parent),
    m_pageManager(pageMgr),
    m_goAnywhereWidget(new GoAnywhereTabWidget(parent)),
    m_isActive(false),
    m_currentViewIndex(0),
    m_type(aType)
{
    Q_ASSERT(m_pageManager);
    Q_UNUSED(view);
    m_actionBack = new QAction("Back", m_widgetParent);
    m_actionBack->setObjectName("Back");
}

// ----------------------------------------------------
// GoAnywhereWidget
// ----------------------------------------------------
 
GoAnywhereWidget::GoAnywhereWidget(QWidget *parent, GoAnywhereView *view) : QTabWidget(parent), m_view(view) 
{
  //  setObjectName(QString::fromUtf8("GoAnywhereObj"));
}

// ----------------------------------------------------
// GoAnywhereView
// ----------------------------------------------------

/*!
 * \class GoAnywhereView
 *
 * \brief The base class for the GoAnywhereViews
 *
 * This class is responsible for visual aspects of a back page
 *
 */

/*!
  Basic GoAnywhereView constructor requires a PageManager to manage the pages
  and a parent QWidget
  @param pageMgr : PageManager
  @param parent : handle to parent widget
*/

GoAnywhereView::GoAnywhereView(WrtPageManager * pageMgr,
                         QWidget* parent,const QString& aType) :
    d(new GoAnywhereViewPrivate(pageMgr, parent, this, aType))
{
    init();
}

/*!
 * GoAnywhereView destructor
*/
GoAnywhereView::~GoAnywhereView()
{
    delete d;
}

/*!
 * GoAnywhereView initlization
*/
void GoAnywhereView::init()
{   
    connect(d->m_actionBack, SIGNAL(triggered()), this, SLOT(back()));
    connect(d->m_goAnywhereWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    d->m_goAnywhereWidget->hide();
    m_jsObject = new GoAnywhereViewJSObject(this, 0);
}


bool GoAnywhereView::eventFilter(QObject* receiver, QEvent* event)
{
    return false;   
}

/*!
  Retrieve the WrtPageManager assigned to this view
  @return WrtPageManager*
*/
WrtPageManager* GoAnywhereView::wrtPageManager()
{
    return d->m_pageManager;
}

/*!
  Return the widget handle of this view
  @return QWidget*
*/
QWidget* GoAnywhereView::qWidget() const
{
    return d->m_goAnywhereWidget;
}

/*!
  Return the title of this view for display
  @return QString
*/
QString GoAnywhereView::title()
{
    return "Go Anywhere View";
}

/*!
 * Return whether this view is active or not
 * @return bool
*/
bool GoAnywhereView::isActive()
{
    return d->m_isActive;
}

/*! 
 * Return the list of public QActions most relevant to the view's current context
 * (most approptiate for contextual menus, etc.)
 * @return QList<QAction*>
*/
QList<QAction*> GoAnywhereView::getContext()
{
    // for now, all actions valid at all times
    // but there may be some logic here to determine context
    QList<QAction*> contextList;
    contextList << d->m_actionBack;
    return contextList;
}

/*!
  Add a wrt view to the GoAnywhereView
  @param view : ControllableView
  @param viewId : int
*/
void GoAnywhereView::addView(ControllableView* view)
{
    d->m_views.push_back(view);
    d->m_viewIds.push_back(view->type());
}

/*!
  Go back from GoAnywhereView view
*/
void GoAnywhereView::back()
{
     emit ok(0);
}

/*!
  Return the view's Back QAction
  For go back to the content view
  @return Action*
*/
QAction * GoAnywhereView::getActionBack()
{
    return d->m_actionBack;
}

/*!
  tabchanged event handler
  @param index : int
*/
void GoAnywhereView::tabChanged(int index)
{
    if (!d->m_isActive)
        return;

    //qDebug() << "start tabChanged -- index:" << index;

    //a. deactivate the oldview
    ControllableViewQWidgetBase* oldView = static_cast<ControllableViewQWidgetBase*>(d->m_views.at(d->m_currentViewIndex));
    //qDebug() << "oldView" << oldView->title();
    
    if (oldView->isActive()) {
        //qDebug() << "ssssss1:" << d->m_goAnywhereWidget->count() << index;
        oldView->deactivate();
        //qDebug() << "ssssss2:" << d->m_goAnywhereWidget->count() << index;
        d->m_goAnywhereWidget->removeTab(d->m_currentViewIndex); // [1] removeTab causes tabChanged() 
        //qDebug() << "ssssss3:" << d->m_goAnywhereWidget->count() << index;
        QString title = oldView->title();
        if (title.indexOf("Window") != -1)
            title = "Windows";
        d->m_goAnywhereWidget->insertTab(d->m_currentViewIndex, new QPushButton(d->m_goAnywhereWidget), title); 
        // Have to reset parent on the widget because tabbed widget sets itself as parent
        if(oldView->qWidget())
            oldView->qWidget()->setParent(0);
        //qDebug() << "insertTab" << d->m_goAnywhereWidget->count() << index;
    } else {
        //qDebug() << "removeTab returns"; // be here because of [1] or [2]
        return;
    }

    //b. activate the newview
    //qDebug() << "ssssss4:" << d->m_goAnywhereWidget->count() << index;

    // ControllableView* newView = d->m_views.at(index);
    // TODO: Rewrite GoAnyWhere view to use QGraphicsWidget
    ControllableViewQWidgetBase* newView = static_cast<ControllableViewQWidgetBase*>(d->m_views.at(index));
    //qDebug() << "newView" << newView->title();
    if (!newView->isActive()) {
        newView->activate();
        QString title = newView->title();
        if (title.indexOf("Window") != -1)
            title = "Windows";
        d->m_goAnywhereWidget->insertTab(index + 1, newView->qWidget(), title); 
    }
    //qDebug() << "remove empty Tab before:" << d->m_goAnywhereWidget->count() << ":" << index + 1;
    QWidget* emptyWidget = d->m_goAnywhereWidget->widget(index);
    d->m_goAnywhereWidget->removeTab(index); // [2] removeTab causes tabChanged()
    delete emptyWidget;
    //qDebug() << "remove empty Tab after:" << d->m_goAnywhereWidget->count();
    d->m_currentViewIndex = index;
    emit goAnywhereViewChanged(d->m_viewIds.at(d->m_currentViewIndex));
    //qDebug() << "end of tabchange :) " << d->m_currentViewIndex;
}

/*!
 * Activate the view's resources. Could be connected by client to view visibility
*/
void GoAnywhereView::activate()
{
    Q_ASSERT(!d->m_isActive);
    
    d->m_currentViewIndex = 0;
    for (int i = 0; i < d->m_views.size(); i++) {

        // TODO: Rewrite GoAnyWhere view to use QGraphicsWidget
        // ControllableView* view = d->m_views.at(i);
        ControllableViewQWidgetBase* view = static_cast<ControllableViewQWidgetBase*>(d->m_views.at(i));
        QString title = view->title();
        if (title.indexOf("Window") != -1)
            title = "Windows";
        if (i == d->m_currentViewIndex) { // only activate first view
            view->activate();
            d->m_goAnywhereWidget->addTab(view->qWidget(), title);
        }
        else
            d->m_goAnywhereWidget->addTab(new QPushButton(d->m_goAnywhereWidget), title);    
    }

    if (d->m_views.size() > 0) {
        d->m_goAnywhereWidget->setCurrentIndex(d->m_currentViewIndex);
        emit goAnywhereViewChanged(d->m_viewIds.at(d->m_currentViewIndex));
    }

    d->m_isActive = true;
    d->m_goAnywhereWidget->show(); 
}
    
/*! 
 * Deactivate the view's resources. Could be connected by client to view visibility
*/
void GoAnywhereView::deactivate()
{
    Q_ASSERT(d->m_isActive);
    // cleanup
    d->m_isActive = false;

    QVector<QWidget*> trash;

    for (int i = 0; i < d->m_goAnywhereWidget->count(); i++) {
        if (i != d->m_currentViewIndex)
            trash.push_back(d->m_goAnywhereWidget->widget(i));
    }

    // clear tabwidget
    d->m_goAnywhereWidget->clear();
    
    // delete empty widget
    for (int i = 0; i < trash.size(); ++i) {
        delete trash.at(i);
    }
     
    ControllableView* currentView =NULL;
    if(d->m_views.count() > 0) {
        // deactivate current view
        currentView = d->m_views.at(d->m_currentViewIndex);
    }

    if (currentView && currentView->isActive())
    {    
        currentView->deactivate();

        // Have to reset parent on the widget because tabbed widget sets itself as parent
        if(static_cast<ControllableViewQWidgetBase*>(currentView)->qWidget())
            static_cast<ControllableViewQWidgetBase*>(currentView)->qWidget()->setParent(0);
    }

    d->m_goAnywhereWidget->hide(); 
}

/*!
  Return the central slide rect of the History View
  @return QRect
*/
QRect GoAnywhereView::centralRect() const
{
    if (d->m_views.size() > 0) {
        HistoryView* historyView = static_cast<HistoryView*>(d->m_views.at(0)); // FIXME, the first view is HistoryView

        if (historyView) {
            QRect r = historyView->centralRect();
            QPoint topLeft = historyView->widget()->parentWidget()->mapToParent(r.topLeft()).toPoint();
            QPoint bottomRight = historyView->widget()->parentWidget()->mapToParent(r.bottomRight()).toPoint();
            qDebug() << r << " vs " << QRect(topLeft, bottomRight);
            return QRect(topLeft, bottomRight);
        }
    }
    return QRect();
}

/*!
  Return the central slide image of the History View
  @return QImage
*/
QImage GoAnywhereView::currentSlide()
{
    if (d->m_views.size() > 0) {
        HistoryFlowView* historyView = static_cast<HistoryFlowView*>  (d->m_views.at(0)); // FIXME, the first view is HistoryView

        if (historyView) {
            return historyView->getCurrentSlide();
        }
    }
    return QImage();
}

ControllableView* GoAnywhereView::createNew(QWidget *parent)
{
    return new GoAnywhereView(WrtPageManager::getSingleton(),parent,GoAnywhereView::Type());
}
 
} // namespace WRT
