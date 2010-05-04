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


#ifndef _CHROMESNIPPETJSOBJECT_H_INCLUDED
#define _CHROMESNIPPETJSOBJECT_H_INCLUDED

#include "chromesnippet.h"
#include "utilities.h"
#include "visibilityanimator.h"
#include "attentionanimator.h"

/*! \brief Javascript wrapper for ChromeSnippets.  
 * The ChromeSnippetJSObject class is a Javascript wrapper for ChromeSnippets.
 * All signals, slots and properties of chrome snippets are accessible from javascript.  Snippets are children of the 
 * \c window.snippets object and as such can be accessed using their DOM element ids.
 * For example, given a snippet defined in HTML like this:
 * \code
 * <div class="GinebraSnippet" id="ExampleSnippet" name="example" 
 *      data-GinebraAnchor="AnchorTop" data-GinebraVisible="false">
 *      ...
 * </div>
 * \endcode
 * You can show it on-screen from Javascript like this:
 * \code
 * window.snippets.ExampleSnippet.show();
 * \endcode  
 */
class ChromeSnippetJSObject : public QObject {
    Q_OBJECT
  public:
    ChromeSnippetJSObject(QObject *parent, ChromeSnippet &snippet, const QString &objName) 
	  : QObject(parent),
		m_snippet(snippet) {
        setObjectName(objName);
    }
    
  signals:
    // Sent when the snippet starts being dragged.
    void dragStarted();

    // Sent when the snippet has finished being dragged.
    void dragFinished();
    
    // Sent when the snippet is shown.
    void onShow();

    // Sent when the snippet is hidden.
    void onHide();
  
  public slots:
    // Show the snippet.  If useAnimation is true the animation set with
    // setVisibilityAnimator() will be used.
    // \sa setVisibilityAnimator
    void show(bool useAnimation = true) { m_snippet.show(useAnimation); }

    // Hide the snippet.  If useAnimation is true the animation set with
    // setVisibilityAnimator() will be used.
    // \sa setVisibilityAnimator
    void hide(bool useAnimation = true) { m_snippet.hide(useAnimation); }

    // Toggle the visibility of the snippet.  If it is hidden this function
    // will show it and vice versa.  Visibility animations will be used.
    void toggleVisibility() { m_snippet.toggleVisibility(); }

    // Toggle attention animation of the snippet.
    // \sa setAttentionAnimator
    void toggleAttention() { m_snippet.toggleAttention(); }

    // Starts an animation timer.  Returns an animation object that can rotate, move or fade the snippet.
    // 
    // \sa GraphicsItemAnimation
    QObject *animate(int duration) { return m_snippet.animate(duration); }

    // Set the position of the snippet's upper-left corner on the screen.
    // \sa position
    void setPosition(int x, int y) { m_snippet.setPosition(x, y); }
    QString getDisplayMode(){ return m_snippet.getDisplayMode(); }
    bool getContextMenuFlag(){ return ChromeSnippet::getContextMenuFlag();}
    
    // Set the animation that is triggered when the snippet is shown or hidden.
    // \param animatorName Possible values are:
    // \li "G_VISIBILITY_FLYOUT_ANIMATOR"
    // \li "G_VISIBILITY_FADE_ANIMATOR"
    // \li "G_VISIBILITY_SLIDE_ANIMATOR"
    // \li "G_VISIBILITY_MALSTROM_ANIMATOR"
    void setVisibilityAnimator(const QString &animatorName) { m_snippet.setVisibilityAnimator(animatorName); }
    
    // Set the animation that is triggered when the snippet wants attention.
    // \param animatorName Possible values are:
    // \li "G_ATTENTION_BOUNCE_ANIMATOR"
    // \li "G_ATTENTION_PULSE_ANIMATOR"
    void setAttentionAnimator(const QString &animatorName) { m_snippet.setAttentionAnimator(animatorName); }

    // return true if snippet graphicItem is visible
    bool isVisible() {return m_snippet.isVisible();}
    
    // Print info about the snippet to debug output.
    void dump() const { m_snippet.dump(); }
    

