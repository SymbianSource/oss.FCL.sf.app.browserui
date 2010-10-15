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
#include <QDesktopWidget>
#include <QGraphicsView>
#include <QGraphicsItem>

#include "WebGestureHelper.h"
#include "qstmgestureevent.h"
#include "qstmfilelogger.h"
#include "qstmuievent_if.h"
#include "qstmgestureapi.h"
#include "qstmstatemachine.h"
#include "bedrockprovisioning.h"

#include "wrtperftracer.h"
#if defined(ORBIT_UI)
#include <hbapplication.h>
#endif


#ifdef Q_OS_SYMBIAN
#include <w32std.h>
#include <coecntrl.h>
#endif

using namespace qstmGesture;
using namespace qstmUiEventEngine;

#define ENABLE_GESTURE_LIB 1


WebGestureHelper::WebGestureHelper(QWidget* ctrl) :
                             m_ctrl(ctrl)
{
    m_gestureEngine = browserApp->gestureEngine();
    setupGestureEngine(m_ctrl);
}


WebGestureHelper::~WebGestureHelper()
{
    delete m_gestures;
    m_gestures = NULL;
    delete m_gestureEngine;
    QGestureRecognizer::unregisterRecognizer(m_qstmGestureType);
    //delete m_dummyRecognizer;

}


