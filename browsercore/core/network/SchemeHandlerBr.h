/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
