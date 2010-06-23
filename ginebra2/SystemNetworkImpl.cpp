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
*  This file implements the SystemNetworkImpl class.
*/
#include <QList>
#include <QString>
#ifdef QT_MOBILITY_BEARER_MANAGEMENT
#include "WebNetworkConnectionManagerSingleton.h"
#endif // QT_MOBILITY_BEARER_MANAGEMENT
#include "NetworkDelegate.h"
#include "SystemNetworkImpl.h"
#include "Utilities.h"

namespace GVA {

SystemNetworkImpl::SystemNetworkImpl()
    : m_currentMode(QSystemNetworkInfo::UnknownMode)
{
    // create Qt Mobility API objects for network information
    m_networkInfo = new QSystemNetworkInfo(this);

    // set up handlers for system network info signals
    safe_connect(m_networkInfo, SIGNAL(networkModeChanged(
        QSystemNetworkInfo::NetworkMode)), this,
        SLOT(handleNetworkModeChanged(QSystemNetworkInfo::NetworkMode)));
    safe_connect(m_networkInfo, SIGNAL(networkNameChanged(
        QSystemNetworkInfo::NetworkMode, const QString&)), this,
        SLOT(handleNetworkNameChanged(QSystemNetworkInfo::NetworkMode, const QString&)));
    safe_connect(m_networkInfo, SIGNAL(networkSignalStrengthChanged(
        QSystemNetworkInfo::NetworkMode, int)), this,
        SLOT(handleNetworkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int)));
    safe_connect(m_networkInfo, SIGNAL(networkStatusChanged(
        QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus)), this,
        SLOT(handleNetworkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus)));

#ifdef QT_MOBILITY_BEARER_MANAGEMENT        	
    // Get the singleton instance of WebNetworkConnectionManager 
    WRT::WebNetworkConnectionManager &webNetworkConnectionManager 
    	  = WRT::WebNetworkConnectionManagerSingleton::Instance();
     	
    safe_connect(&webNetworkConnectionManager, SIGNAL(networkNameChanged(
        QSystemNetworkInfo::NetworkMode, const QString&)), this,
        SLOT(handleNetworkNameChanged(QSystemNetworkInfo::NetworkMode, const QString&)));
    safe_connect(&webNetworkConnectionManager, SIGNAL(networkSignalStrengthChanged(
        QSystemNetworkInfo::NetworkMode, int)), this,
        SLOT(handleNetworkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int)));

      // Update all configurations
    webNetworkConnectionManager.updateConfigurations();
#endif // QT_MOBILITY_BEARER_MANAGEMENT
}

SystemNetworkImpl::~SystemNetworkImpl()
{
    delete m_networkInfo;
}

//! Gets the network name for the current network mode.
QString SystemNetworkImpl::getNetworkName() const
{
    QString netName = m_networkInfo->networkName(m_currentMode);

    // if WLAN SSID name is unknown show "WiFi"
    if ((m_currentMode == QSystemNetworkInfo::WlanMode) &&
        (netName == "")) {
        netName = "WiFi";
    }

    //qDebug() << "DeviceDelegate: network name " << netName;
    return (netName);
}

//! Gets the network signal strength for the current network mode.
int SystemNetworkImpl::getNetworkSignalStrength() const
{
      int strength = m_networkInfo->networkSignalStrength(m_currentMode);

    // Strength in WLAN mode is reported as -1 by QtMobility
    if ((strength == -1) && (m_currentMode == QSystemNetworkInfo::WlanMode)) {
        strength = 100;
    }

    return (strength);
}

//! Handles the networkNetworkModeChanged signal from system network info.
/*!
  \param mode network mode of connection that changed
*/
void SystemNetworkImpl::handleNetworkModeChanged(
    QSystemNetworkInfo::NetworkMode mode)
{
      qDebug() << "handleNetworkModeChanged" << "Mode:" << mode;
      m_currentMode = mode;
}

//! Handles the networkSignalStrengthChanged signal from system network info.
/*!
  \param mode network mode of connection that changed
  \param strength new signal strength
*/
void SystemNetworkImpl::handleNetworkSignalStrengthChanged(
    QSystemNetworkInfo::NetworkMode mode, int strength)
{
    qDebug() << "handleNetworkSignalStrengthChanged" << "Mode:" << mode << "strength:" << strength;

    // Bootstrap the mode change if no networkModeChanged signal is recived
      if (m_currentMode == 0)
          m_currentMode = mode;

    // Only send signal strength changes for current mode.
    if (mode == m_currentMode)
        emit networkSignalStrengthChanged(strength);
}

//! Handles the networkNameChanged signal from system network info.
/*!
  \param mode network mode of connection that changed
  \param name new network name
*/
void SystemNetworkImpl::handleNetworkNameChanged(
        QSystemNetworkInfo::NetworkMode mode, const QString& name)
{
      qDebug() << "handleNetworkStatusChanged" << "Mode:" << mode << "name:" << name;

      // Bootstrap the mode change if no networkModeChanged signal is recived
      if (m_currentMode == 0)
          m_currentMode = mode;

    // Only send network name changes for current mode.
    if (mode == m_currentMode)
        emit networkNameChanged(name);
}

//! Handles the networkStatusChanged signal from system network info.
/*!
  \param mode network mode of connection that changed
  \param status network status of connection that changed
*/
void SystemNetworkImpl::handleNetworkStatusChanged(
        QSystemNetworkInfo::NetworkMode mode, QSystemNetworkInfo::NetworkStatus status)
{
      qDebug() << "handleNetworkSignalStrengthChanged" << "Mode:" << mode << "status:" << status;
}

} // GVA

