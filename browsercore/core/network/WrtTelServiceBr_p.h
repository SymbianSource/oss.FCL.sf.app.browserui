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
#ifndef __WRTTELSERVICE_P_H
#define __WRTTELSERVICE_P_H

#if defined(Q_OS_SYMBIAN)


#include "WrtTelServiceBr.h"
#include <QtCore/QObject>
#include <QString>

#include <ConeResLoader.h>

// AIW related headers
#include <AiwServiceHandler.h>

#if !defined (SYMBIAN_PUB_SDK)
#include <AiwDialData.h>
#include <AiwDialDataExt.h>
#endif

// FORWARD DECLARATIONS

// CLASS DECLARATION
  
class WrtTelServicePrivate : public QObject
    , public MAiwNotifyCallback
{
  
  public:
  
    WrtTelServicePrivate( );
    ~WrtTelServicePrivate();

    void MakeCall( QString number, bool sendDTMFAlwaysConfirm, bool voipCall );
    
    QString ValidateNumber( const QString number );

    QString FindPostdParm( QString& number );

    QString FindAndRipDTMFSequence( QString& number );

    QString FindAndRipURIParameters( QString& number );

    QString FindAndRipVoipParameters( QString& number );

    void ValidateDTMFNumber( QString& number );
    
  protected:
  
    /**
    * From MAiwNotifyCallback
    */
    TInt HandleNotifyL( TInt aCmdId,
                        TInt aEventId,
                        CAiwGenericParamList& aEventParamList,
                        const CAiwGenericParamList& aInParamList );

    
  private:
    void DoMakeCallL( QString number, bool sendDTMFAlwaysConfirm );
        
    HBufC* qt_QString2HBufCNewL(const QString& aString);

    
  private:  // Data
    bool m_CallInProgress;
    TFileName                   m_ResourceFileName; ///< Temp storage for the file name.
    RConeResourceLoader         m_ResourceLoader; ///< Resource loader.
    bool                        m_ResourceOpened; ///< Indicate if the resource is opened.
    // AIW related members
    CAiwServiceHandler* m_ServiceHandler;

#if !defined (SYMBIAN_PUB_SDK)
    // DialData.
    CAiwDialData*      m_DialData;
#endif

  };
  
#endif  

#endif // __WRTTELSERVICE_P_H
