/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#ifndef __WRTTELSERVICE_H
#define __WRTTELSERVICE_H

#include <QtCore/QObject>
#include <QString>

  
// FORWARD DECLARATIONS
class WrtTelServicePrivate;


// CLASS DECLARATION

/**
*  Implements telephony services for browser.
*  This class implements required telephony services for multipurpose browser.
*/
class WrtTelService : public QObject {
  
	  Q_OBJECT
	  
public:
	
	  WrtTelService();
		~WrtTelService();

		void MakeCall( QString number, bool sendDTMFAlwaysConfirm );

		void MakeVOIPCall( QString number, bool sendDTMFAlwaysConfirm );
		
private:
    WrtTelServicePrivate* d;
	};

#endif // __WRTTELSERVICE_H
