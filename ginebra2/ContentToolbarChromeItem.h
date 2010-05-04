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


#ifndef __GINEBRA_CONTENTTOOLBARCHROMEITEM_H
#define __GINEBRA_CONTENTTOOLBARCHROMEITEM_H

#include <QtGui>
#include "ToolbarChromeItem.h"

class QTimeLine;
class QTimer;

namespace GVA {

  class ToolbarFadeAnimator: public QObject
  {

    Q_OBJECT

    public:

      ToolbarFadeAnimator();
      ~ToolbarFadeAnimator();
      void start(bool visible);
      void stop();


    private slots:
      void valueChange(qreal step);

    Q_SIGNALS:
      void updateVisibility(qreal step);
      void finished();

    private:
      QTimeLine *m_timeLine;
      
  };

  class ContentToolbarChromeItem : public ToolbarChromeItem
  {
    Q_OBJECT



    enum  ContentToolbarState {

      CONTENT_TOOLBAR_STATE_FULL, 
      CONTENT_TOOLBAR_STATE_PARTIAL, 
      CONTENT_TOOLBAR_STATE_ANIM_TO_PARTIAL, 
      CONTENT_TOOLBAR_STATE_ANIM_TO_FULL, 
      CONTENT_TOOLBAR_STATE_INVALID 
    };

    enum ContentToolbarInactivityTimerState {

      CONTENT_TOOLBAR_INACTIVITY_TIMER_NONE,
      CONTENT_TOOLBAR_INACTIVITY_TIMER_ALLOWED

    };
/*
    typedef void (ContentToolbarChromeItem::*EnterFunctionType)(bool);
    typedef void (ContentToolbarChromeItem::*ExitFunctionType)();

    struct ContentToolbarState_t {
    
      EnterFunctionType enterFunc;
      ExitFunctionType exitFunc;

    };

*/
    public:
      ContentToolbarChromeItem(QGraphicsItem* parent = 0);
      virtual ~ContentToolbarChromeItem();
      virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget);
      virtual void setSnippet(WebChromeContainerSnippet * snippet);

    protected:
      virtual void resizeEvent(QGraphicsSceneResizeEvent * ev);
  
  
    private slots:
      void onChromeComplete(); 
      void onLoadStarted(); 
      void onLoadFinished(bool); 
      void onInactivityTimer();
      void onAnimFinished();
      void onUpdateVisibility(qreal);
      void onSnippetMouseEvent( QEvent::Type type);
      void onWebViewMouseEvents( QEvent::Type type);
      void onSnippetShow();
      void onSnippetHide();
      void onMVCloseComplete();

    private:
      void handleMousePress();
      void handleMouseRelease();
      void addFullBackground();
      void changeState( ContentToolbarState state, bool animate = false);
      void onStateEntry(ContentToolbarState state, bool animate);
      void onStateExit(ContentToolbarState state);
      void resetInactivityTimer();
      bool mvSnippetVisible();


      //void initStates(); 

      // State Enter and Exit functions
      void  stateEnterFull(bool);
      void  stateEnterPartial(bool animate=false);
      void  stateEnterAnimToPartial(bool animate =false);
      void  stateEnterAnimToFull(bool animate =false);

      ToolbarFadeAnimator * m_animator;
      QPainterPath* m_background;
      ChromeSnippet* m_middleSnippet;
      QTimer* m_inactivityTimer;
      qreal m_bgopacity;
      ContentToolbarState m_state;
      ContentToolbarInactivityTimerState m_inactiveTimerState;
    
      //ContentToolbarState_t m_states[10];
      

  };

} // end of namespace GVA

#endif // __GINEBRA_CONTENTTOOLBARCHROMEITEM_H
