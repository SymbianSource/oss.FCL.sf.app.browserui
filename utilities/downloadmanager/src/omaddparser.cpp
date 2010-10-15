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

#include "omaddparser.h"
#include <QString>

const QString nameTag = "name";
const QString DDVersionTag = "DDVersion";
const QString typeTag = "type";
const QString sizeTag = "size";
const QString vendorTag = "vendor";
const QString descriptionTag = "description";
const QString productTag = "product";
const QString mediaObjectTag = "mediaObject";
const QString progressiveDownloadFlagTag = "progressiveDownloadFlag";
const QString serverTag = "server";
const QString suppressUserConfirmTag = "suppressUserConfirmation";
const QString updatedDDURITag = "updatedDDURI";
const QString objectURITag = "objectURI";
const QString installNotifyURITag = "installNotifyURI";
const QString nextURLTag = "nextURL";

// private implementation class
class OMADownloadDescParserPrivate {
    DM_DECLARE_PUBLIC(OMADownloadDescParser);
public:
    OMADownloadDescParserPrivate();
    ~OMADownloadDescParserPrivate();
    OMADownloadDescriptor *m_downloadDesc;
    QStack<QString> m_stack;
    QString m_characterData;
    QString m_errorString;
    bool m_isProductTag; // flag to indicate if it is product tag
    bool m_isMediaObjTag; // flag to indicate if it is media object tag
    OMA2DownloadProduct *m_currProduct;
    OMA2DownloadMediaObj *m_currMediaObj;
};

class OMADownloadDescriptorPrivate {
    DM_DECLARE_PUBLIC(OMADownloadDescriptor);
public:
    OMADownloadDescriptorPrivate();
    ~OMADownloadDescriptorPrivate();
    QMap<QString, QVariant> m_attrMap;
    ProductList m_productList;
};

class OMA2DownloadProductPrivate {
    DM_DECLARE_PUBLIC(OMA2DownloadProduct);
public:
    OMA2DownloadProductPrivate();
    ~OMA2DownloadProductPrivate();
    QMap<QString, QVariant> m_productAttrMap;
    MediaObjectList m_mediaObjList;
};

class OMA2DownloadMediaObjPrivate {
    DM_DECLARE_PUBLIC(OMA2DownloadMediaObj);
public:
    QMap<QString, QVariant> m_mediaObjAttrMap;
};

OMADownloadDescParserPrivate::OMADownloadDescParserPrivate():m_downloadDesc(0)
                                                            ,m_isProductTag(false)
                                                            ,m_isMediaObjTag(false)
                                                            ,m_currProduct(0)
                                                            ,m_currMediaObj(0)
{ }

OMADownloadDescParserPrivate::~OMADownloadDescParserPrivate()
{
    if (m_downloadDesc) {
        delete m_downloadDesc;
        m_downloadDesc = 0;
    }
}

OMADownloadDescParser::OMADownloadDescParser()
{
    DM_INITIALIZE(OMADownloadDescParser);
}

OMADownloadDescParser::~OMADownloadDescParser()
{
    DM_UNINITIALIZE(OMADownloadDescParser);
}

bool OMADownloadDescParser::startDocument()
{
    DM_PRIVATE(OMADownloadDescParser);
    priv->m_downloadDesc = new OMADownloadDescriptor();
    return true;
}

bool OMADownloadDescParser::startElement(const QString&, const QString&, const QString& qName, const QXmlAttributes&)
{
    DM_PRIVATE(OMADownloadDescParser);
    priv->m_characterData = "";
    priv->m_stack.push(qName);
    if (qName == productTag) {
        priv->m_isProductTag = true;
        priv->m_currProduct = new OMA2DownloadProduct();
        if (priv->m_currProduct) {
            priv->m_downloadDesc->addProduct(priv->m_currProduct);
            return true;
        }
        return false;
    }
    if (qName == mediaObjectTag) {
        priv->m_isMediaObjTag = true;
        priv->m_currMediaObj = new OMA2DownloadMediaObj();
        if (priv->m_currMediaObj) {
            priv->m_currProduct->addMediaObject(priv->m_currMediaObj);
            return true;
        }
        return false;
    }
    return true;
}

