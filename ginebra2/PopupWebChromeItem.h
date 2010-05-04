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


#ifndef __GINEBRA_POPUPWEBCHROMEITEM_H__
#define __GINEBRA_POPUPWEBCHROMEITEM_H__

#include "WebChromeItem.h"

namespace GVA {

// To use PopupWebChromeItem:
//
// 1. Add data-GinebraItemType="popup" to the DIV element defining your
//    snippet in chrome.html
//
// 2. Connect to the externalMouseEvent signal AFTER chromeLoadComplete
//    has been emitted:
//
//      window.chrome.chromeComplete.connect(chromeLoadComplete);
//
//      function chromeLoadComplete() {
//        window.snippets.ContextMenuId.externalMouseEvent.connect(
//          function(type, name, description) {
//            if (name == "QGraphicsSceneMouseReleaseEvent") {
//              window.snippets.ContextMenuId.hide();
//            }
//          }
//        );
//      }

class PopupWebChromeItem : public WebChromeItem
{
    Q_OBJECT

public:
    PopupWebChromeItem(
            const QRectF & ownerArea,
            ChromeWidget * chrome,
            const QWebElement & element,
            QGraphicsItem * parent = 0);

    virtual ~PopupWebChromeItem();

    virtual void init(WebChromeSnippet * snippet);

signals:
    void externalMouseEvent(
            int type,
            const QString & name,
            const QString & description);

protected:
    virtual bool event(QEvent * event);
    virtual bool eventFilter(QObject * object, QEvent * event);

private:
    void checkForExternalEvent(QObject * object, QEvent * event);
    void emitExternalEvent(QEvent * event);
};

} // end of namespace GVA

#endif // __GINEBRA_POPUPWEBCHROMEITEM_H__
