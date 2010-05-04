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


#ifndef __GINEBRA_BLUECHROMESNIPPET_H__
#define __GINEBRA_BLUECHROMESNIPPET_H__

#include <QtGui>

namespace GVA {

  class BlueChromeSnippet : public QGraphicsWidget
  {
  public:
    BlueChromeSnippet(QGraphicsItem* parent = 0);
    virtual ~BlueChromeSnippet();
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget);

  };

} // end of namespace GVA

#endif // __GINEBRA_BLUECHROMESNIPPET_H__
