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


#ifndef WEBNETWORKCONNECTIONMANAGER_H_
#define WEBNETWORKCONNECTIONMANAGER_H_

#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>
#include "WebNetworkSession.h"

QTM_USE_NAMESPACE

namespace WRT {

class WebNetworkConnectionManager : public QObject
{
    Q_OBJECT
    
public:
    WebNetworkConnectionManager(QObject *parent = 0);
    virtual ~WebNetworkConnectionManager();
    void updateConfigurations();
    QNetworkConfiguration defaultConfiguration() const;
    void createSession(QNetworkConfiguration config);
    void deleteSession();
    
private Q_SLOTS:
    void configurationUpdateCompleted();
    void configurationAdded(const QNetworkConfiguration& config);
    void configurationRemoved(const QNetworkConfiguration& config);
    void onlineStateChanged(bool isOnline);
    void configurationChanged(const QNetworkConfiguration& config);
       
private:
    QNetworkConfigurationManager m_NetworkConfigurationManager;
    WebNetworkSession *m_WebNetworkSession;
};
}

#endif /* WEBNETWORKCONNECTIONMANAGER_H_ */
