/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: This implements the geolocation database API's.
 *
 */

#include<QString>
#include<QFile>
#include<QFileInfo>
#include<QDebug>
#include<QSqlQuery>
#include<QSqlError>
#include<QWidget>
#include<QDateTime>
#include<QtGui>

#include "geolocationManager.h"

/* Declare the user defined meta type for use with QVariant in geolocation. */
Q_DECLARE_METATYPE(QWebPage::PermissionPolicy);

GeolocationManager* GeolocationManager::getSingleton() 
{
    static GeolocationManager* s_instance;
    if(!s_instance) {
        s_instance = new GeolocationManager();
    }
    Q_ASSERT(s_instance);
    return s_instance;    
}

GeolocationManager::GeolocationManager(QWidget *parent) :
    QObject(parent)
{
    setObjectName("geolocationManager");
    
    /* Register user defined Meta Types used in geolocation API. */
    qRegisterMetaType<QWebPage::PermissionPolicy>("QWebPage::PermissionPolicy");
    
    m_geo = QSqlDatabase::database(GEOLOCATION_DB_NAME);
    if (!m_geo.isValid()) {
        m_geo = QSqlDatabase::addDatabase("QSQLITE", GEOLOCATION_DB_NAME);
        m_geo.setDatabaseName(GEOLOCATION_DB_FILE);
    }
    if (m_geo.open()) {
        if(!doesTableExist(GEOLOCATION_TABLE_NAME)) {
            createGeolocationSchema();
        }
    }
}

GeolocationManager::~GeolocationManager()
{
    m_geo.close();
    QSqlDatabase::removeDatabase(GEOLOCATION_DB_NAME);
}

bool GeolocationManager::doesTableExist(QString tableName)
{
    bool retVal = false;
       
    if (!m_geo.isValid() || !m_geo.isOpen())
         return false;
       
    QSqlQuery query(m_geo);
    query.prepare("SELECT name FROM sqlite_master WHERE type = 'table' AND name=:tableName");
    query.bindValue(":tableName", QVariant(tableName));
    if (query.exec()) {
        if (query.next()) 
            retVal = true;
        query.finish();
    }
    return retVal;
}
   
void GeolocationManager::createGeolocationSchema()
{
    if (!doQuery("CREATE TABLE IF NOT EXISTS geolocationdata("
        "domain text PRIMARY KEY,"
        "creationtime int, "
        "permission text)")) {
        // TODO: do some error handling here!
        return;
    }
    // We do lookups by domain
    if (!doQuery("CREATE INDEX IF NOT EXISTS geo_domain_idx ON geolocationdata(domain ASC)")) {
        // TODO: do some error handling here!
        return;
    }
}

// TODO refactor this - nothing except the schema creation can use it as is
bool GeolocationManager::doQuery(QString query)
{
    QSqlQuery db_query(m_geo);
    bool ok = db_query.exec(query);
    if (!ok) {
        qDebug() << "GeolocationManager::doQuery" << QString("ERR: %1 %2").arg(
                db_query.lastError().type()).arg(db_query.lastError().text())
                << " Query: " << db_query.lastQuery();
    }
    return ok;
}

/**===================================================================================
 * Description: Converts database permission string into enum
 * Returns: permission enum
 ======================================================================================*/
QWebPage::PermissionPolicy GeolocationManager::convertStringIntoPermission(QString permission)
{
    QWebPage::PermissionPolicy permissionPolicy = QWebPage::PermissionUnknown;
    	
    if (permission == "G") 
        permissionPolicy = QWebPage::PermissionGranted;
    else if (permission == "D") 
        permissionPolicy = QWebPage::PermissionDenied;
    else if (permission == "U") 
        permissionPolicy = QWebPage::PermissionUnknown;
    
    return permissionPolicy;
}

/**===================================================================================
 * Description: Converts permission enum into database string
 * Returns: String
 ======================================================================================*/
QString GeolocationManager::convertPermissionIntoString(QWebPage::PermissionPolicy permission)
{
    QString retVal;

    switch (permission) {
      case QWebPage::PermissionGranted:
         retVal = "G";
         break;
      case QWebPage::PermissionDenied:
         retVal = "D";
         break;
      case QWebPage::PermissionUnknown:
         retVal = "U";
         break;
      default:
         retVal = "";    
    }
    
    return retVal;
}