void WebGestureHelper::setupGestureEngine(QWidget* ctrl)
{
    m_gestures = m_gestureEngine->createContext(qptrdiff(ctrl));
    QStm_GestureParameters& param = m_gestures->config();

    // Get settings from provisioning
    BEDROCK_PROVISIONING::BedrockProvisioning * settings =
        BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning();

    int logEnabled = settings->valueAsInt("GesturesEnableLogging");
    qstmEnableDebug((logEnabled != 0));
    m_gestures->setLogging(logEnabled);

    param.setEnabled(qstmGesture::EGestureUidTap, settings->valueAsInt("GesturesEnableTap") != 0);
    param[qstmGesture::EDoubleTapTimeout] = settings->valueAsInt("GesturesDoubleTapTimeout"); // 300 7x = 400

    param.setEnabled(qstmGesture::EGestureUidPan, settings->valueAsInt("GesturesEnablePan") != 0);
    param[qstmGesture::EPanSpeedLow] = settings->valueAsInt("GesturesPanSpeedLow");
    param[qstmGesture::EPanSpeedHigh] = settings->valueAsInt("GesturesPanSpeedHigh"); // 100 7x = 400
    param[qstmGesture::EPanDisabledWhileHovering] = settings->valueAsInt("GesturesPanDisabledWhileHovering") != 0; // !7x
    param[qstmGesture::EMoveTolerance] = settings->valueAsInt("GesturesMoveTolerance"); // !7x

    param.setEnabled(qstmGesture::EGestureUidHover, settings->valueAsInt("GesturesEnableHover") != 0);
    param[qstmGesture::EHoverSpeed] = settings->valueAsInt("GesturesHoverSpeed"); // !7x
    param[qstmGesture::EHoverDisabledWhilePanning] = settings->valueAsInt("GesturesHoverDisabledWhilePanning") != 0; // !7x

    param.setEnabled(qstmGesture::EGestureUidLeftRight,  settings->valueAsInt("GesturesEnableLeftRight") != 0);
    param.setEnabled(qstmGesture::EGestureUidUpDown,     settings->valueAsInt("GesturesEnableUpDown") != 0);
    param[qstmGesture::EAxisLockThreshold] = (int)(100 * settings->value("GesturesAxisLockThreshold").toDouble());
    
    param.setEnabled(qstmGesture::EGestureUidFlick,      settings->valueAsInt("GesturesEnableFlick") != 0);
    param[qstmGesture::EFlickSpeed] = settings->valueAsInt("GesturesFlickSpeed", 25); /*param[stmGesture::EPanSpeedHigh];*/ // !7x

    param.setEnabled(qstmGesture::EGestureUidRelease,    settings->valueAsInt("GesturesEnableRelease") != 0);
    param.setEnabled(qstmGesture::EGestureUidTouch,      settings->valueAsInt("GesturesEnableTouch") != 0);

    param.setEnabled(qstmGesture::EGestureUidEdgeScroll, settings->valueAsInt("GesturesEnableEdgeScroll") != 0);
    param[qstmGesture::EEdgeScrollRange] = settings->valueAsInt("GesturesEdgeScrollRange"); // !7x

    param.setEnabled(qstmGesture::EGestureUidCornerZoom, settings->valueAsInt("GesturesEnableCornerZoom") != 0);
    param[qstmGesture::EZoomCornerSize] = settings->valueAsInt("GesturesZoomCornerSize"); // 7 !7x

    param.setEnabled(qstmGesture::EGestureUidPinch, settings->valueAsInt("GesturesEnablePinch") != 0);
    param.setEnabled(qstmGesture::EGestureUidLongPress, settings->valueAsInt("GesturesEnableLongPress") != 0);

    param.setEnabled(qstmGesture::EGestureUidUnknown, settings->valueAsInt("GesturesEnableUnknown") != 0);
    
    QStm_GestureArea& touchArea = *param.area(qstmGesture::ETouchArea);
    QStm_GestureArea& tTimeArea = *param.area(qstmGesture::ETouchTimeArea);
    QStm_GestureArea& holdArea  = *param.area(qstmGesture::EHoldArea);

    touchArea.m_shape    = QStm_GestureArea::QStm_Shape(settings->valueAsInt("GesturesTouchAreaShape"));
    touchArea.m_timeout  = settings->valueAsInt("GesturesTouchAreaTimeout"); // 150 7x = 0
    touchArea.m_size     = QSize(settings->valueAsInt("GesturesTouchAreaWidth"), 
                                 settings->valueAsInt("GesturesTouchAreaHeight")); // 7 7x = 4

    tTimeArea.m_shape    = QStm_GestureArea::QStm_Shape(settings->valueAsInt("GesturesTimeAreaShape"));
    tTimeArea.m_timeout  = settings->valueAsInt("GesturesTimeAreaTimeout"); // 150 7x = 200
    tTimeArea.m_size     = QSize(settings->valueAsInt("GesturesTimeAreaWidth"),
                                 settings->valueAsInt("GesturesTimeAreaHeight")); // 7 7x = 4

    holdArea.m_shape    = QStm_GestureArea::QStm_Shape(settings->valueAsInt("GesturesHoldAreaShape"));
    holdArea.m_timeout  = settings->valueAsInt("GesturesHoldAreaTimeout"); // 7x = 1500
    holdArea.m_size     = QSize(settings->valueAsInt("GesturesHoldAreaWidth"), 
                                settings->valueAsInt("GesturesHoldAreaHeight")); // 7 7x = 4

    param[ qstmGesture::ESuppressTimeout     ] = settings->valueAsInt("GesturesSuppressTimeout");
    param[ qstmGesture::EMoveSuppressTimeout ] = settings->valueAsInt("GesturesMoveSuppressTimeout");
    param[ qstmGesture::ECapacitiveUpUsed    ] = settings->valueAsInt("GesturesCapacitiveUpUsed") != 0;
    param[ qstmGesture::EAdjustYPos          ] = settings->valueAsInt("GesturesAdjustYPos") != 0; // 7x = true ifndef WINSCW
    param[ qstmGesture::EEnableFiltering     ] = settings->valueAsInt("GesturesEnableFiltering") != 0; // 7x = true
    param[ qstmGesture::EWServMessageInterception ] = (settings->valueAsInt("GesturesWServMessageInterception") != 0);
    // Wonder Twin powers, Activate!  Form of an ice "pan"!  Shape of a "pinch"-nose pug! 
    m_gestures->activate(ctrl);

    /*
     * Only one instance of dummy recognizer is needed.
     * First context will have none-null pointer to it.
     */
    //if (QStm_Gesture::assignedType() == Qt::CustomGesture) {
        m_dummyRecognizer = new QStm_QtDummyGestureRecognizer(m_gestures);
        m_gestures->addListener(m_dummyRecognizer);
        m_qstmGestureType = QGestureRecognizer::registerRecognizer(m_dummyRecognizer);
        QStm_Gesture::setAssignedGestureType(m_qstmGestureType);
    //}
    //QObject::connect(m_gestures, SIGNAL(uiEvent(const qstmUiEventEngine::QStm_UiEventIf&)), 
    //        m_dummyRecognizer, SLOT(handleQStmUiEvent(const qstmUiEventEngine::QStm_UiEventIf&)));
    
    // HACK!!! Disable this if you need Double Tap gesture. !!!
    //m_gestures->enableDblClick(true);
    }


