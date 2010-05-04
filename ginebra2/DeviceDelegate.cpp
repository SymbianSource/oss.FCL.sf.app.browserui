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

#include <QList>
#include <QString>
#include "DeviceDelegate.h"
#include "Utilities.h"

namespace GVA {
//! DeviceDelegate default constructor.
DeviceDelegate::DeviceDelegate()
    : m_batteryCharging(false), m_updating(false)
{
    // this will be the name javascript uses to access properties and signals 
    // from this class
    setObjectName("deviceDelegate");
    
#ifdef QT_MOBILITY_BEARER_SYSINFO
    // create Qt Mobility API objects for device info
    m_deviceInfo = new QSystemDeviceInfo(this);
    m_networkInfo = new QSystemNetworkInfo(this);
    m_networkConfigManager = new QNetworkConfigurationManager(this);
    
    // Guess the network mode currently used for browser - 
    // a guess is really the best we can do right now.
    m_currentMode = getInternetConfigurationMode();
    
    // need the configuration manager to update its configuration list, when it
    // is done it will send us a signal
    safe_connect(m_networkConfigManager, SIGNAL(updateCompleted()), 
        this, SLOT(handleUpdateComplete()));

    // tell configuration manager to update its configuration list
    m_updating = true;
    m_networkConfigManager->updateConfigurations();
    
    // set up handlers for other network configuration manager signals
    safe_connect(m_networkConfigManager, SIGNAL(configurationAdded(const QNetworkConfiguration&)),
            this, SLOT(configurationAdded(const QNetworkConfiguration&)));
    safe_connect(m_networkConfigManager, SIGNAL(configurationRemoved(const QNetworkConfiguration&)),
            this, SLOT(configurationRemoved(const QNetworkConfiguration&)));
    safe_connect(m_networkConfigManager, SIGNAL(configurationChanged(const QNetworkConfiguration&)),
            this, SLOT(configurationChanged(const QNetworkConfiguration)));
    
    // When the m_deviceInfo signals battery level changed, 
    // DeviceDelegate emits batteryLevelChanged
    safe_connect(m_deviceInfo, SIGNAL(batteryLevelChanged(int)), 
        this, SIGNAL(batteryLevelChanged(int)));
    
    // set current charging state then keep it up to date with signal handler
    m_batteryCharging = (m_deviceInfo->currentPowerState() == 
        QSystemDeviceInfo::WallPowerChargingBattery) ? true : false;
    //qDebug() << "DeviceDelegate: charging state = " << m_deviceInfo->currentPowerState();
    //qDebug() << "DeviceDelegate: charging = " << m_batteryCharging;
    safe_connect(m_deviceInfo, SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)), 
        this, SLOT(handlePowerStateChanged(QSystemDeviceInfo::PowerState)));
    
    // set up handlers for system network info signals
    safe_connect(m_networkInfo, SIGNAL(networkSignalStrengthChanged(
        QSystemNetworkInfo::NetworkMode, int)), this, 
        SLOT(handleNetworkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int)));
    
    safe_connect(m_networkInfo, SIGNAL(networkNameChanged(
        QSystemNetworkInfo::NetworkMode, const QString&)), this, 
        SLOT(handleNetworkNameChanged(QSystemNetworkInfo::NetworkMode, const QString&)));
#endif // QT_MOBILITY_BEARER_SYSINFO
}

//! DeviceDelegate destructor.
DeviceDelegate::~DeviceDelegate()
{
    // clean up
#ifdef QT_MOBILITY_BEARER_SYSINFO
    delete m_deviceInfo;
    delete m_networkInfo;
    delete m_networkConfigManager;
#endif
}

