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

#include "ctkServiceProperties_p.h"

#include <ctkException.h>

//----------------------------------------------------------------------------
ctkServiceProperties::ctkServiceProperties(const ctkProperties& props)
{
  for(ctkProperties::ConstIterator i = props.begin(), end = props.end();
      i != end; ++i)
  {
    if (find(i.key()) != -1)
    {
      QString msg("ctkProperties object contains case variants of the key: ");
      msg += i.key();
      throw ctkInvalidArgumentException(msg);
    }
    ks.append(i.key());
    vs.append(i.value());
  }
}

//----------------------------------------------------------------------------
QVariant ctkServiceProperties::value(const QString &key) const
{
  int index = find(key);
  if (index < 0) return QVariant();
  return vs[index];
}

//----------------------------------------------------------------------------
QVariant ctkServiceProperties::value(int index) const
{
  return (index < 0 || index >= vs.size()) ? QVariant() : vs[index];
}

//----------------------------------------------------------------------------
QStringList ctkServiceProperties::keys() const
{
  QStringList result;
  for(int i = 0; i < ks.size(); ++i)
  {
    result.append(ks[i]);
  }
  return result;
}

//----------------------------------------------------------------------------
int ctkServiceProperties::find(const QString &key) const
{
  for (int i = 0; i < ks.size(); ++i)
  {
    if (ks[i].compare(key, Qt::CaseInsensitive) == 0)
      return i;
  }
  return -1;
}

//----------------------------------------------------------------------------
int ctkServiceProperties::findCaseSensitive(const QString &key) const
{
  for (int i = 0; i < ks.size(); ++i)
  {
    if (ks[i] == key)
      return i;
  }
  return -1;
}
