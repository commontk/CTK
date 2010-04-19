/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkAbstractObjectFactory_tpp
#define __ctkAbstractObjectFactory_tpp

// CTK includes
#include "ctkAbstractObjectFactory.h"

// QT includes
#include <QDebug>

//----------------------------------------------------------------------------
template<typename BaseClassType, typename ClassType>
ctkFactoryObjectItem<BaseClassType,ClassType>::ctkFactoryObjectItem(const QString& _key)
  :ctkAbstractFactoryItem<BaseClassType>(_key)
{
}

//----------------------------------------------------------------------------
template<typename BaseClassType, typename ClassType>
bool ctkFactoryObjectItem<BaseClassType,ClassType>::load()
{
  this->instantiateObjectFunc = &instantiateObject<BaseClassType, ClassType>;
  return true;
}

//----------------------------------------------------------------------------
template<typename BaseClassType, typename ClassType>
BaseClassType* ctkFactoryObjectItem<BaseClassType,ClassType>::instanciator()
{
  return this->instantiateObjectFunc();
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractObjectFactory<BaseClassType>::ctkAbstractObjectFactory()
{
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractObjectFactory<BaseClassType>::~ctkAbstractObjectFactory()
{
}

//-----------------------------------------------------------------------------
template<typename BaseClassType>
template<typename ClassType>
bool ctkAbstractObjectFactory<BaseClassType>::registerObject(const QString& key)
{
  // Check if already registered
  if (this->item(key))
    {
    return false;
    }
  QSharedPointer<ctkFactoryObjectItem<BaseClassType, ClassType> > objectItem =
    QSharedPointer<ctkFactoryObjectItem<BaseClassType, ClassType> >(
      new ctkFactoryObjectItem<BaseClassType, ClassType>(key) );
  return this->registerItem(objectItem);
}

#endif
