/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

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

#ifndef __ctkAbstractPluginFactory_tpp
#define __ctkAbstractPluginFactory_tpp

// CTK includes
#include "ctkAbstractPluginFactory.h"

// QT includes
#include <QPluginLoader>
#include <QDebug>

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkFactoryPluginItem<BaseClassType>::ctkFactoryPluginItem(const QString& _key,
                                                            const QString& _path)
  :ctkAbstractFactoryItem<BaseClassType>(_key),Path(_path)
{
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
bool ctkFactoryPluginItem<BaseClassType>::load()
{
  this->Loader.setFileName(this->path());
  return this->Loader.load();
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
QString ctkFactoryPluginItem<BaseClassType>::path()const
{ 
  return this->Path; 
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
QString ctkFactoryPluginItem<BaseClassType>::loadErrorString()const
{ 
  return this->Loader.errorString();
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
BaseClassType* ctkFactoryPluginItem<BaseClassType>::instanciator()
{
  //qDebug() << "PluginItem::instantiate - name:" << this->path();
  QObject * object = this->Loader.instance();
  if (!object)
    {
    qWarning() << "Failed to instantiate plugin:" << this->path();
    return 0;
    }
  BaseClassType* castedObject = qobject_cast<BaseClassType*>(object);
  if (!castedObject)
    {
    qWarning() << "Failed to access interface [" << BaseClassType::staticMetaObject.className()
               << "] in plugin:" << this->path();
    delete object; // Clean memory
    return 0;
    }
  return castedObject;
}

//----------------------------------------------------------------------------
template<typename BaseClassType, typename FactoryItemType>
ctkAbstractPluginFactory<BaseClassType, FactoryItemType>::ctkAbstractPluginFactory():ctkAbstractFactory<BaseClassType>()
{
}

//----------------------------------------------------------------------------
template<typename BaseClassType, typename FactoryItemType>
ctkAbstractPluginFactory<BaseClassType, FactoryItemType>::~ctkAbstractPluginFactory()
{
}

//-----------------------------------------------------------------------------
template<typename BaseClassType, typename FactoryItemType>
QString ctkAbstractPluginFactory<BaseClassType, FactoryItemType>::fileNameToKey(
  const QString& fileName)
{
  return fileName; 
}

//----------------------------------------------------------------------------
template<typename BaseClassType, typename FactoryItemType>
bool ctkAbstractPluginFactory<BaseClassType, FactoryItemType>::registerLibrary(const QFileInfo& file, QString& key)
{
  key = this->fileNameToKey(file.fileName());
  // Check if already registered
  if (this->item(key))
    {
    return false;
    }
  QSharedPointer<FactoryItemType> _item =
    QSharedPointer<FactoryItemType>(new FactoryItemType(key, file.filePath()));
  return this->registerItem(_item);
}

#endif
