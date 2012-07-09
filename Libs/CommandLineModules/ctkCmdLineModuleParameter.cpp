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
#include <QStringList>

struct ctkCmdLineModuleParameterPrivate
{
  ctkCmdLineModuleParameterPrivate()
    : Hidden(false), Constraints(false), Index(-1), Multiple(false), Aggregate("false")
  {}

  QString Tag;
  QString Name;
  QString Description;
  QString Label;
  //QString CPPType;
  QString Type;
  QString Reference;
  bool Hidden;
  QString ArgType;
  //QString StringToType;
  QString Default;
  QString Flag;
  QString LongFlag;
  bool Constraints;
  QString Minimum;
  QString Maximum;
  QString Step;
  QString Channel;
  int Index;
  int Multiple;
  QString Aggregate;
  QString FileExtensionsAsString;
  QStringList FileExtensions;
  QString CoordinateSystem;
  QStringList Elements;
  QString FlagAliasesAsString;
  QString DeprecatedFlagAliasesAsString;
  QString LongFlagAliasesAsString;
  QString DeprecatedLongFlagAliasesAsString;
  QStringList FlagAliases;
  QStringList DeprecatedFlagAliases;
  QStringList LongFlagAliases;
  QStringList DeprecatedLongFlagAliases;

  QStringList splitAndTrim(const QString& str, const QString& separator)
  {
    QStringList l = str.split(separator, QString::SkipEmptyParts);
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
};

//----------------------------------------------------------------------------
ctkCmdLineModuleParameter::ctkCmdLineModuleParameter()
  : d_ptr(new ctkCmdLineModuleParameterPrivate)
{ }

//----------------------------------------------------------------------------
ctkCmdLineModuleParameter::~ctkCmdLineModuleParameter()
{
  delete d_ptr;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setTag(const QString& tag)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Tag = tag;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::tag() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->Tag;
}

////----------------------------------------------------------------------------
//void ctkCmdLineModuleParameter::setCPPType(const QString& type)
//{
//  Q_D(ctkCmdLineModuleParameter);
//  d->CPPType = type;
//}

////----------------------------------------------------------------------------
//QString ctkCmdLineModuleParameter::cppType() const
//{
//  Q_D(const ctkCmdLineModuleParameter);
//  return d->CPPType;
//}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setType(const QString& type)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Type = type;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::type() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->Type;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setReference(const QString& ref)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Reference = ref;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::reference() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->Reference;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setHidden(bool hidden)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Hidden = hidden;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameter::hidden() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->Hidden;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameter::isReturnParameter() const
{
  Q_D(const ctkCmdLineModuleParameter);
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
  Q_D(const ctkCmdLineModuleParameter);
  return (d->Flag != "" || d->LongFlag != "");
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameter::isIndexParameter() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return (d->Index > -1);
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setArgType(const QString& argType)
{
  Q_D(ctkCmdLineModuleParameter);
  d->ArgType = argType;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::argType() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->ArgType;
}

////----------------------------------------------------------------------------
//void ctkCmdLineModuleParameter::setStringToType(const QString& stringToType)
//{
//  Q_D(ctkCmdLineModuleParameter);
//  d->StringToType = stringToType;
//}

////----------------------------------------------------------------------------
//QString ctkCmdLineModuleParameter::stringToType() const
//{
//  Q_D(const ctkCmdLineModuleParameter);
//  return d->StringToType;
//}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setName(const QString& name)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Name = name;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::name() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->Name;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setLongFlag(const QString& longFlag)
{
  Q_D(ctkCmdLineModuleParameter);
  d->LongFlag = longFlag;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::longFlag() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->LongFlag;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setLongFlagAliasesAsString(const QString& aliases)
{
  Q_D(ctkCmdLineModuleParameter);
  d->LongFlagAliases = d->splitAndTrim(aliases, ",");
  d->LongFlagAliasesAsString = d->LongFlagAliases.join(", ");
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::longFlagAliasesAsString() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->LongFlagAliasesAsString;
}

//----------------------------------------------------------------------------
QStringList ctkCmdLineModuleParameter::longFlagAliases() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->LongFlagAliases;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setDeprecatedLongFlagAliasesAsString(const QString& aliases)
{
  Q_D(ctkCmdLineModuleParameter);
  d->DeprecatedLongFlagAliases = d->splitAndTrim(aliases, ",");
  d->DeprecatedLongFlagAliasesAsString = d->DeprecatedLongFlagAliases.join(", ");
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::deprecatedLongFlagAliasesAsString() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->DeprecatedLongFlagAliasesAsString;
}

//----------------------------------------------------------------------------
QStringList ctkCmdLineModuleParameter::deprecatedLongFlagAliases() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->DeprecatedLongFlagAliases;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setLabel(const QString& label)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Label = label;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::label() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->Label;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setConstraints(bool constraints)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Constraints = constraints;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameter::constraints() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->Constraints;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setMaximum(const QString& maximum)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Maximum = maximum;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::maximum() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->Maximum;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setMinimum(const QString& minimum)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Minimum = minimum;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::minimum() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->Minimum;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setStep(const QString& step)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Step = step;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::step() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->Step;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setDescription(const QString& description)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Description = description;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::description() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->Description;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setChannel(const QString& channel)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Channel = channel;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::channel() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->Channel;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setIndex(int index)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Index = index;
}

//----------------------------------------------------------------------------
int ctkCmdLineModuleParameter::index() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->Index;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setDefaultValue(const QString& def)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Default = def;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::defaultValue() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->Default;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setFlag(const QString& flag)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Flag = flag;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::flag() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->Flag;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setFlagAliasesAsString(const QString& aliases)
{
  Q_D(ctkCmdLineModuleParameter);
  d->FlagAliases = d->splitAndTrim(aliases, ",");
  d->FlagAliasesAsString = d->FlagAliases.join(", ");
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::flagAliasesAsString() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->FlagAliasesAsString;
}

//----------------------------------------------------------------------------
QStringList ctkCmdLineModuleParameter::flagAliases() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->FlagAliases;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setDeprecatedFlagAliasesAsString(const QString& aliases)
{
  Q_D(ctkCmdLineModuleParameter);
  d->DeprecatedFlagAliases = d->splitAndTrim(aliases, ",");
  d->DeprecatedFlagAliasesAsString = d->DeprecatedFlagAliases.join(", ");
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::deprecatedFlagAliasesAsString() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->DeprecatedFlagAliasesAsString;
}

//----------------------------------------------------------------------------
QStringList ctkCmdLineModuleParameter::deprecatedFlagAliases() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->DeprecatedFlagAliases;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setMultiple(bool multiple)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Multiple = multiple;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleParameter::multiple() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->Multiple;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setAggregate(const QString& aggregate)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Aggregate = aggregate;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::aggregate() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->Aggregate;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setFileExtensionsAsString(const QString& extensions)
{
  Q_D(ctkCmdLineModuleParameter);
  d->FileExtensions = d->splitAndTrim(extensions, ",");
  d->FileExtensionsAsString = d->FileExtensions.join(",");
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::fileExtensionsAsString() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->FileExtensionsAsString;
}

//----------------------------------------------------------------------------
QStringList ctkCmdLineModuleParameter::fileExtensions() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->FileExtensions;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setCoordinateSystem(const QString& coordinateSystem)
{
  Q_D(ctkCmdLineModuleParameter);
  d->CoordinateSystem = coordinateSystem;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleParameter::coordinateSystem() const
{
  Q_D(const ctkCmdLineModuleParameter);
  return d->CoordinateSystem;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::addElement(const QString &elem)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Elements.push_back(elem);
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleParameter::setElements(const QStringList& elems)
{
  Q_D(ctkCmdLineModuleParameter);
  d->Elements = elems;
}

//----------------------------------------------------------------------------
QStringList ctkCmdLineModuleParameter::elements() const
{
  Q_D(const ctkCmdLineModuleParameter);
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
