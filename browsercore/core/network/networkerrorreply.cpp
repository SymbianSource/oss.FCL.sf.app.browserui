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

#include "networkerrorreply.h"

namespace WRT {


/*!
 * \class NetworkErrorReply
 * A specialization of the abstract base class QNetworkReply. This can be returned by 
 * WebNetworkAccessManager::createRequest() to generate an error without actually
 * submitting a request to the network, thus blocking network access. 
 */

/*!
 * Constructor
 * \param[in] errorCode - The error code to set
 * \param[in] errorString - The error text to set
 */


  NetworkErrorReply::NetworkErrorReply(NetworkError errorCode, const QString& errorString, const QUrl& url)
    : QNetworkReply()
      
  {
    setError(errorCode,errorString);
    setUrl(url);
  }
  
  void NetworkErrorReply::abort()
  {
    
  }
  
  qint64 NetworkErrorReply::readData(char* data, qint64 size){
    return 0;
  }
}
