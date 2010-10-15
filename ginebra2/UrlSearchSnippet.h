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

#ifndef __URLSEARCH_SNIPPET_H
#define __URLSEARCH_SNIPPET_H

#include "EditorWidget.h"
#include "NativeChromeItem.h"

namespace GVA {

class ChromeWidget;

class GUrlSearchItem : public NativeChromeItem
{
    Q_OBJECT

public:
    GUrlSearchItem(ChromeSnippet * snippet, ChromeWidget * chrome, QGraphicsItem * parent = 0);
    virtual ~GUrlSearchItem();
    QString url() const { return m_urlSearchEditor->text();}
    void setUrl(const QString &url) {m_urlSearchEditor->setText(url);}

    void cut() { m_urlSearchEditor->cut(); }
    void copy() { m_urlSearchEditor->copy(); }
    void paste() { m_urlSearchEditor->paste(); }
    void setContextMenuStatus(bool on) { m_urlSearchEditor->setContextMenuStatus(on); }
    void setFocusForEditor() { m_urlSearchEditor->setFocusForEditor(); }

Q_SIGNALS:
    void changeEditMode(bool);
    void contextEvent(bool);

private slots:
    void onContextEvent(bool isContentSelected);

protected:
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    virtual void resizeEvent(QGraphicsSceneResizeEvent * event);

private:
    void createEditor();
    void createIcons();
    void loadToMainWindow();
    void updateUrlSearchBtn();
    QString formattedUrl() const; 
    void changeLoadState(bool editing=true);
    QString urlToBeDisplayed();
    QString currentTitle();
    void changeLayout(const bool isSearch);
#ifdef BROWSER_LAYOUT_TENONE
    void changeToTitle();
    QString getWindowsViewTitle();
    void layoutToEditMode(const bool editMode);
    void hideIndicatorButton();
#endif

private slots:

    void onContentMouseEvent(QEvent::Type type);
    void setUrlText(const QString &);
    void onChromeComplete();
    void setStarted();
    void setProgress(int percent);
    void setFinished(bool ok);
    void setPageCreated();
    void setPageChanged();
	void setPageFailed();
    void clearProgress();
    void viewChanged(ControllableViewBase *newView);
    void urlSearchActivatedByEnterKey();
    void urlSearchActivated();
    void focusChanged(bool focusIn);
    void resize();
    void updateLoadStateAndSuggest(int /*position*/, int charsRemoved, int charsAdded);
    void onNewWindowTransitionComplete();
    void tapped(QPointF&);
    
#ifdef BROWSER_LAYOUT_TENONE
    void changeToUrl(QPointF&);
    void onTitleChange(const QString&);
    void showSecureIcon(bool show);
    void urlTitleIndBtnActivated();
#endif


private:
    ChromeWidget * m_chrome;

    // configurable attributes
    qreal m_border;
    qreal m_padding;
    qreal m_viewPortWidth;
    qreal m_viewPortHeight;
    QColor m_borderColor;

    // ui components
    QGraphicsWidget * m_viewPort;
    ActionButton * m_urlSearchBtn;
#ifdef BROWSER_LAYOUT_TENONE
    ActionButton * m_urlTitleIndBtn;
    ActionButton * m_urlTitleIndSecureBtn;
#endif
    GProgressEditor * m_urlSearchEditor;
    

    // variables
    int m_pendingClearCalls;
    qreal m_buttonIconWidth;
    qreal m_buttonIconHeight;
    bool m_backFromNewWinTrans;
    bool m_justFocusIn;
};

class GUrlSearchSnippet : public ChromeSnippet {
  Q_OBJECT
public:
  GUrlSearchSnippet(const QString & elementId, ChromeWidget * chrome,
              QGraphicsWidget * widget, const QWebElement & element);
  
  static GUrlSearchSnippet * instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element);

  /// The URL of the web page.
  QString url() const;
  void setUrl(const QString &url);
  Q_PROPERTY(QString url READ url WRITE setUrl)

Q_SIGNALS:
  void contextEvent(bool isContentSelected, QString snippetId);

public slots:
  void cut();
  void copy();
  void paste();
  bool editable() { return true; }
  bool useNativeCopyPasteMenu() { return true; }
  void sendContextMenuEvent(bool isContentSelected);
  void setContextMenuStatus(bool on) { urlSearchItem()->setContextMenuStatus(on); }
  void grabFocus() { urlSearchItem()->setFocusForEditor(); }

private:
  GUrlSearchItem *urlSearchItem();
  GUrlSearchItem const *constUrlSearchItem() const;
};

} // namespace GVA

#endif // __URLSEARCH_SNIPPET_H
