/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkAbstractQObjectFactory_tpp
#define __ctkAbstractQObjectFactory_tpp

// QT includes
#include <QDebug>

// CTK includes
#include "ctkAbstractObjectFactory.h"

//----------------------------------------------------------------------------
// ctkAbstractQObjectFactory methods

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractQObjectFactory<BaseClassType>::ctkAbstractQObjectFactory()
{
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractQObjectFactory<BaseClassType>::~ctkAbstractQObjectFactory()
{
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
QString ctkAbstractQObjectFactory<BaseClassType>::objectNameToKey(const QString& objectName)
{
  return objectName; 
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
template<typename ClassType>
bool ctkAbstractQObjectFactory<BaseClassType>::registerQObject(QString& key)
{
  key = this->objectNameToKey(QString::fromLatin1(ClassType::staticMetaObject.className()));
  return this->ctkAbstractObjectFactory<BaseClassType>::template registerObject<ClassType>(key);
}

#endif
