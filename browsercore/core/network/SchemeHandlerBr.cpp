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

// INCLUDE FILES
#include <QUrl>
#include <QDesktopServices>
#include <QStringList>

#if defined(Q_OS_SYMBIAN) 
#include "WrtTelServiceBr.h"
#include <miutset.h>                // KUidMsgTypeSMTP
#include <sendui.h>                 // CSendUi
#include <cmessagedata.h>           // CMessageData
//For Rtsp Scheme
#include <aiwgenericparam.h> 
#include <DocumentHandler.h>
#include <apparc.h>
#endif
#include "SchemeHandlerBr.h"
#include "bedrockprovisioning.h"

#define MAILTO_SCHEME "mailto"
#define TEL_SCHEME "tel"
#define SIP_SCHEME "sip"
#define WTAI_SCHEME "wtai"
#define WTAI_MC "/mc;"
#define WTAI_AP "/ap;"
#define RTSP_SCHEME "rtsp"

#if defined(Q_OS_SYMBIAN)
_LIT( KRtspFileName, "c:\\system\\temp\\RtspTemp.ram" );
#endif

namespace WRT {
  
/*!
 * \class SchemeHandler
 * \brief Schemehandler class
 */
// ---------------------------------------------------------
// SchemeHandler::SchemeHandler()
// ---------------------------------------------------------
//
SchemeHandler::SchemeHandler()
#if defined(Q_OS_SYMBIAN)
    :
     m_telService(NULL)
#endif
{
}

SchemeHandler::~SchemeHandler()
{
#if defined(Q_OS_SYMBIAN) 
    delete m_telService;
#endif
}

/*!
 * @param url :
 * @return 
 */
bool SchemeHandler::HandleSpecialScheme(const QUrl &url)
{
    return HandleUrlEmbedded( url, ReadSdConfirmDtmfValue() );
}


/*!
 *   @return  turn/false
 */
bool SchemeHandler::ReadSdConfirmDtmfValue()
{
    return BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("ConfirmDTMF",false).toBool();
}

/*!
 * HandleUrlEmbedded
 * @return true/false
 */
bool SchemeHandler::HandleUrlEmbedded( const QUrl &url, bool /*confirmDTMF*/ )
{
    QString path = url.path();
    QString scheme = url.scheme().toLower();

#if defined(Q_OS_SYMBIAN) 
    if (!m_telService)
        m_telService = new WrtTelService();
#endif

    if (scheme.startsWith("h") || scheme.startsWith("f"))
        // quickly exit if scheme is http, https, or file
        return false;

    else if (scheme.startsWith(MAILTO_SCHEME))
        return HandleMailtoScheme(url);

    else if (scheme.startsWith(RTSP_SCHEME))
        return HandleRtspScheme(url);

#if defined(Q_OS_SYMBIAN) 
    else if (scheme.startsWith(TEL_SCHEME)) {
        //m_telService->MakeCall(path, confirmDTMF);
        QDesktopServices::openUrl(url);
        return true;
    }
    else if (scheme.startsWith(SIP_SCHEME)) {
        //m_telService->MakeVOIPCall(path, confirmDTMF);
        QDesktopServices::openUrl(url);
        return true;
    }
    else if (scheme.startsWith(WTAI_SCHEME)) {
        if (path.startsWith(WTAI_MC)) {
            QString number = path.mid(path.indexOf(';') + 1);
            //m_telService->MakeCall(number, confirmDTMF);
            number = "tel:" + number;
            QUrl dialUrl(number);
            QDesktopServices::openUrl(dialUrl);
            return true;
        }
    }
#endif

    return false;
}


#if !defined(Q_OS_SYMBIAN) 
/*
 * HandleMailtoScheme
 * @return  true/false
 */
bool SchemeHandler::HandleMailtoScheme(const QUrl &url)
{
#ifndef QT_NO_DESKTOPSERVICES
    return QDesktopServices::openUrl(url);
#else
    return false;
#endif
}
#else // Q_OS_SYMBIAN
/*
 * HandleMailtoScheme
 * @return  true/false
 */

bool SchemeHandler::HandleMailtoScheme(const QUrl &url)
{
    TRAPD(err, HandleMailtoSchemeL(url));
    return err ? false : true;
}
void SchemeHandler::HandleMailtoSchemeL(const QUrl &url)
{
    QString recipient = url.path();
    QString subject = url.queryItemValue("subject");
    QString body = url.queryItemValue("body");
    QString to = url.queryItemValue("to");
    QString cc = url.queryItemValue("cc");
    QString bcc = url.queryItemValue("bcc");

    // these fields might have comma separated addresses
    QStringList recipients = recipient.split(",");
    QStringList tos = to.split(",");
    QStringList ccs = cc.split(",");
    QStringList bccs = bcc.split(",");


    CSendUi* sendUi = CSendUi::NewLC();

    // Construct symbian sendUI data holder
    CMessageData* messageData = CMessageData::NewLC();

    // Subject
    TPtrC subj( qt_QString2TPtrC(subject) );
    messageData->SetSubjectL( &subj );

    // Body
    CParaFormatLayer* paraFormat = CParaFormatLayer::NewL();
    CleanupStack::PushL( paraFormat );
    CCharFormatLayer* charFormat = CCharFormatLayer::NewL();
    CleanupStack::PushL( charFormat );
    CRichText* bodyRichText = CRichText::NewL( paraFormat, charFormat );
    CleanupStack::PushL( bodyRichText );

    TPtrC bodyPtr( qt_QString2TPtrC(body) );
    if( bodyPtr.Length() )
        bodyRichText->InsertL( 0, bodyPtr );
    else
        bodyRichText->InsertL( 0, KNullDesC );

    messageData->SetBodyTextL( bodyRichText );

    // To
    foreach(QString item, recipients)
        messageData->AppendToAddressL(qt_QString2TPtrC(item));

    foreach(QString item, tos)
        messageData->AppendToAddressL(qt_QString2TPtrC(item));

    // Cc
    foreach(QString item, ccs)
        messageData->AppendCcAddressL(qt_QString2TPtrC(item));

    // Bcc
    foreach(QString item, bccs)
        messageData->AppendBccAddressL(qt_QString2TPtrC(item));

    TRAP_IGNORE(sendUi->CreateAndSendMessageL( KUidMsgTypeSMTP, messageData ));
    CleanupStack::PopAndDestroy( 5 ); // bodyRichText, charFormat, paraFormat, messageData, sendUi
}
// Returned TPtrC is valid as long as the given parameter is valid and unmodified
TPtrC SchemeHandler::qt_QString2TPtrC( const QString& string )
{
    return reinterpret_cast<const TUint16*>(string.utf16());
}
#endif

#if !defined(Q_OS_SYMBIAN) 
/*
 * HandleRtspScheme
 * @return  true/false
 */
bool SchemeHandler::HandleRtspScheme(const QUrl &url)
{
#ifndef QT_NO_DESKTOPSERVICES
    return QDesktopServices::openUrl(url);
#else
    return false;
#endif
}
#else // Q_OS_SYMBIAN
/*
 * HandleRtspSchemeL
 * @return  true/false
 */

bool SchemeHandler::HandleRtspScheme(const QUrl &url)
{
    TRAPD(err, HandleRtspSchemeL(url));
    return (err ? false : true);
}
void SchemeHandler::HandleRtspSchemeL(const QUrl &url)
{
    //Launch the appropriate application in embedded mode 

    RFs rfs;
    RFile ramFile;

    HBufC* urlBuf = qt_QString2HBufC(url.toString());
    CleanupStack::PushL(urlBuf);
    // 8-bit buffer is required.
    HBufC8* urlBuf8 = HBufC8::NewLC( urlBuf->Size() );
    urlBuf8->Des().Copy( *urlBuf );
    
    // Open the file.
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);

