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
#include <QAction>

#ifdef ORBIT_UI
#define UI_FRAMEWORK "orbit_ui"
#elif defined(Q_WS_MAEMO_5)
#define UI_FRAMEWORK "maemo5_ui"
#else
#define UI_FRAMEWORK ""
#endif // ORBIT_UI

#ifdef QT_MOBILITY_SERVICE_FRAMEWORK
#define MOBILITY_SERVICE_FRAMEWORK "mobility_service_framework"
#else
#define MOBILITY_SERVICE_FRAMEWORK "junk"
#endif // QT_MOBILITY_SERVICE_FRAMEWORK

#ifdef QT_GEOLOCATION
#define GEOLOCATION true
#else
#define GEOLOCATION false
#endif

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
    QString layoutType() ;
    QString serviceFramework() const { return MOBILITY_SERVICE_FRAMEWORK; }
    bool geolocation() const { return GEOLOCATION; }
    void sendToBackground();
    
    /// Create a QAction object
    QObject *createAction(const QString &text = QString::null, const QString &iconPath = QString::null, bool checkable = false);
    void addMenuBarAction(QObject *action);
    void setMenuBarEnabled(bool value);
    
    /*
     * You can use this to breakpoint inside your javascript.  Here's how:
     * 1) put a breakpoint on this function
     * 2) to your javascript, add: app.breakpoint();
     * 3) when the breakpoint hits, you will be in the C++ stack at the point of that JS execution
     * It doesn't really help you step through javascript (unless you know the javascript core well), but helps you debug C++ and javascript interaction
     */
    void breakpoint() {}
  signals:
    void aboutToQuit();
    
    /// Sent when an action needs to be added to the platform menu bar.
    void addMenuBarActionRequest(QAction *action);
    
    /// Sent when the platform menu bar's menu should be disabled.
    void setMenuBarEnabledRequest(bool value);
    
  private:
    QCoreApplication * m_app;
};

}
#endif