bool OMADownloadDescParser::endElement(const QString&, const QString&, const QString& qName)
{
    DM_PRIVATE(OMADownloadDescParser);
    QString tagName = priv->m_stack.pop();
    if (qName == tagName) {
        if (tagName == productTag)
            priv->m_isProductTag = false; 
        else if (tagName == mediaObjectTag)
            priv->m_isMediaObjTag = false;
        else {
            if ( priv->m_characterData != "") {
                if (priv->m_isProductTag) {
                    if (priv->m_isMediaObjTag) {
                        priv->m_currMediaObj->setAttribute(qName, QVariant(priv->m_characterData));
                        priv->m_characterData = "";
                    } else {
                        priv->m_currProduct->setAttribute(qName, QVariant(priv->m_characterData));
                        priv->m_characterData = "";
                    }
                } else {
                    priv->m_downloadDesc->setAttribute(qName, QVariant(priv->m_characterData));
                    priv->m_characterData = "";
                }
            }
        }
    return true;
    } else {
        priv->m_errorString = QObject::tr("Tag Mismatch. Check content");
        return false;
    }
}

bool OMADownloadDescParser::characters(const QString& ch)
{
    DM_PRIVATE(OMADownloadDescParser);
    QString str = ch.trimmed();
    if (str != "")
        priv->m_characterData += str;
    return true;
}

QString OMADownloadDescParser::errorString() 
{
    DM_PRIVATE(OMADownloadDescParser);
    return priv->m_errorString;
}

bool OMADownloadDescParser::fatalError(const QXmlParseException &/*exception*/)
{
    return false;
}


OMADownloadDescriptorPrivate::OMADownloadDescriptorPrivate()
{
}

OMADownloadDescriptorPrivate::~OMADownloadDescriptorPrivate()
{
    if (!m_productList.isEmpty()) {
        for (int i=0; i<m_productList.count(); i++)
            delete m_productList[i];
     }
}

OMADownloadDescriptor* OMADownloadDescParser::downloadDescriptor()
{
    DM_PRIVATE(OMADownloadDescParser);
    return priv->m_downloadDesc;
}

OMADownloadDescriptor::OMADownloadDescriptor()
{
    DM_INITIALIZE(OMADownloadDescriptor);
}

OMADownloadDescriptor::~OMADownloadDescriptor()
{
    DM_UNINITIALIZE(OMADownloadDescriptor);
}

QVariant OMADownloadDescriptor::getAttribute(OMADownloadAttribute attr)
{
    DM_PRIVATE(OMADownloadDescriptor);
    switch(attr) {
    case OMADownloadDescName:
        return QVariant(priv->m_attrMap[nameTag]);

    case OMADownloadDescVersion:
        return QVariant(priv->m_attrMap[DDVersionTag]);

    case OMADownloadDescObjectURI:
        return QVariant(priv->m_attrMap[objectURITag]);

    case OMADownloadDescType:
        return QVariant(priv->m_attrMap[typeTag]);

    case OMADownloadDescSize:
        return QVariant(priv->m_attrMap[sizeTag]);

    case OMADownloadDescVendor:
        return QVariant(priv->m_attrMap[vendorTag]);

    case OMADownloadDescDescription:
        return QVariant(priv->m_attrMap[descriptionTag]);

    case OMADownloadDescInstallNotifyURI:
        return QVariant(priv->m_attrMap[installNotifyURITag]);

    case OMA2DownloadDescUpdatedDDURI:
        return QVariant(priv->m_attrMap[updatedDDURITag]);

    case OMADownloadDescNextURL:
        return QVariant(priv->m_attrMap[nextURLTag]);

    default:
        return QVariant();
    }
}

void OMADownloadDescriptor::setAttribute(const QString& attrName, const QVariant& value)
{
    DM_PRIVATE(OMADownloadDescriptor);
    priv->m_attrMap[attrName] = value;
}

QList<OMA2DownloadProduct*> OMADownloadDescriptor::productList()
{
    DM_PRIVATE(OMADownloadDescriptor);
    return (priv->m_productList); 
}

int OMADownloadDescriptor::productCount()
{
    DM_PRIVATE(OMADownloadDescriptor);
    return priv->m_productList.count();
}

void OMADownloadDescriptor::addProduct(OMA2DownloadProduct *Obj)
{
    DM_PRIVATE(OMADownloadDescriptor);
    priv->m_productList.append(Obj);
}


