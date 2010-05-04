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


#ifndef __WRTNETWORKREPLY_H__
#define __WRTNETWORKREPLY_H__

#include "brtglobal.h"
#include <QNetworkReply>

namespace WRT 
{
  class WRT_BROWSER_EXPORT NetworkErrorReply : public QNetworkReply
  {
  public:
    NetworkErrorReply(NetworkError, const QString&, const QUrl&);
    void abort();
    qint64 readData(char*, qint64);
  };
}
#endif
