/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

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

#ifndef __ctkAbstractLibraryFactory_h
#define __ctkAbstractLibraryFactory_h

// Qt includes
#include <QFileInfo>
#include <QLibrary>
#include <QStringList>

// CTK includes
#include "ctkAbstractFactory.h"

//----------------------------------------------------------------------------
template<typename BaseClassType>
class ctkFactoryLibraryItem : public ctkAbstractFactoryItem<BaseClassType>
{
protected:
  typedef typename QHash<QString, void*>::const_iterator ConstIterator;
  typedef typename QHash<QString, void*>::iterator       Iterator;

public:
  explicit ctkFactoryLibraryItem(const QString& key, const QString& path);
  virtual bool load();
  QString path()const;
  virtual QString loadErrorString()const;

  void setSymbols(const QStringList& symbols);

  //-----------------------------------------------------------------------------
  /// 
  /// Resolve symbols
  void resolve();
  
  //-----------------------------------------------------------------------------
  /// 
  /// Get symbol address
  void* symbolAddress(const QString& symbol)const;

private:
  QLibrary              Library;
  QString               Path;
  QHash<QString, void*> ResolvedSymbols;
  QStringList           Symbols;
};

//----------------------------------------------------------------------------
template<typename BaseClassType, typename FactoryItemType>
class ctkAbstractLibraryFactory : public ctkAbstractFactory<BaseClassType>
{
public:
  //-----------------------------------------------------------------------------
  /// 
  /// Constructor
  explicit ctkAbstractLibraryFactory();
  virtual ~ctkAbstractLibraryFactory();

  //-----------------------------------------------------------------------------
  /// 
  /// Set the list of symbols
  void setSymbols(const QStringList& symbols);

  //-----------------------------------------------------------------------------
  /// 
  /// Register a plugin in the factory
  virtual bool registerLibrary(const QFileInfo& file, QString& key);

private:
  ctkAbstractLibraryFactory(const ctkAbstractLibraryFactory &);  /// Not implemented
  void operator=(const ctkAbstractLibraryFactory&); /// Not implemented

  QStringList Symbols;
};

#include "ctkAbstractLibraryFactory.tpp"

#endif
