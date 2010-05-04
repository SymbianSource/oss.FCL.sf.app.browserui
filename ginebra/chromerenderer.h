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


#ifndef GINEBRA_RENDERER_H
#define GINEBRA_RENDERER_H

#include "qwebpage.h"
#include <QtGui>

class QWebPage;
class ChromeWidget;

/**
 * \brief Manages the chrome web page. 
 * 
 * The ChromeRenderer class owns and manages the web page that contains a graphical
 * representation of the the components of the browser chrome.  These components are
 * layed-out and rendered by the web page but actually displayed by ChromeWidget and 
 * ChromeSnippet.
 * 
 * \sa ChromeSnippet
 * \sa ChromeWidget
 */
class ChromeRenderer : public QWidget
{
    Q_OBJECT

public:
    ChromeRenderer(QWidget *parent = 0);
    virtual ~ChromeRenderer();

    QWebPage *page() const;
    void setPage(QWebPage *page);
    void setWidget(ChromeWidget *widget) {m_widget = widget;}
    QVariant inputMethodQuery(Qt::InputMethodQuery property) const;

    QSize sizeHint() const;

    virtual bool event(QEvent *);
    
signals:
    void symbianCarriageReturn();  // HACK

protected:
    //void resizeEvent(QResizeEvent *e);
    //void paintEvent(QPaintEvent *ev);

    /*    virtual void changeEvent(QEvent*);

    virtual void mouseMoveEvent(QMouseEvent*);
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseDoubleClickEvent(QMouseEvent*);
    virtual void mouseReleaseEvent(QMouseEvent*);*/
    virtual void keyPressEvent(QKeyEvent*);
    virtual void keyReleaseEvent(QKeyEvent*);
    virtual void focusInEvent(QFocusEvent*);
    virtual void focusOutEvent(QFocusEvent*);
    virtual void inputMethodEvent(QInputMethodEvent*);

    virtual bool focusNextPrevChild(bool next);

 private:
    QWebPage * m_page;
    ChromeWidget * m_widget;
};

#endif // GINEBRA_RENDERER_H
