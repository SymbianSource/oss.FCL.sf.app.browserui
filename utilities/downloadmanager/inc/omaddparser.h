/**
   This file is part of CWRT package **

   Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies). **

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU (Lesser) General Public License as 
   published by the Free Software Foundation, version 2.1 of the License. 
   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of 
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
   (Lesser) General Public License for more details. You should have 
   received a copy of the GNU (Lesser) General Public License along 
   with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OMADDPARSER_H_
#define OMADDPARSER_H_

#include "dmcommon.h"
#include "dmcommoninternal.h"
#include "dmpimpl.h"
#include <QtXml>
#include <QVariant>
#include <QStack>

// forward declarations
class QString;
class OMADownloadDescriptor;
class OMADownloadDescParserPrivate;
class OMADownloadDescriptorPrivate;
class OMA2DownloadProduct;
class OMA2DownloadProductPrivate;
class OMA2DownloadMediaObj;
class OMA2DownloadMediaObjPrivate;

typedef QList<OMA2DownloadProduct*> ProductList;
typedef QList<OMA2DownloadMediaObj*> MediaObjectList;

enum OMADownloadAttribute
{
    /* OMA Download Attributes */
    OMADownloadDescName = 0,                  // "name" in OMA dd
    OMADownloadDescVersion,                   // "version" in OMA dd
    OMADownloadDescObjectURI,                 // "objectURI" in OMA dd
    OMADownloadDescType,                      // "type" in OMA dd
    OMADownloadDescSize,                      // "size" in OMA dd
    OMADownloadDescVendor,                    // "vendor" in OMA dd
    OMADownloadDescDescription,               // "description" in OMA dd
    OMADownloadDescInstallNotifyURI,          // "installNotifyURI" in OMA dd
    OMADownloadDescNextURL,                   // "nextURL" in OMA dd
    OMADownloadDescInfoURL,                   // "infoURL" in OMA dd
    OMADownloadDescIconURL,                   // "iconURL" in OMA dd
    OMADownloadDescInstallParam,              // "instalParam" in OMA dd

    /* OMA2 Download Attributes */
    OMA2DownloadDescUpdatedDDURI,             // "updatedDDURI" in OMA dd2
    OMA2DownloadDescProductName,              // "product" in OMA dd2
    OMA2DownloadDescProductInstallNotifyURI,  // "installNotifyURI" of product in OMA dd2
    OMA2DownloadDescProductSuppressConfirmation,  // "suppressuserconfirmation" of product in OMA dd2
    OMA2DownloadDescMediaObjName,             // "mediaObject" in OMA dd2
    OMA2DownloadDescMediaObjType,             // "type" in OMA dd2
    OMA2DownloadDescMediaObjSize,             // "size" in OMA dd2
    OMA2DownloadDescMediaObjProgressiveDl,    // "progressivedl" in OMA dd2
    OMA2DownloadDescMediaObjServer,           // "server" in OMA dd2
    OMA2DownloadDescMediaObjInstallNotifyURI, // "installNotifyURI" of mediaObj in OMA dd2
    OMA2DownloadDescMediaObjSuppressConfirmation, // "suppressuserconfirmation" of mediaobj in OMA dd2
    OMA2DownloadDescLicense,                  // "license" in OMA dd2
    OMA2DownloadDescOrder,                    // "order" in OMA dd2
    OMA2DownloadDescText                      // "text" in OMA dd2 
};
// class declaration

// for parsing the OMA download descriptor
class  OMADownloadDescParser : public QXmlDefaultHandler
{
    DM_DECLARE_PRIVATE(OMADownloadDescParser);

public:
    OMADownloadDescParser();
    ~OMADownloadDescParser();
    bool startDocument();
    bool startElement(const QString&, const QString&, const QString& ,
                       const QXmlAttributes&);
    bool endElement(const QString&, const QString&, const QString&);
    bool characters(const QString& ch);
    QString errorString();
    bool fatalError(const QXmlParseException &exception);
    OMADownloadDescriptor* downloadDescriptor();
};

// represents OMA download descriptor
class OMADownloadDescriptor
{
    DM_DECLARE_PRIVATE(OMADownloadDescriptor);
public:
    OMADownloadDescriptor();
    ~OMADownloadDescriptor();
    // fetches the OMA DD related attributes
    QVariant getAttribute(OMADownloadAttribute attr);
    // sets the attributes if any
    void setAttribute(const QString&, const QVariant&);
    // fetches the product List
    QList<OMA2DownloadProduct*> productList();
    // fetches the product count in a dd
    int productCount();
    // adding product to the list
    void addProduct(OMA2DownloadProduct*);
};

class OMA2DownloadProduct
{
    DM_DECLARE_PRIVATE(OMA2DownloadProduct);
public:
    OMA2DownloadProduct();
    ~OMA2DownloadProduct();
    // fetches Product related attributes
    QVariant getAttribute(OMADownloadAttribute attr);
    // sets the Product attributes if any
    void setAttribute(const QString&, const QVariant&);
    // fetches the Media Object list
    QList<OMA2DownloadMediaObj*> mediaObjList();
    // fetches the number of media downloads in a product
    int mediaCount();
    // fetches the size of all the media objects present in dd
    qint64 albumSize();
    // adding media object to the media object list.
    void addMediaObject(OMA2DownloadMediaObj*);
};

class OMA2DownloadMediaObj
{
    DM_DECLARE_PRIVATE(OMA2DownloadMediaObj);
public:
    OMA2DownloadMediaObj();
    ~OMA2DownloadMediaObj();
    // fetches the Media Object attributes
    QVariant getAttribute(OMADownloadAttribute attr);
    // sets the Media Object attributes if any
    void setAttribute(const QString&, const QVariant&);
};
#endif       