#ifdef QT_MOBILITY_BEARER_SYSINFO
//! Converts bearer name string to network mode enumerator.
//! The network configuration uses strings to represent the same info that
//! the system network info uses an enumeration to represent
/*!
  \param name bearer name to convert
*/
QSystemNetworkInfo::NetworkMode DeviceDelegate::bearerNameToMode(QString name) const
{
    QSystemNetworkInfo::NetworkMode mode = QSystemNetworkInfo::UnknownMode;
    
    if (name == "Unknown")
        mode = QSystemNetworkInfo::UnknownMode;
    else if (name == "Ethernet")
        mode = QSystemNetworkInfo::EthernetMode;
    else if (name == "WLAN")
        mode = QSystemNetworkInfo::WlanMode;
    else if (name == "2G") {
        // there currently isn't a 2G in the enumeration but by trial and 
        // error I found that it corresponds to either GSM or WCDMA mode
        // You can tell which mode to choose be getting the signal strength;  
        // if you have the wrong mode it returns -1.
        if (m_networkInfo->networkSignalStrength(QSystemNetworkInfo::GsmMode) >= 0)
            mode = QSystemNetworkInfo::GsmMode; // T-Mobile uses this mode
        else
            mode = QSystemNetworkInfo::WcdmaMode; // AT&T uses this mode
        //qDebug() << "DeviceDelegate: 2G";
    } else if (name == "CDMA2000")
        mode = QSystemNetworkInfo::CdmaMode;
    else if (name == "WCDMA")
        mode = QSystemNetworkInfo::WcdmaMode;
    else if (name == "HSPA")
        // HSPA isn't currently in the enumeration
        mode = QSystemNetworkInfo::UnknownMode;
    else if (name == "Bluetooth")
        mode = QSystemNetworkInfo::BluetoothMode;
    else if (name == "WiMAX")
        mode = QSystemNetworkInfo::WimaxMode;
    
    //qDebug() << "DeviceDelegate: Mode " << mode;
    
    return (mode);
}
#endif // QT_MOBILITY_BEARER_SYSINFO

//! Gets the current battery level.
int DeviceDelegate::getBatteryLevel() const
{
#ifdef QT_MOBILITY_BEARER_SYSINFO
    return (m_deviceInfo->batteryLevel());
#else
    return (100); // can't get real level, return full
#endif
}

//! Returns true if the battery is charging.
bool DeviceDelegate::isBatteryCharging() const
{
    return (m_batteryCharging);
}

//! Gets the network signal strength for the current network mode.
int DeviceDelegate::getNetworkSignalStrength() const
{
#ifdef QT_MOBILITY_BEARER_SYSINFO
    int strength = m_networkInfo->networkSignalStrength(m_currentMode);
    
    // Strength in WLAN mode is reported as -1 by QtMobility
    if ((strength == -1) && (m_currentMode == QSystemNetworkInfo::WlanMode)) {
        strength = 100;
    }
    
    return (strength);
#else
    return (100); // can't get real level, return full
#endif
}

//! Gets the network name for the current network mode.
QString DeviceDelegate::getNetworkName() const
{
#ifdef QT_MOBILITY_BEARER_SYSINFO
    QString netName = m_networkInfo->networkName(m_currentMode);
    
    // if WLAN SSID name is unknown show "WiFi"
    if ((m_currentMode == QSystemNetworkInfo::WlanMode) &&
        (netName == "")) {
        netName = "WiFi";
    }
    
    //qDebug() << "DeviceDelegate: network name " << netName;
    return (netName);
#else
    return (""); // can't get real name
#endif
}

#ifdef QT_MOBILITY_BEARER_SYSINFO
//! Emits a signal for the specified signal strength.
/*!
  \param strength new signal strength
*/
void DeviceDelegate::updateSignalStrength(int strength)
{
    //qDebug() << "DeviceDelegate: Signal Strength " << strength;
    // currently getting a -1 signal strength for WLAN, change to 100.
    if (strength == -1) {
        if (m_currentMode == QSystemNetworkInfo::WlanMode) {
            //qDebug() << "DeviceDelegate: In WLAN mode so use strength 100.";
            strength = 100;
        } //else if (m_currentMode != QSystemNetworkInfo::UnknownMode) {
        //    qDebug() << "DeviceDelegate: Neg str in mode " << m_currentMode;
        //}
    }
    
    emit networkSignalStrengthChanged(strength);
}