    // Replace file if exists or Create file if not exist yet
    User::LeaveIfError( ramFile.Replace( rfs, KRtspFileName, EFileWrite | EFileShareAny ) );
    CleanupClosePushL(ramFile);
    // Write to file      
    User::LeaveIfError( ramFile.Write(*urlBuf8) );
    ramFile.Flush();

    CleanupStack::PopAndDestroy(/*ramFile*/);
    CleanupStack::PopAndDestroy(/*rfs*/);
    CleanupStack::PopAndDestroy( /*urlBuf8*/ );
    CleanupStack::PopAndDestroy( /*urlBuf*/ );
    
    RFile ramDocTempFile;
    CDocumentHandler* docHandler = CDocumentHandler::NewL();
    docHandler->OpenTempFileL( KRtspFileName, ramDocTempFile );

    CleanupClosePushL( ramDocTempFile );

    TDataType dataType( _L8("audio/x-pn-realaudio-plugin") );
    CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC();
    TAiwVariant filename( _L8("c:\\system\\temp\\RtspTemp.ram") );
    TAiwGenericParam param( EGenericParamFile, filename );
    
    paramList->AppendL( param );    

    // Allow save among Options

    TBool allowSave( ETrue );
    TAiwVariant allowSaveVariant( allowSave );
    TAiwGenericParam genericParamAllowSave
                     ( EGenericParamAllowSave, allowSaveVariant );

    paramList->AppendL( genericParamAllowSave );

    // launch RAM file via DocHandler

    docHandler->OpenFileEmbeddedL( ramDocTempFile, dataType, *paramList );

    CleanupStack::PopAndDestroy( paramList );
    CleanupStack::PopAndDestroy( &ramDocTempFile );
}

HBufC* SchemeHandler::qt_QString2HBufC(const QString& string) {
    TPtrC16 str(reinterpret_cast<const TUint16*>(string.utf16()));
    return str.Alloc();
}
#endif
}
