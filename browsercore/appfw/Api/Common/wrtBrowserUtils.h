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


#ifndef __wrtBrowserUtils_H__
#define __wrtBrowserUtils_H__

#include <qwebelement.h>
#include <qwebframe.h>
#include "wrtBrowserDefs.h"
#include "brtglobal.h"

#define LINK_TAG      "link"
#define INPUT_TAG     "input"
#define A_TAG         "a"
#define HREF_ATTR     "href"
#define AREA_TAG      "area"
#define TEXT_AREA_TAG "textarea"
#define BUTTON_TAG    "button"
#define OBJECT_TAG    "object"
#define EMBED_TAG     "embed"
#define IMAGE_TAG     "img"
#define SELECT_TAG    "select"
#define LI_TAG        "li"
#define OL_TAG        "ol"
#define DL_TAG        "dl"
#define FORM_TAG      "form"
#define HTML_TAG      "html"

class WRT_BROWSER_EXPORT wrtBrowserUtils
{
public:
	wrtBrowserUtils();
	~wrtBrowserUtils();
	static wrtBrowserDefs::BrowserElementType getTypeFromElement(QWebHitTestResult htRes);
private:
	static wrtBrowserDefs::BrowserElementType ElementType(QWebElement* node);
};

#endif //__wrtBrowserUtils_H__