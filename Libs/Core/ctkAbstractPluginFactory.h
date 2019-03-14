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
  bool load() override;
  virtual QString loadErrorString()const;

protected:
  BaseClassType* instanciator() override;

private:
  QPluginLoader    Loader;
};

//----------------------------------------------------------------------------
/// \ingroup Core
template<typename BaseClassType>
class ctkAbstractPluginFactory : public ctkAbstractFileBasedFactory<BaseClassType>
{
protected:
  bool isValidFile(const QFileInfo& file)const override;
  ctkAbstractFactoryItem<BaseClassType>* createFactoryFileBasedItem() override;
};

#include "ctkAbstractPluginFactory.tpp"

#endif
