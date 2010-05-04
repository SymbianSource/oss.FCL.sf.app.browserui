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


#include "UrlSearchSnippet.h"
#include "Utilities.h"

#include "ChromeRenderer.h"
#include "ChromeWidget.h"
#include "ViewController.h"
#include "WebChromeSnippet.h"

#include "webpagecontroller.h"

namespace GVA {

// Methods for class UrlEditorWidget

UrlEditorWidget::UrlEditorWidget(QGraphicsItem * parent)
: QGraphicsTextItem(parent)
{
    // Disable wrapping, force text to be stored and displayed
    // as a single line.

    QTextOption textOption = document()->defaultTextOption();
    textOption.setWrapMode(QTextOption::NoWrap);
    document()->setDefaultTextOption(textOption);

    // Enable cursor keys.

    setTextInteractionFlags(Qt::TextEditorInteraction);

    // This is needed to initialize m_textLine.

    setText("");
}

UrlEditorWidget::~UrlEditorWidget()
{
}

void UrlEditorWidget::setText(const QString & text)
{
    setPlainText(text);
    m_textLine = document()->begin().layout()->lineForTextPosition(0);
}

qreal UrlEditorWidget::cursorX()
{
    return m_textLine.cursorToX(textCursor().position());
}

void UrlEditorWidget::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    // Paint without ugly selection ants (the dashed line that surrounds
    // the selected text).

    QStyleOptionGraphicsItem newOption = *option;
    newOption.state &= (!QStyle::State_Selected | !QStyle::State_HasFocus);

    painter->save();

    QGraphicsTextItem::paint(painter, &newOption, widget);

    painter->restore();
}

void UrlEditorWidget::keyPressEvent(QKeyEvent * event)
{
    // Signal horizontal cursor movement so UrlSearchSnippet can
    // implement horizontal scrolling.

    qreal oldX = cursorX();

    QGraphicsTextItem::keyPressEvent(event);

    qreal newX = cursorX();

    if (newX != oldX) {
        emit cursorXChanged(newX);
    }
}

// Methods for class UrlSearchSnippet

UrlSearchSnippet::UrlSearchSnippet(ChromeSnippet * snippet, ChromeWidget * chrome, QGraphicsItem * parent)
: NativeChromeItem(snippet, parent)
, m_chrome(chrome)
, m_percent(0)
, m_pendingClearCalls(0)
, m_viewPortWidth(0.0)
, m_viewPortHeight(0.0)
{
    setFlags(QGraphicsItem::ItemIsMovable);

    // Extract style information from element CSS.

    // For border-related properties, we constrain all values (top, left, etc.)
    // to be the same.  These can be set using the css shorthand (e.g. padding:10px),
    // but the computed css style will be for the four primitive values (padding-top,
    // padding-left) etc, which will all be equal.  Hence we just use one of the
    // computed primitive values (top) to represent the common value.

    QWebElement we = m_snippet->element();

    NativeChromeItem::CSSToQColor(
            we.styleProperty("color", QWebElement::ComputedStyle),
            m_textColor);

    NativeChromeItem::CSSToQColor(
            we.styleProperty("background-color", QWebElement::ComputedStyle),
            m_backgroundColor);

    NativeChromeItem::CSSToQColor(
            we.styleProperty("border-top-color", QWebElement::ComputedStyle),
            m_borderColor);

    QString cssPadding = we.styleProperty("padding-top", QWebElement::ComputedStyle);
    m_padding = cssPadding.remove("px").toInt();

    QString cssBorder = we.styleProperty("border-top-width", QWebElement::ComputedStyle);
    m_border = cssBorder.remove("px").toInt();

    // The viewport clips the editor when text overflows

    m_viewPort = new QGraphicsWidget(this);
    m_viewPort->setFlags(QGraphicsItem::ItemClipsChildrenToShape);

    // The actual text editor item

    m_editor = new UrlEditorWidget(m_viewPort);
    m_editor->setDefaultTextColor(m_textColor);
    m_editor->installEventFilter(this);

    // Monitor editor cursor position changes for horizontal scrolling.

    safe_connect(m_editor, SIGNAL(cursorXChanged(qreal)),
            this, SLOT(makeVisible(qreal)));

    // Monitor resize events.

    safe_connect(m_chrome->renderer(), SIGNAL(chromeResized()),
            this, SLOT(resize()));

    // Update state as soon as chrome completes loading.

    safe_connect(m_chrome, SIGNAL(chromeComplete()),
            this, SLOT(setStarted()));

    // Monitor page loading.

    WebPageController * pageController = WebPageController::getSingleton();

    safe_connect(pageController, SIGNAL(pageUrlChanged(const QString)),
            this, SLOT(setUrlText(const QString &)));

    safe_connect(pageController, SIGNAL(pageLoadStarted()),
            this, SLOT(setStarted()));

    safe_connect(pageController, SIGNAL(pageLoadProgress(const int)),
            this, SLOT(setProgress(int)));

    safe_connect(pageController, SIGNAL(pageLoadFinished(bool)),
            this, SLOT(setFinished(bool)));

    // Monitor view changes.

    ViewController * viewController = chrome->viewController();

    safe_connect(viewController, SIGNAL(currentViewChanged()),
            this, SLOT(viewChanged()));
}

UrlSearchSnippet::~UrlSearchSnippet()
{
}

bool UrlSearchSnippet::eventFilter(QObject * object, QEvent * event)
{
    // Filter editor key events.

    if (object != m_editor) {
        return false;
    }

    if (event->type() != QEvent::KeyPress) {
        return false;
    }

    QKeyEvent * keyEvent = static_cast<QKeyEvent*>(event);

    switch (keyEvent->key()) {
    case Qt::Key_Select:
    case Qt::Key_Return:
    case Qt::Key_Enter:
        // Signal that a carriage return-like key-press happened.
        emit activated();
        return true;

    case Qt::Key_Down:
    case Qt::Key_Up:
        // Swallow arrow up/down keys, editor has just one line.
        return true;

    default:
        return false;
    }
}

