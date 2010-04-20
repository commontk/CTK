/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkAbstractPluginFactory_h
#define __ctkAbstractPluginFactory_h

// QT includes
#include <QPluginLoader>
#include <QFileInfo>

// CTK includes
#include "ctkAbstractFactory.h"

//----------------------------------------------------------------------------
template<typename BaseClassType>
class ctkFactoryPluginItem : public ctkAbstractFactoryItem<BaseClassType>
{
public:
  explicit ctkFactoryPluginItem(const QString& key, const QString& path);
  virtual bool load();
  QString path()const;
  virtual QString loadErrorString()const;

protected:
  virtual BaseClassType* instanciator();

private:
  QPluginLoader    Loader;
  QString          Path;
};

//----------------------------------------------------------------------------
template<typename BaseClassType, typename FactoryItemType = ctkFactoryPluginItem<BaseClassType> >
class ctkAbstractPluginFactory : public ctkAbstractFactory<BaseClassType>
{
public:
  /// 
  /// Constructor
  explicit ctkAbstractPluginFactory();
  virtual ~ctkAbstractPluginFactory();

  /// 
  /// Register a plugin in the factory
  virtual bool registerLibrary(const QFileInfo& file, QString& key);

private:
  ctkAbstractPluginFactory(const ctkAbstractPluginFactory &);  /// Not implemented
  void operator=(const ctkAbstractPluginFactory&); /// Not implemented
};

#include "ctkAbstractPluginFactory.tpp"

#endif
