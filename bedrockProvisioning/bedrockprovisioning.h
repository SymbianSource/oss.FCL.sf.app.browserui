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


#ifndef BEDROCK_PROVISIONING_H
#define BEDROCK_PROVISIONING_H

#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include "bedrockprovisioningglobal.h"

#define BEDROCK_PROVISIONING_UID "200267EA"
#define BEDROCK_PROVISIONING_NOT_FOUND -1
#define BEDROCK_ORGANIZATION_NAME "Nokia"

namespace BEDROCK_PROVISIONING {
	
class BEDROCKPROVISIONING_EXPORT BedrockProvisioningEnum 
{

friend class BedrockProvisioning ;

public:

    enum BedrockProvisioningType{
        NodeType
       ,StringType
       ,CommandType
       ,ListType
       ,IntType
       ,DblType
    };
    enum BedrockProvisioningCategory{
       RootCategory = -1
       ,Category1
       ,Category2
       ,Category3
       ,Category4
    };
    enum BedrockProvisioningFlag{
        NoFlags = 0
       ,ReadOnly = 1
       ,WebAttribute = 2
       ,Hidden = 4
       //, = 8
       //, = 16
       //, = 32
       //, = 64
    // Componant level settings for filtering setting changes in controller
       ,DatabaseSetting = 128
       ,BookmarksSetting = 256
    };
    
    BedrockProvisioningEnum(const QString &key);

    QVariant value();
    QString displayValue();
    QString key() const { return m_key; }
    QString desc() const { return m_desc; }
    QStringList valid() const  { return m_validList; }
    QStringList values() const { return m_valuesList; }
    int flags() const { return m_flags; }
    int type() const { return m_type; }
    int category() const { return m_category; }
    int attributeEnum() const { return m_enum; }
   // bool isList() { return m_type == ::ListType; }
    bool isAction() { return m_type == BedrockProvisioningEnum::CommandType; }
    bool hasRange() { return (m_type == BedrockProvisioningEnum::IntType && valid().size()==2 && values().isEmpty()); }
    int minValue() { return (hasRange() ? valid().at(0).toInt() : 0 ); }
    int maxValue() { return (hasRange() ? valid().at(1).toInt() : 0 ); }
    QString group() const { return m_group; }
   
private:

    BedrockProvisioningEnum();

protected:

    void setValid(const QString& valid);
    void setValues(const QString& values);
    void setFlags(int flags) { m_flags = flags; }
   
    QString m_key;
    QString m_desc;
    QString m_group;
    int m_type;
    QVariant m_value;
    QStringList m_validList;
    QStringList m_valuesList;
    int m_flags;
    int m_enum;
    int m_category;
};	
	
class BEDROCKPROVISIONING_EXPORT BedrockProvisioning : public QSettings
{
    Q_OBJECT
signals:
    void settingChanged(const QString &key);
public:
    static BedrockProvisioning* createBedrockProvisioning();
    
    QList<BedrockProvisioningEnum> settings(const QString &group = QString());
    BedrockProvisioningEnum setting(const QString &key);

    
    QString valueAsString(const QString &key, const QVariant &defaultValue = QVariant());
    int valueAsInt(const QString &key, const QVariant &defaultValue = QVariant());
    double valueAsDouble(const QString &key, const QVariant &defaultValue = QVariant());
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant());
    int setValue(const QString &key, const int value);
    int setValue(const QString &key, const double value);
    int setValue(const QString &key, const QString &value);
    int setValue(const QString &key, const QVariant &value);
    void resync(); 
    
    void setFactorySettings();
    
private:
    BedrockProvisioning( QObject *parent = 0, QString uid=BEDROCK_PROVISIONING_UID  );
    void init(bool clear = false);
    void addProvisioning(const QString &key, QSettings::SettingsMap& att, bool forceRefresh = false);
    
private:
    static BedrockProvisioning* m_BedrockProvisioning;
    QString m_appuid;
};
}  //BEDROCK_PROVISIONING namespace
#endif //BEDROCK_PROVISIONING_H
