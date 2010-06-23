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
#include "Application.h"

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

void GinebraApplication::debug(const QString &msg) {
      qDebug() << msg;
}

}