//! Search for an active internet access point and return the network mode.
//! Returns the network mode used by the browser (best guess anyway)
//! My best guess is that the first active internet access point is the one 
//! used by the browser.
QSystemNetworkInfo::NetworkMode DeviceDelegate::getInternetConfigurationMode()
{
    // function return value
    QSystemNetworkInfo::NetworkMode mode = QSystemNetworkInfo::UnknownMode;
    QString bearerName; // network configuration bearer name
    bool found = false; // flag set when configuration found
    // active network configurations
    QList<QNetworkConfiguration> activeConfigurations = 
        m_networkConfigManager->allConfigurations(QNetworkConfiguration::Active);
    
    // check each active configuration until we find one that fits the bill
    while (!found && !activeConfigurations.isEmpty()) {
        QNetworkConfiguration config = activeConfigurations.takeFirst();
        
        //qDebug() << "DeviceDelegate: Have Active Config - type " << config.type();
        // service networks contain children configurations which need to be explored
        if (config.type() == QNetworkConfiguration::ServiceNetwork) {
            //qDebug() << "DeviceDelegate: Found a ServiceNetwork!";
            foreach (const QNetworkConfiguration &child, config.children()) {
                if ((child.type() == QNetworkConfiguration::InternetAccessPoint) &&
                    (child.state() == QNetworkConfiguration::Active)) {
                    found = true;
                    m_currentConfigIdentifier = child.identifier();
                    bearerName = child.bearerName();
                    //qDebug() << "   Found InternetAccessPoint - " << bearerName;
                    //qDebug() << "   identifier: " << child.identifier();
                    break; // exit foreach loop
                }
            }
        } else if (config.type() == QNetworkConfiguration::InternetAccessPoint) {
            found = true;
            m_currentConfigIdentifier = config.identifier();
            bearerName = config.bearerName();
            //qDebug() << "DeviceDelegate: Found an InternetAccessPoint - " << bearerName;
            //qDebug() << "   identifier: " << config.identifier();
        }
    }
    
    // get the mode of the found network configuration
    if (found) {
        //qDebug() << "DeviceDelegate::getInternetConfigurationMode: use bearer " << bearerName;
        mode = bearerNameToMode(bearerName);
    } else {
        //qDebug() << "DeviceDelegate: Failed to find an active internet access point.";
        m_currentConfigIdentifier = "";
    }
    
    return (mode);
}

//! Handles the updateCompleted signal from the configuration manager.
void DeviceDelegate::handleUpdateComplete()
{
    // search for appropriate network configuration mode again
    QSystemNetworkInfo::NetworkMode mode = getInternetConfigurationMode();
    
    // if the mode has changed we need to update the signal strength and network name
    if (mode != m_currentMode) {
        m_currentMode = mode;
        updateSignalStrength(m_networkInfo->networkSignalStrength(m_currentMode));
        emit networkNameChanged(getNetworkName());
    }
    
    m_updating = false;
}

//! Handles the configurationAdded signal from the configuration manager.
/*!
  \param config added network configuration
*/
void DeviceDelegate::configurationAdded(const QNetworkConfiguration &config)
{
    //qDebug() << "DeviceDelegate: Configuration Added.";
    if (!m_updating && (m_currentMode == QSystemNetworkInfo::UnknownMode) &&
        (config.state() == QNetworkConfiguration::Active) && 
        (config.type() == QNetworkConfiguration::InternetAccessPoint)) {
        // use this new configuration's mode, update the signal strength and network name
        //qDebug() << "DeviceDelegate: Use new configuration with bearer " << config.bearerName();
        m_currentConfigIdentifier = config.identifier();
        m_currentMode = bearerNameToMode(config.bearerName());
        updateSignalStrength(m_networkInfo->networkSignalStrength(m_currentMode));
        emit networkNameChanged(getNetworkName());
    }
}

