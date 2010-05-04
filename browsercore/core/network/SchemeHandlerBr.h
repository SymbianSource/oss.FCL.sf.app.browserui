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


#ifndef SCHEME_HANDLER_H
#define SCHEME_HANDLER_H

// INCLUDES

#include <QtCore/QObject>
#if defined(Q_OS_SYMBIAN)
#include <txtrich.h>                // CRichText
#endif

// CLASS DECLARATION

class QUrl;
#if defined(Q_OS_SYMBIAN)
class WrtTelService;
#endif


namespace WRT {


/**
* Scheme Handler IF definition class
*/
class SchemeHandler : public QObject {

Q_OBJECT

public:
    SchemeHandler();
    ~SchemeHandler();
    
    bool HandleSpecialScheme(const QUrl &url);

private:
    bool ReadSdConfirmDtmfValue();
    bool HandleUrlEmbedded( const QUrl &url, bool confirmDTMF );
    
    bool HandleMailtoScheme(const QUrl &url);
    bool HandleRtspScheme(const QUrl &url);
#if defined(Q_OS_SYMBIAN)
    void HandleRtspSchemeL(const QUrl &url);
    void HandleMailtoSchemeL(const QUrl &url);
    TPtrC qt_QString2TPtrC( const QString& string );
    HBufC* qt_QString2HBufC(const QString& string);
#endif
    
private:
#if defined(Q_OS_SYMBIAN)
    WrtTelService* m_telService; ///< Owned.
#endif
};
}
#endif /* def SCHEME_HANDLER_H */
