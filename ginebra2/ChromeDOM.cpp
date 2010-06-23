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

#include <QtGui>
#include <QWebElement>
#include <QWebFrame>
#include <QWebPage>

#include "ChromeDOM.h"
#include "ChromeRenderer.h"
#include "ChromeSnippet.h"
#include "ChromeWidget.h"
#include "WebChromeSnippet.h"
#include "WebChromeContainerSnippet.h"
#include "PageSnippet.h"
#include "PageItem.h"
#include "ProgressBarItem.h"
#include "ProgressBarSnippet.h"
#include "ToolbarChromeItem.h"
#include "ContentToolbarChromeItem.h"
#include "ContentToolbarSnippet.h"
#include "WindowToolbarSnippet.h"
#include "RecentUrlToolbarSnippet.h"
#include "BookmarksToolbarSnippet.h"
#include "SettingsToolbarSnippet.h"
#include "ActionButton.h"
#include "ActionButtonSnippet.h"
#include "mostvisitedpageview.h"
#include "mostvisitedsnippet.h"
#include "UrlSearchSnippet.h"
#include "EditorSnippet.h"

#include <QDebug>

//TODO: Replace JS strings with DOM api. Make stateful: i.e. get the doc element from the current page

namespace GVA {

  ChromeDOM::ChromeDOM(QWebPage * page, ChromeWidget * chrome)
    : m_page(page),
      m_chrome(chrome),
      m_height(0),
      m_bytes(0)
  {
    m_renderer = m_chrome->renderer();
    m_renderer->clearRenderList();
  }

  ChromeDOM::~ChromeDOM()
  {
  }

  QVariant ChromeDOM::evalInChromeContext(QString js){
    return m_page->mainFrame()->evaluateJavaScript(js);
  }

  QWebElement ChromeDOM::getElementById(const QString &id)
  {
    return m_page->mainFrame()->documentElement().findFirst("#" + id);
  }

  QRect ChromeDOM::getElementRect(const QString &id)
  {
    return getElementById(id).geometry();
  }

  QSize ChromeDOM::getElementSize(const QString &id)
  {
    QRect rect = getElementRect(id);
    return QSize(rect.width(), rect.height());
  }

  QString ChromeDOM::getElementAttribute(const QString &id, const QString &attribute)
  {
    return getElementById(id).attribute(attribute);
  }

  //Get the cacheable script element. Only one is allowed so get the first one.
  /*QString ChromeDOM::getCacheableScript()
  {
    QWebElement doc = m_page->mainFrame()->documentElement();
    return doc.findAll("script.GinebraCacheable").toList()[0].toPlainText();
  }
  */

  //Get a list of cached handlers for a snippet

  QList<CachedHandler> ChromeDOM::getCachedHandlers(const QString &elementId, const QRectF & ownerArea)
  {
    QWebElement snippet = getElementById(elementId);
    QList <QWebElement> controls =  snippet.findAll(".GinebraCached").toList();
    QList <CachedHandler> handlers;
    for (int i = 0; i < controls.size(); i++){
      QWebElement elem = controls.at(i);
      //Element rectangle relative to snippet, so we can handle mouse events relative to snippet
      //qDebug() << "====> Owner X: " << ownerArea.x() << " Owner Width: " << ownerArea.width() << " Elem X: " << elem.geometry().x() << " Elem Width: " << elem.geometry().width();
      QRectF elemRect(elem.geometry().x() - ownerArea.x(), elem.geometry().y() - ownerArea.y(), elem.geometry().width(), elem.geometry().height());
      //NB: For now we handle only onclick from cache. Should add at least long-press too.
      CachedHandler handler(elem.attribute("id"), elem.attribute("data-GinebraOnClick"), elemRect, m_chrome, elem.attribute("data-GinebraTargetView"));
      //qDebug() << "Cached handler" << handler.elementId() << ": "  << handler.script() << ": "  << handler.rect();
      handlers.append(handler);
    }
    return handlers;
  }

