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


#ifndef CHROMEWIDGETJSOBJECT_H
#define CHROMEWIDGETJSOBJECT_H

#include <QObject>
#include <assert.h>

#include "chromewidget.h"

/**
 * \brief Javascript API wrapper for ChromeWidget.
 * The slots and signals of this class are exposed to javascript through the 
 * "window.snippets" object.  Ex: "window.snippets.show('TopChromeId', 10, 10)" will
 * display the chrome snippet with id "TopChromeId" at position 10,10 on the screen.
 * \sa ChromeWidget
 * \sa ChromeSnippetJSObject
 */
class ChromeWidgetJSObject : public QObject {
    Q_OBJECT
  public:
    ChromeWidgetJSObject(QObject *parent, ChromeWidget *widget, const QString &objectName)
    : QObject(parent),
        m_chromeWidget(widget) {
        setObjectName(objectName);
    }

  public slots:
    /// Show the snippet with an element ID of 'id' at the position indicated.
    /// \sa ChromeSnippetJSObject::show
    void show(const QString& id, int x=0, int y=0) {
        assert(m_chromeWidget);
        m_chromeWidget->show(id, x, y);
    }
    
    /// Hide the snippet with an element ID of 'id'.
    /// \sa ChromeSnippetJSObject::hide
    void hide(const QString& id) {
        assert(m_chromeWidget);
        m_chromeWidget->hide(id);
    }
    
    /// Toggle the visibility of the given snippet.
    /// \sa ChromeSnippetJSObject::toggleVisibility
    void toggleVisibility(const QString& id) {
        assert(m_chromeWidget);
        m_chromeWidget->toggleVisibility(id);
    }
    
    /// Set the location of the given snippet.
    /// \sa ChromeWidgetJSObject::show() 
    void setLocation(const QString& id, int x, int y) {
        assert(m_chromeWidget);
        m_chromeWidget->setLocation(id, x, y);
    }
    
    /// Set the anchor of the given snippet.
    /// Possible values are "AnchorTop", "AnchorBottom", "AnchorCenter", "AnchorFullScreen".
    /// \sa ChromeSnippetJSObject::setAnchor 
    void setAnchor(const QString& id, const QString& anchor) {
        assert(m_chromeWidget);
        m_chromeWidget->setAnchor(id, anchor);
    }
    
    /// Toggle the attention animation of the given snippet.
    /// \sa ChromeSnippetJSObject::toggleAttention
    void toggleAttention(const QString& id) {
        assert(m_chromeWidget);
        m_chromeWidget->toggleAttention(id);
    }
    
    /// Set the visibility animation of the given snippet.
    /// Valid values of animatorName are "G_VISIBILITY_SLIDE_ANIMATOR", "G_VISIBILITY_FLYOUT_ANIMATOR",
    /// "G_VISIBILITY_MALSTROM_ANIMATOR" and "G_VISIBILITY_FADE_ANIMATOR".
    void setVisibilityAnimator(const QString& id, const QString & animatorName) {
        assert(m_chromeWidget);
        m_chromeWidget->setVisibilityAnimator(id, animatorName);
    }

    /// Set the visibility animation of the given snippet.
    /// Valid values of animatorName are "G_ATTENTION_BOUNCE_ANIMATOR" and "G_ATTENTION_PULSE_ANIMATOR".
    void setAttentionAnimator(const QString& id, const QString & animatorName) {
        assert(m_chromeWidget);
        m_chromeWidget->setAttentionAnimator(id, animatorName);
    }

    /// Update the child widgets
    void updateGeometry() {
        assert(m_chromeWidget);
        m_chromeWidget->updateChildGeometries();
    }

    /// Dump all snippets to qDebug().
    void dump() {
        assert(m_chromeWidget);
        m_chromeWidget->dump();
    }
    
  signals:
    /// Sent when the chrome starts loading.
    void loadStarted();
    
    /// Sent when the chrome has finished loading.
    void loadComplete();
    
    /// Sent when a snippet has started being dragged.
    void dragStarted();
    
    /// Sent when a snippet has finished being dragged.
    void dragFinished();
    
    /// Sent when the chrome viewport has been resized.
    void viewPortResize(QRect);    
  
    void symbianCarriageReturn();  // HACK

  private:
    ChromeWidget *m_chromeWidget;
};

#endif // CHROMEWIDGETJSOBJECT_H
