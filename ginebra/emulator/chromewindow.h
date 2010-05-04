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


#ifndef _GINEBRA_CHROME_WINDOW_H_
#define _GINEBRA_CHROME_WINDOW_H_

#include <QMainWindow>
 #include <QDebug>

class ChromeView;
class ChromeJSObject;
class QGraphicsScene;
class QAction;
class ChromeView;
class QWheelEvent;
class ContentView;

/**
 * \brief Top-level window wrapper for browser.
 * 
 * The ChromeWindow class provides a menu and toolbar for the browser for development
 * and testing on the desktop.  Not used in S60 builds.
 * 
 * \sa ChromeConsole
 */
class ChromeWindow : public QMainWindow
{
    Q_OBJECT

public:
    ChromeWindow(const QString chromePath);
    ChromeWindow();
    ~ChromeWindow();

private:
    void init(const QString chromePath);
    void initMenus();
    void initActions();
    void initToolbars();

private:
//    void wheelEvent(QWheelEvent *event);
    
private slots:
    void toggleProxy();
    void portrait();
    void landscape();
    void openConsole();

    void setChrome();

private:
    QAction *m_networkProxyAction;
    QAction *m_portraitAction;
    QAction *m_landscapeAction;

    QAction *m_setToBedrockChromeAction;
    QAction *m_setToDemoChromeAction;
    
    QAction *m_reloadAction;
    QAction *m_consoleAction;
    ChromeView *m_view;
};
#endif
