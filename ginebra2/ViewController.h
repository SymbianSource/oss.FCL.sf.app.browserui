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


#include <QMap>

#include "controllableviewimpl.h"

#ifndef VIEWCONTROLLER_H_
#define VIEWCONTROLLER_H_

class QWebPage;

namespace GVA {

/*! \ingroup JavascriptAPI
 * \brief View controller for all content views.
 *
 * The views are created as children of this object, they can accessed in javascript like this:
 * \code
 * views.WebView.show()
 * \endcode
 */

// TO DO: create a seperate js object for this class.

  class ViewController : public QObject {
    Q_OBJECT
  public:
    ViewController();
    virtual ~ViewController();
    void addView(ControllableViewBase *controllableView);

    // A list of all existing views.
    Q_PROPERTY(QObjectList views READ getViews)
    QObjectList getViews();

    void showCurrent();

    ControllableViewBase *view(const QString &name) { return m_viewMap[name]; }

    ControllableViewBase *currentView();

    void viewChanged();
    
  public slots:
    // Returns the currently visible view as a javascript-usable object.
    QObject *current() {
        return static_cast<QObject *>(m_current.value()->jsObject());
    }
    
    // Show the view named 'name'.
    void showView(const QString &name);
    
    // Freeze the current view.
    void freezeView();
    
    // Unfreeze the current view.
    void unfreezeView();

    // Deprecated, use showView().
    void showContent(const QString &type) { showView(type); }
    
    void dump();

  signals:
    // Sent when the current view is about to change.
    void currentViewChanging();
    
    // Sent when the current view has changed.
    void currentViewChanged();
    
    // Not for javascript use.
    void javaScriptWindowObjectCleared(QWebPage *);

  private:
    typedef QMap<QString, ControllableViewBase *> ViewMap;

    ViewMap m_viewMap;
    ViewMap::iterator m_current;
  };

}


#endif /* VIEWCONTROLLER_H_ */
