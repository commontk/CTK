/*=============================================================================

Library: CTK

Copyright (c) 2010 Brigham and Women's Hospital (BWH) All Rights Reserved.

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

#include "ctkModuleParameterGroup.h"

ctkModuleParameterGroup
::ctkModuleParameterGroup(const ctkModuleParameterGroup &parameters)
  : QHash<QString, QString>( QHash<QString, QString>( parameters ) )
{
  this->Parameters = parameters.Parameters;
}

void
ctkModuleParameterGroup
::operator=(const ctkModuleParameterGroup &parameters)
{
  QHash<QString, QString>::operator=(parameters);
  this->Parameters = parameters.Parameters;
}

void ctkModuleParameterGroup::addParameter( const ctkModuleParameter &parameter )
{
  this->Parameters.push_back(parameter);
}

const QVector<ctkModuleParameter>& ctkModuleParameterGroup::parameters() const
{
  return this->Parameters;
}

QVector<ctkModuleParameter>& ctkModuleParameterGroup::parameters()
{
  return this->Parameters;
}

QTextStream & operator<<(QTextStream &os, const ctkModuleParameterGroup &group)
{ 
  os << QHash<QString, QString>(group);

  os << "  Parameters: " << endl;
  QVector<ctkModuleParameter>::const_iterator it = group.parameters().begin();
  while (it != group.parameters().end())
    {
    os << *it;
    ++it;
    }
  return os;
}
