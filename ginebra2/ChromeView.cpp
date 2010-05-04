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


#include <QtGui>
#include <QWebPage>

#include "ChromeView.h"
#include "ChromeWidget.h"
#ifndef NO_QSTM_GESTURE
#include "qstmgestureevent.h"
#endif

#ifdef Q_OS_SYMBIAN
#ifdef SET_DEFAULT_IAP
#include "sym_iap_util.h"
#endif //SET_DEFAULT_IAP
#endif //Q_OS_SYMBIAN

namespace GVA {

ChromeView::ChromeView(ChromeWidget * chrome, QWidget * parent)
  : QGraphicsView(chrome->scene(), parent),
    m_topWidget(chrome)
{
  //setGeometry(chrome->geometry().toRect());
  setObjectName("ChromeView");
  //When content view is external widget, make the background transparent
  //setStyleSheet("QGraphicsView#ChromeView {margin:0; border: 0; padding:0; background:transparent}");
  setStyleSheet("QGraphicsView#ChromeView {margin:0; border: 0; padding:0; background:#fff}");
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  //NB: maybe not needed?
  setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
  //installEventFilter(this);
  //chrome->page()->setView(this);
#ifndef NO_QSTM_GESTURE
  ungrabGesture(Qt::PanGesture);
  ungrabGesture(Qt::TapGesture);
  ungrabGesture(Qt::TapAndHoldGesture);
  ungrabGesture(Qt::PinchGesture);
  ungrabGesture(Qt::SwipeGesture);
#endif

#ifdef Q_OS_SYMBIAN
#ifdef SET_DEFAULT_IAP
    QTimer::singleShot(0, this, SLOT(setDefaultIap()));
  //setDefaultIap();
#endif //SET_DEFAULT_IAP
#endif //Q_OS_SYMBIAN
}
  
ChromeView::~ChromeView()
{

}

void ChromeView::resizeEvent(QResizeEvent * ev)
{
  //Resize the chrome to match the view and scene rectangle size

  if(m_topWidget)
     m_topWidget->setGeometry(0,0, ev->size().width(), ev->size().height());
  QGraphicsView::resizeEvent(ev);
}

//Never scroll the chrome
//NB: this shouldn't be needed, but some events from
//the chrome are causing scrolling. Need to track this
//down further.

void ChromeView::scrollContentsBy(int dx, int dy)
{
  // qDebug() << "View scroll";
  //  QGraphicsView::scrollContentsBy(dx, dy);
}

  //Eat key events not otherwise consumed.
  /*  bool ChromeView::eventFilter(QObject * obj, QEvent * ev)
  {
   if(ev->type() == QEvent::KeyPress){
   		int key = static_cast<QKeyEvent*>(ev)->key();
   		if(key == Qt::Key_Down || key == Qt::Key_Up || key ==Qt::Key_Left){
         return true;
       }
    }
    return QObject::eventFilter(obj,ev);
 
    }*/
  
bool ChromeView::event(QEvent* event)
{
#ifndef NO_QSTM_GESTURE
	  if (event->type() == QEvent::Gesture) {
	      QStm_Gesture* gesture = getQStmGesture(event);
	      if (gesture) {
			  QPoint pos = mapFromGlobal(gesture->position());
	    	  QGraphicsScene* gs = scene();
			  QGraphicsItem* gi = gs->itemAt(QPointF(pos));

			  if (gi) {
			      gs->sendEvent(gi, event);
			  }
			  return true;
	      }
	  }	  
#endif
	return QGraphicsView::event(event);
}

#ifdef Q_OS_SYMBIAN
#ifdef SET_DEFAULT_IAP
void ChromeView::setDefaultIap()
{
#ifdef ENABLE_PERF_TRACE
    PERF_DEBUG() << "Setting up default IAP.\n";
#endif

    qt_SetDefaultIap();
}
#endif // SET_DEFAULT_IAP
#endif // Q_OS_SYMBIAN

} // namespace GVA