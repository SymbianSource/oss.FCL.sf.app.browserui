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

#ifndef QSTMGESTUREENGINE_H_
#define QSTMGESTUREENGINE_H_

#include "qstmgestureengine_if.h"
#include "qstmuievent_if.h"
#include "qstmgesturedefs.h"

namespace qstmGesture
{

/*!
 * QStm_GestureEngine handles the processing of the UI events.
 * It acts as a stmUiEventEngine::MUiEventObserver to receive the UI events.
 * The gesture recognisers are stored in a list, and at each HandleUiEventL the
 * list of gesture recognisers is processed.  The first one to return EGestureActive
 * from the recognise method "owns" the gesture.  If some other recogniser owned it
 * previously, its release method is called. Gesture recogniser can also lock the gesture
 * by returning ELockToThisGesture. Then only that gesture recogniser will be called
 * until release is detected or the recogniser returns something else than ELockToThisGesture.
 */
class QStm_GestureEngine : public QObject, 
                           public QStm_GestureEngineIf, 
                           public qstmUiEventEngine::QStm_UiEventObserverIf
{
public:
    QStm_GestureEngine();
    virtual ~QStm_GestureEngine();

    /*!
     *  add gesture to the end of the list of gestures
     */
    virtual bool addGesture(const QStm_GestureRecogniserIf* newGesture) ;
    /*!
     * inset a gesture to the specific position
     */
    virtual bool insertGesture(const QStm_GestureRecogniserIf* newGesture, int position = 0);

    virtual int findGesture(const QStm_GestureRecogniserIf* newGesture, int startPos = 0) const;
    virtual int findGestureReverse(const QStm_GestureRecogniserIf* gesture, int startPos) const;
    virtual int findGesture(QStm_GestureUid uid, int startPos = 0) const;
    virtual int findGestureReverse(QStm_GestureUid aUid, int startPos) const;
    virtual int gestureCount() const;
    /*!
     * remove a gesture from the list
     */
    virtual bool removeGesture(const QStm_GestureRecogniserIf* oldGesture) ;
    /*!
     * get the number of non-empty event streams.
     * Event streams correspond UI events generated by one touch pointer (=finger).
     * The low level state machine handles the necessary filtering etc.
     * so that it is safe to remove the event stream after UI Release event has been processed.
     */
    virtual int activeStreamCount() const ;
    /*!
     * get the UI events of stream X
     * \param indexOfActiveStream defines which active stream is used.
     * Note that QStm_UiEventIf contains all the events from down up to the current event.
     * Assumption: the UI events contain the target "window handle", i.e.
     * the gesture recognition needs to be aware of all possible windows of the application.
     */
    virtual const qstmUiEventEngine::QStm_UiEventIf* getUiEvents(int indexOfActiveStream) const ;
    /*!
     * enable/disable logging
     */
    virtual void enableLogging(bool loggingEnabled) {m_loggingEnabled = loggingEnabled;} ;
    /*!
     * The qstmUiEventEngine::QStm_UiEventObserverIf interface
     */
    virtual void handleUiEvent( const qstmUiEventEngine::QStm_UiEventIf& event ) ;
    
    QStm_GestureRecogniserIf* gestureAt(int idx) { return m_gestures[idx] ; }
    
private:
    /*!
     * The list of available gesture recognisers
     */
    QList<QStm_GestureRecogniserIf*>  m_gestures;
    /*!
     * Is 5 UI event streams enough?  Jos tulee Torvisen voittaja?
     * We need to store only the latest event since the interface has
     * methods to walk trough the events. The current (sept 2009) 9.2 seems to have dual-touch support
     */
    const qstmUiEventEngine::QStm_UiEventIf* m_uiEventStream[qstmUiEventEngine::KMaxNumberOfPointers] ;

    void storeUiEvent(const qstmUiEventEngine::QStm_UiEventIf& event) ;
    void walkTroughGestures() ;
    void updateUiEvents() ;
    int m_numOfActiveStreams ;
    int m_currentGestureOwner ;
    int m_currentLockedGesture ;
    bool m_loggingEnabled ;
};

}



#endif /* QSTMGESTUREENGINE_H_ */
