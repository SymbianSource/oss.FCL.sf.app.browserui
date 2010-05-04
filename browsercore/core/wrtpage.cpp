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


#include "wrtpage.h"
#include "qwebhistory.h"
#include "webpagedata.h"
//#include "wrtsettings.h"
#include "bedrockprovisioning.h"
#ifdef QT_MOBILITY_BEARER_MANAGEMENT
#include "WebNetworkConnectionManager.h"
#endif // QT_MOBILITY_BEARER_MANAGEMENT

#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QDir>

void QWEBKIT_EXPORT qt_webpage_setGroupName(QWebPage* page, const QString& groupName);
void QWEBKIT_EXPORT qt_websettings_setOfflineWebApplicationCachePath(const QString& path);

QDataStream &operator<<(QDataStream &out, const WebPageData &myObj)
{
   out << myObj.m_thumbnail << myObj.m_zoomFactor << myObj.m_contentsPos;
   return out;
}

QDataStream &operator>>(QDataStream &in, WebPageData &myObj)
{
   in >> myObj.m_thumbnail >> myObj.m_zoomFactor >> myObj.m_contentsPos;
   return in;
}

namespace WRT {

WrtPage::WrtPage(QObject *parent) : QWebPage(parent)
{
    settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, !BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("PopupBlocking").toInt());

    // Initialization below can only be done once per Webkit instance (and not once per page)
    // otherwise it hits an ASSERT in WebKit in debug mode
    static bool initialized = false;

    if (!initialized) {
        QString storagePath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);

        QWebSettings::setOfflineStoragePath(QDir::toNativeSeparators(QDir(storagePath).filePath("WebDatabase")));
#if defined CWRTINTERNALWEBKIT
        qt_websettings_setOfflineWebApplicationCachePath(QDir::toNativeSeparators(QDir(storagePath).filePath("ApplicationCache")));
#endif
        initialized = true;
    }
    
#ifdef QT_MOBILITY_BEARER_MANAGEMENT
    m_ConnectionManager = 0;
#endif // QT_MOBILITY_BEARER_MANAGEMENT
}

WrtPage::~WrtPage()
{
    QList<QWebHistoryItem> items = history()->items();

    for (int i = 0; i < history()->count(); i++) {
        QWebHistoryItem item = items.at(i);
        WebPageData* data = (WebPageData*)(item.userData().value<void*>());
        if(data){
            QVariant variant;
            item.setUserData(variant);
            delete data;
        }
    }
#ifdef QT_MOBILITY_BEARER_MANAGEMENT
    if (m_ConnectionManager)
        delete m_ConnectionManager;
#endif // QT_MOBILITY_BEARER_MANAGEMENT
}

void WrtPage::setPageGroupName(const QString& groupName)
{
    qt_webpage_setGroupName(this, groupName);
}

QWebPage* WrtPage::createWindow(QWebPage::WebWindowType webWindowType)
{
    WrtPage* wrtPage = new WrtPage();
    emit createWindow(wrtPage);
    return wrtPage;
}


/*!
// implementation of javaScriptConsoleMessage defined in WebKit qwebpage.h
// User settings are provided through WrtSettingsUI.
// The following choices are offered:
// Logging Off;Log to file;Show pop-up notes;Log to file and show pop-up notes
*/
void WrtPage::javaScriptConsoleMessage(const QString& messageSource, const QString& messageLevel, const QString& message, int lineNumber, const QString& sourceID)
{

    int jsSetting = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value ( "JavaScriptConsoleLog" ).toInt();
    QString logFileName = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value ( "JavaScriptConsoleLogFilePath" ).toString();
    QString msg = ( "JS Console Log: "+ ( QDate::currentDate() ).toString ( "dd.MM.yyyy" ) +" "+ ( QTime::currentTime() ).toString ( "hh:mm:ss ap" ) +"\nMessage src: "+messageSource+" : "+messageLevel+"\nMessage: " + message + "\nSource: "+sourceID+" : "+QString::number(lineNumber) );

    if ( jsSetting == 2 || jsSetting == 3 ){
      //display this simple dialog box for now.
      QMessageBox::about(view(), "JavaScript Console", msg);
    }
    if ( jsSetting == 1 || jsSetting == 3 ){
    // message is logged to file
        QFile jsFile ( logFileName );
        if ( !jsFile.open ( QIODevice::Append ) ) return;

        QTextStream jsLog ( &jsFile );
        jsLog << msg << "\n";
        jsFile.close();
    }
    // Emit message in case some view manager wants to implement a better GUI to display console messages
    emit jsConsoleMessage(messageSource, messageLevel, message, lineNumber, sourceID);

}

#ifdef QT_MOBILITY_BEARER_MANAGEMENT
void WrtPage::setNetworkConnectionManager(WebNetworkConnectionManager *manager)
{
	  if (manager == m_ConnectionManager)
	  	  return;
	  if (m_ConnectionManager && m_ConnectionManager->parent() == this)
	  	  delete m_ConnectionManager;
    m_ConnectionManager = manager;
}
#endif // QT_MOBILITY_BEARER_MANAGEMENT
}   // end of namespace WRT

