/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkAbstractFactory_tpp
#define __ctkAbstractFactory_tpp

// QT includes
#include <QDebug>

// CTK includes
#include "ctkAbstractFactory.h"

//----------------------------------------------------------------------------
// ctkAbstractFactoryItem methods

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractFactoryItem<BaseClassType>::ctkAbstractFactoryItem()
  :Instance()
{
  this->Verbose = false;
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
bool ctkAbstractFactoryItem<BaseClassType>::instantiated()const 
{
  return (this->Instance != 0); 
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
void ctkAbstractFactoryItem<BaseClassType>::setVerbose(bool value)
{
  this->Verbose = value;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
bool ctkAbstractFactoryItem<BaseClassType>::verbose()const
{
  return this->Verbose;
}

//----------------------------------------------------------------------------
// ctkAbstractFactory methods

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractFactory<BaseClassType>::ctkAbstractFactory()
{
  this->Verbose = false;
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
QStringList ctkAbstractFactory<BaseClassType>::keys() const
{
  // Since by construction, we checked if a name was already in the QHash,
  // there is no need to call 'uniqueKeys'
  return this->RegisteredItemMap.keys();
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
bool ctkAbstractFactory<BaseClassType>::registerItem(const QString& key,
  const QSharedPointer<ctkAbstractFactoryItem<BaseClassType> > & _item)
{
  // Sanity checks
  if (!_item || key.isEmpty() || this->item(key))
    {
    if (this->verbose())
      {
      qDebug() << __FUNCTION__ << "key is empty or already exists:"
               << key << "; item: " << _item;
      }
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
    if (this->verbose())
      {
      qCritical() << "Failed to load object:" << key << errorStr ;
      }
    return false;
    }
  
  // Store its reference using a QSharedPointer
  this->RegisteredItemMap[key] = _item;
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

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractFactory<BaseClassType>::setVerbose(bool value)
{
  this->Verbose = value;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
bool ctkAbstractFactory<BaseClassType>::verbose()const
{
  return this->Verbose;
}

#endif
