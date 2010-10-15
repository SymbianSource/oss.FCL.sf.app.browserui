/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not,
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#include <QtGui>
#include <QAction>
#include "Application.h"

#ifdef Q_OS_SYMBIAN
#include <eikenv.h>
#include <apgtask.h>                // TApaTaskList, TApaTask
#endif

/* Application wraps the QApplication class to provide a JS interface. This could be implemented
 * as a derived class, but this would force the application developer to use this class instead
 * of QApplication.
 */

namespace GVA {

GinebraApplication::GinebraApplication()
  : m_app(QApplication::instance())
{
  setObjectName("app");
  connect(m_app, SIGNAL(aboutToQuit()), this, SIGNAL(aboutToQuit()));
  //qDebug()<<QLibraryInfo::location(QLibraryInfo::PluginsPath);
  QCoreApplication::addLibraryPath(QLibraryInfo::location(QLibraryInfo::PluginsPath) + "imageformats");
}

GinebraApplication::~GinebraApplication()
{
}

void GinebraApplication::quit()
{
  m_app->quit();
}

void GinebraApplication::sendToBackground()
{
    //qDebug() << "GinebraApplication::goToBackground()";
#ifdef Q_OS_SYMBIAN
    TApaTask task(CEikonEnv::Static()->WsSession());
    task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
    task.SendToBackground();
#endif
}

void GinebraApplication::debug(const QString &msg) {
      qDebug() << msg;
}

QObject *GinebraApplication::createAction(const QString &text, const QString &iconPath, bool checkable) {
    // Note: the object created here won't be garbage collected by the script engines but will be deleted
    // at shutdown because it is a child of the GinebraApplication object.  Unused actions should be
    // deleted by hand from javascript using the delete operator.
    QAction *action = new QAction(this);
    action->setText(text);
    
    #ifdef Q_WS_MAEMO_5
    action->setCheckable(checkable);

    if(!iconPath.isEmpty()) {
        action->setIcon(QIcon(iconPath));
        action->setIconVisibleInMenu(true);
    }
    #endif
    
    return action;
}

QString GinebraApplication::layoutType() {

    QString layout;
#ifdef BROWSER_LAYOUT_TENONE
    layout = "tenone";
#else
    layout = "default";
#endif

    return layout;
}

void GinebraApplication::addMenuBarAction(QObject *action) {
    QAction *a = dynamic_cast<QAction *>(action);
    if(a) {
        emit addMenuBarActionRequest(a);
    }
    else {
        qDebug() << "GinebraApplication::addMenuBarAction: invalid argument";
    }
}

void GinebraApplication::setMenuBarEnabled(bool value) {
    emit setMenuBarEnabledRequest(value);
}


}
