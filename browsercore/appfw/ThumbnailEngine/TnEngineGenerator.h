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


#ifndef TNENGINEGENERATOR_H
#define TNENGINEGENERATOR_H

#include <QObject>
#include "TnEngineScaler.h"
class QPainter;
class QPixmap;
class QTimer;


namespace WRT {

class TnEngineHandler;

class TnEngineGenerator : public MTnEngineScalerCallback
{
Q_OBJECT
public:
    static TnEngineGenerator* initWithTnEngine(TnEngineHandler& TnEngine);
    virtual ~TnEngineGenerator();

public:
    void setKeepBitmap(bool keepBitmaps);
    void invalidate();
    void update(bool scrolling=false);
    void scroll();
    void clear();
    void draw(QPainter& gc, const QRect& to) const;
    void setKeepsBitmaps(bool keepBitmaps);
    bool keepsBitmaps() const;

private:
    void scalingCompleted(QPixmap& result, const QRect& targetRect);

public slots:
    bool startAsyncBufferUpdate();
public:
    void deleteUnscaledBitmap();
    void deleteBufferBitmap();

private:
    bool fastBufferUpdate();
    bool checkAndCreateBitmaps();
    void invalidateBuffers();
    void calcBufferPosition();
    QRect bufferRect() const;

private:
    TnEngineGenerator(TnEngineHandler& TnEngine);
    void init();

private: 

    TnEngineHandler* m_TnEngine;
    TnEngineScaler* m_scaler;
    QPixmap* m_unscaledBitmap;
    QPixmap* m_bufferBitmap;
    QRegion m_validLQRegion;
    QRegion m_validHQRegion;
    QPoint m_bufferPos;
    QSize m_docSize;
    bool m_keepsBitmaps;
    QTimer* m_asyncUpdateStarted;
};

}

#endif      // TnEngineGENERATOR_H

// End of File
