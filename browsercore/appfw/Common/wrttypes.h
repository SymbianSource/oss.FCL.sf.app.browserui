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


#ifndef __WRTTYPES_H__
#define __WRTTYPES_H__

namespace WRT {

    /*!
      This id specifies a WRT view id
    */
    enum WrtViewId {
        ContentViewId = 0, //!< The ContentView (i.e., web pages)
        HistoryViewId,     //!< The history of visited pages
        ThumbnailViewId,   //!< A miniature, navigable view of the current page
        BookmarksViewId,   //!< A view to manage bookmarks
        ShowImagesViewId,  //!< A view that lists images available on the current page
        NullViewId //!< Dummy View Id, This must be the last ID, the array of view handlers depends on it
    };

    /*!
      This is a bitmask describing what the view can or cannot do
    */
    enum ViewCapabilities {
        ViewCanCancel = 1, //!< The view can be cancelled
        ViewCanClose = 2 //!< The view can be closed
    };

}
#endif
