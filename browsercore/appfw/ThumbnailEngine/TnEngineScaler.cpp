/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <QTimer>
#include <QPainter>
#include "TnEngineScaler.h"

namespace WRT {

TnEngineScaler::TnEngineScaler(MTnEngineScalerCallback& callback)
: m_callback(&callback)
{
    m_resultBitmap = 0;
}

void TnEngineScaler::init()
{
    m_scalingTimer = new QTimer();
    connect(m_scalingTimer, SIGNAL(timeout()), this, SLOT(run()));
}

TnEngineScaler* TnEngineScaler::initWithCallback(MTnEngineScalerCallback& callback)
{
    TnEngineScaler* self = new TnEngineScaler(callback);
    self->init();
    return self;
}

TnEngineScaler::~TnEngineScaler()
{
    cancel();
    delete m_resultBitmap;
    delete m_scalingTimer;
}

void TnEngineScaler::startScaling(QPixmap& source, const QRect& targetRect)
{
    // cancel outstanding request
    cancel();
    if (!m_resultBitmap || m_resultBitmap->size() != targetRect.size()) {
        deleteResultBitmap();
        m_resultBitmap = new QPixmap(targetRect.size());
    }
    *m_resultBitmap = source.scaled(targetRect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    m_targetRect = targetRect;
    m_scalingTimer->start(20);
}

void TnEngineScaler::deleteResultBitmap()
{
    delete m_resultBitmap;
    m_resultBitmap = 0;
}

void TnEngineScaler::cancel()
{
    m_scalingTimer->stop();
    deleteResultBitmap();
}

void TnEngineScaler::run()
{
    m_scalingTimer->stop();
    m_callback->scalingCompleted(*m_resultBitmap, m_targetRect);
    // if the callback called StartScalingL(), we must not delete the bitmap
    if (!isActive()) {
        deleteResultBitmap();
    }
}

bool TnEngineScaler::isActive()
{
    return m_scalingTimer->isActive();
}

}

