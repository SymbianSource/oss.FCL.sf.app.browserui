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


#ifndef __GINEBRA_CHROMESNIPPET_H__
#define __GINEBRA_CHROMESNIPPET_H__

#include <QString>
#include <QObject>
#include <QTransform>
#include <QWebElement>
#include "ChromeWidget.h"

class QGraphicsWidget;

namespace GVA {

  class VisibilityAnimator;

  /*! \ingroup JavascriptAPI
   */
  class ChromeSnippet : public QObject
  {
    Q_OBJECT
  public:
    ChromeSnippet(const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element);
    virtual ~ChromeSnippet();
    ChromeWidget* chrome() { return m_chrome; }
    void setInitiallyVisible(bool initiallyVisible) { m_initiallyVisible = initiallyVisible; } //NB: needed?
    void setHidesContent(bool hidesContent) { m_hidesContent = hidesContent; }
    void setAnchor(ChromeAnchor anchor) {m_anchor = anchor;}
    ChromeAnchor anchor() { return m_anchor; }
    int anchorOffset() {return m_anchorOffset;}
    bool initiallyVisible() { return m_initiallyVisible; } //NB: needed?
    bool hidesContent() { return m_hidesContent; }
    QString elementId() { return m_elementId; }
    QGraphicsWidget* widget() { return m_widget; }
    virtual void setWidget(QGraphicsWidget * widget) { m_widget = widget; }
    QString parentId() { return m_parentId; }
    void setParentId(const QString& parent) { m_parentId = parent; }
    void setTransform(QTransform transform);
    QTransform transform();
    QPointF position() const;
    virtual void addChild(ChromeSnippet * child);
    QWebElement element() {return m_element;}
    void dump();
    void addLink(ChromeSnippet*);
    QList<ChromeSnippet *> links() {return m_links;}
  public slots:
    void setAnchor(const QString& anchor, bool update = true);
    void setAnchorOffset(int offset, bool update = true);
    virtual void toggleVisibility(bool animate = true);
    void setVisible(bool visiblity, bool animate = true);
    void show(bool animate = true) { setVisible(true, animate);}
    virtual void hide(bool animate = true) { setVisible(false, animate);}
    void setOpacity(qreal opacity);
    qreal opacity();
    void setEffect(const QString & effect);
    void enableEffect(bool enable);
    void toggleEffect();
    void grabFocus();
    void setVisibilityAnimator(const QString& animator);
    void visibilityFinished(bool visiblity);
    void moveBy(int dx, int dy);
    void anchorTo(const QString & id, int x = 0, int y = 0);
    void unAnchor();
    void anchorToView(const QString& view, const QString& where="top");
    void detachFromView(const QString& view, const QString& where="top");
    QObject * getGeometry() const;
    QObject * getPosition() const;
    void setPosition(int x, int y);
    QObject * childGeometry(const QString id) const;
    QObject * animate(int duration);
    bool isVisible() { return m_visible; }
    bool isHiding() { return m_hiding; }
    int zValue();
    void setZValue(int z);
    //NB: deprecate repaint: if this is needed, then there are bugs that are preventing updates
    void repaint() { m_widget->update(); }
    void onContextMenuEvent(QGraphicsSceneContextMenuEvent * ev); 
  signals:
    void hidden();
    void shown();
    void externalMouseEvent(
            int type,
            const QString & name,
            const QString & description);
    void contextMenuEvent(int x, int y);
    void snippetMouseEvent(QEvent::Type );

  protected slots:
    virtual void positionChildren();
  private slots:
    void onChromeComplete();

  public:
    Q_PROPERTY(QString id READ getId)
    QString getId() const { return m_elementId; }
    Q_PROPERTY(bool visible READ isVisible)
    Q_PROPERTY(bool hiding READ isHiding)
    Q_PROPERTY(QString anchor READ getAnchorString)
    QString getAnchorString();
    Q_PROPERTY(int anchorOffset READ anchorOffset WRITE setAnchorOffset)
    Q_PROPERTY(int zValue READ zValue WRITE setZValue)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(bool dontShow READ getDontShowFlag WRITE setDontShowFlag)
  protected:
    bool getDontShowFlag() {return m_dontshowFlag;}
    void setDontShowFlag(bool flag){ m_dontshowFlag = flag;}
  protected:
    QString m_elementId;
    QWebElement m_element;
    QString m_parentId;
    ChromeWidget * m_chrome;
    QGraphicsWidget * m_widget; //Take ownership
    bool m_initiallyVisible; //NB: needed ?
    bool m_visible;
    bool m_hidesContent;
    ChromeAnchor m_anchor;
    int m_anchorOffset;
    VisibilityAnimator * m_vAnimator;
    QGraphicsEffect * m_effect;
    bool m_hiding;
    bool m_dontshowFlag;
    QList<ChromeSnippet*> m_links;
  };

} // end of namespace GVA

#endif // __GINEBRA_CHROMESNIPPET_H__
