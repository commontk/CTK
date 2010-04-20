/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkAbstractQObjectFactory_tpp
#define __ctkAbstractQObjectFactory_tpp

// QT includes
#include <QDebug>

// CTK includes
#include "ctkAbstractObjectFactory.h"

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
BaseClassType* ctkAbstractQObjectFactory<BaseClassType>::instantiate(const QString& itemKey)
{
  return this->ctkAbstractObjectFactory<BaseClassType>::instantiate(itemKey);
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractQObjectFactory<BaseClassType>::uninstantiate(const QString& itemKey)
{
  this->ctkAbstractObjectFactory<BaseClassType>::uninstantiate(itemKey);
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
template<typename ClassType>
bool ctkAbstractQObjectFactory<BaseClassType>::registerQObject(QString& key)
{
  key = QString::fromLatin1(ClassType::staticMetaObject.className());
  return this->ctkAbstractObjectFactory<BaseClassType>::template registerObject<ClassType>(key);
}

#endif
