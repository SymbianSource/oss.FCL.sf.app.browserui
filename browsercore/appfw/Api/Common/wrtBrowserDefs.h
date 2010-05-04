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


#ifndef wrtBrowserDefsS_H
#define wrtBrowserDefsS_H

class wrtBrowserDefs
    {
    public:
        
        /**
        * Type of the focused element.
        */
        enum BrowserElementType
            {
            EElementNone = 0, ///< No element is present.
            EElementImageBox, ///< Box containing an image.
            /**
            * Element used to create either of the following:
            * A link to another document
            * A bookmark within a document
            */
            EElementAnchor,
            EElementTelAnchor, ///< Anchor with a "tel:" scheme
            EElementMailtoAnchor, ///< Anchor with a "mailto:" scheme
            EElementInputBox, ///< Input box that can be selected and activated.
            EElementActivatedInputBox, ///< Input box that contains an entry field into which the user can type.
            /**
            * Box that contains a list of items. The user
            * can select one of the items by clicking it.
            */
            EElementSelectBox,
            /**
            * Push button that can contain text or images. For example, Submit.
            */
            EElementButton,
            EElementTextAreaBox, ///< Input box that contains more than one line.
            EElementRootBox, ///< Root of the document.
            EElementObjectBox, ///< Placeholder for a plug-in that has not yet been downloaded.
            /**
            * Box containing a plug-in that the user can
            * manipulate; for example, by navigating links.
            */
            EElementActivatedObjectBox,
            EElementDownloadedObjectBox, ///< Plug-in that is present but is not being manipulated.
            EElementFileSelectionBoxNoContent, ///< File-browsing box in which no file is selected.
            EElementFileSelectionBoxWithContent, ///< File-browsing box in which a file was selected previously.
            EElementAreaBox, ///< Image map.
            EElementCheckBoxChecked, ///< A check box that was selected.
            EElementCheckBoxUnChecked, ///< A check box that was not selected.
            EElementRadioButtonSelected, ///< A radio button that was selected.
            EElementRadioButtonUnSelected, ///< A radio button that was not selected.

            EElementMouseButtonListener, ///< A Mouse button listener
            EElementScrollBar,    ///< A scroll bar

            EElementSmartLinkTel, ///< A telephone number in the page.
            EElementSmartLinkEmail, ///< an email address in the page.
            EElementBrokenImage, ///< A missing image
            EElementSmartLinkVoip, ///< A voip smartlink
            EElementSelectMultiBox,
			EElementTable,
			EElementListBox,
			EElementForm
        };
    };

#endif      // wrtBrowserDefsS_H

// End of File
