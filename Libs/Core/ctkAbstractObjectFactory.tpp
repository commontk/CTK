/*=========================================================================

  Library:   CTK
 
  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
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
