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

#ifndef QSTMMAYBETAPGESTURERECOGNISER_H_
#define QSTMAYBEMTAPGESTURERECOGNISER_H_

#include <qstmgestureengine_if.h>
#include <qstmgesture_if.h>
#include <qstmgesturelistener_if.h>
#include "uitimer.h"

namespace qstmGesture
{

class QStm_MaybeTapGestureRecogniser : public QStm_GestureRecogniser
{
	Q_OBJECT
public:
    static const QStm_GestureUid KUid = EGestureUidMaybeTap;

    virtual ~QStm_MaybeTapGestureRecogniser();

    virtual QStm_GestureRecognitionState recognise(int numOfActiveStreams, QStm_GestureEngineIf* ge) ;
    virtual void release(QStm_GestureEngineIf* ge) ;
    virtual void enable(bool enabled) ;
    virtual bool isEnabled() ;
    virtual void setOwner(void* owner) ;

    virtual QStm_GestureUid gestureUid() const { return KUid; }

    void addTapListener(QStm_GestureListenerIf* listener, void* listenerOwner) ;
    void removeTapListener(QStm_GestureListenerIf* listener, void* listenerOwner) ;
    virtual void enableLogging(bool loggingOn) ;
    void setTapRange(int rangeInMillimetres);
    QStm_MaybeTapGestureRecogniser(QStm_GestureListenerIf* listener) ;
    
private:
    bool m_loggingenabled ;
    bool isPointClose(const QPoint& firstPoint, const QPoint& secondPoint) ;
private:
    void* m_powner ; // The owning control for this gesture

    QPoint m_firstTapXY ;
    void* m_firstTapTarget ;
    float m_firstTapSpeed ;
    bool m_gestureEnabled ;
    int  m_rangesizeInPixels ;
    // use simple arrays to store the listeners and corresponding windows
    QList<QStm_GestureListenerIf*>  m_tapListeners ;
    QList<void*>                    m_tapListenerWindows ;
    int                             m_numOfActiveStreams;
};

} // namespace

#endif /* QSTMTAPGESTURERECOGNISER_H_ */
