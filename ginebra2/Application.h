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

#ifndef __GINEBRA_APPLICATION_H__
#define __GINEBRA_APPLICATION_H__

#include <QObject>

class QCoreApplication;

namespace GVA {

class GinebraApplication : public QObject
{
  Q_OBJECT
  public:
    GinebraApplication();
    ~GinebraApplication();
  public slots:
    void quit();
    void debug(const QString &msg);
  signals:
    void aboutToQuit();
  private:
    QCoreApplication * m_app;
};
  
}
#endif
