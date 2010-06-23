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

#ifndef WEBNETWORKCONNECTIONMANAGER_H_
#define WEBNETWORKCONNECTIONMANAGER_H_
#include "brtglobal.h"
#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>
#ifdef QT_MOBILITY_SYSINFO
#include <qsysteminfo.h>
#endif // QT_MOBILITY_SYSINFO

QTM_USE_NAMESPACE

class WebNetworkSession;

namespace WRT {

class WRT_BROWSER_EXPORT WebNetworkConnectionManager : public QObject
{
    Q_OBJECT
    
public:
    WebNetworkConnectionManager(QObject *parent = 0);
    virtual ~WebNetworkConnectionManager();
    
    void updateConfigurations();
    QNetworkConfiguration defaultConfiguration() const;
    void createSession(QNetworkConfiguration config);
    void deleteSession();

#ifdef QT_MOBILITY_SYSINFO
Q_SIGNALS:
	  void networkModeChanged(QSystemNetworkInfo::NetworkMode mode);
	  void networkNameChanged(QSystemNetworkInfo::NetworkMode mode, const QString &netName);
	  void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode mode, int strength);
#endif // QT_MOBILITY_SYSINFO

private Q_SLOTS:
    void handleConfigurationUpdateCompleted();
    void handleConfigurationAdded(const QNetworkConfiguration& config);
    void handleConfigurationRemoved(const QNetworkConfiguration& config);
    void handleOnlineStateChanged(bool isOnline);
    void handleConfigurationChanged(const QNetworkConfiguration& config);
    void handleSessionConfigurationChanged(const QNetworkConfiguration &config);
    void handleSessionStateChanged(const QNetworkConfiguration &config, 
	  	       QNetworkSession::State state);
       
private:
#ifdef QT_MOBILITY_SYSINFO
	  void initializeMapString(void);
	  
	  QMap<QString, QSystemNetworkInfo::NetworkMode> m_mapStringNetworkMode; 
#endif // QT_MOBILITY_SYSINFO

    QNetworkConfigurationManager m_NetworkConfigurationManager;
    WebNetworkSession *m_WebNetworkSession;
};

}
#endif /* WEBNETWORKCONNECTIONMANAGER_H_ */
