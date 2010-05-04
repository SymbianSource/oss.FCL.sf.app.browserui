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


#ifndef CHROMECONSOLE_H
#define CHROMECONSOLE_H

#include <QWidget>
#include <QtGui>
#include "../ChromeWidget.h"


/**
 * \brief Javascript console window for development and testing
 * 
 * The ChromeConsole class displays a simple dialog that allows the user to enter javascript 
 * statements into a text field and then execute them in the chrome's javascript engine.
 * Not used in S60 builds.
 */
class ChromeConsole : public QDialog
{
    Q_OBJECT
public:
    ChromeConsole(GVA::ChromeWidget *chromeWidget);

    QVariant evaluateExpression(const QString &expression);

public slots:
    void evaluate();
    void reloadChrome();
    void dump();
    void exit() { QApplication::exit(0); }
    virtual void accept();

protected:
    virtual void keyPressEvent(QKeyEvent *event);

protected:
    QTextEdit m_textEdit;
    GVA::ChromeWidget *m_chromeWidget;
    class Ui_ConsoleDialog *m_ui;
    QStringList m_expressionHistory;
    int m_historyIndex;
};


#endif // CHROMECONSOLE_H
