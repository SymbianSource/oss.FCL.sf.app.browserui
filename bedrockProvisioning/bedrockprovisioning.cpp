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


#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QtCore/QStringList>
#include <QtGui/QMessageBox>
#include <qwebsettings.h>  
#include <QProcessEnvironment>
//#include "qtnresources.h"
//#include "qtnstrings.h"
//#include "wrtversions.h"
#include "bedrockprovisioning.h" 

#ifdef Q_OS_SYMBIAN
#include "f32file.h"
#endif

#define VALUEDELIM ";"
#define RANGEDELIM ":"

namespace BEDROCK_PROVISIONING {
	
BedrockProvisioningEnum::BedrockProvisioningEnum(const QString &key):
        m_key(key)
      , m_type(StringType)
      , m_flags(0)
      , m_enum(-1)
      , m_category(0)
      
{
}

QString BedrockProvisioningEnum::displayValue()
{
    // convert actual value to a string name from the list
    if (m_type == ListType && m_valuesList.count() == m_validList.count()){
        for (int i=0; i < m_validList.count(); i++)
            if (m_value.toString() == m_valuesList[i])
                return m_validList[i];
    }
    // don't display any value for commands
    else if (m_type == CommandType || m_type == NodeType)
        return QString();

    return m_value.toString();
}

QVariant BedrockProvisioningEnum::value()
{
    return m_value;
}

void BedrockProvisioningEnum::setValid(const QString &valid)
{
    if (valid.contains(VALUEDELIM))
        m_validList = valid.split(VALUEDELIM);
    else if (valid.contains(RANGEDELIM))
        m_validList = valid.split(RANGEDELIM);
}
void BedrockProvisioningEnum::setValues(const QString &values)
{
    m_valuesList = values.split(VALUEDELIM);
}
////////////////////////////////////////////////////	
	
	
BedrockProvisioning* BedrockProvisioning::m_BedrockProvisioning = 0;

BedrockProvisioning* BedrockProvisioning::createBedrockProvisioning()
{
    if (m_BedrockProvisioning == 0)
        m_BedrockProvisioning = new BedrockProvisioning();
    return m_BedrockProvisioning;
}

void BedrockProvisioning::resync() // DIMA
{
    QStringList list = allKeys();
    SettingsMap map;
    foreach (QString key, list)
        map.insert(key, value(key));

    sync();

    list = allKeys();
    foreach (QString key, list)
        if (value(key) != map.value(key))
            emit settingChanged(key);
}

BedrockProvisioning::BedrockProvisioning( QObject* parent, QString uid ) :
      QSettings(IniFormat, UserScope, BEDROCK_ORGANIZATION_NAME, uid, parent)
{
    setObjectName(QString::fromUtf8("BedrockProvisioning"));
    m_appuid = uid;
    init();
}

// Why the heck isn't this in a global .h file somewhere?
#define BEDROCK_APPLICATION_NAME "Bedrock"

void BedrockProvisioning::init(bool clear)
{
	qDebug() << "brp::init IN";
	// Unfortunately, this is getting called before WebPageController::initUASettingsAndData() which sets these
    QCoreApplication::setApplicationName(BEDROCK_APPLICATION_NAME);
    QCoreApplication::setOrganizationName(BEDROCK_ORGANIZATION_NAME);
    if (clear)
        QSettings::clear();
    
    QString key;
    beginGroup(m_appuid);
    {
        QSettings::setValue("Version", "BedrockInternal");  //Add more values separated by ';' here
        if (!QSettings::contains("BedrockVersion"))
            QSettings::setValue("BedrockVersion", QString("BetaRelease"));
            	
        if (!QSettings::contains("ZoomTextOnly"))
            QSettings::setValue("ZoomTextOnly", "0");
        //QSettings::setValue("ZoomTextOnly/descl", qtn_wrtsetting_zoomtextonly);
        QSettings::setValue("ZoomTextOnly/type", BedrockProvisioningEnum::ListType);
        QSettings::setValue("ZoomTextOnly/valid", "Yes;No");
        QSettings::setValue("ZoomTextOnly/values", "1;0");
        QSettings::setValue("ZoomTextOnly/flags", BedrockProvisioningEnum::WebAttribute);
        QSettings::setValue("ZoomTextOnly/enum", QWebSettings::ZoomTextOnly);
        QSettings::setValue("ZoomTextOnly/cat", BedrockProvisioningEnum::Category1);    	
        	
        if (!QSettings::contains("PopupBlocking"))
            QSettings::setValue("PopupBlocking", "0");
        //QSettings::setValue("PopupBlocking/descl", qtn_wrtsetting_popupblocking);
        QSettings::setValue("PopupBlocking/type", BedrockProvisioningEnum::ListType);
        QSettings::setValue("PopupBlocking/valid", "On;Off");
        QSettings::setValue("PopupBlocking/values", "1;0");
        QSettings::setValue("PopupBlocking/cat", BedrockProvisioningEnum::Category2);
        	   	
        if (!QSettings::contains("JavaScriptConsoleLog"))
            QSettings::setValue("JavaScriptConsoleLog", "0");
        //QSettings::setValue("JavaScriptConsoleLog/descl", qtn_wrtsetting_jsconsolelog);
        QSettings::setValue("JavaScriptConsoleLog/type", BedrockProvisioningEnum::ListType);
        QSettings::setValue("JavaScriptConsoleLog/valid", "Logging Off;Log to file;Show pop-up notes;Log to file and show pop-up notes");
        QSettings::setValue("JavaScriptConsoleLog/values", "0;1;2;3");
        QSettings::setValue("JavaScriptConsoleLog/cat", BedrockProvisioningEnum::Category4);

     	  if (!QSettings::contains("JavaScriptConsoleLogFilePath"))
            QSettings::setValue("JavaScriptConsoleLogFilePath", "jsLog_web.log");
        //QSettings::setValue("JavaScriptConsoleLogFilePath/descl", qtn_wrtsetting_jsconsolelogfilepath);
        QSettings::setValue("JavaScriptConsoleLogFilePath/cat", BedrockProvisioningEnum::Category4);
                	
        if (!QSettings::contains("ChromeBaseDirectory")) {
          #if defined Q_OS_SYMBIAN
            static const QString defaultChromeBaseDir = "/data/Others/chrome/";
          #elif defined Q_WS_MAEMO_5
            static const QString envChromeBaseDir = QProcessEnvironment::systemEnvironment().value("BROWSER_CHROME"); // allow env var overriding for dev
            static const QString defaultChromeBaseDir = (envChromeBaseDir != "") ? envChromeBaseDir : "/opt/browser/chrome/";
          #else /* Win or Linux */
            static const QString envChromeBaseDir = QProcessEnvironment::systemEnvironment().value("BROWSER_CHROME"); // allow env var overriding for dev
            static const QString defaultChromeBaseDir = (envChromeBaseDir != "") ? envChromeBaseDir : "chrome/";
          #endif
          
          QSettings::setValue("ChromeBaseDirectory", defaultChromeBaseDir); 
        }        	
        // This is for Ginebra2 but will go away once Ginebra1 is gone.
        if (!QSettings::contains("ChromeBaseDirectory2")) {
          #if defined Q_OS_SYMBIAN
            static const QString defaultChromeBaseDir = "/data/Others/ginebra2/chrome/";
          #elif defined Q_WS_MAEMO_5
            static const QString envChromeBaseDir = QProcessEnvironment::systemEnvironment().value("BROWSER_CHROME"); // allow env var overriding for dev
            static const QString defaultChromeBaseDir = (envChromeBaseDir != "") ? envChromeBaseDir : "/opt/browser/chrome/";
          #else /* Win or Linux */
            static const QString envChromeBaseDir = QProcessEnvironment::systemEnvironment().value("BROWSER_CHROME"); // allow env var overriding for dev
            static const QString defaultChromeBaseDir = (envChromeBaseDir != "") ? envChromeBaseDir : "chrome/";
          #endif
          
          QSettings::setValue("ChromeBaseDirectory2", defaultChromeBaseDir); 
        }        	
        if (!QSettings::contains("StartUpChrome")) {
          QSettings::setValue("StartUpChrome", "bedrockchrome/chrome.html"); 
        }        	

        if (!QSettings::contains("SplashImage")) {
          QSettings::setValue("SplashImage", "localpages/bedrock_splash.png"); 
        }        	
        	
        if (!QSettings::contains("NetworkProxy")) {
// For s60 arm and maemo arm (i.e. not x86 emulator build) we need to set no proxy
#if (defined(Q_OS_SYMBIAN)  && !defined(Q_CC_NOKIAX86)) || (defined(Q_WS_MAEMO_5) && !defined(QT_ARCH_I386))
          // empty proxy only for ARMV5 Symbian targets
	        QSettings::setValue("NetworkProxy", QString()); 
// everything else, linux, win, s60 emulator, maemo emulator needs proxy
#else
  	      QSettings::setValue("NetworkProxy", "bswebproxy01.americas.nokia.com");
#endif
        }
        //QSettings::setValue("NetworkProxy/descl", qtn_wrtsetting_networkproxy);
        QSettings::setValue("NetworkProxy/cat", BedrockProvisioningEnum::Category3);
        	
        if (!QSettings::contains("DeveloperExtras"))
            QSettings::setValue("DeveloperExtras", "0");
        //QSettings::setValue("DeveloperExtras/descl", qtn_wrtsetting_developerextrasenabled);
        QSettings::setValue("DeveloperExtras/type", BedrockProvisioningEnum::ListType);
        QSettings::setValue("DeveloperExtras/valid", "Enabled;Disabled");
        QSettings::setValue("DeveloperExtras/values", "1;0");
        QSettings::setValue("DeveloperExtras/flags", BedrockProvisioningEnum::WebAttribute);
        QSettings::setValue("DeveloperExtras/enum", QWebSettings::DeveloperExtrasEnabled);
        QSettings::setValue("DeveloperExtras/cat", BedrockProvisioningEnum::Category4);
        	
        if (!QSettings::contains("DiskCacheEnabled"))
            QSettings::setValue("DiskCacheEnabled", "1");

        if (!QSettings::contains("DiskCacheMaxSize"))
            QSettings::setValue("DiskCacheMaxSize", "4194304");

#ifdef Q_OS_SYMBIAN
        const QString diskCacheBaseDir = "d:/system/";
#else
#ifndef QT_NO_DESKTOPSERVICES
        const QString diskCacheBaseDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
        qDebug() << "dt diskCacheBaseDir = " << diskCacheBaseDir;
#else
        const QString diskCacheBaseDir = QDir::homePath();
        qDebug() << "hd diskCacheBaseDir = " << diskCacheBaseDir;
#endif
#endif

        if (!QSettings::contains("DiskCacheDirectoryPath")) {
        	qDebug() << "setting diskcachedirpath to " << diskCacheBaseDir + "brCache";
            QSettings::setValue("DiskCacheDirectoryPath", diskCacheBaseDir + "brCache");
        }

        if (!QSettings::contains("Cookies"))
            QSettings::setValue("Cookies", "1");
        //QSettings::setValue("Cookies/descl", qtn_wrtsetting_cookiesenabled);
        QSettings::setValue("Cookies/type", BedrockProvisioningEnum::ListType);
        QSettings::setValue("Cookies/valid", "Enabled;Disabled");
        QSettings::setValue("Cookies/values", "1;0");
        QSettings::setValue("Cookies/cat", BedrockProvisioningEnum::Category1);

        if (!QSettings::contains("StartPage"))
#ifdef PLAT_101
            QSettings::setValue("StartPage", "localpages/startpage_101.html");
#else
            QSettings::setValue("StartPage", "localpages/startpage.html");
#endif            
        QSettings::setValue("StartPage/cat", BedrockProvisioningEnum::Category1);

        if (!QSettings::contains("SaveSession"))
            QSettings::setValue("SaveSession", "1");
        QSettings::setValue("SaveSession/type", BedrockProvisioningEnum::ListType);
        QSettings::setValue("SaveSession/valid", "Enabled;Disabled");
        QSettings::setValue("SaveSession/values", "1;0");
        QSettings::setValue("SaveSession/cat", BedrockProvisioningEnum::Category1);
        
        qDebug() << "in group brp cwrtCache = " << this->valueAsString("DiskCacheDirectoryPath");
    	qDebug() << "brp::init endGroup " << m_appuid;
    }
    endGroup(); // m_appuid
    sync();
    qDebug() << "m_appuid = " << m_appuid;
    qDebug() << "orgname = " << this->organizationName();
    qDebug() << "appname = " << this->applicationName();
    qDebug() << "filename = " << this->fileName();
    qDebug() << "brp cwrtCache = " << this->valueAsString("DiskCacheDirectoryPath");
    qDebug() << "settings: " << this->allKeys().join("::");
	qDebug() << "brp::init OUT";
}

BedrockProvisioningEnum BedrockProvisioning::setting(const QString &key)
{
    BedrockProvisioningEnum setting(key);

    bool appMissing( QSettings::group().isEmpty() && !key.startsWith(m_appuid));
    if (appMissing)
        beginGroup(m_appuid);
    setting.m_value = QSettings::value(key);
    QSettings::beginGroup(key);
    QStringList childkeys = QSettings::allKeys();
    setting.m_group = QSettings::group();
    for (int i=0; i < childkeys.count(); i++){
        if (childkeys[i] == "type")
            setting.m_type = QSettings::value(childkeys[i]).toInt();
        else if (childkeys[i] == "desc")
                setting.m_desc = QSettings::value(childkeys[i]).toString();
        //else if (childkeys[i] == "descl")// localized
                //setting.m_desc = tr(setting_strings[QSettings::value(childkeys[i]).toInt()]);
        else if (childkeys[i] == "valid")
            setting.setValid(QSettings::value(childkeys[i]).toString());
        else if (childkeys[i] == "values")
            setting.setValues(QSettings::value(childkeys[i]).toString());
        else if (childkeys[i] == "flags")
            setting.setFlags(QSettings::value(childkeys[i]).toInt());
        else if (childkeys[i] == "enum")
            setting.m_enum = QSettings::value(childkeys[i]).toInt();
        else if (childkeys[i] == "cat")
            setting.m_category = QSettings::value(childkeys[i]).toInt();
        }
    endGroup();
    if (appMissing)
        endGroup();


    return setting;
}

QString BedrockProvisioning::valueAsString(const QString &key, const QVariant &defaultValue)
{
    return value(key, defaultValue).toString();
}

int BedrockProvisioning::valueAsInt(const QString &key, const QVariant &defaultValue)
{
    return value(key, defaultValue).toInt();
}

double BedrockProvisioning::valueAsDouble(const QString &key, const QVariant &defaultValue)
{
    return value(key, defaultValue).toDouble();
}

QVariant BedrockProvisioning::value(const QString &key, const QVariant &defaultValue)
{
    bool appMissing (!key.startsWith(m_appuid));
    if (appMissing)
        beginGroup(m_appuid);

    QVariant val = QSettings::value(key, defaultValue);
    
    if (appMissing)
        endGroup();
    
    return val;
}

int BedrockProvisioning::setValue(const QString &key, const int value)
{
    return setValue(key, QVariant(value));
}

int BedrockProvisioning::setValue(const QString &key, const double value)
{
    return setValue(key, QVariant(value));
}

int BedrockProvisioning::setValue(const QString &key, const QString &value)
{
    return setValue(key, QVariant(value));
}

int BedrockProvisioning::setValue(const QString &key, const QVariant &val)
{
    int ret(0);
    bool appMissing (!key.startsWith(m_appuid));
    if (appMissing)
        beginGroup(m_appuid);

    if (contains(key)){
        QSettings::setValue(key, val);
        sync();
        }
    else
        ret = BEDROCK_PROVISIONING_NOT_FOUND;// not found

    if (appMissing)
        endGroup();

    return ret;
}


void BedrockProvisioning::setFactorySettings()
{
    QSettings::clear();
    // TODO: add factory default settings
}

void BedrockProvisioning::addProvisioning(const QString &key, QSettings::SettingsMap& att, bool forceRefresh)
{
    if (forceRefresh || !QSettings::contains(key)) {
        QSettings::setValue(key, att);
        sync();
    }
}



} // end of namespace BEDROCK_PROVISIONING
