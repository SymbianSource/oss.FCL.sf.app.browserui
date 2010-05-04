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


// Do not use WrtPageManager!!!
// Use WebPageController

#include "webpagecontroller.h"

namespace WRT {
    class BWF_EXPORT WrtPageManager : public WebPageController
    {
        Q_OBJECT;
    public:
        WrtPageManager(QObject* parent = 0) : WebPageController(parent) {}
        static WrtPageManager* getSingleton() { return static_cast<WrtPageManager*>(WebPageController::getSingleton()); }
    };
}
