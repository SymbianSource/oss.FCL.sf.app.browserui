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


#include <QNetworkRequest>
#include <QNetworkReply>
#include <QAuthenticator>

#if QT_VERSION >= 0x040500
#include <QNetworkDiskCache>
#endif

//#include "wrtsettings.h"
#include "bedrockprovisioning.h"
#include "wrtbrowsercontainer.h"
#include "webcookiejar.h"
#include "webnetworkaccessmanager.h"

#include "WebDialogProvider.h"

#include "networkerrorreply.h"
#include "SchemeHandlerBr.h"
#include "qwebframe.h"

namespace WRT {

WebNetworkAccessManager::WebNetworkAccessManager(WrtBrowserContainer* container, QObject* parent) : QNetworkAccessManager(container), m_browserContainer(container)
{
    m_cookieJar = new CookieJar();
    m_reply = NULL;
    this->setCookieJar(m_cookieJar);
    connect(this, SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)), m_browserContainer, SLOT(slotAuthenticationRequired(QNetworkReply *, QAuthenticator *)));

    connect(this, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy & , QAuthenticator * )), m_browserContainer, SLOT(slotProxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)));
    setupCache();
    setupNetworkProxy();
}

#ifdef NETWORK_DEBUG
void WebNetworkAccessManager::error(QNetworkReply::NetworkError)
{
//    qDebug() << "Network::error:" << n_reply->error() << " String:" << n_reply->errorString();   
//    QString errorNumber = QString::number(n_reply->error() );
    QNetworkReply::NetworkError error = n_reply->error(); 
    		 
    if ( error != 5 && error != 0 )
    {      
    switch ( error ) 
    {     	
        case QNetworkReply::HostNotFoundError: 
        WebDialogProvider::showTimedMessage(NULL, "No Connection ", 2000);
        break; 
 /*    
 // ContentNotFoundError cause many problems. For now, we ignore 
 // TODO: We will find more graceful way to handle this error
        case QNetworkReply::ContentNotFoundError: 
        WebDialogProvider::showTimedMessage(NULL, "Content Not Found", 2000);
        break; 
 */   
        case QNetworkReply::ProtocolUnknownError: 
        WebDialogProvider::showTimedMessage(NULL, "Protocol Unknown", 2000);
        break; 
    
        default: 
    	  break;    	 // Ignore other error msgs 
    }
   }
   return; 
}
#endif 

WebNetworkAccessManager::~WebNetworkAccessManager()
{
    delete m_cookieJar;
    //setCache(NULL);
    delete m_reply;
}

QNetworkReply* WebNetworkAccessManager::createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
    QNetworkRequest req = request;
    
    req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

    if (m_reply != NULL) {
        delete m_reply;
        m_reply = NULL;
    }
    QNetworkReply* reply = NULL;


    if(m_browserContainer->mainFrame()) {
        if(m_browserContainer->mainFrame()->url().scheme().contains("https")) {

            if (op == QNetworkAccessManager::PostOperation && req.url().scheme().contains("http")) {

                m_text = tr("Secure Page Warning:");
                m_informativeText = tr("Do you want to continue?");
                m_detailedText = tr("Submit your data over an unencrypted connection.");
                m_buttons = QMessageBox::Yes | QMessageBox::No;
                m_defaultButton = QMessageBox::Yes;
                m_icon = QMessageBox::Warning;

                m_req = &req;

                emit showMessageBox(this);
                reply =  m_reply;
            }
        }
    }
    if (reply == NULL) {
		reply = createRequestHelper(op, req, outgoingData);
    }
#ifdef NETWORK_DEBUG	
    n_reply = reply; 
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
	    this, SLOT(error(QNetworkReply::NetworkError))); 
#endif 		
    return reply;
}

void WebNetworkAccessManager::onMessageBoxResponse(int retValue)
{

    if (retValue == QMessageBox::No) {
        m_reply = new NetworkErrorReply(QNetworkReply::ContentAccessDenied, "Not allowed", m_req->url());
        //a finished signal will be sent and the member is invoked as soon as the application enters the main event loop
        QMetaObject::invokeMethod(m_reply, "finished", Qt::QueuedConnection);
    }
}


QNetworkReply* WebNetworkAccessManager::createRequestHelper(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
    QNetworkRequest req = request;

    // Don't load the content if we are restoring the session 
    if (m_browserContainer->restoreSession()) {
        // handled in scheme handler - block network access
        QNetworkReply* reply = new NetworkErrorReply(QNetworkReply::OperationCanceledError, "Page Restored", req.url());
        QMetaObject::invokeMethod(reply, "finished", Qt::QueuedConnection);
        return reply;
    }
    
    if (m_browserContainer->schemeHandler()->HandleSpecialScheme(req.url())) {
        // handled in scheme handler - block network access
        QNetworkReply* reply = new NetworkErrorReply(QNetworkReply::OperationCanceledError, "Scheme Handled", req.url());
        QMetaObject::invokeMethod(reply, "finished", Qt::QueuedConnection);
        return reply;
    }

    //Accept-Language header
    QLocale language;
    QString langCountryCode = language.name();
    //QLocale::name returns the language and country as a  string of the form "language_country", where
    //language is a lowercase, two-letter ISO 639 language code, and country is an uppercase, two-letter 
    //ISO 3166 country code. But the format is expected to be "language-country". Note that hyphen is expected 
    //instead of underscore.
    langCountryCode.replace(QString("_"), QString("-"));
    req.setRawHeader("Accept-Language", langCountryCode.toUtf8());

    return QNetworkAccessManager::createRequest(op, req, outgoingData);
}

void WebNetworkAccessManager::setupNetworkProxy()
{
   QNetworkProxy proxy;
	 
   QString proxyString = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("NetworkProxy");
   
   if (proxyString.isEmpty())
   	{
      proxy.setType(QNetworkProxy::NoProxy);
      proxy.setHostName("");
      proxy.setPort(0);
   	}
   	else
		{
      proxy.setType(QNetworkProxy::HttpProxy);
      proxy.setHostName(proxyString);
      proxy.setPort(8080);
 		}

   	setProxy(proxy);
}
// Setup cache
// Need to use WrtSettingsUI to setup Disk Cache Directory Path
void WebNetworkAccessManager::setupCache()
{

#if QT_VERSION >= 0x040500
    qDiskCache = new QNetworkDiskCache(this);
    
    if ( !BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("DiskCacheEnabled").toBool() ) 
		return;

    QString diskCacheDir = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("DiskCacheDirectoryPath").toString();
    if(diskCacheDir.isEmpty()) return;
    // setup cache
    qDiskCache->setCacheDirectory(diskCacheDir);

    int cacheMaxSize = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("DiskCacheMaxSize").toInt();
    qDiskCache->setMaximumCacheSize(cacheMaxSize);

    setCache(qDiskCache);

#endif
}

}
