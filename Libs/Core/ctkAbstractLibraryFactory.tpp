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

#ifndef __ctkAbstractLibraryFactory_tpp
#define __ctkAbstractLibraryFactory_tpp

// CTK includes
#include "ctkAbstractFactory.h"

//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkFactoryLibraryItem<BaseClassType>::ctkFactoryLibraryItem(const QString& _key,
                                                            const QString& _path)
  :ctkAbstractFactoryItem<BaseClassType>(_key)
  ,Path(_path)
{
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
bool ctkFactoryLibraryItem<BaseClassType>::load()
{
  this->Library.setFileName(this->path());
  bool loaded = this->Library.load();
  if (loaded)
    {
    this->resolve();
    return true;
    }
  return false;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
QString ctkFactoryLibraryItem<BaseClassType>::path()const
{ 
  return this->Path; 
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
QString ctkFactoryLibraryItem<BaseClassType>::loadErrorString()const
{ 
  return this->Library.errorString();
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkFactoryLibraryItem<BaseClassType>::setSymbols(const QStringList& symbols)
{ 
  this->Symbols = symbols; 
}

//-----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkFactoryLibraryItem<BaseClassType>::resolve()
{
  foreach(const QString& symbol, this->Symbols)
    {
    // Sanity checks
    if (symbol.isEmpty()) 
      { 
      continue; 
      }
      
    // Make sure the symbols haven't been registered
    if (this->ResolvedSymbols.contains(symbol))
      {
      qWarning() << "Symbol '" << symbol << "' already resolved - Path:" << this->Path;
      continue;
      }
    
    void * resolvedSymbol = this->Library.resolve(symbol.toLatin1());
    if (!resolvedSymbol)
      {
      qWarning() << "Failed to resolve symbol '" << symbol << "' - Path:" << this->Path;
      }
    this->ResolvedSymbols[symbol] = resolvedSymbol;
    }
}

//-----------------------------------------------------------------------------
template<typename BaseClassType>
void* ctkFactoryLibraryItem<BaseClassType>::symbolAddress(const QString& symbol)const
{
  ConstIterator iter = this->ResolvedSymbols.find(symbol);
  
  Q_ASSERT(iter != this->ResolvedSymbols.constEnd());
  if ( iter == this->ResolvedSymbols.constEnd())
    {
    return 0;
    }
  return iter.value();
}

//-----------------------------------------------------------------------------
template<typename BaseClassType, typename FactoryItemType>
ctkAbstractLibraryFactory<BaseClassType, FactoryItemType>::ctkAbstractLibraryFactory()
  :ctkAbstractFactory<BaseClassType>()
{
}
  
//-----------------------------------------------------------------------------
template<typename BaseClassType, typename FactoryItemType>
ctkAbstractLibraryFactory<BaseClassType, FactoryItemType>::~ctkAbstractLibraryFactory()
{
}

//-----------------------------------------------------------------------------
template<typename BaseClassType, typename FactoryItemType>
void ctkAbstractLibraryFactory<BaseClassType, FactoryItemType>::setSymbols(
  const QStringList& symbols) 
{
  this->Symbols = symbols; 
}

//-----------------------------------------------------------------------------
template<typename BaseClassType, typename FactoryItemType>
QString ctkAbstractLibraryFactory<BaseClassType, FactoryItemType>::fileNameToKey(
  const QString& fileName)
{
  return fileName; 
}

//-----------------------------------------------------------------------------
template<typename BaseClassType, typename FactoryItemType>
bool ctkAbstractLibraryFactory<BaseClassType, FactoryItemType>::registerLibrary(
  const QFileInfo& file, QString& key)
{
  key = this->fileNameToKey(file.fileName());
  // Check if already registered
  if (this->item(key))
    {
    return false;
    }
  QSharedPointer<FactoryItemType> _item =
    QSharedPointer<FactoryItemType>(new FactoryItemType(key, file.filePath()));
  _item->setSymbols(this->Symbols);
  return this->registerItem(_item);
}

#endif
