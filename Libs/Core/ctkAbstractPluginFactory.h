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

#ifndef __ctkAbstractPluginFactory_h
#define __ctkAbstractPluginFactory_h

// Qt includes
#include <QPluginLoader>
#include <QFileInfo>

// CTK includes
#include "ctkAbstractFileBasedFactory.h"

//----------------------------------------------------------------------------
/// \ingroup Core
template<typename BaseClassType>
class ctkFactoryPluginItem : public ctkAbstractFactoryFileBasedItem<BaseClassType>
{
public:
  explicit ctkFactoryPluginItem(const QString& path);
  virtual bool load();
  virtual QString loadErrorString()const;

protected:
  virtual BaseClassType* instanciator();

private:
  QPluginLoader    Loader;
};

//----------------------------------------------------------------------------
/// \ingroup Core
template<typename BaseClassType>
class ctkAbstractPluginFactory : public ctkAbstractFileBasedFactory<BaseClassType>
{
public:
  /// Constructor
  explicit ctkAbstractPluginFactory();

  /// Register a plugin in the factory
  virtual bool registerLibrary(const QString& key, const QFileInfo& file);

protected:
  virtual ctkAbstractFactoryItem<BaseClassType>* createFactoryPluginItem(const QFileInfo& file);

private:
  ctkAbstractPluginFactory(const ctkAbstractPluginFactory &);  /// Not implemented
  void operator=(const ctkAbstractPluginFactory&); /// Not implemented
};

#include "ctkAbstractPluginFactory.tpp"

#endif