//! Handles the configurationRemoved signal from the configuration manager.
/*!
  \param config removed network configuration
*/
void DeviceDelegate::configurationRemoved(const QNetworkConfiguration &config)
{
    //qDebug() << "DeviceDelegate: Configuration Removed.";
    if ((!m_updating) && (config.identifier() == m_currentConfigIdentifier)) {
        //qDebug() << "DeviceDelegate: Find new Configuration.";
        // search for appropriate network configuration mode again
        m_currentMode = getInternetConfigurationMode();
        updateSignalStrength(m_networkInfo->networkSignalStrength(m_currentMode));
        emit networkNameChanged(getNetworkName());
    }
}

//! Handles the configurationChanged signal from the configuration manager.
/*!
  \param config changed network configuration
*/
void DeviceDelegate::configurationChanged(const QNetworkConfiguration &config)
{
    //qDebug() << "DeviceDelegate: Configuration Changed.";
    if (!m_updating) {
        // if network mode currently unknown maybe this change will allow us to
        // identify the netork mode
        if (m_currentMode == QSystemNetworkInfo::UnknownMode) {
            // can we now identify the network mode?
            if ((config.state() == QNetworkConfiguration::Active) && 
                (config.type() == QNetworkConfiguration::InternetAccessPoint)) {
                //qDebug() << "DeviceDelegate: Use new configuration with bearer " << config.bearerName();
                m_currentConfigIdentifier = config.identifier();
                m_currentMode = bearerNameToMode(config.bearerName());
                updateSignalStrength(m_networkInfo->networkSignalStrength(m_currentMode));
                emit networkNameChanged(getNetworkName());
            }
        // Did the configuration currently being used change in a notable way?
        } else if ((config.identifier() == m_currentConfigIdentifier) && 
            ((config.state() != QNetworkConfiguration::Active) || 
             (config.type() != QNetworkConfiguration::InternetAccessPoint))) {
            //qDebug() << "DeviceDelegate: Change configuration.";
            // search for appropriate network configuration mode again
            m_currentMode = getInternetConfigurationMode();
            updateSignalStrength(m_networkInfo->networkSignalStrength(m_currentMode));
            emit networkNameChanged(getNetworkName());
        }
    }
}

//! Handles the networkSignalStrengthChanged signal from system network info.
/*!
  \param mode network mode of connection that changed
  \param strength new signal strength
*/
void DeviceDelegate::handleNetworkSignalStrengthChanged(
    QSystemNetworkInfo::NetworkMode mode, int strength)
{
    // Only send signal strength changes for current mode.
    if (mode == m_currentMode)
        updateSignalStrength(strength);
}

//! Handles the networkNameChanged signal from system network info.
/*!
  \param mode network mode of connection that changed
  \param name new network name
*/
void DeviceDelegate::handleNetworkNameChanged(
        QSystemNetworkInfo::NetworkMode mode, const QString& name)
{
    // Only send network name changes for current mode.
    if (mode == m_currentMode)
        emit networkNameChanged(name);
}

//! Handles the powerStateChanged signal from system device info.
/*!
  \param state new power state
*/
void DeviceDelegate::handlePowerStateChanged(QSystemDeviceInfo::PowerState state)
{
    bool batteryCharging = 
        (state == QSystemDeviceInfo::WallPowerChargingBattery) ? true : false;
    
    //qDebug() << "DeviceDelegate: new charging state = " << state;
    if (batteryCharging != m_batteryCharging) {
        m_batteryCharging = batteryCharging;
        //qDebug() << "DeviceDelegate: new charging = " << m_batteryCharging;
        // emit battery level - subscriber will get charging state if desired
        emit batteryLevelChanged(m_deviceInfo->batteryLevel());
    }
}

#endif // QT_MOBILITY_BEARER_SYSINFO

} // GVA

