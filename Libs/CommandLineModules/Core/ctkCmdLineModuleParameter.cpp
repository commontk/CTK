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

#include "ctkCmdLineModuleParameter.h"

#include "ctkCmdLineModuleParameterPrivate.h"

#include <QStringList>
#include <QTextStream>


//----------------------------------------------------------------------------
ctkCmdLineModuleParameter::ctkCmdLineModuleParameter()
  : d(new ctkCmdLineModuleParameterPrivate())
{ }

//----------------------------------------------------------------------------
ctkCmdLineModuleParameter::~ctkCmdLineModuleParameter()
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleParameter::ctkCmdLineModuleParameter(const ctkCmdLineModuleParameter &other)
  : d(other.d)
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleParameter& ctkCmdLineModuleParameter::operator=(const ctkCmdLineModuleParameter& other)
{
  d = other.d;
  return *this;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::tag() const
{
  return d->Tag;
}

////----------------------------------------------------------------------------
//QString ctkCmdLineModuleParameter::cppType() const
//{
//  Q_D(const ctkCmdLineModuleParameter);
//  return d->CPPType;
//}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::type() const
{
  return d->Type;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::reference() const
{
  return d->Reference;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameter::hidden() const
{
  return d->Hidden;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameter::isReturnParameter() const
{
  // could check for tag == float, int, float-vector, ...
  if (d->Channel == "output"
      && !this->isFlagParameter() && !this->isIndexParameter())
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameter::isFlagParameter() const
{
  return (d->Flag != "" || d->LongFlag != "");
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameter::isIndexParameter() const
{
  return (d->Index > -1);
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::argType() const
{
  return d->ArgType;
}

////----------------------------------------------------------------------------
//QString ctkCmdLineModuleParameter::stringToType() const
//{
//  Q_D(const ctkCmdLineModuleParameter);
//  return d->StringToType;
//}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::name() const
{
 return d->Name;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::longFlag() const
{
  return d->LongFlag;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::longFlagAliasesAsString() const
{
  return d->LongFlagAliasesAsString;
}

//----------------------------------------------------------------------------
QStringList ctkCmdLineModuleParameter::longFlagAliases() const
{
  return d->LongFlagAliases;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::deprecatedLongFlagAliasesAsString() const
{
  return d->DeprecatedLongFlagAliasesAsString;
}

//----------------------------------------------------------------------------
QStringList ctkCmdLineModuleParameter::deprecatedLongFlagAliases() const
{
  return d->DeprecatedLongFlagAliases;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::label() const
{
  return d->Label;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameter::constraints() const
{
  return d->Constraints;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::maximum() const
{
  return d->Maximum;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::minimum() const
{
  return d->Minimum;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::step() const
{
  return d->Step;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::description() const
{
  return d->Description;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::channel() const
{
  return d->Channel;
}

//----------------------------------------------------------------------------
int ctkCmdLineModuleParameter::index() const
{
  return d->Index;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::defaultValue() const
{
  return d->Default;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::flag() const
{
  return d->Flag;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::flagAliasesAsString() const
{
  return d->FlagAliasesAsString;
}

//----------------------------------------------------------------------------
QStringList ctkCmdLineModuleParameter::flagAliases() const
{
  return d->FlagAliases;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::deprecatedFlagAliasesAsString() const
{
  return d->DeprecatedFlagAliasesAsString;
}

//----------------------------------------------------------------------------
QStringList ctkCmdLineModuleParameter::deprecatedFlagAliases() const
{
  return d->DeprecatedFlagAliases;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameter::multiple() const
{
  return d->Multiple;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::aggregate() const
{
  return d->Aggregate;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::fileExtensionsAsString() const
{
  return d->FileExtensionsAsString;
}

//----------------------------------------------------------------------------
QStringList ctkCmdLineModuleParameter::fileExtensions() const
{
  return d->FileExtensions;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::coordinateSystem() const
{
  return d->CoordinateSystem;
}

//----------------------------------------------------------------------------
QStringList ctkCmdLineModuleParameter::elements() const
{
  return d->Elements;
}

//----------------------------------------------------------------------------
//QStringList& ctkCmdLineModuleParameter::elements()
//{
//  return d->Elements;
//}

//----------------------------------------------------------------------------
QTextStream& operator<<(QTextStream& os, const ctkCmdLineModuleParameter& parameter)
{
  os << "    Parameter" << '\n';
  os << "      " << "Tag: " << parameter.tag() << '\n';
  os << "      " << "Name: " << parameter.name() << '\n';
  os << "      " << "Description: " << parameter.description() << '\n';
  os << "      " << "Label: " << parameter.label() << '\n';
  os << "      " << "Type: " << parameter.type() << '\n';
  os << "      " << "Reference: " << parameter.reference() << '\n';
  os << "      " << "Hidden: " << (parameter.hidden() ? "true" : "false") << '\n';
  //os << "      " << "CPPType: " << parameter.cppType() << '\n';
  os << "      " << "ArgType: " << parameter.argType() << '\n';
  //os << "      " << "StringToType: " << parameter.stringToType() << '\n';
  os << "      " << "Default: " << parameter.defaultValue() << '\n';
  os << "      " << "Elements: " << parameter.elements().join(", ") << '\n';
  os << "      " << "Constraints: " << (parameter.constraints() ? "true" : "false") << '\n';
  os << "      " << "Minimum: " << parameter.minimum() << '\n';
  os << "      " << "Maximum: " << parameter.maximum() << '\n';
  os << "      " << "Step: " << parameter.step() << '\n';
  os << "      " << "Flag: " << parameter.flag() << '\n';
  os << "      " << "Flag aliases: " << parameter.flagAliasesAsString() << '\n';
  os << "      " << "Deprecated Flag aliases: " << parameter.deprecatedFlagAliasesAsString() << '\n';
  os << "      " << "LongFlag: " << parameter.longFlag() << '\n';
  os << "      " << "LongFlag aliases: " << parameter.longFlagAliasesAsString() << '\n';
  os << "      " << "Deprecated LongFlag aliases: " << parameter.deprecatedLongFlagAliasesAsString() << '\n';
  os << "      " << "Channel: " << parameter.channel() << '\n';
  os << "      " << "Index: " << parameter.index() << '\n';
  os << "      " << "Multiple: " << (parameter.multiple() ? "true" : "false") << '\n';
  os << "      " << "Aggregate: " << parameter.aggregate() << '\n';
  os << "      " << "FileExtensions: " << parameter.fileExtensionsAsString() << '\n';
  os << "      " << "CoordinateSystem: " << parameter.coordinateSystem() << '\n';
  return os;
}
