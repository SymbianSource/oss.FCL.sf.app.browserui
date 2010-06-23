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
#include "WebNetworkConnectionManager.h"
#include "WebNetworkSession.h"

namespace WRT {

/*!
    Constructs a WebNetworkConfigurationManager with the given \a parent.
*/
WebNetworkConnectionManager::WebNetworkConnectionManager(QObject *parent)
    : QObject(parent), m_WebNetworkSession(0)
{ 
    // set up handlers for Network Configuration Manager signals
    connect(&m_NetworkConfigurationManager, SIGNAL(updateCompleted()), 
            this, SLOT(handleConfigurationUpdateCompleted()));
    connect(&m_NetworkConfigurationManager, SIGNAL(configurationAdded(const QNetworkConfiguration&)),
            this, SLOT(handleConfigurationAdded(const QNetworkConfiguration&)));
    connect(&m_NetworkConfigurationManager, SIGNAL(configurationRemoved(const QNetworkConfiguration&)),
            this, SLOT(handleConfigurationRemoved(const QNetworkConfiguration&)));
    connect(&m_NetworkConfigurationManager, SIGNAL(onlineStateChanged(bool)), 
            this, SLOT(handleOnlineStateChanged(bool)));
    connect(&m_NetworkConfigurationManager, SIGNAL(configurationChanged(const QNetworkConfiguration&)),
            this, SLOT(handleConfigurationChanged(const QNetworkConfiguration&)));

#ifdef QT_MOBILITY_SYSINFO 
    // initialize the mapping between network mode string and SystemNetworkInfo::NetworkMode
    initializeMapString();
#endif // QT_MOBILITY_SYSINFO
}

/*!
    Frees the resources associated with the WebNetworkConfigurationManager object.
*/
WebNetworkConnectionManager::~WebNetworkConnectionManager()
{
    if (m_WebNetworkSession)
        deleteSession();
}

/*!
    Initiates an update of all configurations. This may be used to initiate WLAN scans or other
    time consuming updates which may be required to obtain the correct state for configurations.

    This call is asynchronous. On completion of this update the updateCompleted() signal is
    emitted. If new configurations are discovered or old ones were removed or changed the update
    process may trigger the emission of one or multiple configurationAdded(),
    configurationRemoved() and configurationChanged() signals.

    If a configuration state changes as a result of this update all existing QNetworkConfiguration
    instances are updated automatically.
*/
void WebNetworkConnectionManager::updateConfigurations()
{
    m_NetworkConfigurationManager.updateConfigurations();
}

/*!
    Returns the default configuration to be used. This function always returns a discovered
    configuration; otherwise an invalid configuration. Only configuration type SNAP is handled.

    In some cases it may be required to call updateConfigurations() and wait for the
    updateCompleted() signal before calling this function.
*/
QNetworkConfiguration WebNetworkConnectionManager::defaultConfiguration() const
{
    const bool canStartIAP = (m_NetworkConfigurationManager.capabilities() 
                              & QNetworkConfigurationManager::CanStartAndStopInterfaces);
    QNetworkConfiguration config = m_NetworkConfigurationManager.defaultConfiguration();
    if (!config.isValid() || !canStartIAP)
        return config;
        
    switch(config.type()) {
        case QNetworkConfiguration::InternetAccessPoint:
            // no user interaction -> system starts IAP immediately
            qDebug() << "IAP";
            break;
        case QNetworkConfiguration::ServiceNetwork:
            // no user interaction -> system determines best IAP in group and starts it
            qDebug() << "SNAP";
            break;
        case QNetworkConfiguration::UserChoice:
            // IAP resolved by asking user as part of QNetworkSession::open()
            qDebug() << "User Choice";
            break;
    }
    
    return config;
}

/*! 
    Create a Web Network Session using a QNetworkConfiguration and connect to its signals.
    
*/
void WebNetworkConnectionManager::createSession(QNetworkConfiguration config)
{   
    m_WebNetworkSession = new WebNetworkSession(config);

    // set up handlers for the WebNetworkSession signals
    connect(m_WebNetworkSession, SIGNAL(sessionConfigurationChanged(const QNetworkConfiguration &)),
            this, SLOT(handleSessionConfigurationChanged(const QNetworkConfiguration &)));
    connect(m_WebNetworkSession, SIGNAL(sessionStateChanged(const QNetworkConfiguration &, QNetworkSession::State)),
            this, SLOT(handleSessionStateChanged(const QNetworkConfiguration &, QNetworkSession::State))); 
}

/*! 
    Delete a Web Network Session.
    
*/
void WebNetworkConnectionManager::deleteSession(void)
{   
    delete m_WebNetworkSession;
}

/*!
    Handle the updateCompleted signal from Network Configuration Manager.
    
    It creates a Web Network Session using the default configuration.
*/
void WebNetworkConnectionManager::handleConfigurationUpdateCompleted()
{
	  qDebug() << "configurationUpdateCompleted: create new network connection session";
	  if (m_WebNetworkSession)
	  {
	  	  qDebug() << "Delete old network connection session";
	  	  deleteSession();
	  }
	  
    createSession(defaultConfiguration());
}

/*! 
    Handle the configurationAdded signal from Network Configuration Manager.
*/
void WebNetworkConnectionManager::handleConfigurationAdded(const QNetworkConfiguration& config)
{
    qDebug() << "Configuration" << config.name() << "Added";    
}

/*! 
    Handle the configurationRemoved signal from Network Configuration Manager
*/
void WebNetworkConnectionManager::handleConfigurationRemoved(const QNetworkConfiguration& config)
{
    qDebug() << "Configuration" << config.name() << "Removed";
}

/*! 
    Handle the conlineStateChanged signal from Network Configuration Manager.
*/
void WebNetworkConnectionManager::handleOnlineStateChanged(bool isOnline)
{
    if (!isOnline)
    {
        qDebug() << "offline";
    }
    else
    {
        qDebug() << "online";
    }
    // flash icon to indicate the online state change with "online" and "offline".
}

/*! 
    Handle the configurationChanged signal from Network Configuration Manager.
*/
void WebNetworkConnectionManager::handleConfigurationChanged(const QNetworkConfiguration &config)
{
    qDebug() << "Configuration" << config.name() << "Changed";  
}

/*! 
    Handle the networkNameChanged signal from Network Configuration Manager and translate 
    sessionConfiguration signal to networknameChanged.
    
    It a\ emits networkNameChanged signal for no cellular network connetion.
*/
void WebNetworkConnectionManager::handleSessionConfigurationChanged(const QNetworkConfiguration &config)
{  
    qDebug() << "handleSessionConfigurationChanged" << "bearname:" << config.bearerName();

#ifdef QT_MOBILITY_SYSINFO  
    QSystemNetworkInfo::NetworkMode mode;
    mode = m_mapStringNetworkMode[config.bearerName()];
    switch(mode)
    {
    	  case QSystemNetworkInfo::EthernetMode:
    	  case QSystemNetworkInfo::WlanMode:
    	  case QSystemNetworkInfo::BluetoothMode:
    	  case QSystemNetworkInfo::WimaxMode:
            emit networkNameChanged(mode, config.name());
            break;
        case QSystemNetworkInfo::GsmMode:
        case QSystemNetworkInfo::CdmaMode:
        case QSystemNetworkInfo::WcdmaMode:
        	  break;
        case QSystemNetworkInfo::UnknownMode:
        default:
        	  emit networkNameChanged(mode, config.name());
        	  break;
    }
#endif // QT_MOBILITY_SYSINFO
}

/*! 
    Handle the networkSignalStrengthChanged from Network Configuration Manager and
    translate sessionStateChanged to networkSignalStrengthChanged.
    
    It a\ emits networkSignalStrengthChanged for non cellular network connection.
*/
void WebNetworkConnectionManager::handleSessionStateChanged(const QNetworkConfiguration &config, 
	       QNetworkSession::State state)
{
	  int strength = 0;
	  
	  qDebug() << "handleSessionStateChanged" << "bearname:" << config.bearerName();
	  
	  switch(state)
	  {
	      case QNetworkSession::Connecting:
	      case QNetworkSession::Connected:
	      case QNetworkSession::Roaming:
	      	  strength = 100;
	      	  break;
	      default:
	       	  break;
	  }

#ifdef QT_MOBILITY_SYSINFO
    QSystemNetworkInfo::NetworkMode mode;
	  mode = m_mapStringNetworkMode[config.bearerName()];
    switch(mode)
    {
    	  case QSystemNetworkInfo::EthernetMode:
    	  case QSystemNetworkInfo::WlanMode:
        case QSystemNetworkInfo::BluetoothMode:
    	  case QSystemNetworkInfo::WimaxMode:
    	  	  emit networkSignalStrengthChanged(mode, strength);
            break;
        case QSystemNetworkInfo::GsmMode:
        case QSystemNetworkInfo::CdmaMode:
        case QSystemNetworkInfo::WcdmaMode:
        	  break;
        case QSystemNetworkInfo::UnknownMode:
        default:
        	  emit networkSignalStrengthChanged(mode, strength);
        	  break;
    }
#endif // QT_MOBILITY_SYSINFO
}

#ifdef QT_MOBILITY_SYSINFO
void WebNetworkConnectionManager::initializeMapString(void)
{
	  m_mapStringNetworkMode["Ethernet"] = QSystemNetworkInfo::EthernetMode;
	  m_mapStringNetworkMode["WLAN"] = QSystemNetworkInfo::WlanMode;
	  m_mapStringNetworkMode["2G"] = QSystemNetworkInfo::GsmMode;
	  m_mapStringNetworkMode["CDMA2000"] = QSystemNetworkInfo::CdmaMode;
	  m_mapStringNetworkMode["WCDMA"] = QSystemNetworkInfo::WcdmaMode;
	  m_mapStringNetworkMode["HSPA"] = QSystemNetworkInfo::UnknownMode;
	  m_mapStringNetworkMode["Bluetooth"] = QSystemNetworkInfo::BluetoothMode;
	  m_mapStringNetworkMode["WiMAX"] = QSystemNetworkInfo::WimaxMode;
	  m_mapStringNetworkMode[""] = QSystemNetworkInfo::UnknownMode;
}
#endif // QT_MOBILITY_SYSINFO

} // WRT
