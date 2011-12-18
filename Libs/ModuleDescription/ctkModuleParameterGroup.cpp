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

#include "ctkModuleParameterGroup.h"

struct ctkModuleParameterGroupPrivate
{
  ~ctkModuleParameterGroupPrivate()
  {
    qDeleteAll(Parameters);
  }

  QString Label;
  QString Description;
  bool Advanced;
  QList<ctkModuleParameter*> Parameters;
};

//----------------------------------------------------------------------------
ctkModuleParameterGroup::ctkModuleParameterGroup()
  : d_ptr(new ctkModuleParameterGroupPrivate())
{
}

//----------------------------------------------------------------------------
ctkModuleParameterGroup::~ctkModuleParameterGroup()
{
  delete d_ptr;
}

//----------------------------------------------------------------------------
void ctkModuleParameterGroup::setLabel(const QString& label)
{
  Q_D(ctkModuleParameterGroup);
  d->Label = label;
}

//----------------------------------------------------------------------------
QString ctkModuleParameterGroup::label() const
{
  Q_D(const ctkModuleParameterGroup);
  return d->Label;
}

//----------------------------------------------------------------------------
void ctkModuleParameterGroup::setDescription(const QString& description)
{
  Q_D(ctkModuleParameterGroup);
  d->Description = description;
}

//----------------------------------------------------------------------------
QString ctkModuleParameterGroup::description() const
{
  Q_D(const ctkModuleParameterGroup);
  return d->Description;
}

//----------------------------------------------------------------------------
void ctkModuleParameterGroup::setAdvanced(bool advanced)
{
  Q_D(ctkModuleParameterGroup);
  d->Advanced = advanced;
}

//----------------------------------------------------------------------------
bool ctkModuleParameterGroup::advanced() const
{
  Q_D(const ctkModuleParameterGroup);
  return d->Advanced;
}

//----------------------------------------------------------------------------
void ctkModuleParameterGroup::addParameter(ctkModuleParameter* parameter)
{
  Q_D(ctkModuleParameterGroup);
  d->Parameters.push_back(parameter);
}

//----------------------------------------------------------------------------
QList<ctkModuleParameter*> ctkModuleParameterGroup::parameters() const
{
  Q_D(const ctkModuleParameterGroup);
  return d->Parameters;
}

//----------------------------------------------------------------------------
bool ctkModuleParameterGroup::hasParameter(const QString& name) const
{
  Q_D(const ctkModuleParameterGroup);
  foreach(const ctkModuleParameter* param, d->Parameters)
  {
    if (param->name() == name) return true;
  }
  return false;
}

//----------------------------------------------------------------------------
ctkModuleParameter* ctkModuleParameterGroup::parameter(const QString& name) const
{
  Q_D(const ctkModuleParameterGroup);
  foreach(ctkModuleParameter* param, d->Parameters)
  {
    if (param->name() == name) return param;
  }
  return 0;
}

//----------------------------------------------------------------------------
bool ctkModuleParameterGroup::hasReturnParameters() const
{
  Q_D(const ctkModuleParameterGroup);
  // iterate over each parameter in d group
  foreach(const ctkModuleParameter* param, d->Parameters)
  {
    if (param->isReturnParameter())
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------
bool ctkModuleParameterGroup::writeParameterFile(QTextStream& in, bool withHandlesToBulkParameters) const
{
  Q_D(const ctkModuleParameterGroup);
  // iterate over each parameter in d group
  foreach(const ctkModuleParameter* param, d->Parameters)
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
QTextStream & operator<<(QTextStream &os, const ctkModuleParameterGroup &group)
{ 
  os << "  Advanced: " << (group.advanced() ? "true" : "false") << '\n';
  os << "  Label: " << group.label() << '\n';
  os << "  Description: " << group.description() << '\n';
  os << "  Parameters: " << '\n';
  foreach(ctkModuleParameter* param, group.parameters())
  {
    os << *param;
  }
  return os;
}
