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


#include "GWebContentViewJSObject.h"
#include "WebViewEventContext.h"

namespace GVA {

void GWebContentViewJSObject::statusBarMessage( const QString & text ){
    emit onStatusBarMessage(text);
}

void GWebContentViewJSObject::statusBarVisibilityChangeRequested(bool visible){
    emit onStatusBarVisibilityChangeRequested(visible);
}

void GWebContentViewJSObject::onContextEvent(::WebViewEventContext *context){
    ::QWebFrame *chrome = chromeFrame();
    if(chrome) {
        chrome->addToJavaScriptWindowObject(context->objectName(), context, QScriptEngine::ScriptOwnership);
    }
    emit contextEvent(context);
}
}
