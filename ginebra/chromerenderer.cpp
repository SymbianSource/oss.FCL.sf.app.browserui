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


#include "chromerenderer.h"
#include "chromewidget.h"
#include <Qt>
#include <QNetworkProxy>
#include <QNetworkAccessManager>
// -------------------------------------------------------

// Simple wrapper class for QWebPage to allow intercepting of javascript errors in the chrome.
class ChromeWebPage : public QWebPage {
  public:
    ChromeWebPage(ChromeRenderer *renderer) : QWebPage(renderer) {
        qDebug() << "ChromeWebPage::ChromeWebPage";
        #if (defined(Q_OS_SYMBIAN) && !defined(Q_CC_NOKIAX86))
            // empty proxy only for ARMV5 Symbian targets
        #else
            networkAccessManager()->setProxy((QNetworkProxy(QNetworkProxy::HttpProxy,QString("bswebproxy01.americas.nokia.com"), 8080)));
        #endif
    }

    // Called when javascript errors are hit in the chrome page.
    virtual void javaScriptConsoleMessage(const QString & message, int lineNumber, const QString & sourceID) {
        qDebug() << "Chrome javascript error:";
        qDebug() << (const char*)QString("  %1:%2 %3")
                    .arg(sourceID)
                    .arg(lineNumber)
                    .arg(message).toAscii();
    }
};


// -------------------------------------------------------

ChromeRenderer::ChromeRenderer(QWidget *parent)
  : QWidget(parent),
    m_page(0),
    m_widget(0)
{
  setMouseTracking(true);
}

ChromeRenderer::~ChromeRenderer()
{
    if (m_page)
      m_page->setView(0);

    if (m_page && m_page->parent() == this)
        delete m_page;
}

QSize ChromeRenderer::sizeHint() const
{
    return QSize(1,1);
}

QWebPage *ChromeRenderer::page() const
{
    if (!m_page) {
        ChromeRenderer *that = const_cast<ChromeRenderer *>(this);
        that->setPage(new ChromeWebPage(that));
    }
    return m_page;
}

void ChromeRenderer::setPage(QWebPage* page){
  if (m_page == page)
    return;
  if (m_page) {
    if (m_page->parent() == this) {
      delete m_page;
    } else {
      m_page->disconnect(this);
    }
  }
  m_page = page;
  if (m_page) {
    m_page->setView(this);
    m_page->setPalette(palette());
    connect(m_page, SIGNAL(microFocusChanged()),
        this, SLOT(updateMicroFocus()));
  }
  //setAttribute(Qt::WA_OpaquePaintEvent, m_page); // Needed?

  update();
}


bool ChromeRenderer::event(QEvent *e)
{
 //qDebug() << "ChromeRenderer::event " << e;
 if (m_page) {
    if (e->type() == QEvent::ShortcutOverride) {
      m_page->event(e);

    } else if (e->type() == QEvent::Leave) {
      m_page->event(e);
    }
  }

  return QWidget::event(e);
}
/*
void ChromeRenderer::resizeEvent(QResizeEvent *e)
{
    if (m_page)
        m_page->setViewportSize(e->size());
}


void ChromeRenderer::paintEvent(QPaintEvent *ev)
{
  qDebug() << "ChromeRenderer::paintEvent: " << ev;
  //if (!m_widget)
  //      return;

    //QWebFrame *frame = m_page->mainFrame();
    //QPainter p(this);

    //frame->render(&p, ev->region());
    //m_widget->repaintRequested(ev->rect());

}

void ChromeRenderer::mouseMoveEvent(QMouseEvent* ev)
{
    if (m_page) {
        const bool accepted = ev->isAccepted();
        m_page->event(ev);
        ev->setAccepted(accepted);
    }
}

void ChromeRenderer::mousePressEvent(QMouseEvent* ev)
{
  qDebug() << "ChromeRenderer::mousePressEvent: " << ev;
    if (m_page) {
        const bool accepted = ev->isAccepted();
        m_page->event(ev);
        ev->setAccepted(accepted);
    }
}

void ChromeRenderer::mouseDoubleClickEvent(QMouseEvent* ev)
{
    if (m_page) {
        const bool accepted = ev->isAccepted();
        m_page->event(ev);
        ev->setAccepted(accepted);
    }
}

void ChromeRenderer::mouseReleaseEvent(QMouseEvent* ev)
{
    if (m_page) {
        const bool accepted = ev->isAccepted();
        m_page->event(ev);
        ev->setAccepted(accepted);
    }
}*/

void ChromeRenderer::keyPressEvent(QKeyEvent* ev)
{
    //qDebug() << "ChromeRenderer::keyPressEvent: " << ev;
    bool isAccepted(ev->isAccepted());
    if (m_page){
        m_page->event(ev);
        isAccepted = ev->isAccepted();
    }
#ifdef Q_OS_SYMBIAN //this is the hack for symbian fep key handler
    if(!isAccepted && (ev->key() == Qt::Key_Select || ev->key() == Qt::Key_Return || ev->key() == Qt::Key_Enter)) {
            emit symbianCarriageReturn();
            isAccepted = ev->isAccepted();
    }
#endif
    if (!ev->isAccepted()) {
        //qDebug() << "ChromeRenderer::keyPressEvent: -> accepted";
        QWidget::keyPressEvent(ev);
    }
}

void ChromeRenderer::keyReleaseEvent(QKeyEvent* ev)
{
    if (m_page)
        m_page->event(ev);
    if (!ev->isAccepted())
        QWidget::keyReleaseEvent(ev);
}

void ChromeRenderer::focusInEvent(QFocusEvent* ev)
{
    //qDebug() << "ChromeRenderer::focusInEvent: " << ev;
    if (m_page && ev->reason() != Qt::PopupFocusReason)
        m_page->event(ev);
    else
        QWidget::focusInEvent(ev);
}

void ChromeRenderer::focusOutEvent(QFocusEvent* ev)
{
    //qDebug() << "ChromeRenderer::focusOutEvent: " << ev;
    if (m_page)
        m_page->event(ev);
    else
        QWidget::focusOutEvent(ev);
}

bool ChromeRenderer::focusNextPrevChild(bool next)
{
    if (m_page && m_page->focusNextPrevChild(next))
        return true;
    return QWidget::focusNextPrevChild(next);
}

void ChromeRenderer::inputMethodEvent(QInputMethodEvent *e)
{
    if (m_page)
       m_page->event(e);
}

QVariant ChromeRenderer::inputMethodQuery(Qt::InputMethodQuery property) const
{
    if (m_page)
        return m_page->inputMethodQuery(property);

    return QVariant();
}

/*
void ChromeRenderer::changeEvent(QEvent *e)
{
    if (m_page && e->type() == QEvent::PaletteChange) {
        m_page->setPalette(palette());
    }
    QWidget::changeEvent(e);
}
*/
