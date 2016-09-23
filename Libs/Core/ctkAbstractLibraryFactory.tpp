/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkAbstractLibraryFactory_tpp
#define __ctkAbstractLibraryFactory_tpp

// CTK includes
#include "ctkAbstractLibraryFactory.h"

//----------------------------------------------------------------------------
// ctkFactoryLibraryItem methods
/*
//----------------------------------------------------------------------------
template<typename BaseClassType>
ctkFactoryLibraryItem<BaseClassType>::ctkFactoryLibraryItem(const QString& _path)
  :ctkAbstractFactoryFileBasedItem<BaseClassType>(_path)
{
}*/

//----------------------------------------------------------------------------
template<typename BaseClassType>
bool ctkFactoryLibraryItem<BaseClassType>::load()
{
  this->Library.setFileName(this->path());
  bool loaded = this->Library.load();
  if (loaded)
    {
    if (!this->resolve())
      {
      this->appendLoadErrorString(this->Library.errorString());
      return false;
      }
    return true;
    }
  else
    {
    this->appendLoadErrorString(this->Library.errorString());
    }
  return false;
}

//----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkFactoryLibraryItem<BaseClassType>::setSymbols(const QStringList& symbols)
{ 
  this->Symbols = symbols; 
}

//-----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkFactoryLibraryItem<BaseClassType>
::setLoadHints(QLibrary::LoadHints hints)
{
  this->Library.setLoadHints(hints);
}

//-----------------------------------------------------------------------------
template<typename BaseClassType>
bool ctkFactoryLibraryItem<BaseClassType>::resolve()
{
  foreach(const QString& symbol, this->Symbols)
    {
    // Sanity checks
    if (symbol.isEmpty()) 
      {
      this->appendLoadErrorString(QLatin1String("Failed to resolve empty symbol !"));
      continue;
      }

    // Skip if the symbols has already been resolved
    if (this->ResolvedSymbols.contains(symbol))
      {
      this->appendLoadWarningString(QString("Symbol '%1' already resolved").arg(symbol));
      continue;
      }

    SymbolAddressType resolvedSymbol = this->Library.resolve(symbol.toLatin1());
    if (!resolvedSymbol)
      {
      this->appendLoadErrorString(QString("Failed to resolve mandatory symbol '%1'").arg(symbol));
      return false;
      }
    this->ResolvedSymbols[symbol] = resolvedSymbol;
    }
  return true;
}

//-----------------------------------------------------------------------------
template<typename BaseClassType>
typename ctkFactoryLibraryItem<BaseClassType>::SymbolAddressType
ctkFactoryLibraryItem<BaseClassType>::symbolAddress(const QString& symbol)const
{
  ConstIterator iter = this->ResolvedSymbols.find(symbol);
  if ( iter == this->ResolvedSymbols.constEnd())
    {
    return this->Library.resolve(symbol.toLatin1());
    }
  return iter.value();
}

//----------------------------------------------------------------------------
// ctkAbstractLibraryFactory methods

//-----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractLibraryFactory<BaseClassType>::setSymbols(
  const QStringList& symbols)
{
  this->Symbols = symbols;
}

//-----------------------------------------------------------------------------
template<typename BaseClassType>
bool ctkAbstractLibraryFactory<BaseClassType>
::isValidFile(const QFileInfo& file)const
{
  return this->ctkAbstractFileBasedFactory<BaseClassType>::isValidFile(file) &&
    QLibrary::isLibrary(file.fileName());
}

//-----------------------------------------------------------------------------
template<typename BaseClassType>
void ctkAbstractLibraryFactory<BaseClassType>
::initItem(ctkAbstractFactoryItem<BaseClassType>* item)
{
  this->ctkAbstractFileBasedFactory<BaseClassType>::initItem(item);
  dynamic_cast<ctkFactoryLibraryItem<BaseClassType>*>(item)->setSymbols(this->Symbols);
}

#endif
