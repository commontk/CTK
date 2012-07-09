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

struct ctkCmdLineModuleParameterGroupPrivate
{
  ~ctkCmdLineModuleParameterGroupPrivate()
  {
    qDeleteAll(Parameters);
  }

  QString Label;
  QString Description;
  bool Advanced;
  QList<ctkCmdLineModuleParameter*> Parameters;
};

//----------------------------------------------------------------------------
ctkCmdLineModuleParameterGroup::ctkCmdLineModuleParameterGroup()
  : d_ptr(new ctkCmdLineModuleParameterGroupPrivate())
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleParameterGroup::~ctkCmdLineModuleParameterGroup()
{
  delete d_ptr;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameterGroup::setLabel(const QString& label)
{
  Q_D(ctkCmdLineModuleParameterGroup);
  d->Label = label;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameterGroup::label() const
{
  Q_D(const ctkCmdLineModuleParameterGroup);
  return d->Label;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameterGroup::setDescription(const QString& description)
{
  Q_D(ctkCmdLineModuleParameterGroup);
  d->Description = description;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameterGroup::description() const
{
  Q_D(const ctkCmdLineModuleParameterGroup);
  return d->Description;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameterGroup::setAdvanced(bool advanced)
{
  Q_D(ctkCmdLineModuleParameterGroup);
  d->Advanced = advanced;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameterGroup::advanced() const
{
  Q_D(const ctkCmdLineModuleParameterGroup);
  return d->Advanced;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameterGroup::addParameter(ctkCmdLineModuleParameter* parameter)
{
  Q_D(ctkCmdLineModuleParameterGroup);
  d->Parameters.push_back(parameter);
}

//----------------------------------------------------------------------------
QList<ctkCmdLineModuleParameter*> ctkCmdLineModuleParameterGroup::parameters() const
{
  Q_D(const ctkCmdLineModuleParameterGroup);
  return d->Parameters;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameterGroup::hasParameter(const QString& name) const
{
  Q_D(const ctkCmdLineModuleParameterGroup);
  foreach(const ctkCmdLineModuleParameter* param, d->Parameters)
  {
    if (param->name() == name) return true;
  }
  return false;
}

//----------------------------------------------------------------------------
ctkCmdLineModuleParameter* ctkCmdLineModuleParameterGroup::parameter(const QString& name) const
{
  Q_D(const ctkCmdLineModuleParameterGroup);
  foreach(ctkCmdLineModuleParameter* param, d->Parameters)
  {
    if (param->name() == name) return param;
  }
  return 0;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameterGroup::hasReturnParameters() const
{
  Q_D(const ctkCmdLineModuleParameterGroup);
  // iterate over each parameter in d group
  foreach(const ctkCmdLineModuleParameter* param, d->Parameters)
  {
    if (param->isReturnParameter())
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameterGroup::writeParameterFile(QTextStream& in, bool withHandlesToBulkParameters) const
{
  Q_D(const ctkCmdLineModuleParameterGroup);
  // iterate over each parameter in d group
  foreach(const ctkCmdLineModuleParameter* param, d->Parameters)
  {
    // write out all parameters or just the ones that are not bulk parameters
    QString tag = param->tag();
    if (withHandlesToBulkParameters ||
        !(tag == "image" || tag == "geometry" || tag == "transform" ||
          tag == "table" || tag == "measurement" ||
          tag == "point" || tag == "region"))  // point and region are special
    {
      in << param->name() << " = " << param->defaultValue() << endl;
      // multiple="true" may have to be handled differently
    }
  }
  return true;
}

//----------------------------------------------------------------------------
QTextStream & operator<<(QTextStream &os, const ctkCmdLineModuleParameterGroup &group)
{ 
  os << "  Advanced: " << (group.advanced() ? "true" : "false") << '\n';
  os << "  Label: " << group.label() << '\n';
  os << "  Description: " << group.description() << '\n';
  os << "  Parameters: " << '\n';
  foreach(ctkCmdLineModuleParameter* param, group.parameters())
  {
    os << *param;
  }
  return os;
}
