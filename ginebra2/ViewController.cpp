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


#include "ViewController.h"
#include <QDebug>

namespace GVA {
    
  ViewController::ViewController()
    : m_viewMap() {
    m_current = m_viewMap.begin();
  }

  ViewController::~ViewController() {
    foreach(ControllableViewBase *view, m_viewMap) {
      delete view;
    }
  }

  void ViewController::addView(ControllableViewBase *controllableView) {
    assert(controllableView);
    qDebug() << "ViewController::addView: adding " << controllableView
             << " jsObject=" << controllableView->jsObject();
    QString key;
    // Set up parent/child link for javascript access to the view.
    if(controllableView->jsObject()) {
      // Use the view's javascript object.
      controllableView->jsObject()->setParent(this);
      key = controllableView->jsObject()->objectName();
    }
    else {
      // Use the view itself.
      controllableView->setParent(this);
      key = controllableView->objectName();
    }
    if(key.isNull()) {
      qWarning("ViewController::addView: missing objectName.");
    }
    m_viewMap.insert(key, controllableView);
  }

  QObjectList ViewController::getViews() {
    QObjectList *result = new QObjectList;
    foreach(ControllableViewBase *view, m_viewMap) {
      result->append(view);
    }
    return *result;
  }
  
  void ViewController::showCurrent() {
      qDebug() << "ViewController::showCurrent: " << m_current.value();
      ControllableViewBase *currentView = m_current.value();
      if(!currentView) return;

      if(!currentView->isActive()) {
          emit currentViewChanging();
          // Activate the current view.
          currentView->activate();
          currentView->show();

          // Deactivate all others.
          foreach(ControllableViewBase *view, m_viewMap) {
              if(view && view->isActive() && view != currentView) {
                  view->hide();
                  view->deactivate();
              }
          }
          emit currentViewChanged();
      }
  }
  
  void ViewController::showView(const QString &name) {
      ViewMap::iterator it = m_viewMap.find(name);
      if(it != m_viewMap.end()) {
          m_current = it;
          showCurrent();
      }
  }
  
  void ViewController::freezeView() {
      if(!m_viewMap.isEmpty() ) {
          m_current.value()->freeze();
      }
  }
  
  void ViewController::unfreezeView() {
      if(!m_viewMap.isEmpty() ) {
          m_current.value()->unfreeze();
      }
  }

  void ViewController::dump() {
    qDebug() << "ViewController::dump:"
        << " count=" << m_viewMap.count()
        << " current=" << m_current.value();
    foreach(ControllableViewBase *view, m_viewMap) {
      qDebug() << "  " << view;
    }
  }

  void ViewController::viewChanged() {
      emit currentViewChanged();
  }
  
  ControllableViewBase* ViewController::currentView() {
      if(!m_viewMap.isEmpty())
          return m_current.value();
      else
          return NULL;
  }

}

