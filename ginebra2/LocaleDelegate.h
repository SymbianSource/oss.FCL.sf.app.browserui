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


#ifndef LOCALEDELEGATE_H
#define LOCALEDELEGATE_H

#include <QObject>
#include <QString>


#include "ChromeWidget.h"

class QString;
namespace GVA {
/*
 * This class will be enriched later on
 * 
 */
class LocaleDelegate : public QObject {
    Q_OBJECT
  public:
    LocaleDelegate(ChromeWidget* chrome);
    ~LocaleDelegate();
    
  public slots:
    QString translateText(const QString &);
    
};

#endif // LOCALEDELEGATE_H
}
