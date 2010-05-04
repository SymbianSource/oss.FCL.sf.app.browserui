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

#ifndef __gva_snippets_h__
#define __gva_snippets_h__

#include <QObject>
#include <QMap>


namespace GVA{

  class ChromeSnippet;
  class ChromeWidget;

  class Snippets : public QObject
  {
  public:
    Snippets(ChromeWidget * chrome, QObject * parent);
    virtual ~Snippets();
    ChromeSnippet * getSnippet(const QString & id);
    void addSnippet(ChromeSnippet *, const QString & id);
    QObjectList getList(); 
    void clear();
    void dump();
  private:
    QMap<QString, ChromeSnippet *> m_snippetMap;
    ChromeWidget * m_chrome;
  };
}

#endif
