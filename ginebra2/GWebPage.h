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


#ifndef __GINEBRA_GWEBPAGE_H__
#define __GINEBRA_GWEBPAGE_H__

#include <QDebug>
#include <QWebPage>
#include <QWebFrame>
#include "ChromeWidget.h"

namespace GVA {

  // ------------------------------
  // Simple wrapper class for QWebPage to allow interception of javascript errors.
  class WebPageWrapper : public QWebPage {
    public:
      WebPageWrapper(QObject *parent, const QString &prefix)
        : QWebPage(parent),
          m_prefix(prefix) {
        qDebug() << "WebPageWrapper::WebPageWrapper";
      }

      // Called when javascript errors are hit in the chrome page.
      virtual void javaScriptConsoleMessage(const QString & message, int lineNumber, const QString & sourceID) {
        qDebug() << m_prefix << ":";
        qDebug() << (const char*)QString("===\t%2:%3 %4")
              .arg(sourceID)
              .arg(lineNumber)
              .arg(message).toAscii();
      }
      QString m_prefix;
  };
  // ------------------------------

  class GWebPage : public QObject {
      Q_OBJECT
    public:
      GWebPage(QWebPage *page) {
        m_page = page;
      }

      Q_PROPERTY(QString name READ objectName)  // JS API
      Q_PROPERTY(QString title READ getTitle)  // JS API
      QString getTitle() {
        return m_page->mainFrame()->title();
      }

      QWebPage *page() { return m_page; }
      operator QWebPage *() { return m_page; }

      void dump() {
        qDebug() << "GWebPage::dump: " << this;
        qDebug() << "   page=" << (m_page ? m_page : 0);
      }

    protected:
      QWebPage *m_page;
  };

  // ------------------------------
  /*! \ingroup JavascriptAPI
   * \brief A content view that has full access to the Javascript APIs.
   *
   * Example code to load an HTML file into a super page:
   * \code
   * window.views.WebView.createSuperPage("BookmarkView", true);
   * window.views.WebView.BookmarkView.load("./chrome/BookmarkView.html");
   * \endcode
   */
  class GSuperWebPage : public GWebPage {
      Q_OBJECT
    public:
      GSuperWebPage(WebPageWrapper *page, ChromeWidget *chromeWidget)
        : GWebPage(page),
          m_chromeWidget(chromeWidget)
      {
        if(!m_page) {
          m_page = new WebPageWrapper(this, "Superpage javascript error");
        }
        qDebug() << "GSuperWebPage::GSuperWebPage: page=" << GWebPage::page();
        connect(GWebPage::page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(onJavaScriptWindowObjectCleared()));
      }
    public slots:
      void load(const QString &url) {   // JS API
        qDebug() << "GSuperWebPage::load: " << url;
        page()->mainFrame()->load(url);
      }
    private slots:
      void onJavaScriptWindowObjectCleared() {
        qDebug() << "GSuperWebPage::onJavaScriptWindowObjectCleared: " << objectName();
        if(m_chromeWidget)
          m_chromeWidget->exportJSObjectsToPage(m_page);
      }

    private:
      ChromeWidget *m_chromeWidget;  // not owned
  };
}

#endif // __GINEBRA_GWEBPAGE_H__
