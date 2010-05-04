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


#ifndef _GINEBRA_CHROME_DOM_H_
#define _GINEBRA_CHROME_DOM_H_

#include <QObject>
#include <QtCore/qvariant.h>
#include <QList>
#include <QHash>
#include <QWebElement>
#include "CachedHandler.h"

class QWebPage;
class QGraphicsItem;

namespace GVA {

class ChromeSnippet;
class ChromeWidget;
class ChromeRenderer;

class ChromeDOM : public QObject //TBD: Need QObject here?
{
  Q_OBJECT

public:
  ChromeDOM(QWebPage * page, ChromeWidget * chrome);
  ~ChromeDOM();
  QVariant evalInChromeContext(QString js);
  QWebElement getElementById(const QString &id);
  QSize getElementSize(const QString &id);
  QRect getElementRect(const QString &id);
  QString getElementAttribute(const QString &id, const QString &attribute);
  QList <QWebElement> getInitialElements();
  ChromeSnippet * getSnippet(const QString &docElementId, QGraphicsItem* parent = 0);
  //QString getCacheableScript();
  QList<CachedHandler> getCachedHandlers(const QString &elementId, const QRectF & ownerArea);
  int height() { return m_height; }
private:
  QWebPage * m_page;
  ChromeRenderer * m_renderer;
  ChromeWidget * m_chrome;
  int m_height;
  int m_bytes;
};

} // end of namespace GVA

#endif