bool WebGestureHelper::shouldHandleGesture()
{
    bool shouldHandle = (BedrockSettings->value("EnableGestures").toInt() != 0);
#ifdef Q_OS_SYMBIAN    
    shouldHandle = !CCoeEnv::Static()->AppUi()->IsDisplayingMenuOrDialog();
#endif    

    QWidget* modal = QApplication::activeModalWidget();
    shouldHandle =  shouldHandle && (modal == NULL);
    return shouldHandle;
}



bool  WebGestureHelper::symbianEventFilter(const QSymbianEvent *event)
{
    return m_gestures->handleSymbianPlatformEvent(event);

}

bool WebGestureHelper::x11EventFilter (XEvent* event)
{
    return m_gestures->handleX11PlatformEvent(event);
}

bool WebGestureHelper::winEventFilter(void* event)
{
    return m_gestures->handleWinPlatformEvent(event);
}

bool WebGestureHelper::isFilteredByGestureEngine()
{
    bool wasFiltered = false;
    
    QStm_GestureEngineApi* gestEng = gestureEngine();
    if (gestEng) {
        qstmUiEventEngine::QStm_StateMachine* sm = gestEng->getStateMachine();
        if (sm) {
            //for (int i = 0; i < qstmUiEventEngine::KMaxNumberOfPointers && !wasFiltered; i++) {
                wasFiltered = sm->wasLastMessageFiltered();
            //}
        }
    }
    return wasFiltered; 
}


#if defined(ORBIT_UI)
BrowserApp::BrowserApp(QS60MainApplicationFactory appfactory, int & argc, char** argv) : ParentApp(appfactory, argc, argv)
{
    m_gestureHelper = 0;
    m_mainWindow = 0;
    m_gestureEngine = new QStm_GestureEngineApi();
}
#endif // ORBIT_UI

BrowserApp::BrowserApp(int & argc, char** argv) : ParentApp(argc, argv)
{
    m_gestureHelper = 0;
    m_mainWindow = 0;
    m_gestureEngine = new QStm_GestureEngineApi();
}

void BrowserApp::setGestureHelper(WebGestureHelper* gh)
{
    m_gestureHelper = gh;
/*    if (mainWindow()) {
        qDebug() << __PRETTY_FUNCTION__ << " - grabbing gestures";
        mainWindow()->ungrabGesture(Qt::PanGesture);
        mainWindow()->ungrabGesture(Qt::TapGesture);
        mainWindow()->ungrabGesture(Qt::TapAndHoldGesture);
        mainWindow()->ungrabGesture(Qt::PinchGesture);
        mainWindow()->ungrabGesture(Qt::SwipeGesture);
        mainWindow()->grabGesture(QStm_Gesture::assignedType());
    }
*/ 
}


bool BrowserApp::symbianEventFilter(const QSymbianEvent *event)
{
#if(ENABLE_GESTURE_LIB)
    bool wasFiltered = false;
    if (m_gestureHelper && m_gestureHelper->shouldHandleGesture()) {
        m_gestureHelper->symbianEventFilter(event);
        wasFiltered = m_gestureHelper->isFilteredByGestureEngine();
    }
    return false;
#endif
    return false;
}


bool BrowserApp::x11EventFilter ( XEvent* event )
{
#if (ENABLE_GESTURE_LIB)
    bool wasFiltered = false;
    if (m_gestureHelper && m_gestureHelper->shouldHandleGesture()) {
        wasFiltered = m_gestureHelper->isFilteredByGestureEngine();
        m_gestureHelper->x11EventFilter(event);
    }
    return false;
#endif
    return false;
}

bool BrowserApp::winEventFilter(MSG* message, long* result)
{
#if (ENABLE_GESTURE_LIB)
    bool wasFiltered = false;
    if (m_gestureHelper && m_gestureHelper->shouldHandleGesture()) {
        if(m_gestureHelper->winEventFilter(message))
            wasFiltered = m_gestureHelper->isFilteredByGestureEngine();
        if(wasFiltered) *result = 0;
    }
    return false;
#endif
    return false;
}