void UrlSearchSnippet::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    // Make sure any required horizontal scrolling happens
    // before rendering UrlEditorWidget.

    makeVisible(m_editor->cursorX());

    NativeChromeItem::paint(painter, option,widget);

    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(m_backgroundColor);

    // First, do progress bar.

    QRectF g = boundingRect();
    g.setWidth(g.width() * m_percent / 100.0);
    painter->fillRect(g, QColor::fromRgb(0, 200, 200, 50));

    // Next, background matte.

    if (m_border > 0) {
        QPen pen;
        pen.setWidth(m_border);
        pen.setBrush(m_borderColor);
        painter->setPen(pen);
    }

    QPainterPath background;
    background.addRect(boundingRect());
    background.addRoundedRect(
            m_padding,
            m_padding,
            m_viewPortWidth,
            m_viewPortHeight,
            4,
            4);
    painter->drawPath(background);

    painter->restore();
}

void UrlSearchSnippet::resizeEvent(QGraphicsSceneResizeEvent * event)
{
    QSizeF size = event->newSize();

    m_viewPort->resize(size);

    m_viewPortWidth  = size.width()  - m_padding * 2;
    m_viewPortHeight = size.height() - m_padding * 2;

    m_viewPort->setGeometry(
            m_padding,
            (size.height() - m_editor->boundingRect().height()) / 2,
            m_viewPortWidth,
            m_viewPortHeight);

    m_editor->setTextWidth(m_viewPortWidth);
}

void UrlSearchSnippet::resize()
{
    QWebElement we = m_snippet->element();

    QRectF g = we.geometry();

    qreal newWidth  = g.width();

    qreal newHeight = g.height();

    QGraphicsWidget::resize(newWidth, newHeight);
}

void UrlSearchSnippet::setUrlText(const QString & text)
{
    m_editor->setText(text);
    m_editor->setPos(0, m_editor->pos().y());

    makeVisible(m_editor->cursorX());
}

void UrlSearchSnippet::setStarted()
{
    // Strictly speaking we should set progress to 0.
    // But set it higher to give immediate visual feedback
    // that something is happening.

    int progress = 0;

    WebPageController * pageController = WebPageController::getSingleton();

    if (pageController->isPageLoading()) {
        progress = 5;
    }

    setProgress(progress);
}

void UrlSearchSnippet::setProgress(int percent)
{
    m_percent = percent;
    update();
}

// Wait a half-second before actually clearing the progress bar.
//
// We have to be careful of the following two use cases:
//
// 1. Another page starts loading between the call to setFinished()
//    and the scheduled call to clearProgress().
//
//    We don't want to clear the progress bar if another page is
//    loading.  WebPageController::isPageLoading() can tell us
//    if that is the case.
//
// 2. Another page finishes loading between the call to setFinished()
//    and the scheduled call to clearProgress().  The sequence here is:
//
//      setFinished(ok) // for URL #1
//      setFinished(ok) // for URL #2
//      clearProgress() // for URL #1
//      clearProgress() // for URL #2
//
//    We don't want to clear the progress bar in the first call to
//    clearProgress() because we want the progress bar to retain its
//    appearance for the full timeout period.  We manage this by
//    tracking the number of pending calls to clearProgress() and
//    only clearing the progress bar when that number becomes 0.

void UrlSearchSnippet::setFinished(bool ok)
{
    if (ok) {
        setProgress(99);
    }

    ++m_pendingClearCalls;

    QTimer::singleShot(500, this, SLOT(clearProgress()));
}

void UrlSearchSnippet::clearProgress()
{
    --m_pendingClearCalls;

    if (m_pendingClearCalls > 0) {
        return;
    }

    WebPageController * pageController = WebPageController::getSingleton();

    if (pageController->isPageLoading()) {
        return;
    }

    setProgress(0);
}

void UrlSearchSnippet::viewChanged()
{
    WebPageController * pageController = WebPageController::getSingleton();

    setUrlText(pageController->currentDocUrl());

    int progress = pageController->loadProgressValue();
    if (progress >= 100) {
        progress = 0;
    }
    setProgress(progress);
}

// We divide the viewport into 3 distinct regions:
//
//
//        [ left | middle | right ]
//
// [ editor, shifted left by editorShift pixels ]
//
// When a cursor is in the middle section of the viewport we
// leave the editor shift unchanged, to preserve stability.
//
// When a cursor is in the right section or beyond we shift
// the editor left until the cursor appears at the border
// between the middle and right sections.
//
// When a cursor is in the left section or beyond we shift
// the editor right until the cursor appears at the border
// between the left and middle sections.
//
// We never shift the editor right of the viewport.

void UrlSearchSnippet::makeVisible(qreal cursorX)
{
    qreal leftScrollBorder  = 0;

    qreal rightScrollBorder = m_viewPortWidth - 10;

    qreal editorShift = -1 * m_editor->pos().x();

    qreal localX = cursorX - editorShift;

    if (localX < leftScrollBorder) {
        // Before left section, scroll right.
        // In left section, scroll right.
        qreal shift = qMin(leftScrollBorder - localX, editorShift);
        m_editor->moveBy(shift, 0);
        return;
    }

    if (localX >= rightScrollBorder) {
        // In right section, scroll left.
        // After right section, scroll left.
        qreal shift = localX - rightScrollBorder;
        m_editor->moveBy(-shift, 0);
        return;
    }

    // In middle section, no scroll needed.
    return;
}

} // namespace GVA