    // repaint the snippet
    void repaint() { m_snippet.repaint();}

  public:
    /*! The id of the DOM element of this snippet.  This is the value set in
     * the HTML id tag in the chrome file. 
     * In the example below "ExampleSnippet" is the id.
     * \code 
     * <div class="GinebraSnippet" id="ExampleSnippet" name="example" 
     *      data-GinebraAnchor="AnchorTop" data-GinebraVisible="true">
     * ...
     * </div>
     * \endcode
     */
    Q_PROPERTY(QString id READ getId)
    QString getId() const { return m_snippet.docElementId(); }
    
    /*! The snippet's screen geometry.
     * Example javascript code:
     * \code var width = window.snippets.ExampleSnippet.geometry.width;
     * \endcode
     */
    Q_PROPERTY(QObject * geometry READ getGeometry)
    // \sa geometry
    QObject *getGeometry() const { return m_snippet.getGeometry(); }
    
    /*! 
     * \property position
     * \brief The position of the snippet's top-left corner.
     * Example javascript code:
     * \code var x = window.snippets.ExampleSnippet.position.x;
     * \endcode
     */
    Q_PROPERTY(QObject * position READ getPosition)
    // \sa position
    QObject *getPosition() const { return m_snippet.getPosition(); }
    
    /*! 
     * \property draggable
     * \brief True if the snippet can be dragged with the mouse/touch.  Defaults to false.
     * \sa dragStarted
     * \sa dragFinished
     */    
    Q_PROPERTY(bool draggable READ getDraggable WRITE setDraggable)
    // \sa draggable
    bool getDraggable() const { return m_snippet.draggable(); }
    // \sa draggable
    void setDraggable(bool value) { m_snippet.setDraggable(value); }
    
    // The z-order value of the snippet.
    Q_PROPERTY(qreal zValue READ getZValue WRITE setZValue)
    // \sa zValue
    qreal getZValue() const { return m_snippet.zValue(); }
    // \sa zValue
    void setZValue(qreal value) { m_snippet.setZValue(value); }

    /*!
     * \property isHiding
     * \brief True if the snippet is hiding behind other snippets.
     */
    bool isHiding() const { return m_snippet.isHiding(); }
    void setHiding(bool value) { return m_snippet.setHiding(value); }
    Q_PROPERTY(int isHiding READ isHiding WRITE setHiding)
    
    // Determines where the snippet is displayed.  When "AnchorTop" or "AnchorBottom"
    // are used the size of the content viewport is reduced to allow the snippet to
    // fit in above or below it.  The default value is "AnchorNone".
    // Possible values are:
    // \li "AnchorTop"
    // \li "AnchorBottom"
    // \li "AnchorCenter"
    // \li "AnchorFullScreen"
    // \li "AnchorNone" - Position is determined by the \ref position property.
    Q_PROPERTY(QString anchor READ getAnchor WRITE setAnchor)
    // \sa anchor
    QString getAnchor() { return m_snippet.anchor(); }
    // \sa anchor
    void setAnchor(const QString& anchor) { m_snippet.setAnchor(anchor); }
    
    Q_PROPERTY(int anchorOffset READ getAnchorOffset WRITE setAnchorOffset)
    // \sa anchorOffset
    int getAnchorOffset() { return m_snippet.anchorOffset(); }
    // \sa anchorOffset
    void setAnchorOffset(int anchorOffset) { m_snippet.setAnchorOffset(anchorOffset); }
    
    // Set to true if the snippet should cover the content viewport.
    Q_PROPERTY(bool hidesContent READ getHidesContent WRITE setHidesContent)
    // \sa hidesContent
    bool getHidesContent(){ return m_snippet.hidesContent();}
    // \sa hidesContent
    void setHidesContent(bool hidesContent) { m_snippet.setHidesContent(hidesContent); }
    
  private:
    ChromeSnippet &m_snippet;
    
    friend class ChromeSnippet;
};


#endif  // _CHROMESNIPPETJSOBJECT_H_INCLUDED
