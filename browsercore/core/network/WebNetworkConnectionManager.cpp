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


#include "WebNetworkConnectionManager.h"

namespace WRT {

WebNetworkConnectionManager::WebNetworkConnectionManager(QObject *parent)
    : QObject(parent), m_WebNetworkSession(0)
{ 
    connect(&m_NetworkConfigurationManager, SIGNAL(updateCompleted()), 
            this, SLOT(configurationUpdateCompleted()));
    connect(&m_NetworkConfigurationManager, SIGNAL(configurationAdded(const QNetworkConfiguration&)),
            this, SLOT(configurationAdded(const QNetworkConfiguration&)));
    connect(&m_NetworkConfigurationManager, SIGNAL(configurationRemoved(const QNetworkConfiguration&)),
            this, SLOT(configurationRemoved(const QNetworkConfiguration&)));
    connect(&m_NetworkConfigurationManager, SIGNAL(onlineStateChanged(bool)), 
            this, SLOT(onlineStateChanged(bool)));
    connect(&m_NetworkConfigurationManager, SIGNAL(configurationChanged(const QNetworkConfiguration&)),
            this, SLOT(configurationChanged(const QNetworkConfiguration&)));
    	      
    updateConfigurations();
}

WebNetworkConnectionManager::~WebNetworkConnectionManager()
{
    if (m_WebNetworkSession)
        deleteSession();
}

void WebNetworkConnectionManager::updateConfigurations()
{
    m_NetworkConfigurationManager.updateConfigurations();
}

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

void WebNetworkConnectionManager::createSession(QNetworkConfiguration config)
{   
    m_WebNetworkSession = new WebNetworkSession(config);
}

void WebNetworkConnectionManager::deleteSession(void)
{   
    delete m_WebNetworkSession;
}

void WebNetworkConnectionManager::configurationUpdateCompleted()
{
	  qDebug() << "configurationUpdateCompleted: create new network connection session";
	  if (m_WebNetworkSession)
	  {
	  	  qDebug() << "Delete old network connection session";
	  	  deleteSession();
	  }
	  
    createSession(defaultConfiguration());
}

void WebNetworkConnectionManager::configurationAdded(const QNetworkConfiguration& config)
{
    qDebug() << "Configuration" << config.name() << "Added";    
}

void WebNetworkConnectionManager::configurationRemoved(const QNetworkConfiguration& config)
{
    qDebug() << "Configuration" << config.name() << "Removed";
}

void WebNetworkConnectionManager::onlineStateChanged(bool isOnline)
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

void WebNetworkConnectionManager::configurationChanged(const QNetworkConfiguration &config)
{
    qDebug() << "Configuration" << config.name() << "Changed";  
}
}
