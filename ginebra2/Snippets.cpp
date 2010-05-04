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


#include "Snippets.h"
#include "ChromeSnippet.h"
#include "ChromeWidget.h"

namespace GVA {
  Snippets::Snippets(ChromeWidget * chrome, QObject * parent)
    : QObject(parent),
      m_chrome(chrome)
  {
    setObjectName("snippets");
  }

  Snippets::~Snippets()
  {
    // clear();
  }

  ChromeSnippet * Snippets::getSnippet(const QString & id)
  {
    return m_snippetMap.value(id);
  }

  void Snippets::addSnippet(ChromeSnippet * snippet, const QString & id)
  {
    m_snippetMap[id] = snippet;
  }

  QObjectList Snippets::getList() 
  {
    QObjectList result;
    foreach(GVA::ChromeSnippet *snippet, m_snippetMap) {
      result.append(snippet);
    }
    return result;
  }

  void Snippets::clear()
  {
   
    foreach(ChromeSnippet *snippet, m_snippetMap){
      if(snippet){
        //If anchored, shrink the anchor bar
        m_chrome->snippetHiding(snippet);
        //Remove about-to-be-deleted snippet from scene
        m_chrome->getScene()->removeItem(snippet->widget());
        delete snippet;
      }
    }

    m_snippetMap.clear();
  }

  void Snippets::dump() {
    qDebug() << "Snippets::dump: count=" << m_snippetMap.count();
    foreach(ChromeSnippet *snippet, m_snippetMap){
      snippet->dump();
    }
  }
}

