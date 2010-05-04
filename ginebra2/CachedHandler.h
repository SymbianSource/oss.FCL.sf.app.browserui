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


#ifndef __GINEBRA_CACHEDHANDLER_H__
#define __GINEBRA_CACHEDHANDLER_H__

#include <QString>
#include <QRect>

namespace GVA {

  class ChromeWidget;

  class CachedHandler
  {
  public:
    CachedHandler(const QString & elementId, const QString & script, const QRectF & rect, ChromeWidget * m_chrome, const QString &targetView);
    QString elementId() const {return m_id;}
    QString script() const {return m_script;}
    inline const QRectF & rect() const {return m_rect;}
    void invoke() const;
  private:
    QString m_id;
    QString m_script;
    QRectF m_rect;
    ChromeWidget * m_chrome;
    QString m_targetView;
  };

} // end of namespace GVA

#endif // __GINEBRA_CACHEDHANDLER_H__