  ChromeSnippet *ChromeDOM::getSnippet(const QString &docElementId, QGraphicsItem* parent) {
    Q_UNUSED(parent)

    ChromeSnippet * result = 0;
    QWebElement doc = m_page->mainFrame()->documentElement();
    QWebElement element = doc.findFirst("#" + docElementId);
    QRect rect = element.geometry();//getElementRect(docElementId);
    m_height += rect.height();
    //m_bytes += rect.width() * rect.height() * 3; //Calculate total rendered area at 24 bit depth
    //qDebug() << "Chrome total rects at 24 bits: " << m_bytes;
    //    qDebug() << "Snippet: ID: " << docElementId << " Owner Area: " << rect << " Element Rect: " << element.geometry();
    if (!rect.isNull()) {
        QString className = element.attribute("data-GinebraNativeClass", "__NO_CLASS__");
        if (className == "__NO_CLASS__") {
            if (element.attribute("data-GinebraContainer", "false") == "true") {
                ChromeItem * widget = new ChromeItem(NULL);
                result = new WebChromeContainerSnippet(docElementId, m_chrome, rect, element,
                    widget);
                widget->setSnippet(result);
            }
            else {
                result = new WebChromeSnippet(docElementId, m_chrome, rect, element);
                m_renderer->addRenderItem((static_cast<WebChromeSnippet*> (result))->item());
            }
        }
        else {
     
            if (className == "ContentToolbar" ) {
                ContentToolbarChromeItem * widget = new ContentToolbarChromeItem();
                result = new ContentToolbarSnippet(docElementId, m_chrome, rect, element, widget);
                widget->setSnippet(result);
            }
            else if (className == "WindowToolbar" ) {
                ToolbarChromeItem * widget = new ToolbarChromeItem();
                result = new WindowToolbarSnippet(docElementId, m_chrome, rect, element, widget);
                widget->setSnippet(result);
            }
            else if (className == "RecentUrlToolbar" ) {
                ToolbarChromeItem * widget = new ToolbarChromeItem();
                result = new RecentUrlToolbarSnippet(docElementId, m_chrome, rect, element, widget);
                widget->setSnippet(result);
            }
            else if (className == "BookmarksToolbar" ) {
                ToolbarChromeItem * widget = new ToolbarChromeItem();
                result = new BookmarksToolbarSnippet(docElementId, m_chrome, rect, element, widget);
                widget->setSnippet(result);
            }
            else if (className == "SettingsToolbar" ) {
                ToolbarChromeItem * widget = new ToolbarChromeItem();
                result = new SettingsToolbarSnippet(docElementId, m_chrome, rect, element, widget);
                widget->setSnippet(result);
            }
            else if (className == "ToolBar" ) {
                ToolbarChromeItem * widget = new ToolbarChromeItem();
                result = new WebChromeContainerSnippet(docElementId, m_chrome, rect, element,
                    widget);
                widget->setSnippet(result);
            }
            else {
                QGraphicsWidget * w = 0;
                if (className == "MostVisitedPagesWidget") {
                    result = new MostVisitedSnippet(docElementId, m_chrome, 0, element);
                    MostVisitedPagesWidget* mostVisited;
                    mostVisited = new MostVisitedPagesWidget(result, m_chrome);
                    mostVisited->hide();
                    w = mostVisited;
                }
                else if (className == "ActionButton") {
                    result = new ActionButtonSnippet(docElementId, m_chrome, 0, element);
                    w = new ActionButton(result);
                }
                else if (className == "PageSnippet") {
                    result = new PageSnippet(docElementId, m_chrome, 0, element);
                    w = new PageItem(result, m_chrome);
                }
                else if (className == "ProgressBar") {
                    result = new ProgressBarSnippet(docElementId, m_chrome, 0, element);
                    w = new ProgressBarItem(result);
                }
                else if (className == "UrlSearchSnippet") {
                    result = new GUrlSearchSnippet(docElementId, m_chrome, 0, element);
                    w = new GUrlSearchItem(result, m_chrome);
                }
                else if (className == "TextEditSnippet") {
                    result = new EditorSnippet(docElementId, m_chrome, 0, element);
                    w = new TextEditItem(result, m_chrome);
                }
                else {
                    //Chrome snippet takes ownership of w
                    result = new ChromeSnippet(docElementId, m_chrome, 0, element);
                    w = new QGraphicsWidget();
                }
                result->setWidget(w);
                //Have snippet deterimine its own size when in anchor layout
                w->resize(rect.width(), rect.height());
                w->setPreferredSize(rect.width(), rect.height());
                w->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
                //Have snippet determine its own location when NOT in anchor layout
                w->setPos(rect.x(), rect.y());
            }
        }

        QWebElement parentElem;

        if (!(parentElem = findChromeParent(element)).isNull()) {
            result->setParentId(parentElem.attribute("id"));
        }
        //Set auto-layout attributes
        result->setAnchor(element.attribute("data-GinebraAnchor", "AnchorNone"), false);
        result->setAnchorOffset(element.attribute("data-GinebraAnchorOffset", "0").toInt());
        result->setInitiallyVisible(element.attribute("data-GinebraVisible", "false") == "true");
        result->setHidesContent(element.attribute("data-GinebraHidesContent", "false") == "true");
    }
    return result;
  }
  
  QWebElement ChromeDOM::findChromeParent(QWebElement element)
  {
    while(!(element = element.parent()).isNull()){
      if (element.attribute("class") == "GinebraSnippet"){
	return element;
      }
    }
    return element;
  }

  QList <QWebElement> ChromeDOM::getInitialElements()
  {
    m_renderer->clearRenderList();
    QWebElement test = getElementById("TestTableCell9");
    //qDebug() << "TEST ELEMENT:" << test.toPlainText();
    m_height = 0;
    QWebElement doc = m_page->mainFrame()->documentElement();
#if QT_VERSION < 0x040600 //TBD: Do we care, given that the dom api is not officially supported before 4.6?
    return doc.findAll(".GinebraSnippet");
#else
    return doc.findAll(".GinebraSnippet").toList();
#endif
  }

} // end of namespace GVA
