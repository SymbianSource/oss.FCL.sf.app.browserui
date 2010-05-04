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


#include "CachedHandler.h"
#include "ChromeWidget.h"
//#include "ContentViewDelegate.h"
#include "controllableviewimpl.h"

namespace GVA {

    CachedHandler::CachedHandler(const QString & elementId, const QString & script, const QRectF & rect, ChromeWidget * chrome, const QString &targetView)
    : m_id(elementId),
      m_script(script),
      m_rect(rect),  //NB: maybe this should be a QRect
      m_chrome(chrome),
      m_targetView(targetView)
  {
  }
  //NB: Return a QVariant?
  void CachedHandler::invoke() const
  {
    ControllableViewBase *view = m_chrome->getView(m_targetView.isEmpty() ? "WebView" : m_targetView);
    if(view)
      view->triggerAction(m_script);
  }

} // end of namespace GVA

