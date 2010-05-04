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


#include  "qwebframe.h"
#include "scriptobjects.h"
#include "actionjsobject.h"
#include "controllableview.h"
#include "controllableviewjsobject.h"
#include <QGraphicsWidget>


ControllableViewJSObject::ControllableViewJSObject(ControllableView *contentView, QWebFrame *chromeFrame, const QString &objectName)
  : m_contentView(contentView),
    m_actionsParent(0)
{
    setObjectName(objectName);
    setChromeFrame(chromeFrame);
    updateActions();
}

ControllableViewJSObject::~ControllableViewJSObject() {
    if(m_actionsParent) {
        // Also deletes the actions since they are children of m_actionsParent.
        delete m_actionsParent;
    }
}


void ControllableViewJSObject::updateActions() {   // slot
    if(m_actionsParent) {
        delete m_actionsParent;
        m_actionsParent = 0;
    }
    if(m_contentView && !m_contentView->getContext().isEmpty()) {
        m_actionsParent = new QObject(this);
        m_actionsParent->setObjectName("actions");
        foreach(QAction *action, m_contentView->getContext()) {
            ActionJSObject *jso = new ActionJSObject(m_actionsParent, action);
        }
    }
}

void ControllableViewJSObject::setChromeFrame(QWebFrame *frame) {
    m_chromeFrame = frame;
    if(m_chromeFrame)
        m_chromeFrame->addToJavaScriptWindowObject(objectName(), this);
}

QObject *ControllableViewJSObject::getGeometry() const {
    ScriptRect *r = new ScriptRect(m_contentView->widget()->geometry().toRect());
    if(m_chromeFrame)
        m_chromeFrame->addToJavaScriptWindowObject(objectName() + "_rect", r, QScriptEngine::ScriptOwnership);
    return r;
}

void ControllableViewJSObject::setGeometry(int x, int y, int w, int h) {
    m_contentView->widget()->setGeometry(x, y, w, h);
}

QObject *ControllableViewJSObject::getPosition() const {
    ScriptPoint *p = new ScriptPoint(m_contentView->widget()->pos().toPoint());
    if(m_chromeFrame)
        m_chromeFrame->addToJavaScriptWindowObject(objectName() + "_point", p);
    return p;
}

void ControllableViewJSObject::setPosition(int x, int y) {
    m_contentView->widget()->setPos(x, y);
}

QString ControllableViewJSObject::getType() const { 
    return m_contentView ? m_contentView->type() : QString::null;
}

void ControllableViewJSObject::activate() {
    m_contentView->activate();
}

void ControllableViewJSObject::deactivate() {
    m_contentView->deactivate();
}

void ControllableViewJSObject::show() { 
    m_contentView->show(); 
}

void ControllableViewJSObject::hide() { 
    m_contentView->hide(); 
}

QString ControllableViewJSObject::getTitle() const {
    return m_contentView ? m_contentView->title() : QString::null;
}


