/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkAbstractFactory_tpp
#define __ctkAbstractFactory_tpp

#include "ctkAbstractFactory.h"
#include <QDebug>

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractFactoryItem<BaseClassType>::ctkAbstractFactoryItem(const QString& _key)
  :Instance()
  ,Key(_key)
{
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
QString ctkAbstractFactoryItem<BaseClassType>::loadErrorString()const
{ 
  return QString(); 
}

//----------------------------------------------------------------------------
template<typename BaseClassType>  
BaseClassType* ctkAbstractFactoryItem<BaseClassType>::instantiate()
{
  if (this->Instance)
    {
    return this->Instance;
    }
  this->Instance = this->instanciator();
  return this->Instance;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
bool ctkAbstractFactoryItem<BaseClassType>::instantiated() 
{
  return (this->Instance != 0); 
}


//----------------------------------------------------------------------------
template<typename BaseClassType>
QString ctkAbstractFactoryItem<BaseClassType>::key() 
{ 
  return this->Key; 
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactoryItem<BaseClassType>::uninstantiate()
{
  if (!this->Instance)
    {
    return;
    }
  delete this->Instance;
  // Make sure the pointer is set to 0. Doing so, Will prevent attempt to
  // delete unextising object if uninstantiate() methods is called multiple times.
  this->Instance = 0; 
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractFactory<BaseClassType>::ctkAbstractFactory()
{
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractFactory<BaseClassType>::~ctkAbstractFactory()
{
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactory<BaseClassType>::printAdditionalInfo()
{
  qDebug() << "ctkAbstractFactory<BaseClassType> (" << this << ")";
  // TODO
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
BaseClassType* ctkAbstractFactory<BaseClassType>::instantiate(const QString& itemKey)
{
  ctkAbstractFactoryItem<BaseClassType>* _item = this->item(itemKey);
  return (_item ? _item->instantiate() : 0);
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactory<BaseClassType>::uninstantiate(const QString& itemKey)
{
  ctkAbstractFactoryItem<BaseClassType> * _item = this->item(itemKey);
  if (!_item)
    {
    return;
    }
  _item->uninstantiate();
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
QStringList ctkAbstractFactory<BaseClassType>::names() const
{
  // Since by construction, we checked if a name was already in the QHash,
  // there is no need to call 'uniqueKeys'
  return this->RegisteredItemMap.keys();
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
bool ctkAbstractFactory<BaseClassType>::registerItem(
  const QSharedPointer<ctkAbstractFactoryItem<BaseClassType> > & _item)
{
  // Sanity checks
  if (!_item || _item->key().isEmpty() || this->item(_item->key()))
    {
    return false;
    }
  
  // Attempt to load it
  if (!_item->load())
    {
    QString errorStr;
    if (!_item->loadErrorString().isEmpty())
      {
      errorStr = " - " + _item->loadErrorString();
      }
    qCritical() << "Failed to load object:" << _item->key() << errorStr ;
    return false;
    }
  
  // Store its reference using a QSharedPointer
  this->RegisteredItemMap[_item->key()] = _item;
  return true;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractFactoryItem<BaseClassType> * ctkAbstractFactory<BaseClassType>::item(const QString& itemKey)const
{
  ConstIterator iter = this->RegisteredItemMap.find(itemKey);
  if ( iter == this->RegisteredItemMap.constEnd())
    {
    return 0;
    }
  return iter.value().data();
}

#endif
