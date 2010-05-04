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


#ifndef __ZOOMMETADATA_H__
#define __ZOOMMETADATA_H__

// To store the view port proeperties that affect zoom set through meta data. 
// This is available in the main frame of the page but to avoid parsing the meta
// data again, store it

struct ZoomMetaData{

public:

   /* These are currently not needed when we set the zoom again */
   /* 
   int viewportWidth;
   int viewportHeight;
   qreal initialScale;
   */ 
   qreal minScale;
   qreal maxScale;
   bool userScalable;

};
#endif  //__ZOOMMETADATA_H__
