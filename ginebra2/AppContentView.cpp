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


#include "AppContentView.h"
#include "ChromeWidget.h"
#include <QGraphicsWebView>
#include <QWebPage>
#include <QWebFrame>
#ifndef NO_QSTM_GESTURE
#include "qstmgestureevent.h"
#endif
#include <QDebug>



namespace GVA 
{

  AppContentView::AppContentView(ChromeWidget * chrome, QObject * parent)
    : ContentViewDelegate(chrome, parent),
      m_view(new QGraphicsWebView),
      m_page(new QWebPage),
      m_timeLine(0),
      m_zoomIn(false) 
  {
#ifndef NO_QSTM_GESTURE
    m_view->installEventFilter(this);
#endif  
    m_page->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    m_page->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    //NB: Here's how to set default webview backgound color
    QPalette viewPalette = m_view->palette();
    viewPalette.setBrush(QPalette::Base, Qt::white);
    //viewPalette.setColor(QPalette::Window, Qt::transparent);
    m_page->setPalette(viewPalette);
    m_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_view->setPage(m_page);
    setObjectName("appView");
    QObject::connect(m_view, SIGNAL(titleChanged(const QString &)), this, SIGNAL(titleChanged(const QString &)));
    QObject::connect(m_view, SIGNAL(loadStarted()), this, SIGNAL(loadStarted()));
    QObject::connect(m_view, SIGNAL(loadProgress(int)), this, SIGNAL(loadProgress(int)));
    QObject::connect(m_view, SIGNAL(loadFinished(bool)), this, SIGNAL(loadFinished(bool)));
    QObject::connect(m_view, SIGNAL(urlChanged(const QUrl&)), this, SLOT(onUrlChanged(const QUrl&)));
    QObject::connect(m_page->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SIGNAL(javaScriptWindowObjectCleared()));
 }
  
  AppContentView::~AppContentView()
  {
    delete m_timeLine;
    delete m_page;
    delete m_view;
  }

  void AppContentView::load(const QString& url)
  {
    // TO DO: need to filter out non-local URLs.  Should only load local files.
      
	qDebug() << "AppContentView::load: " << url;
    m_page->mainFrame()->load(QUrl(url));
    //m_page->setViewportSize(m_page->mainFrame()->contentsSize()); //NB:?
  }

  void AppContentView::setHtml(const QString& html)
  {
    m_view->setHtml(html);
    // m_page->setViewportSize(m_page->mainFrame()->contentsSize()); //NB:?
  }  

  void AppContentView::triggerAction(const QString & action)
  {
    QWebPage::WebAction webAction;
    if(action=="Stop")
      webAction = QWebPage::Stop;
    else if (action=="Back")
      webAction = QWebPage::Back;
    else if(action=="Reload")
      webAction = QWebPage::Reload;
    else if (action=="Forward")
      webAction = QWebPage::Forward;
    else
      return;
    m_view->triggerPageAction(webAction);
  } 

  void AppContentView::stop()
  {
    m_view->stop();
  }
  
  void AppContentView::back()
  {
    m_view->back();
  }
  
  void AppContentView::forward()
  {
    m_view->forward();
  }
  
  void AppContentView::reload()
  {
    m_view->reload();
  }
  
  void AppContentView::zoomBy(qreal delta)
  {
    m_page->mainFrame()->setZoomFactor(m_page->mainFrame()->zoomFactor() + delta);
  }

  void AppContentView::scrollBy(int deltaX, int deltaY)
  {
    m_page->mainFrame()->setScrollPosition(m_page->mainFrame()->scrollPosition() + QPoint(deltaX, deltaY));
  }

  int AppContentView::scrollX()
  {
    return m_page->mainFrame()->scrollPosition().x();
  }

  int AppContentView::scrollY()
  {
    return m_page->mainFrame()->scrollPosition().y();
  }

  int AppContentView::contentWidth()
  {
    return m_page->mainFrame()->contentsSize().width();
  }

  int AppContentView::contentHeight()
  {
    return m_page->mainFrame()->contentsSize().height();
  }

  void AppContentView::onUrlChanged(const QUrl& url)
  {
    emit urlChanged(url.toString());
  }


  void AppContentView::updateZoom(qreal delta){
    if(m_zoomIn)
      zoomBy(0.1);
    else
      zoomBy(-0.1);
  }


  void AppContentView::zoom(bool in)
  {
    m_zoomIn = in;
    if(!m_timeLine) {
      m_timeLine = new QTimeLine();
      connect(m_timeLine, SIGNAL(valueChanged(qreal)),
	      this, SLOT(updateZoom(qreal)));
    }
    else {
      m_timeLine->stop();
    }
    m_timeLine->start();
  }

  void AppContentView::toggleZoom(){
    zoom(!m_zoomIn);
  }

  void AppContentView::stopZoom() {
    m_timeLine->stop();
  }

  void AppContentView::addJSObjectToWindow(QObject *object) {
    m_page->mainFrame()->addToJavaScriptWindowObject(object->objectName(), object);
  }
#ifndef NO_QSTM_GESTURE
  bool AppContentView::eventFilter(QObject* object, QEvent* event)
  {
	  if (event->type() == QEvent::Gesture) {
		  QStm_Gesture* gesture = getQStmGesture(event);
		  if (gesture) {
		      QStm_GestureType gtype = gesture->getGestureStmType();
		  	  if (gtype == QStmTapGestureType) {
				  QPoint gpos = gesture->position();
				  QWidget* topWidget = QApplication::topLevelAt(gpos);
				  QPoint pos = topWidget->mapFromGlobal(gpos);
				  QWidget* w = topWidget->childAt(pos);
				  gesture->sendMouseEvents(w);	   
				  return true;
			  }  
		  }
		  return true;
	  }
	  return false;
  }
#endif
} // end of namespace GVA