/*
// For now, treat gesture touch / release as mouse events
bool BrowserApp::event(QEvent* event)
{
    if (event->type() == QEvent::Gesture) {
        qDebug() << __PRETTY_FUNCTION__ << " - got a gesture";
        QStm_Gesture* gesture = getQStmGesture(event);
        if (gesture) {
//            QStm_GestureType gtype = gesture->getGestureStmType();
//            if (gtype == QStmTouchGestureType || gtype == QStmReleaseGestureType) {
                qDebug() << __PRETTY_FUNCTION__ << " - sending mouse events";
                gesture->sendMouseEvents();
                return true;
//            }
        }
    }
    return QApplication::event(event);
}
*/
QStm_QtDummyGestureRecognizer::QStm_QtDummyGestureRecognizer(QStm_GestureContext* ctx) :
                                      QGestureRecognizer(),
                                      m_context(ctx),
                                      m_currentGesture(NULL)
{
}

QStm_QtDummyGestureRecognizer::~QStm_QtDummyGestureRecognizer()
{
}


QGesture* QStm_QtDummyGestureRecognizer::create(QObject* /*target*/)
{
    return new QStm_Gesture();
}

QGestureRecognizer::Result QStm_QtDummyGestureRecognizer::recognize(QGesture *state,
                                                                    QObject */*watched*/,
                                                                    QEvent *event)
{
    QGestureRecognizer::Result ret = QGestureRecognizer::Ignore;
    if (event->type() == QStm_GestureEvent::stmGestureEventType()) {
        QStm_Gesture* gesture = static_cast<QStm_Gesture*>(state);
        *gesture = *m_currentGesture;
        if (m_currentGesture->gestureState() == Qt::GestureFinished ||
            m_currentGesture->isGestureEnded()) {
            ret = QGestureRecognizer::FinishGesture;
    }
        else {
            ret = QGestureRecognizer::TriggerGesture;
        }
        event->accept();
    }
    return ret;
}

void QStm_QtDummyGestureRecognizer::reset(QGesture */*state*/)
{

}



void QStm_QtDummyGestureRecognizer::handleQStmUiEvent(const qstmUiEventEngine::QStm_UiEventIf& uiEvent)
{
}


QStm_GestureListenerApiIf::QStm_ProcessingResult QStm_QtDummyGestureRecognizer::handleGestureEvent(
                                              QStm_GestureUid uid, QStm_GestureIf* gesture)
{
    if (!m_currentGesture) {
        Q_ASSERT(gesture);        
        m_currentGesture = new QStm_Gesture();
    }

    int stmGestType = gesture ? gesture->getType() : -1;

    m_currentGesture->setGestureStmType(m_currentGesture->gestureUidToStmType(uid, stmGestType));
    
    if (gesture) { //gesture enter
        m_currentGesture->setGestureSubType(gesture->getType());
        m_currentGesture->setDirection(gesture->getDirection());
        QPoint vec = gesture->getLengthAndDirection();
        switch (m_currentGesture->getGestureStmType()) {
            case QStmLeftRightGestureType:
                vec.ry() = 0;
                m_currentGesture->setGestureStmType(QStmPanGestureType);
            break;
            case QStmUpDownGestureType:
                vec.rx() = 0;
            m_currentGesture->setGestureStmType(QStmPanGestureType);
            break;
        }
        
        m_currentGesture->setLengthAndDirection(vec);
        m_currentGesture->setSpeed(gesture->getSpeed());
        QPoint pos = gesture->getLocation();
        m_currentGesture->setPosition(pos);
        QPoint pos2 = gesture->getLocation2();
        m_currentGesture->setPosition2(pos2);
        m_currentGesture->setDetails(gesture->getDetails());
        m_currentGesture->setSpeedVec(gesture->getSpeedVec());
        QWidget* w = static_cast<QWidget*>(gesture->target());
        m_currentGesture->setTarget(w);
        m_currentGesture->setGestureState(Qt::NoGesture);
        m_currentGesture->setTimestamp(gesture->timestamp());
        m_currentGesture->setHotSpot(pos);

        QStm_GestureEvent dummy;
        m_currentGesture->sendEvent(&dummy);
        
    }
    else {
        m_currentGesture->setGestureState(Qt::GestureFinished);
    }

    return QStm_GestureListenerApiIf::EContinue;
}




