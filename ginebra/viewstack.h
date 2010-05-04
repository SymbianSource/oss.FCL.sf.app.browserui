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


#ifndef VIEWSTACK_H
#define VIEWSTACK_H
#include <QObject>

class ViewController;
class ChromeView;


class ViewStack : public QObject
{
    Q_OBJECT

public:
    static ViewStack* getSingleton();
    ViewController *getViewController() { return m_viewController;}
    void setViewController(ViewController *viewController) { m_viewController = viewController;}
    ChromeView *getChromeView() { return m_chromeView;}
    void setChromeView(ChromeView *chromeView) { m_chromeView = chromeView;}

public slots:
    void switchView(const QString &to, const QString &from);

private:
    void fromWindowView(const QString &to);
    void fromGoAnywhereView(const QString &to);
    void fromWebView(const QString &to);
    void fromBookmarkHistoryView(const QString &to);
    void fromBookmarkTreeView(const QString &to);
    void toWindowView();
    void toGoAnywhereView();
    void toWebView();
    void toBookmarkHistoryView();
    void toBookmarkView();
    void initWindowView();

private slots:
    void loadHistoryItem(int item);
    void goBackFromGoAnywhereView();
    void goBackFromWindowView();
    void goBackFromRecentUrlView();
    void goBackFromBookmarkView();
    void showContentView(int progress);


signals:
    void activateBookmark();
    void deActivateBookmark();
    void activateHistory();
    void deActivateHistory();
    void activateBookMarkHistory();
    void deActivateBookMarkHistory();
    void activateWindowView();
    void deActivateWindowView();
    void activateWebView();
    void deActivateWebView();

    void pageChanged(int);

private:
    ViewController *m_viewController; // Not owned
    ChromeView *m_chromeView; //Not owned
};

#endif // VIEWSTACK_H
