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


#ifndef CHROMEWIDGETJSOBJECT_H_
#define CHROMEWIDGETJSOBJECT_H_

#include <QObject>
#include <QUrl>

#include "ChromeWidget.h"

namespace GVA {

class ChromeWidget;

/*!
 * \defgroup JavascriptAPI Javascript API objects.
 * All classes in this group provide Javascript API access to the underlying
 * C++ classes in the browser via their public slots, signals and properties.
 */

/*! \ingroup JavascriptAPI
 * \brief Javascript API wrapper for ChromeWidget.
 */
class ChromeWidgetJSObject : public QObject {
  Q_OBJECT
public:
  ChromeWidgetJSObject(QObject *parent, ChromeWidget *chromeWidget)
    : QObject(parent),
      m_chromeWidget(chromeWidget) {
  }

public slots:
  int width() { return m_chromeWidget->width(); }
  void alert(const QString & msg) { m_chromeWidget->alert(msg); }
  qreal slideView(qreal delta) { return m_chromeWidget->slideView(delta); }
  void chromeInitialized() { m_chromeWidget->chromeInitialized(); }
  void reloadChrome() { m_chromeWidget->reloadChrome(); }
  void loadUrlToCurrentPage(const QString & url) { m_chromeWidget->loadUrlToCurrentPage(url); }

signals:
  void chromeComplete();
  void aspectChanged(int aspect);
  void prepareForGeometryChange();
  //NB: This should be symbian ifdef'd but that would require symbian-specific chrome
  void symbianCarriageReturn();

public:
  /// Returns a list of all existing snippets.
  QObjectList getSnippets() { return m_chromeWidget->getSnippets(); }
  Q_PROPERTY(QObjectList snippets READ getSnippets)

  /// \brief Returns either "portrait" or "landscape".
  QString getDisplayMode() const { return m_chromeWidget->getDisplayMode(); }
  Q_PROPERTY(QString displayMode READ getDisplayMode)

  QObject* getDisplaySize() const { return m_chromeWidget->getDisplaySize(); }
  Q_PROPERTY(QObject* displaySize READ getDisplaySize)

  /// Returns the path to the chrome directory.  Ex: "chrome/bedrockchrome".
  QString getBaseDirectory() const { return m_chromeWidget->getBaseDirectory(); }
  Q_PROPERTY(QString baseDirectory READ getBaseDirectory)

private:
  ChromeWidget *m_chromeWidget;
};

}

#endif /* CHROMEWIDGETJSOBJECT_H_ */
