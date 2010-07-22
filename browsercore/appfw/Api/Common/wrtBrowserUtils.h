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

#ifndef __wrtBrowserUtils_H__
#define __wrtBrowserUtils_H__

#include <QWebElement>
#include <QWebFrame>
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