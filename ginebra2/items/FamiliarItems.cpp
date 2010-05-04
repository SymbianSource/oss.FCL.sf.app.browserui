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

#include "FamiliarItems.h"
#include "../Settings.h"

#include <QVariant>

#include <QDebug>

namespace GVA
{

  FamiliarItems::FamiliarItems()
  {
    m_db = QSqlDatabase::database("GvaFamiliarItems");
    if(!m_db.isValid()){
      m_db = QSqlDatabase::addDatabase("QSQLITE", "GvaFamiliarItems");
      Settings * settings = Settings::instance();
      m_db.setDatabaseName(settings->installRoot() + "/familiar_items_db.sql");
      m_db.open();
      QSqlQuery q(m_db);
      q.exec("create table items(url varchar(100) primary key, title varchar(40), thumbnail varchar(40), tags int)");
    }
  }
  
  FamiliarItems::~FamiliarItems()
  {
    QSqlDatabase::removeDatabase("GvaFamiliarItems");
    m_db.close();
  }
  
  void FamiliarItems::updateItem(const FamiliarItems::Item & item)
  {
    QSqlQuery q(m_db);
    QString qs =  QString("replace into items values('%1', '%2', '%3', %4)").arg(item.url.left(100)).arg(item.title.left(40)).arg(item.thumbnail.left(40)).arg(static_cast<int>(item.tags));
    bool ok = q.exec(qs);
    qDebug() << "Query: " << qs << ": " << ok;
  }
  
  FamiliarItems::Item FamiliarItems::item(const QString url)
  {
    FamiliarItems::Item item;
    item.url = url;
    QSqlQuery q(m_db);
    q.exec(QString("select title, thumbnail, tags from items where url = '%1'").arg(url.left(100)));
    q.first();
    item.title = q.value(0).toString();
    item.thumbnail = q.value(1).toString();
    item.tags = static_cast<FamiliarItems::ItemTag>(q.value(2).toInt());
    return item;
  }
  
  QString FamiliarItems::thumbnail(const QString url)
  {
    QSqlQuery q(m_db);
    q.exec(QString("select thumbnail from items where url = '%1'").arg(url.left(100)));
    q.first();
    QString t = q.value(0).toString();
    return t;
  }
  
  QString FamiliarItems::title(const QString url)
  {
    QSqlQuery q(m_db);
    QString qs = QString("select title from items where url = '%1'").arg(url.left(100)); 
    bool ok = q.exec(qs);
    qDebug() << "Query: " << qs << " : " << ok;
    q.first();
    QString t = q.value(0).toString();
    return t;
  }
  
  FamiliarItems::ItemTag FamiliarItems::tag(const QString url)
  {
    QSqlQuery q(m_db);
    q.exec(QString("select tags from items where url = '%1'").arg(url.left(100)));
    q.first();
    int t = q.value(0).toInt();
    return static_cast<FamiliarItems::ItemTag>(t);
  }
  
}
