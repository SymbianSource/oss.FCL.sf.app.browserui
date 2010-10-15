/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __COPY_CUT_PASTE_SNIPPET_H__
#define __COPY_CUT_PASTE_SNIPPET_H__

#include "ChromeSnippet.h"
#include "ChromeWidget.h"
#include "NativeChromeItem.h"
#include "ExternalEventCharm.h"

#include <QPushButton>

namespace GVA {

    class CopyCutPasteButton :  public QObject
    {
        Q_OBJECT

    public:
        CopyCutPasteButton(const QString & text, QWidget * parent = 0);
        virtual ~CopyCutPasteButton() {}

        void render(QPainter * painter, const QPoint & targetOffset = QPoint());
        void setMousePressed(bool pressed) { m_mousePressed = pressed; }
        QString text() const { return m_text; }
        void setText(QString & text) { m_text = text; }
        QSize size() const { return m_size; }
        void setFixedSize(int w, int h) { m_size = QSize(w, h); }
        void setDisabled(bool disabled) { m_disabled = disabled; }
        bool isEnabled() const { return !m_disabled; }
        
        void mousePressEvent(QGraphicsSceneMouseEvent * event);
        void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

    signals:
        void clicked();

    private:
        QString m_text;
        bool m_mousePressed;
        QSize m_size;
        bool m_disabled;
    };

    class CopyCutPasteItem : public NativeChromeItem
    {
        Q_OBJECT

    public:
        CopyCutPasteItem(ChromeSnippet * snippet, ChromeWidget * chrome, QGraphicsItem * parent = 0);
        virtual ~CopyCutPasteItem();
        void buildMenu(bool isContentSelected);
        void setEditorSnippet(ChromeSnippet * snippet) { m_editorSnippet = snippet; }
        ChromeSnippet * editorSnippet() { return m_editorSnippet; }

    private slots:
        void cut();
        void copy();
        void paste();

    protected:
        virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
        virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

    private:
        void reset();

    private:
        CopyCutPasteButton * m_cutButton;
        CopyCutPasteButton * m_copyButton;
        CopyCutPasteButton * m_pasteButton;
        ChromeSnippet * m_editorSnippet;
    };

  class CopyCutPasteSnippet : public ChromeSnippet
  {
    Q_OBJECT
    public:
       CopyCutPasteSnippet(const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element);
       virtual ~CopyCutPasteSnippet() { if (m_externalEventCharm) delete m_externalEventCharm ;}
       static CopyCutPasteSnippet * instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element);
       virtual void setChromeWidget(QGraphicsWidget * widget);

    public slots:
        virtual void setVisible(bool visiblity, bool animate = true);

    private:
       void connectAll();
       CopyCutPasteItem *copyCutPasteItem();
       
    private slots:
       void onTimeout();
       void onChromeComplete();
       void onContextEvent(bool isContentSelected, QString snippetId);
       void onExternalMouseEvent(QEvent *, const QString &, const QString &);
       void onAspectChanged(int aspect);

    private:
       QTimer * m_timer;
       ExternalEventCharm * m_externalEventCharm;
  };
}

#endif
