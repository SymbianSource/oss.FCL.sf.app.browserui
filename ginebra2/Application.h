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
#ifndef __GINEBRA_APPLICATION_H__
#define __GINEBRA_APPLICATION_H__

#include <QObject>

#ifdef ORBIT_UI
#define UI_FRAMEWORK "orbit_ui"
#else
#define UI_FRAMEWORK ""
#endif // ORBIT_UI

class QCoreApplication;

namespace GVA {

/*! \ingroup JavascriptAPI
 * \brief This class owns the QCoreApplication object.
 *
 * Example javascript code print a string to the debug output:
 * \code
 * app.debug("hello");
 * \endcode
 */
class GinebraApplication : public QObject
{
  Q_OBJECT
  public:
    GinebraApplication();
    ~GinebraApplication();
  public slots:
    void quit();
    void debug(const QString &msg);
    QString ui() const { return UI_FRAMEWORK; }
    void sendToBackground();
  signals:
    void aboutToQuit();
  private:
    QCoreApplication * m_app;
};

}
#endif
