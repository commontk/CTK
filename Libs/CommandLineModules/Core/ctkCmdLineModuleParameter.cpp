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

#include "ctkCmdLineModuleParameter_p.h"

#include <QStringList>
#include <QTextStream>

//----------------------------------------------------------------------------
ctkCmdLineModuleParameterPrivate::ctkCmdLineModuleParameterPrivate()
  : Hidden(false), Constraints(false), Channel("input"), Index(-1), Multiple(false)
{}

//----------------------------------------------------------------------------
QStringList ctkCmdLineModuleParameterPrivate::splitAndTrim(const QString& str, const QString& separator)
{
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QStringList l = str.split(separator, Qt::SkipEmptyParts);
  #else
  QStringList l = str.split(separator, QString::SkipEmptyParts);
  #endif
  l.removeDuplicates();
  // trim the strings
  QMutableStringListIterator i(l);
  while(i.hasNext())
  {
    QString& n = i.next();
    n = n.trimmed();
  }
  return l;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameterPrivate::setFileExtensionsAsString(const QString& extensions)
{
  FileExtensions = splitAndTrim(extensions, ",");
  FileExtensionsAsString = FileExtensions.join(",");
}

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

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::type() const
{
  return d->Type;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameter::hidden() const
{
  return d->Hidden;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameter::isReturnParameter() const
{
  if (d->Channel == "output" && this->isIndexParameter() &&
      this->index() == 1000)
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
QTextStream& operator<<(QTextStream& os, const ctkCmdLineModuleParameter& parameter)
{
  os << "    Parameter" << '\n';
  os << "      " << "Tag: " << parameter.tag() << '\n';
  os << "      " << "Name: " << parameter.name() << '\n';
  os << "      " << "Description: " << parameter.description() << '\n';
  os << "      " << "Label: " << parameter.label() << '\n';
  os << "      " << "Type: " << parameter.type() << '\n';
  os << "      " << "Hidden: " << (parameter.hidden() ? "true" : "false") << '\n';
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
  os << "      " << "FileExtensions: " << parameter.fileExtensionsAsString() << '\n';
  os << "      " << "CoordinateSystem: " << parameter.coordinateSystem() << '\n';
  return os;
}
