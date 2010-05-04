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


#ifndef QSTMSTATEENGINECONFIG_H_
#define QSTMSTATEENGINECONFIG_H_

#include "qstmuievent_if.h"
#include "qstmtimerinterface.h"
#include "qstmstatemachine_v2.h"


namespace qstmUiEventEngine
{
class QStm_UiEventSender;


class QStm_StateEngineConfiguration : public QObject
{
public:
	QStm_StateEngineConfiguration() ;
    void construct();
    ~QStm_StateEngineConfiguration() ;

    void setTouchTimeArea(const long fingersize_mm) ;
    void setTouchArea(const long fingersize_mm) ;
    QStm_AreaShape getTouchAreaShape() ;
    void setTouchAreaShape(const QStm_AreaShape shape) ;
    unsigned int getTouchTimeout() ;
    void setTouchTimeout(unsigned int) ;
    void setHoldArea(const long fingersize_mm) ;
    QStm_AreaShape getHoldAreaShape() ;
    void setHoldAreaShape(const QStm_AreaShape shape) ;
    unsigned int getHoldTimeout() ;
    void setHoldTimeout(unsigned int a) ;
    unsigned int getTouchSuppressTimeout() ;
    void setTouchSuppressTimeout(unsigned int a) ;
    unsigned int getMoveSuppressTimeout() ;
    void setMoveSuppressTimeout(unsigned int a) ;
    bool addUiEventObserver(QStm_UiEventObserverIf* observer) ;
    bool removeUiEventObserver(QStm_UiEventObserverIf* observer) ;
    void enableLogging(bool a) ;
    void setMoveTolerance(long fingersize_mm) ;
    QPoint getMoveTolerance() { return m_moveTolerance; }

private:
    void setTolerance(long fingersize_mm, QPoint& tolerance, QStm_AreaShape shape) ;

public:
    QStm_UiEventSender* m_uiEventSender ;

    QStm_AreaShape m_touchAreaShape ;
    QStm_AreaShape m_holdAreaShape ;
    bool           m_enableLogging ;
    QPoint m_touchTimeTolerance ;
    QPoint m_touchTolerance ;
    QPoint m_holdTolerance ;
    QPoint m_moveTolerance ;
    int  m_touchTimerLimit ;
    int  m_holdTimerLimit ;
    int  m_suppressTimerLimit ;
    int  m_moveSuppressTimerLimit ;


};

} // namespace qstmUiEventEngine

#endif /* QSTMSTATEENGINECONFIG_H_ */
