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


#include "ChromeConsole.h"
#include "../ChromeWidget.h"
#include "qwebpage.h"
#include "qwebframe.h"
#include "ui_console.h"


ChromeConsole::ChromeConsole(GVA::ChromeWidget *chromeWidget)
  : QDialog(),
    m_chromeWidget(chromeWidget),
    m_historyIndex(0)
{
    m_ui = new Ui_ConsoleDialog;
    m_ui->setupUi(this);

    QList<int> sizes;
    sizes.append(300);
    sizes.append(30);
    m_ui->splitter->setSizes(sizes);

    // Read the saved state from disk.
    try {
        QFile file("console.dat");
        if(file.open(QIODevice::ReadOnly)) {
            QDataStream in(&file);
            QString str;
            in >> str;
            m_ui->outputEdit->setPlainText(str);
            in >> str;
            m_ui->inputEdit->setPlainText(str);
            int i;
            in >> i;
            QTextCursor cursor = m_ui->inputEdit->textCursor();
            cursor.setPosition(i);
            m_ui->inputEdit->setTextCursor(cursor);
            in >> m_expressionHistory;
            file.close();
        }
    }
    catch(...) {
    }
    m_ui->outputEdit->moveCursor(QTextCursor::End);
    m_ui->outputEdit->ensureCursorVisible();
}

QVariant ChromeConsole::evaluateExpression(const QString &expression) {
    m_expressionHistory.push_front(expression);
    m_historyIndex = 0;

    QVariant result = m_chromeWidget->page()->mainFrame()->evaluateJavaScript(expression);
    qDebug() << result;
    return result;
}

void ChromeConsole::evaluate() {   // slot
    QString expression = m_ui->inputEdit->toPlainText();
    m_ui->outputEdit->appendPlainText(">> " + expression);
    QVariant result = evaluateExpression(expression);

    // Figure out the best way to print the result, must be a better way to do this...
    if(result == QVariant()) {
        m_ui->outputEdit->appendPlainText("undefined");
    }
    else if(result.toString() != "") {
        m_ui->outputEdit->appendPlainText(result.toString());
    }
    else if(result.type() == QVariant::List) {
        m_ui->outputEdit->appendPlainText(result.toStringList().join(","));
    }
    else {
        // Probably a complex object, this won't be a real javascript value but it
        // will be informative to the developer.
        //m_ui->outputEdit->appendPlainText(result.typeName());
        QString resultString;
        QDebug debug(&resultString);
        debug << result;
        m_ui->outputEdit->appendPlainText(resultString);
    }

    m_ui->outputEdit->ensureCursorVisible();

    // Force outputEdit to repaint, otherwise get garbled text (at least on Windows).
    // Note: m_ui->outputEdit->update() doesn't work...
    m_ui->outputEdit->hide();
    m_ui->outputEdit->show();
}

void ChromeConsole::reloadChrome() {  // slot
    m_chromeWidget->reloadChrome();
}

void ChromeConsole::dump() {  // slot
    m_chromeWidget->dump();
}

void ChromeConsole::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
          if(event->modifiers() | Qt::ControlModifier)
              evaluate();
          break;
        case Qt::Key_Up:
          if(event->modifiers() | Qt::ControlModifier) {
              if(m_historyIndex < m_expressionHistory.count() - 1)
                  m_ui->inputEdit->setPlainText(m_expressionHistory[++m_historyIndex]);
          }
          break;
        case Qt::Key_Down:
          if(event->modifiers() | Qt::ControlModifier) {
              if(m_historyIndex > 0)
                  m_ui->inputEdit->setPlainText(m_expressionHistory[--m_historyIndex]);
          }
          break;
    }
}

void ChromeConsole::accept() {  // slot
    try {
        // Save the state to disk.
        QFile file("console.dat");
        if(file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);
            out << m_ui->outputEdit->toPlainText();
            out << m_ui->inputEdit->toPlainText();
            out << m_ui->inputEdit->textCursor().position();
            out << m_expressionHistory;
            file.close();
        }
    }
    catch(...) {
    }
    QDialog::accept();
}
