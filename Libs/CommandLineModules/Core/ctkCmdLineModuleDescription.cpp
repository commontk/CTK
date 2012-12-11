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

#include "ctkCmdLineModuleDescription.h"
#include "ctkCmdLineModuleDescription_p.h"

#include "ctkCmdLineModuleParameter.h"
#include "ctkCmdLineModuleParameterGroup.h"

#include "ctkException.h"

#include <QTextStream>


//----------------------------------------------------------------------------
ctkCmdLineModuleDescription::ctkCmdLineModuleDescription()
  : d(new ctkCmdLineModuleDescriptionPrivate())
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleDescription::ctkCmdLineModuleDescription(const ctkCmdLineModuleDescription &description)
  : d(description.d)
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleDescription::~ctkCmdLineModuleDescription()
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleDescription &ctkCmdLineModuleDescription::operator =(const ctkCmdLineModuleDescription &other)
{
  d = other.d;
  return *this;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleDescription::category() const
{
  return d->Category;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleDescription::title() const
{
  return d->Title;
}


//----------------------------------------------------------------------------
QString ctkCmdLineModuleDescription::categoryDotTitle() const
{
  return this->category() + "." + this->title();
}


//----------------------------------------------------------------------------
QString ctkCmdLineModuleDescription::description() const
{
  return d->Description;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleDescription::version() const
{
  return d->Version;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleDescription::documentationURL() const
{
  return d->DocumentationURL;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleDescription::license() const
{
  return d->License;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleDescription::acknowledgements() const
{
  return d->Acknowledgements;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleDescription::contributor() const
{
  return d->Contributor;
}

//----------------------------------------------------------------------------
QIcon ctkCmdLineModuleDescription::logo() const
{
  return d->Logo;
}

//----------------------------------------------------------------------------
QList<ctkCmdLineModuleParameterGroup> ctkCmdLineModuleDescription::parameterGroups() const
{
  return d->ParameterGroups;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleDescription::hasParameter(const QString& name) const
{
  // iterate over each parameter group
  foreach(const ctkCmdLineModuleParameterGroup group, d->ParameterGroups)
  {
    if (group.hasParameter(name)) return true;
  }
  return false;
}

//----------------------------------------------------------------------------
ctkCmdLineModuleParameter ctkCmdLineModuleDescription::parameter(const QString& name) const
{
  foreach(const ctkCmdLineModuleParameterGroup group, d->ParameterGroups)
  {
    if (group.hasParameter(name))
    {
      return group.parameter(name);
    }
  }
  throw ctkInvalidArgumentException(QString("No parameter named \"%1\" available.").arg(name));
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleDescription::hasReturnParameters() const
{
  // iterate over each parameter group
  foreach(const ctkCmdLineModuleParameterGroup group, d->ParameterGroups)
  {
    if (group.hasReturnParameters()) return true;
  }
  return false;
}

//----------------------------------------------------------------------------
QTextStream & operator<<(QTextStream &os, const ctkCmdLineModuleDescription &module)
{
  os << "Title: " << module.title() << '\n';
  os << "Category: " << module.category() << '\n';
  os << "Description: " << module.description() << '\n';
  os << "Version: " << module.version() << '\n';
  os << "DocumentationURL: " << module.documentationURL() << '\n';
  os << "License: " << module.license() << '\n';
  os << "Contributor: " << module.contributor() << '\n';
  os << "Acknowledgements: " << module.acknowledgements() << '\n';
  //os << "Logo: " << module.GetLogo() << '\n';

  os << "ParameterGroups: " << '\n';
  foreach(const ctkCmdLineModuleParameterGroup group, module.parameterGroups())
  {
    os << group;
  }
  return os;
}
