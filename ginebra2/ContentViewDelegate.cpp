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


#include "ContentViewDelegate.h"

namespace GVA {
  
  ContentViewDelegate::ContentViewDelegate(ChromeWidget * chrome, QObject * parent)
    : m_chrome(chrome)
  {
    Q_UNUSED(parent)
  }

  ContentViewDelegate:: ~ContentViewDelegate()
  {
  }

} // end of namespace GVA

