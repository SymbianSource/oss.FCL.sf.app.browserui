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

Q_SIGNALS:
	  void sessionConfigurationChanged(const QNetworkConfiguration &config);
	  void sessionStateChanged(const QNetworkConfiguration &config, 
	  	       QNetworkSession::State state);
        
private Q_SLOTS:
    void handlePreferredConfigurationChanged(const QNetworkConfiguration &config, bool isSeamless);
    void handleNewConfigurationActivated();
    void handleStateChanged(QNetworkSession::State state);
    void handleOpened();
    void handleClosed();
    void handleError(QNetworkSession::SessionError error);
   
private:
    QNetworkConfiguration activeConfiguration();
	
    QNetworkSession *m_NetworkSession;
};

#endif /* WEBNETWORKSESSION_H_ */
