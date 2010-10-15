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

#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QProcessEnvironment>
#include <QWebSettings>
#include "bedrockprovisioning.h"

namespace BEDROCK_PROVISIONING {

BedrockProvisioning* BedrockProvisioning::m_BedrockProvisioning = 0;

BedrockProvisioning* BedrockProvisioning::createBedrockProvisioning()
{
    if (m_BedrockProvisioning == 0)
        m_BedrockProvisioning = new BedrockProvisioning();
    return m_BedrockProvisioning;
}

BedrockProvisioning::BedrockProvisioning( QObject* parent, QString uid ) :
      QSettings(IniFormat, UserScope, BEDROCK_ORGANIZATION_NAME, uid, parent)
{
    
    setObjectName(QString::fromUtf8("BedrockProvisioning"));
    m_appuid = uid;
    init();
}

void BedrockProvisioning::init()
{
    QString key;
    beginGroup(m_appuid);
    {
        if (!QSettings::contains("BedrockVersion"))
            QSettings::setValue("BedrockVersion", QString(BEDROCK_VERSION_DEFAULT));

        if (!QSettings::contains("ZoomTextOnly"))
            QSettings::setValue("ZoomTextOnly", "0");

        if (!QSettings::contains("PopupBlocking"))
            QSettings::setValue("PopupBlocking", "1");

        // Base directory for ROM files (if the browser isn't in ROM this will be the same as DataBaseDirectory)
        if (!QSettings::contains("ROMBaseDirectory")) {
#if defined Q_OS_SYMBIAN
#ifndef QT_NO_DESKTOPSERVICES
            QString baseDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
            if (baseDir.length() > 2 && baseDir.at(1) == ':')
                baseDir = baseDir.mid(2);
#else /* QT_NO_DESKTOPSERVICES */
            QString baseDir = QDir::homePath(); // Not sure if this is valid
#endif /* QT_NO_DESKTOPSERVICES */
#elif defined Q_WS_MAEMO_5
            QString baseDir = "/opt/nokia-browser";
#else /* Win or Linux */
            QString baseDir = "."; /* Should this also be homePath()? */
#endif
            QSettings::setValue("ROMBaseDirectory", baseDir + "/");
        }

        // Base directory for writeable data files, not database directory
        if (!QSettings::contains("DataBaseDirectory")) {
#if defined Q_OS_SYMBIAN
#ifndef QT_NO_DESKTOPSERVICES
            QString baseDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#else /* QT_NO_DESKTOPSERVICES */
            QString baseDir = QDir::homePath();
#endif /* QT_NO_DESKTOPSERVICES */
#elif defined Q_WS_MAEMO_5
            QString baseDir = QDir::homePath();
#else /* Win or Linux */
            QString baseDir = "."; /* Should this also be homePath()? */
#endif
            QSettings::setValue("DataBaseDirectory", baseDir + "/");
        }
        
        if (!QSettings::contains("DataBaseName")) {
#if defined Q_WS_MAEMO_5
						QString baseDir = QDir::homePath();
            QSettings::setValue("DataBaseName", baseDir + "/browserContent.db");
#else						
            QSettings::setValue("DataBaseName", "browserContent.db");
#endif
        }

        if (!QSettings::contains("ChromeBaseDirectory")) {
            QString chromeBaseDir = ":/chrome/";
#ifndef Q_OS_SYMBIAN
            // Maemo, Linux, Windows can override using an env var
            static const QString envChromeBaseDir = QProcessEnvironment::systemEnvironment().value("BROWSER_CHROME"); // allow env var overriding for dev
            if (envChromeBaseDir != "")
                chromeBaseDir = envChromeBaseDir;
#endif
          QSettings::setValue("ChromeBaseDirectory", chromeBaseDir);
        }

        if (!QSettings::contains("LocalPagesBaseDirectory")) {
#ifdef Q_OS_SYMBIAN
            QString localpagesBaseDir = QSettings::value("ROMBaseDirectory").toString() + "localpages/";
#elif defined Q_WS_MAEMO_5
            QDir localpages("/opt/nokia-browser/localpagesmaemo/");
            QString localpagesBaseDir;
            if(localpages.exists())  //harware build
                localpagesBaseDir  = "/opt/nokia-browser/localpagesmaemo/";
            else //emulator
                localpagesBaseDir = "./sf/app/browserui/ginebra2/chrome/localpagesmaemo/";
#else        
            QString localpagesBaseDir = "chrome/localpages/";
            // Linux, Windows can override using an env var
            static const QString envBaseDir = QProcessEnvironment::systemEnvironment().value("BROWSER_LOCALPAGES"); // allow env var overriding for dev
            if (envBaseDir != "")
                localpagesBaseDir = envBaseDir;
#endif
          QSettings::setValue("LocalPagesBaseDirectory", localpagesBaseDir);
        }

        if (!QSettings::contains("StartUpChrome")) {
          QSettings::setValue("StartUpChrome", "chrome.html"); 
        }

        if (!QSettings::contains("SplashImage")) {
          QSettings::setValue("SplashImage", "bedrock_splash.png"); 
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

        if (!QSettings::contains("NetworkPort")) {
// For s60 arm and maemo arm (i.e. not x86 emulator build) we need to set no proxy
#if (defined(Q_OS_SYMBIAN)  && !defined(Q_CC_NOKIAX86)) || (defined(Q_WS_MAEMO_5) && !defined(QT_ARCH_I386))
          // empty proxy only for ARMV5 Symbian targets
	        QSettings::setValue("NetworkPort", QString());
// everything else, linux, win, s60 emulator, maemo emulator needs proxy
#else
  	      QSettings::setValue("NetworkPort", "8080");
#endif
        }
        if (!QSettings::contains("DiskCacheEnabled"))
            QSettings::setValue("DiskCacheEnabled", "1");

        if (!QSettings::contains("DiskCacheMaxSize"))
            QSettings::setValue("DiskCacheMaxSize", "6291456"); //6M

        if (!QSettings::contains("MaxPagesInCache"))
            QSettings::setValue("MaxPagesInCache", "3");

        if (!QSettings::contains("DnsPrefetchEnabled"))
            QSettings::setValue("DnsPrefetchEnabled", "0");
        const QString diskCacheBaseDir = QSettings::value("DataBaseDirectory").toString();

        if (!QSettings::contains("DiskCacheDirectoryPath")) {
            QSettings::setValue("DiskCacheDirectoryPath", diskCacheBaseDir + "brCache");
        }

        if (!QSettings::contains("Cookies"))
            QSettings::setValue("Cookies", "1");

        if (!QSettings::contains("StartPage"))
        {
            QSettings::setValue("StartPage", "startpage.html");
        }

        if (!QSettings::contains("SaveSession"))
            QSettings::setValue("SaveSession", "1");

        if (!QSettings::contains("SaveHistory"))
            QSettings::setValue("SaveHistory", "1");

        if (!QSettings::contains("BrowserEncoding"))
            QSettings::setValue("BrowserEncoding", QString("iso-8859-1"));

        if (!QSettings::contains("Html5LocalStorage")) {
            QSettings::setValue("Html5LocalStorage", "1");
        }

        if (!QSettings::contains("Html5DatabaseStorage")) {
        	  #if defined Q_WS_MAEMO_5
            QSettings::setValue("Html5DatabaseStorage", "0");
            #else
            QSettings::setValue("Html5DatabaseStorage", "1");
            #endif
        }

        if (!QSettings::contains("Html5DatabaseStorageMaxSize")) {
            // This quota applies to each individual persistent
            // store (local storage, database storage) database.
            // There is a separate instance for each security origin
            // that uses the feature.  See QWebSecurityOrigin.
            //
            // Limit: 5MB per origin
            QString maxSize = QString::number(5 * 1024 * 1024); // 5MB per origin
            QSettings::setValue("Html5DatabaseStorageMaxSize", maxSize);
        }

        if (!QSettings::contains("Html5ApplicationCache")) {
            QSettings::setValue("Html5ApplicationCache", "1");
        }

        if (!QSettings::contains("Html5ApplicationCacheMaxSize")) {
            // This quota applies to the single database instance
            // used to store ALL application cache data.  It should
            // be fairly large, as the application cache is used to
            // store entire files -- html/css/javascript text, image
            // files, etc.
            //
            // Limit: 200MB = 5MB per origin * 40 origins
            QString maxSize = QString::number(200 * 1024 * 1024);
            QSettings::setValue("Html5ApplicationCacheMaxSize", maxSize);
        }



        // Gestures library settings
        initGestureParams();


        // reserved entries for local bookmarks
        if (!QSettings::contains("Bookmark0Title")) {
            QSettings::setValue("Bookmark0Title", "Browser Welcome Page");
        }

        if (!QSettings::contains("Bookmark0Url")) {
            QSettings::setValue("Bookmark0Url", "startpage.html");
        }

        if (!QSettings::contains("Bookmark1Title")) {
            QSettings::setValue("Bookmark1Title", "");
        }

        if (!QSettings::contains("Bookmark1Url")) {
            QSettings::setValue("Bookmark1Url", "");
        }

        if (!QSettings::contains("Bookmark2Title")) {
            QSettings::setValue("Bookmark2Title", "");
        }

        if (!QSettings::contains("Bookmark2Url")) {
            QSettings::setValue("Bookmark2Url", "");
        }
    }

	        // userAgentStringSetup, default empty.
	      
	      if (!QSettings::contains("UserAgentString"))
        {
        	  #if defined Q_WS_MAEMO_5
            QSettings::setValue("UserAgentString", "Mozilla/5.0 (iPhone; U; CPU iPhone OS 4_0 like Mac OS X; en-us) AppleWebKit/532.9 (KHTML, like Gecko) Version/4.0.5 Mobile/8A293 Safari/6531.22.7");        
            #else  // Symbian, Linux etc.
            QSettings::setValue("UserAgentString", QString());  
            #endif  
        }          
        
                    				
        
        //Scrolling options
        initScrollingParams();
        
        //Tiling options
        initTilingParams();
        
    endGroup(); // m_appuid
    sync();
}


void BedrockProvisioning::initScrollingParams()
{
    if (!QSettings::contains("KineticDeceleration")) {    
        QSettings::setValue("KineticDeceleration", 2.0);
    }
    
    if (!QSettings::contains("MaxFlickSpeed")) {    
        QSettings::setValue("MaxFlickSpeed", 1.2);
    }

    if (!QSettings::contains("MidFlickSpeed")) {
        QSettings::setValue("MidFlickSpeed", 0.5);
    }

    if (!QSettings::contains("MinFlickSpeed")) {
        QSettings::setValue("MinFlickSpeed", 0.15);
    }

    if (!QSettings::contains("MaxFlickInViewportUnits")) {
       QSettings::setValue("MaxFlickInViewportUnits", 0.7);
    }
    if (!QSettings::contains("MidFlickInViewportUnits")) {
       QSettings::setValue("MidFlickInViewportUnits", 0.4);
    }
    if (!QSettings::contains("MinFlickInViewportUnits")) {
       QSettings::setValue("MinFlickInViewportUnits", 0.2);
    }

    if (!QSettings::contains("MaxFlickDuration")) {
       QSettings::setValue("MaxFlickDuration", 1000);
    }
    if (!QSettings::contains("MidFlickDuration")) {
       QSettings::setValue("MidFlickDuration", 800);
    }
    if (!QSettings::contains("MinFlickDuration")) {
       QSettings::setValue("MinFlickDuration", 500);
    }
    if (!QSettings::contains("ScrollOvershoot")) {
       QSettings::setValue("ScrollOvershoot", 1.70158);
    }
}

void BedrockProvisioning::initTilingParams()
{
    if (!QSettings::contains("EnableTiling")) {    
        QSettings::setValue("EnableTiling", 1);
    }
    if (!QSettings::contains("TilesWidth")) {    
        QSettings::setValue("TilesWidth", 256);
    }
    if (!QSettings::contains("TilesHeight")) {    
        QSettings::setValue("TilesHeight", 256);
    }
    if (!QSettings::contains("TileCreationDelay")) {    
        QSettings::setValue("TileCreationDelay", 50);
    }
    if (!QSettings::contains("TileCoverAreaMultiplier")) {    
        QSettings::setValue("TileCoverAreaMultiplier", 2.5);
    }
    if (!QSettings::contains("TileKeepAreaMultiplier")) {    
        QSettings::setValue("TileKeepAreaMultiplier", 2.5);
    }    
}

void BedrockProvisioning::initGestureParams()
{
    // Enable/disable specific gestures
    if (!QSettings::contains("GesturesEnableTap")) {
        QSettings::setValue("GesturesEnableTap", 1);
    }
    if (!QSettings::contains("GesturesEnablePan")) {
        QSettings::setValue("GesturesEnablePan", 1);
    }
    if (!QSettings::contains("GesturesEnableHover")) {
        QSettings::setValue("GesturesEnableHover", 0);
    }
    if (!QSettings::contains("GesturesEnableLeftRight")) {
        QSettings::setValue("GesturesEnableLeftRight", 1);
    }
    if (!QSettings::contains("GesturesEnableUpDown")) {
        QSettings::setValue("GesturesEnableUpDown", 1);
    }
    if (!QSettings::contains("GesturesEnableFlick")) {
        QSettings::setValue("GesturesEnableFlick", 1);
    }
    if (!QSettings::contains("GesturesEnableRelease")) {
        QSettings::setValue("GesturesEnableRelease", 1);
    }
    if (!QSettings::contains("GesturesEnableTouch")) {
        QSettings::setValue("GesturesEnableTouch", 1);
    }
    if (!QSettings::contains("GesturesEnableEdgeScroll")) {
        QSettings::setValue("GesturesEnableEdgeScroll", 0);
    }
    if (!QSettings::contains("GesturesEnableCornerZoom")) {
        QSettings::setValue("GesturesEnableCornerZoom", 0);
    }
    if (!QSettings::contains("GesturesEnablePinch")) {
        QSettings::setValue("GesturesEnablePinch", 1);
    }
    if (!QSettings::contains("GesturesEnableLongPress")) {
        QSettings::setValue("GesturesEnableLongPress", 1);
    }
    if (!QSettings::contains("GesturesEnableUnknown")) {
        QSettings::setValue("GesturesEnableUnknown", 1);
    }

    // General gesture settings
    if (!QSettings::contains("GesturesEnableLogging")) {
        QSettings::setValue("GesturesEnableLogging", 0);
    }
    if (!QSettings::contains("GesturesSuppressTimeout")) {
        QSettings::setValue("GesturesSuppressTimeout", 0);
    }
    if (!QSettings::contains("GesturesMoveSuppressTimeout")) {
        QSettings::setValue("GesturesMoveSuppressTimeout", 0);
    }
    if (!QSettings::contains("GesturesCapacitiveUpUsed")) {
#ifdef ADVANCED_POINTER_EVENTS
        QSettings::setValue("GesturesCapacitiveUpUsed", 1);
#else
        QSettings::setValue("GesturesCapacitiveUpUsed", 0);
#endif
    }
    if (!QSettings::contains("GesturesAdjustYPos")) {
#ifdef ADVANCED_POINTER_EVENTS
        QSettings::setValue("GesturesAdjustYPos", 0);
#else
        QSettings::setValue("GesturesAdjustYPos", 0);
#endif
    }
    if (!QSettings::contains("GesturesEnableFiltering")) {
        QSettings::setValue("GesturesEnableFiltering", 1);
    }
    if (!QSettings::contains("GesturesWServMessageInterception")) {
        QSettings::setValue("GesturesWServMessageInterception", 0);
    }

    // Individual gesture specific settings
    // Tap
    if (!QSettings::contains("GesturesDoubleTapTimeout")) {
        QSettings::setValue("GesturesDoubleTapTimeout", 600);
    }
    // Pan
    if (!QSettings::contains("GesturesPanSpeedLow")) {
        QSettings::setValue("GesturesPanSpeedLow", 5);
    }
    if (!QSettings::contains("GesturesPanSpeedHigh")) {
        QSettings::setValue("GesturesPanSpeedHigh", 50);
    }
    if (!QSettings::contains("GesturesPanDisabledWhileHovering")) {
        QSettings::setValue("GesturesPanDisabledWhileHovering", 0);
    }
    if (!QSettings::contains("GesturesMoveTolerance")) {
        QSettings::setValue("GesturesMoveTolerance", 0);
    }
    // Hover
    if (!QSettings::contains("GesturesHoverSpeed")) {
        QSettings::setValue("GesturesHoverSpeed", 0);
    }
    if (!QSettings::contains("GesturesHoverDisabledWhilePanning")) {
        QSettings::setValue("GesturesHoverDisabledWhilePanning", 0);
    }
    // Flick
    if (!QSettings::contains("GesturesFlickSpeed")) {
        QSettings::setValue("GesturesFlickSpeed", 35);
    }
    // EdgeScroll
    if (!QSettings::contains("GesturesEdgeScrollRange")) {
        QSettings::setValue("GesturesEdgeScrollRange", 20);
    }
    // CornerZoom
    if (!QSettings::contains("GesturesZoomCornerSize")) {
        QSettings::setValue("GesturesZoomCornerSize", 7);
    }
    // Area settings
    if (!QSettings::contains("GesturesTouchAreaShape")) {
        QSettings::setValue("GesturesTouchAreaShape", 1);
    }
    if (!QSettings::contains("GesturesTouchAreaTimeout")) {
        QSettings::setValue("GesturesTouchAreaTimeout", 50);
    }
    if (!QSettings::contains("GesturesTouchAreaWidth")) {
        QSettings::setValue("GesturesTouchAreaWidth", 7);
    }
    if (!QSettings::contains("GesturesTouchAreaHeight")) {
        QSettings::setValue("GesturesTouchAreaHeight", 7);
    }
    if (!QSettings::contains("GesturesTimeAreaShape")) {
        QSettings::setValue("GesturesTimeAreaShape", 1);
    }
    if (!QSettings::contains("GesturesTimeAreaTimeout")) {
        QSettings::setValue("GesturesTimeAreaTimeout", 0);
    }
    if (!QSettings::contains("GesturesTimeAreaWidth")) {
        QSettings::setValue("GesturesTimeAreaWidth", 11);
    }
    if (!QSettings::contains("GesturesTimeAreaHeight")) {
        QSettings::setValue("GesturesTimeAreaHeight", 11);
    }
    if (!QSettings::contains("GesturesHoldAreaShape")) {
        QSettings::setValue("GesturesHoldAreaShape", 1);
    }
    if (!QSettings::contains("GesturesHoldAreaTimeout")) {
        QSettings::setValue("GesturesHoldAreaTimeout", 1000);
    }
    if (!QSettings::contains("GesturesHoldAreaWidth")) {
        QSettings::setValue("GesturesHoldAreaWidth", 7);
    }
    if (!QSettings::contains("GesturesHoldAreaHeight")) {
        QSettings::setValue("GesturesHoldAreaHeight", 7);
    }
    if (!QSettings::contains("GesturesAxisLockThreshold")) {
        QSettings::setValue("GesturesAxisLockThreshold", 0.5);
    }
    
    if (!QSettings::contains("EnableGestures")) {
            QSettings::setValue("EnableGestures", 1);
    }
}


QString BedrockProvisioning::valueAsString(const QString &key, const QVariant &defaultValue)
{
    return value(key, defaultValue).toString();
}

int BedrockProvisioning::valueAsInt(const QString &key, const QVariant &defaultValue)
{
    return value(key, defaultValue).toInt();
}

qint64 BedrockProvisioning::valueAsInt64(const QString &key, const QVariant &defaultValue)
{
    return value(key, defaultValue).toLongLong();
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

} // end of namespace BEDROCK_PROVISIONING
