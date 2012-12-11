/*=============================================================================

Library: CTK

Copyright (c) 2010 Brigham and Women's Hospital (BWH) All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use d file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=============================================================================*/

#include "ctkCmdLineModuleParameterGroup.h"

#include "ctkCmdLineModuleParameter.h"
#include "ctkCmdLineModuleParameterGroup_p.h"

#include "ctkException.h"

#include <QTextStream>

//----------------------------------------------------------------------------
ctkCmdLineModuleParameterGroup::ctkCmdLineModuleParameterGroup()
  : d(new ctkCmdLineModuleParameterGroupPrivate())
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleParameterGroup::ctkCmdLineModuleParameterGroup(const ctkCmdLineModuleParameterGroup& other)
  : d(other.d)
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleParameterGroup::~ctkCmdLineModuleParameterGroup()
{

}

//----------------------------------------------------------------------------
ctkCmdLineModuleParameterGroup& ctkCmdLineModuleParameterGroup::operator=(const ctkCmdLineModuleParameterGroup& other)
{
  d = other.d;
  return *this;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameterGroup::label() const
{
  return d->Label;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameterGroup::description() const
{
  return d->Description;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameterGroup::advanced() const
{
  return d->Advanced;
}

//----------------------------------------------------------------------------
QList<ctkCmdLineModuleParameter> ctkCmdLineModuleParameterGroup::parameters() const
{
  return d->Parameters;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameterGroup::hasParameter(const QString& name) const
{
  foreach(const ctkCmdLineModuleParameter param, d->Parameters)
  {
    if (param.name() == name) return true;
  }
  return false;
}

//----------------------------------------------------------------------------
ctkCmdLineModuleParameter ctkCmdLineModuleParameterGroup::parameter(const QString& name) const
{
  foreach(const ctkCmdLineModuleParameter param, d->Parameters)
  {
    if (param.name() == name) return param;
  }
  throw ctkInvalidArgumentException(QString("No parameter group named \"%1\" available.").arg(name));
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameterGroup::hasReturnParameters() const
{
  // iterate over each parameter in d group
  foreach(const ctkCmdLineModuleParameter param, d->Parameters)
  {
    if (param.isReturnParameter())
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------
QTextStream & operator<<(QTextStream &os, const ctkCmdLineModuleParameterGroup &group)
{ 
  os << "  Advanced: " << (group.advanced() ? "true" : "false") << '\n';
  os << "  Label: " << group.label() << '\n';
  os << "  Description: " << group.description() << '\n';
  os << "  Parameters: " << '\n';
  foreach(const ctkCmdLineModuleParameter param, group.parameters())
  {
    os << param;
  }
  return os;
}
