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


#include <QtGui>
#include "GreenChromeSnippet.h"

namespace GVA {

  GreenChromeSnippet::GreenChromeSnippet(QGraphicsItem* parent)
    : QGraphicsWidget(parent)
  {
    setFlags(QGraphicsItem::ItemIsMovable);
    setOpacity(0.50);
  }

  GreenChromeSnippet:: ~GreenChromeSnippet()
  {

  }

  void GreenChromeSnippet::paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget)
  {
    painter->fillRect(QRectF(0,0, geometry().width(), geometry().height()), Qt::green);
  }

} // end of namespace GVA


