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


#ifndef __URLSEARCH_SNIPPET_H
#define __URLSEARCH_SNIPPET_H

#include "NativeChromeItem.h"

namespace GVA {

class ChromeWidget;

class UrlEditorWidget : public QGraphicsTextItem
{
    Q_OBJECT

public:
    UrlEditorWidget(QGraphicsItem * parent);
    virtual ~UrlEditorWidget();

    void setText(const QString & text);

    qreal cursorX();

protected:
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    virtual void keyPressEvent(QKeyEvent * event);

signals:
    void cursorXChanged(qreal newx);

private:
    QTextLine m_textLine;
};

class UrlSearchSnippet : public NativeChromeItem
{
    Q_OBJECT

public:
    UrlSearchSnippet(ChromeSnippet * snippet, ChromeWidget * chrome, QGraphicsItem * parent = 0);
    virtual ~UrlSearchSnippet();

protected:
    virtual bool eventFilter(QObject * object, QEvent * event);
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    virtual void resizeEvent(QGraphicsSceneResizeEvent * event);

signals:
    void activated();

private slots:
    void resize();
    void setUrlText(const QString & text);
    void setStarted();
    void setProgress(int percent);
    void setFinished(bool ok);
    void clearProgress();
    void viewChanged();
    void makeVisible(qreal cursorX);

private:
    void internalScroll(qreal deltaX);

private:
    ChromeWidget * m_chrome;
    QString m_text;
    int m_percent;
    int m_pendingClearCalls;

    // Style parameters.

    QColor m_textColor;
    QColor m_backgroundColor;
    QColor m_borderColor;

    int m_border;
    int m_padding;

    // Cached values used for painting and scrolling.

    qreal m_viewPortWidth;
    qreal m_viewPortHeight;

    // At runtime, UrlSearchSnippet is parent to a QGraphicsWidget
    // (m_viewPort) that is parent to a UrlEditorWidget (m_editor).

    QGraphicsWidget * m_viewPort;
    UrlEditorWidget * m_editor;
};

} // namespace GVA

#endif // __URLSEARCH_SNIPPET_H
