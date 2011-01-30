/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/


#include "ctkMTIcon_p.h"

#include <ctkPlugin.h>

ctkMTIcon::ctkMTIcon()
  : _size(-1)
{
}

ctkMTIcon::ctkMTIcon(const QString& fileName, int size, const QSharedPointer<ctkPlugin>& plugin)
  : _fileName(fileName), _size(size), _plugin(plugin)
{
}

ctkMTIcon::ctkMTIcon(const QString& fileName, const QSharedPointer<ctkPlugin>& plugin)
  : _fileName(fileName), _size(-1), _plugin(plugin)
{
}

QString ctkMTIcon::getIconName() const
{
  return _fileName;
}

int ctkMTIcon::getIconSize() const
{
  return _size;
}

QSharedPointer<ctkPlugin> ctkMTIcon::getIconPlugin() const
{
  return _plugin;
}

ctkMTIcon::operator bool () const
{
  return !_plugin.isNull();
}
