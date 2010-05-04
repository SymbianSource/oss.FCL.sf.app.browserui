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


#include "WebViewEventContext.h"
#include <QWebHitTestResult>
#include <QWebElement>

static const QString s_ObjectName = "webEventViewContext";

WebViewEventContext::WebViewEventContext()
{
    setObjectName(s_ObjectName);
}

WebViewEventContext::WebViewEventContext(const QString &viewType, const QWebHitTestResult &hitTest)
  : ViewEventContext(viewType, hitTest.pos()),
    m_linkText(hitTest.linkText()), 
    m_linkUrl(hitTest.linkUrl().toString()), 
    m_linkTitle(hitTest.linkTitle().toString()),
    m_elementId(hitTest.element().attribute("id")),
    m_frameName(hitTest.frame() ? hitTest.frame()->frameName() : QString::null),
    m_imageUrl(hitTest.imageUrl().toString()),
    m_isContentSelected(hitTest.isContentSelected()),
    m_linkElementId(hitTest.linkElement().attribute("id")),
    m_linkFrameName(hitTest.linkTargetFrame() ? hitTest.linkTargetFrame()->frameName() : QString::null)
{
    setObjectName(s_ObjectName);
}


WebViewEventContext::WebViewEventContext(const WebViewEventContext &o)
  : ViewEventContext(o.getViewType(), o.getPosition()),
    m_linkText(o.getLinkText()), 
    m_linkUrl(o.getLinkUrl()), 
    m_linkTitle(o.getLinkTitle()),
    m_elementId(o.getElementId()),
    m_frameName(o.getFrameName()),
    m_imageUrl(o.getImageUrl()),
    m_isContentSelected(o.getIsContentSelected()),
    m_linkElementId(o.getLinkFrameName()),
    m_linkFrameName(o.getLinkFrameName())
{    
    m_viewType = o.getViewType();
    setObjectName(o.objectName());
}

