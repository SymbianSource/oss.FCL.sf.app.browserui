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

#ifndef DEVICEDELEGATE_H
#define DEVICEDELEGATE_H

#include <QObject>
#include <QString>
#ifdef QT_MOBILITY_BEARER_SYSINFO
#include "qsysteminfo.h"
#include "qnetworkconfiguration.h"
#include "qnetworkconfigmanager.h"

QTM_USE_NAMESPACE // using QtMobility namespace

#endif // QT_MOBILITY_BEARER_SYSINFO

namespace GVA {

/*!
  Class to provide device information. It uses QtMobility to provide 
  information about battery level, network signal strength, and network name.
*/
class DeviceDelegate : public QObject {
    Q_OBJECT
  public:
    // default constructor and destructor
    DeviceDelegate();
    ~DeviceDelegate();
    
    // properties accessible to javascript snippets
    Q_PROPERTY(int batteryLevel READ getBatteryLevel)
    Q_PROPERTY(int networkSignalStrength READ getNetworkSignalStrength)
    Q_PROPERTY(QString networkName READ getNetworkName)
    Q_PROPERTY(bool batteryCharging READ isBatteryCharging)
    
    
    // public methods
    int getBatteryLevel() const;
    int getNetworkSignalStrength() const;
    QString getNetworkName() const;
    bool isBatteryCharging() const;
    
  private:
    // private methods
#ifdef QT_MOBILITY_BEARER_SYSINFO
    void updateSignalStrength(int strength);
    QSystemNetworkInfo::NetworkMode bearerNameToMode(QString) const;
    QSystemNetworkInfo::NetworkMode getInternetConfigurationMode();
    
    // private member variables
    QSystemDeviceInfo *m_deviceInfo;
    QSystemNetworkInfo *m_networkInfo;
    QSystemNetworkInfo::NetworkMode m_currentMode;
    QNetworkConfigurationManager *m_networkConfigManager;
#endif
    QString m_currentConfigIdentifier;
    bool m_batteryCharging;
    bool m_updating; // updating network configurations
  
  signals:
    // Sent when the battery level or charging state changes.
    void batteryLevelChanged(int);
    // Sent when the network signal strength changes.
    void networkSignalStrengthChanged(int);
    // Sent when the network name changes.
    void networkNameChanged(const QString&);
  
  private slots:
#ifdef QT_MOBILITY_BEARER_SYSINFO
    // handles signals from network configuration manager
    void configurationAdded(const QNetworkConfiguration &config);
    void configurationRemoved(const QNetworkConfiguration &config);
    void configurationChanged(const QNetworkConfiguration &config);
    void handleUpdateComplete();

    // handles signals from system network info
    void handleNetworkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int);
    void handleNetworkNameChanged(QSystemNetworkInfo::NetworkMode, const QString&);
    
    // handles signals from system device info
    void handlePowerStateChanged(QSystemDeviceInfo::PowerState);
#endif
};

#endif // DEVICEDELEGATE_H

} // GVA
