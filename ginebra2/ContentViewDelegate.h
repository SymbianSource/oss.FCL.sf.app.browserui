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


#ifndef __GINEBRA_CONTENTVIEWDELEGATE_H__
#define __GINEBRA_CONTENTVIEWDELEGATE_H__

#include <QObject>
#include <QtGui>
#include "controllableviewimpl.h"

namespace GVA {
  class ChromeWidget;

  class ContentViewDelegate : public ControllableViewBase
  {
    Q_OBJECT
  public:
    ContentViewDelegate(ChromeWidget *chrome, QObject * parent);
    virtual ~ContentViewDelegate();
    virtual QGraphicsWidget* view() { return widget(); }
    ChromeWidget *chrome() { return m_chrome; }
    QString name() const { return jsObject()->objectName(); }

//    Q_PROPERTY(QString name READ getName)
//    QString getName() const { return objectName(); }
//
// use isActive() instead.
//    Q_PROPERTY(bool visible READ isVisible WRITE setVisible)
//    bool isVisible() { return view()->isVisible(); }
//    void setVisible(bool value) { view()->setVisible(value); }

  public slots:
// use ControllableView version
//    void show() {view()->show();}
//    void hide() {view()->hide();}
    virtual void triggerAction(const QString & action){Q_UNUSED(action);return;}
  protected:
    ChromeWidget * m_chrome;
  };

} // end of namespace GVA

#endif // __GINEBRA_CONTENTVIEWDELEGATE_H__
