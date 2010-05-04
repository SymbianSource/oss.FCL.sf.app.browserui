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


#ifndef __TNENGINESCALER_H__
#define __TNENGINESCALER_H__


#include <QObject>
#include <QPixmap>

namespace WRT {

class MTnEngineScalerCallback : public QObject
{
public:
    virtual void scalingCompleted(QPixmap& result, const QRect& targetRect) = 0;
};


class TnEngineScaler : public QObject
{
Q_OBJECT
public:
    static TnEngineScaler* initWithCallback(MTnEngineScalerCallback& callback);
    virtual ~TnEngineScaler();

public: // New functions
    void startScaling(QPixmap& source, const QRect& targetRect);
    bool isActive();

public slots:
    void run();

public:
    void cancel();

private:
    TnEngineScaler(MTnEngineScalerCallback& callback);
    void init();
    void deleteResultBitmap();

private:    // Data
    MTnEngineScalerCallback* m_callback;
    QPixmap* m_resultBitmap;
    QRect m_targetRect;
    QTimer* m_scalingTimer;
};

}
#endif // __TNENGINESCALER_H__

// End of File