/**===================================================================================
 * Description: Delete the geolocation item.
 * Returns: SUCCESS(0) or Failure(-1 or -2)
 ======================================================================================*/
int GeolocationManager::deleteGeodomain(QString domain)
{
   int retVal = SUCCESS;
       
    if (domain.isEmpty())
        retVal = FAILURE;
       
    if (retVal == SUCCESS) {
        if (m_geo.isOpen()) {
        QSqlQuery query(m_geo);
               
        query.prepare("DELETE FROM geolocationdata WHERE domain=:geodomain");
        query.bindValue(":geodomain", domain);
        if (!query.exec()) {
            lastErrMsg(query);
            retVal = DATABASEERROR;
        }
        } else 
               retVal = FAILURE;
    }
       
    return retVal;
}
 
/**===================================================================================
 * Description: Delete all records from the geolocation table.
 * Returns: SUCCESS(0) or FAILURE(-1 or -2)
 ======================================================================================*/
int GeolocationManager::clearAllGeodata()
{
    int retVal = SUCCESS;
    
    if (m_geo.isOpen()) {
        QSqlQuery query(m_geo);
        
        if (!query.exec("DELETE FROM geolocationdata")) {
            lastErrMsg(query);
            retVal = DATABASEERROR;
        }
    } else
        retVal = FAILURE;
    
    return retVal;
}



/**================================================================
 * Description: Adds a domain to the database. 
 * Returns: SUCCESS(0) or FAILURE (-1 or -2)
 ==================================================================*/
int GeolocationManager::addGeodomain(QString domainToAdd, QWebPage::PermissionPolicy permission)
{
    int retVal = SUCCESS;
    
    if (domainToAdd.isEmpty() || permission == QWebPage::PermissionUnknown)
        retVal = FAILURE;
    
    if (retVal == SUCCESS) {
        QString permissionPolicy(convertPermissionIntoString(permission));
        
        QDateTime dt  = QDateTime::currentDateTime();
        int timestamp = dt.toTime_t();
    
        if (m_geo.isOpen()) {
            QSqlQuery query(m_geo);
            m_geo.transaction();
            query.prepare("SELECT domain FROM geolocationdata WHERE domain=:domain");
            query.bindValue(":domain",domainToAdd);
            if (!query.exec()) {
                 lastErrMsg(query);
                 m_geo.rollback();
                return DATABASEERROR;
            }
            if(!query.next()) {
                QSqlQuery query2(m_geo);
                query2.prepare("INSERT INTO geolocationdata (domain, creationtime, permission) "
                        "VALUES (:domain, :ctime, :ppermisson)");
                query2.bindValue(":domain", QVariant(domainToAdd));
                query2.bindValue(":ctime", QVariant(timestamp));
                query2.bindValue(":ppermission", QVariant(permissionPolicy));
                if (!query2.exec()) {
                    lastErrMsg(query);
                    m_geo.rollback();
                    retVal = DATABASEERROR;
                }
            }
            query.finish();
            if (!m_geo.commit()) {
                 qDebug() << m_geo.lastError().text();
                 m_geo.rollback();
                 return DATABASEERROR;
            }
        } else
            retVal = FAILURE;
    }
    
    return retVal;
}

/**==============================================================
 * Description: Finds attributes (row) including permission policy for a domain.
 * Returns: empty list (not found) or list of attributes
 ===============================================================*/
QList<QVariant> GeolocationManager::findGeodomain(QString domainToFind)
{ 
    QList<QVariant> retValue;
    
    if (m_geo.isOpen()) {
        QSqlQuery query(m_geo);
        query.prepare(
                "SELECT permission FROM geolocationdata WHERE domain=:domain");
        query.bindValue(":domain",domainToFind);
        if (query.exec()) {
            if (query.next()) {
                QWebPage::PermissionPolicy permission = convertStringIntoPermission(query.value(0).toString());
                QVariant v;
                v.setValue(permission);
                retValue << v;
                return retValue;
            }
        } else {
            lastErrMsg(query);
        }
    }
    
    return retValue;
}

/**==============================================================
 * Description: Prints a last error message from the query.
 * Returns: Nothing.
 ===============================================================*/
void GeolocationManager::lastErrMsg(QSqlQuery& query)
{
    qDebug() << "GeolocationManager::lastErrMsg" << QString("ERR: %1 %2").arg(
            query.lastError().type()).arg(query.lastError().text())
            << " Query: " << query.lastQuery();
}

