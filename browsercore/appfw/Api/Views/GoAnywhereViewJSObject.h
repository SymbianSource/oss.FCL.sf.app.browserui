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

#ifndef GoAnywhereViewJSObject_H_
#define GoAnywhereViewJSObject_H_

#include "BWFGlobal.h"

#include "controllableviewjsobject.h"

class ViewEventContext;
namespace WRT
{
    class GoAnywhereView;
    
    /*!
      \brief This class provides the javascript API to GoAnywhereContentViews.
      \sa GoAnywhereContentView
     */
    class BWF_EXPORT GoAnywhereViewJSObject : public ControllableViewJSObject {
        Q_OBJECT
      public:
        GoAnywhereViewJSObject(GoAnywhereView *contentView, QWebFrame *chromeFrame);
        
      signals:
        void contextEvent(QObject *context);
        void goAnywhereViewChanged(QString);
        
      private slots:
        void onContextEvent(ViewEventContext *context);        
    };
}

#endif /* GoAnywhereViewJSObject_H_ */
