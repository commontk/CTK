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

#ifndef __ctkAbstractPluginFactory_tpp
#define __ctkAbstractPluginFactory_tpp

// CTK includes
#include "ctkAbstractPluginFactory.h"

// QT includes
#include <QPluginLoader>
#include <QDebug>

//----------------------------------------------------------------------------
// ctkFactoryPluginItem methods

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkFactoryPluginItem<BaseClassType>::ctkFactoryPluginItem(const QString& _path)
  :ctkAbstractFactoryFileBasedItem<BaseClassType>(_path)
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
    if (this->verbose())
      {
      qWarning() << "Failed to instantiate plugin:" << this->path();
      }
    return 0;
    }
  BaseClassType* castedObject = qobject_cast<BaseClassType*>(object);
  if (!castedObject)
    {
    if (this->verbose())
      {
      qWarning() << "Failed to access interface [" << BaseClassType::staticMetaObject.className()
               << "] in plugin:" << this->path() << "\n instead, got object of type:" << object->metaObject()->className();
      }
    delete object; // Clean memory
    return 0;
    }
  return castedObject;
}

//----------------------------------------------------------------------------
// ctkAbstractPluginFactory methods

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractPluginFactory<BaseClassType>::ctkAbstractPluginFactory()
:ctkAbstractFileBasedFactory<BaseClassType>()
{
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
bool ctkAbstractPluginFactory<BaseClassType>::registerLibrary(const QString& key, const QFileInfo& file)
{
  // Check if already registered
  if (this->item(key))
    {
    return false;
    }
  QSharedPointer<ctkAbstractFactoryItem<BaseClassType> > _item =
    QSharedPointer<ctkAbstractFactoryItem<BaseClassType> >(
      this->createFactoryPluginItem(file));
  if (_item.isNull())
    {
    return false;
    }
  
  _item->setVerbose(this->verbose());
  return this->registerItem(key, _item);
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkAbstractFactoryItem<BaseClassType>* ctkAbstractPluginFactory<BaseClassType>::createFactoryPluginItem(const QFileInfo& file)
{
  return new ctkFactoryPluginItem<BaseClassType>(file.filePath());
}

#endif
