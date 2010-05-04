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
#include "../chromeview.h"
#ifndef Q_OS_SYMBIAN
#include "chromewindow.h"
#endif

#include <QDir>
#include <QTranslator>
#include <QLocale>
#include "bedrockprovisioning.h"
#include "webpagecontroller.h"
#include "viewstack.h"

//#define HARDWARE_DEBUG_TRACE
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

int main(int argc, char * argv[])
{
    int res = 0;
    
#ifdef HARDWARE_DEBUG_TRACE
    initDebugOutput();    
    qInstallMsgHandler(debugOutput);
#endif
    
    QApplication app(argc, argv);
    qDebug() << "main: currentPath=" << QDir::currentPath();
    
    QLocale language;
    QString langCountryCode = language.name();
    langCountryCode.replace(QString("-"), QString("_"));
    //qDebug() << "ChromeView::loadChrome: "  << langCountryCode;  
    //Todo: when platform delivers .ts file
    //QString transFilePath = "Z:/resource/qt/translations/";
    QString transFilePath = ":/translations";
    QTranslator translator;
    QString transFile = QLatin1String("browserLoc_") + langCountryCode.toLower();
    bool installed = translator.load(transFile, transFilePath);
    if (installed)
    {
        QApplication::installTranslator(&translator);
    }
    else if (transFile.count("_") > 1) 
    {
        transFile = transFile.left(transFile.lastIndexOf("_"));
        installed = translator.load(transFile, transFilePath);
        qDebug() << "[LocaleDelegate::loadTranslator()] translation file installed:" << installed;
        if (installed)
            QApplication::installTranslator(&translator);
        else
        {
            qDebug() << " LocaleDelegate::loadTranslator not loaded!";
        }
    }
    
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5)
    // Display in bare-bones widget.
    GiWidget *gw = new GiWidget;
    //app.setMainWindow(gw);
    
    ChromeView *view = new ChromeView(gw);
    gw->setChromeView(view);
    gw->showFullScreen();
    view->setGeometry(0, 0, gw->width(), gw->height());
    
#else // Desktop build.
    // Display in a top-level window with menubar and toolbar etc...
    ChromeWindow *chrome = new ChromeWindow();
    chrome->show();
#endif
    res = app.exec();

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5)
	delete view;
	delete gw;
#else
	delete chrome;
#endif

    delete WebPageController::getSingleton();
    delete ViewStack::getSingleton();

	return res;
}
