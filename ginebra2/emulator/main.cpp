/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <QtGui>
#include <QWebView>
#include <QWebPage>
#include <QWebFrame>
#include <QTranslator>
#include <QLocale>
#include <QWebSettings>
#include <QGraphicsWebView>
#ifndef NO_QSTM_GESTURE
#include "WebGestureHelper.h"
#endif

#include <QNetworkProxyFactory>
#include "browser.h"

#include <QDebug>

//#define HARDWARE_DEBUG_TRACE

#ifdef ENABLE_PERF_TRACE  
    #include "wrtperftracer.h"
    // Global reference to WrtPerfTracer
    WrtPerfTracer* g_wrtPerfTracing;
#endif

#ifdef HARDWARE_DEBUG_TRACE

static const QString DebugLogPath("C:/Data/debug.txt");

static void initDebugOutput()
{
    QFile file(DebugLogPath);
    file.remove();
}

static void debugOutput(QtMsgType type, const char *msg)
{
    QFile file(DebugLogPath);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        return;
    
    QTextStream out(&file);
    out << "\nDebug: " << msg;
    
    file.flush();
    file.close();
}
#endif


#ifdef NO_NETWORK_ACCESS_MANAGER


static QUrl urlFromUserInput(const QString& string)
{
    QString input(string);
    QFileInfo fi(input);
    if (fi.exists() && fi.isRelative())
        input = fi.absoluteFilePath();

#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    return QUrl::fromUserInput(input);
#else
    return QUrl(input);
#endif
}


struct BrowserProxyFactory : public QNetworkProxyFactory
{
    QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery &query = QNetworkProxyQuery())
    {
        QList<QNetworkProxy> plist;

#if defined(__WINSCW__)
        plist.push_back(QNetworkProxy(QNetworkProxy::HttpProxy,QString("dawebproxy01.americas.nokia.com"), 8080));
#endif
        QUrl proxyUrl = urlFromUserInput(qgetenv("http_proxy"));

        if (proxyUrl.isValid() && !proxyUrl.host().isEmpty()) {
            int proxyPort = (proxyUrl.port() > 0) ? proxyUrl.port() : 8080;
            plist.push_back(QNetworkProxy(QNetworkProxy::HttpProxy, proxyUrl.host(), proxyPort));
        }

        plist.append(systemProxyForQuery(query));
        return plist;
    };
};

static BrowserProxyFactory proxies;
#endif //NO_NETWORK_ACCESS_MANAGER

#ifdef ENABLE_PERF_TRACE
    WrtPerfTracer* perfTracer = 0;
#endif

int main(int argc, char * argv[])
{
#ifdef NO_NETWORK_ACCESS_MANAGER	
    QNetworkProxyFactory::setApplicationProxyFactory(&proxies);
#endif //NO_NETWORK_ACCESS_MANAGER    
    int res=0;

#ifdef HARDWARE_DEBUG_TRACE
    initDebugOutput();    
    qInstallMsgHandler(debugOutput);
#endif

#ifdef NO_QSTM_GESTURE
  QApplication app(argc, argv);
#else
  BrowserApp app(argc, argv);
#endif

#ifdef Q_OS_SYMBIAN
  //Object cache settings. NB: these need to be tuned per device
  QWebSettings::globalSettings()->setObjectCacheCapacities(128*1024, 1024*1024, 1024*1024);
#endif

    QString lang = QLocale::system().name();

    //install the common translator from platform
    QTranslator common;
    common.load(":/resource/qt/translations/common_" + lang);
    QApplication::installTranslator(&common);

    
    //install the translator from Browser
    QTranslator translator;
    QString transFilePath = ":/translations";
    //Todo: when platform delivers .ts file
    //QString transFilePath = "Z:/resource/qt/translations/";
    QString transFile = QLatin1String("browserLoc_") +  lang;
    translator.load(transFile, transFilePath);
    QApplication::installTranslator(&translator);
#ifdef Q_OS_SYMBIAN
    QApplication::setKeypadNavigationEnabled(true);
#endif
    GinebraBrowser * browser = new GinebraBrowser();

#ifdef ENABLE_PERF_TRACE
    g_wrtPerfTracing = new WrtPerfTracer(0);
#endif //ENABLE_PERF_TRACE
    
    browser->show();
    res = app.exec();
    delete browser;
    
#ifdef ENABLE_PERF_TRACE    
       // delete g_wrtPerfTracing instance
       if (g_wrtPerfTracing) {
           g_wrtPerfTracing->close();
           delete g_wrtPerfTracing;
       }   
#endif //ENABLE_PERF_TRACE
       
    return res;
}
