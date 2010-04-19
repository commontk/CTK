/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

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

//----------------------------------------------------------------------------
template<typename BaseClassType, typename FactoryItemType>
bool ctkAbstractPluginFactory<BaseClassType, FactoryItemType>::registerLibrary(const QFileInfo& file, QString& key)
{
  key = file.fileName();
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
