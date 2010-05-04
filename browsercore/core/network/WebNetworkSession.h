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


#ifndef WEBNETWORKSESSION_H_
#define WEBNETWORKSESSION_H_
#include <qnetworksession.h>

QTM_USE_NAMESPACE

class WebNetworkSession : public QObject
{
Q_OBJECT
    
public: 
    WebNetworkSession(const QNetworkConfiguration &config, QObject *parent = 0);
    virtual ~WebNetworkSession();
        
private Q_SLOTS:
    void preferredConfigurationChanged(const QNetworkConfiguration &config, bool isSeamless);
    void newConfigurationActivated();
    void stateChanged(QNetworkSession::State state);
    void opened();
    void closed();
    void error(QNetworkSession::SessionError error);
   
private:
    QNetworkSession *m_NetworkSession;
};

#endif /* WEBNETWORKSESSION_H_ */
