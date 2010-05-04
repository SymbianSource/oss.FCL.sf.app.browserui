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

#ifndef __gva_familiaritems_h__
#define __gva_familiaritems_h__

#include <QSqlDatabase>
#include <QSqlQuery>

namespace GVA 
{

  class FamiliarItems 
  {
  public:

    enum ItemTag{
      Bookmark = 0x01,
      History = 0x02,
      BackForward = 0x04,
      Window = 0x08
    };

    struct Item {
      QString url;
      QString title;
      QString thumbnail;
      ItemTag tags;
    };
    
    FamiliarItems();
    ~FamiliarItems();
    void updateItem(const Item &);
    Item item(const QString url);
    QString thumbnail(const QString url);
    QString title(const QString url);
    ItemTag tag(const QString url);
  private:
    QSqlDatabase m_db;
  };
}
#endif
