/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

// INCLUDE FILES
#include <QUrl>
#include <QList>
#include <xqaiwdecl.h>
#include "SchemeHandlerBrQtHighway_p.h"


namespace WRT {


QtHighwaySchemeHandlerPrivate::QtHighwaySchemeHandlerPrivate() : 
    mRequestPtr(NULL)
{
}

QtHighwaySchemeHandlerPrivate::~QtHighwaySchemeHandlerPrivate()
{
    if (mRequestPtr) {
        delete mRequestPtr;
        mRequestPtr = NULL;
    }
}

SchemeHandler::SchemeHandlerError QtHighwaySchemeHandlerPrivate::HandleScheme(const QUrl &url)
{
    return SendStandardUriViewRequest(url);
}


/*!
    Creates request with "com.nokia.symbian.IUriView" interface name and 
    "view(QString)" operation. QtHighway uses the \a url parameter to
    determine the service application and the service application 
    uses it as data. The request is asynchronous. Application launched
    as stand-alone app, not embedded.
*/
SchemeHandler::SchemeHandlerError 
    QtHighwaySchemeHandlerPrivate::SendStandardUriViewRequest(const QUrl& url)
{
    SchemeHandler::SchemeHandlerError retVal = SchemeHandler::NoError;
    bool embedded = false;  // window groups not chained

    // Create request - Apply first implementation of url, "com.nokia.symbian.IUriView"
    // interface name and "view(QString)" operation.
    if (mRequestPtr)
        delete mRequestPtr; // ensure previous requests deleted
    mRequestPtr = mAiwMgr.create(url, embedded);
    
    if (mRequestPtr) {
        // Debug - what service and interface are we using?
        //XQAiwInterfaceDescriptor const &desc = mRequestPtr->descriptor();
        //qDebug() << "SendStandardUriViewRequest: sn=" << desc.serviceName() << "if=" << desc.interfaceName();
    
        // Set function parameters
        QList<QVariant> args;
        args << url.toString();
        mRequestPtr->setArguments(args);
        mRequestPtr->setEmbedded(embedded);
        mRequestPtr->setSynchronous(false); // asynchronous request

        // Send the request
        if (mRequestPtr->send()) {
            // connect request signals to slots
            connect(mRequestPtr, SIGNAL(requestOk(const QVariant&)), 
                this, SLOT(handleOk(const QVariant&)));
            connect(mRequestPtr, SIGNAL(requestError(int,const QString&)), 
                this, SLOT(handleError(int, const QString&)));
        } else {
            // requestError() signal will be sent with error code
            retVal = SchemeHandler::LaunchFailed;
        }
    } else {
        // failed to create request - maybe scheme is unsupported
        retVal = SchemeHandler::SchemeUnsupported;
    }
    
    return retVal;
}

// Aiw request responses
void QtHighwaySchemeHandlerPrivate::handleOk(const QVariant& result)
{
    // service application launched ok, result is application return value
    // service app should handle UI for errors, this is just for cleanup
    if (mRequestPtr) {
        delete mRequestPtr;
        mRequestPtr = NULL;
    }
}

// handles errors in interworking request handling
void QtHighwaySchemeHandlerPrivate::handleError(int errorCode, 
    const QString& errorMessage)
{
    // UI relies on return error from XQAiwRequest::send() to alert user of problem
    // add debug code here to find out cause of error
    if (mRequestPtr) {
        delete mRequestPtr;
        mRequestPtr = NULL;
    }
}

} // WRT
