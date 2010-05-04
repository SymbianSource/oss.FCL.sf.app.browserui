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


#ifndef FACTORY_H
#define FACTORY_H

#include <QMap>

/*! Simple class factory.
  Example usage: <code>Factory<ContentView, QWidget, QString> viewFactory;</code>
  \param AbstractProduct The base class of the objects that are to be created by the factory.
  \param ParamType The class of the object to be passed to the constructor of the created objects.
  \param IdentifierType The type of the class identifier, usually a string or int.
  \param ProductCreator The type of the creator method of the target class.
*/
template <
    class AbstractProduct,
    class ParamType,
    class IdentifierType,
    class ProductCreator = AbstractProduct* (*)(ParamType *)
>
class Factory
{
public:
    void subscribe(const IdentifierType& id, ProductCreator creator)
    {
        m_associations[id] = creator;
    }

    void unsubscribe(const IdentifierType& id)
    {
        m_associations.remove(id);
    }

    AbstractProduct* createObject(const IdentifierType& id, ParamType *parent)
    {
        if(m_associations.contains(id))
        {
            return (m_associations[id])(parent);
        }
        return 0;
    }

private:
    QMap<QString, ProductCreator> m_associations;
};

#endif // FACTORY_H