OMA2DownloadProductPrivate::OMA2DownloadProductPrivate()
{ }

OMA2DownloadProductPrivate::~OMA2DownloadProductPrivate()
{
    if (!m_mediaObjList.isEmpty()) {
        for (int i=0; i<m_mediaObjList.count(); i++)
            delete m_mediaObjList[i];
     }
}

OMA2DownloadProduct::OMA2DownloadProduct()
{
    DM_INITIALIZE(OMA2DownloadProduct);
}

OMA2DownloadProduct::~OMA2DownloadProduct()
{
    DM_UNINITIALIZE(OMA2DownloadProduct);
}

QVariant OMA2DownloadProduct::getAttribute(OMADownloadAttribute attr)
{
    DM_PRIVATE(OMA2DownloadProduct);
    switch(attr) {
    case OMA2DownloadDescProductName:
        return QVariant(priv->m_productAttrMap[nameTag]);

    case OMA2DownloadDescProductInstallNotifyURI:
        return QVariant(priv->m_productAttrMap[installNotifyURITag]);

    case OMA2DownloadDescProductSuppressConfirmation:
        return QVariant(priv->m_productAttrMap[suppressUserConfirmTag]);

    default:
        return QVariant();
    }
}

void OMA2DownloadProduct::setAttribute(const QString& attrName, const QVariant& value)
{
    DM_PRIVATE(OMA2DownloadProduct);
    priv->m_productAttrMap[attrName] = value;
}

QList<OMA2DownloadMediaObj*> OMA2DownloadProduct::mediaObjList()
{
    DM_PRIVATE(OMA2DownloadProduct);
    return (priv->m_mediaObjList);
}

int OMA2DownloadProduct::mediaCount()
{
    DM_PRIVATE(OMA2DownloadProduct);
    return priv->m_mediaObjList.count();
}

void OMA2DownloadProduct::addMediaObject(OMA2DownloadMediaObj *Obj)
{
    DM_PRIVATE(OMA2DownloadProduct);
    priv->m_mediaObjList.append(Obj);
}

qint64 OMA2DownloadProduct::albumSize()
{
    DM_PRIVATE(OMA2DownloadProduct);
    qint64 albumSize = 0;
    for(int i=0; i<priv->m_mediaObjList.count(); i++)
        albumSize = albumSize + priv->m_mediaObjList[i]->getAttribute(OMA2DownloadDescMediaObjSize).toInt();
    return (albumSize);
}


OMA2DownloadMediaObj::OMA2DownloadMediaObj()
{
    DM_INITIALIZE(OMA2DownloadMediaObj);
}

OMA2DownloadMediaObj::~OMA2DownloadMediaObj()
{
    DM_UNINITIALIZE(OMA2DownloadMediaObj);
}

QVariant OMA2DownloadMediaObj::getAttribute(OMADownloadAttribute attr)
{
    DM_PRIVATE(OMA2DownloadMediaObj);
    switch(attr) {
    case OMA2DownloadDescMediaObjName:
        return QVariant(priv->m_mediaObjAttrMap[nameTag]);

    case OMA2DownloadDescMediaObjType:
        return QVariant(priv->m_mediaObjAttrMap[typeTag]);

    case OMA2DownloadDescMediaObjSize:
        return QVariant(priv->m_mediaObjAttrMap[sizeTag]);
        
    case OMA2DownloadDescMediaObjProgressiveDl:
        return QVariant(priv->m_mediaObjAttrMap[progressiveDownloadFlagTag]);

    case OMA2DownloadDescMediaObjServer:
        return QVariant(priv->m_mediaObjAttrMap[serverTag]);

    case OMA2DownloadDescMediaObjInstallNotifyURI:
        return QVariant(priv->m_mediaObjAttrMap[installNotifyURITag]); // need to add the rest of private attributes like license, text, order when implemented in future.

    case OMA2DownloadDescMediaObjSuppressConfirmation:
        return QVariant(priv->m_mediaObjAttrMap[suppressUserConfirmTag]);

    default:
        return QVariant();
    }   
}

void OMA2DownloadMediaObj::setAttribute(const QString& attrName, const QVariant& value)
{
    DM_PRIVATE(OMA2DownloadMediaObj);
    priv->m_mediaObjAttrMap[attrName] = value;
}
