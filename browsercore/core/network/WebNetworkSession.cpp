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


#include "WebNetworkSession.h"

WebNetworkSession::WebNetworkSession(const QNetworkConfiguration &config, QObject *parent)
    : QObject(parent)
{   
    m_NetworkSession = new QNetworkSession(config);
    
    m_NetworkSession->open();
    
    connect(m_NetworkSession, SIGNAL(preferredConfigurationChanged(const QNetworkConfiguration&, bool)),
            this, SLOT(preferredConfigurationChanged(const QNetworkConfiguration&, bool)));   
    connect(m_NetworkSession, SIGNAL(newConfigurationActivated()), this, SLOT(newConfigurationActivated()));
    connect(m_NetworkSession, SIGNAL(stateChanged(QNetworkSession::State)),
            this, SLOT(stateChanged(QNetworkSession::State)));
    connect(m_NetworkSession, SIGNAL(opened()), this, SLOT(opened()));
    connect(m_NetworkSession, SIGNAL(closed()), this, SLOT(closed()));
    connect(m_NetworkSession, SIGNAL(error(QNetworkSession::SessionError)), 
            this, SLOT(error(QNetworkSession::SessionError)));
}

WebNetworkSession::~WebNetworkSession()
{
	  // Close the network connection session before delete.
	  m_NetworkSession->close();
    delete m_NetworkSession;
}

void WebNetworkSession::preferredConfigurationChanged(const QNetworkConfiguration &config, bool isSeamless)
{
    bool isSelected = TRUE;
    
    if (isSeamless)
    {
        m_NetworkSession->migrate();
        qDebug() << "Migrate to new Network Connection: " << config.name(); 
    }
    else
    {
        // Dialog Box to select
        if (isSelected)
        {
            m_NetworkSession->migrate();
            qDebug() << "Migrate to new Network Connection: " << config.name(); 
        }
        else
        {
            m_NetworkSession->ignore();
            qDebug() << "Ignore new Network Connection: " << config.name();
        }
    }
}

void WebNetworkSession::newConfigurationActivated()
{
    bool isConnected = TRUE;
    
    // isConnectionGood = testConnection();
    if (isConnected)
    {
        m_NetworkSession->accept();
        qDebug() << "Accept new Network Connection";
        // flash the new connection network
    }
    else
    {
        m_NetworkSession->reject();
        // flash the old connection network
        qDebug() << "Reject new Network Connection";
    }
}

void WebNetworkSession::stateChanged(QNetworkSession::State state)
{
    switch (state) {
        case QNetworkSession::Invalid:
            qDebug() << "Invalid";
            break;
        case QNetworkSession::NotAvailable:
            qDebug() << "Not Available";
            break;
        case QNetworkSession::Connecting:
            qDebug() << "Connecting";
            break;
        case QNetworkSession::Connected:
            qDebug() << "Connected";
            break;
        case QNetworkSession::Closing:
            qDebug() << "Closing";
            break;
        case QNetworkSession::Disconnected:
            qDebug() << "Disconnected";
            break;
        case QNetworkSession::Roaming:
            qDebug() << "Roaming";
            break;
        default:
            qDebug() << "Unknown";
    }  
}

void WebNetworkSession::opened()
{
    qDebug() << "Session Opened";
}

void WebNetworkSession::closed()
{
    qDebug() << "Session Closed";
}

void WebNetworkSession::error(QNetworkSession::SessionError error)
{
    switch (error)
    {
        case QNetworkSession::UnknownSessionError:
            qDebug() << "UnknownSessionError";
            break;
        case QNetworkSession::SessionAbortedError:
            qDebug() << "SessionAbortedError";
            break;
        case QNetworkSession::RoamingError:
            qDebug() << "RoamingError";
            break;
        case QNetworkSession::OperationNotSupportedError:
            qDebug() << "OperationNotSupportedError";
            break;
        case QNetworkSession::InvalidConfigurationError:
            qDebug() << "InvalidConfigurationError";
            break;
        default:
        	  qDebug() << "Unknown Error";
    }
}
