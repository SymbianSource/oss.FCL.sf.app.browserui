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
