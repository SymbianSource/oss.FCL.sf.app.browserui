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

#include "GoAnywhereViewJSObject.h"
#include "ViewEventContext.h"
#include "GoAnywhereView.h"

#include <QWebFrame>

namespace WRT
{


GoAnywhereViewJSObject::GoAnywhereViewJSObject(GoAnywhereView *contentView, QWebFrame *chromeFrame)
  : ControllableViewJSObject(contentView, chromeFrame, "goAnywhereView")
{
    connect(contentView,SIGNAL(goAnywhereViewChanged(QString)),this,SIGNAL(goAnywhereViewChanged(QString)));
}

void GoAnywhereViewJSObject::onContextEvent(ViewEventContext *context){
    QWebFrame *chrome = chromeFrame();
    if(chrome) {
        // hab - is this needed?
        chrome->addToJavaScriptWindowObject(context->objectName(), context, QScriptEngine::ScriptOwnership);
    }
    emit contextEvent(context);
}

}
