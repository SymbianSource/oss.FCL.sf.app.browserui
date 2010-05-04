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


#ifndef __GINEBRA_GREENCHROMESNIPPET_H__
#define __GINEBRA_GREENCHROMESNIPPET_H__

#include <QtGui>

namespace GVA {

  class GreenChromeSnippet : public QGraphicsWidget
  {
  public:
    GreenChromeSnippet(QGraphicsItem* parent = 0);
    virtual ~GreenChromeSnippet();
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget);
  };

} // end of namespace GVA

#endif // __GINEBRA_GREENCHROMESNIPPET_H__
