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

#include "ctkModuleParameter.h"
#include <QStringList>

struct ctkModuleParameterPrivate
{
  ctkModuleParameterPrivate()
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
ctkModuleParameter::ctkModuleParameter()
  : d_ptr(new ctkModuleParameterPrivate)
{ }

//----------------------------------------------------------------------------
ctkModuleParameter::~ctkModuleParameter()
{
  delete d_ptr;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setTag(const QString& tag)
{
  Q_D(ctkModuleParameter);
  d->Tag = tag;
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::tag() const
{
  Q_D(const ctkModuleParameter);
  return d->Tag;
}

////----------------------------------------------------------------------------
//void ctkModuleParameter::setCPPType(const QString& type)
//{
//  Q_D(ctkModuleParameter);
//  d->CPPType = type;
//}

////----------------------------------------------------------------------------
//QString ctkModuleParameter::cppType() const
//{
//  Q_D(const ctkModuleParameter);
//  return d->CPPType;
//}

//----------------------------------------------------------------------------
void ctkModuleParameter::setType(const QString& type)
{
  Q_D(ctkModuleParameter);
  d->Type = type;
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::type() const
{
  Q_D(const ctkModuleParameter);
  return d->Type;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setReference(const QString& ref)
{
  Q_D(ctkModuleParameter);
  d->Reference = ref;
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::reference() const
{
  Q_D(const ctkModuleParameter);
  return d->Reference;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setHidden(bool hidden)
{
  Q_D(ctkModuleParameter);
  d->Hidden = hidden;
}

//----------------------------------------------------------------------------
bool ctkModuleParameter::hidden() const
{
  Q_D(const ctkModuleParameter);
  return d->Hidden;
}

//----------------------------------------------------------------------------
bool ctkModuleParameter::isReturnParameter() const
{
  Q_D(const ctkModuleParameter);
  // could check for tag == float, int, float-vector, ...
  if (d->Channel == "output"
      && !this->isFlagParameter() && !this->isIndexParameter())
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
bool ctkModuleParameter::isFlagParameter() const
{
  Q_D(const ctkModuleParameter);
  return (d->Flag != "" || d->LongFlag != "");
}

//----------------------------------------------------------------------------
bool ctkModuleParameter::isIndexParameter() const
{
  Q_D(const ctkModuleParameter);
  return (d->Index > -1);
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setArgType(const QString& argType)
{
  Q_D(ctkModuleParameter);
  d->ArgType = argType;
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::argType() const
{
  Q_D(const ctkModuleParameter);
  return d->ArgType;
}

////----------------------------------------------------------------------------
//void ctkModuleParameter::setStringToType(const QString& stringToType)
//{
//  Q_D(ctkModuleParameter);
//  d->StringToType = stringToType;
//}

////----------------------------------------------------------------------------
//QString ctkModuleParameter::stringToType() const
//{
//  Q_D(const ctkModuleParameter);
//  return d->StringToType;
//}

//----------------------------------------------------------------------------
void ctkModuleParameter::setName(const QString& name)
{
  Q_D(ctkModuleParameter);
  d->Name = name;
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::name() const
{
  Q_D(const ctkModuleParameter);
  return d->Name;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setLongFlag(const QString& longFlag)
{
  Q_D(ctkModuleParameter);
  d->LongFlag = longFlag;
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::longFlag() const
{
  Q_D(const ctkModuleParameter);
  return d->LongFlag;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setLongFlagAliasesAsString(const QString& aliases)
{
  Q_D(ctkModuleParameter);
  d->LongFlagAliases = d->splitAndTrim(aliases, ",");
  d->LongFlagAliasesAsString = d->LongFlagAliases.join(", ");
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::longFlagAliasesAsString() const
{
  Q_D(const ctkModuleParameter);
  return d->LongFlagAliasesAsString;
}

//----------------------------------------------------------------------------
QStringList ctkModuleParameter::longFlagAliases() const
{
  Q_D(const ctkModuleParameter);
  return d->LongFlagAliases;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setDeprecatedLongFlagAliasesAsString(const QString& aliases)
{
  Q_D(ctkModuleParameter);
  d->DeprecatedLongFlagAliases = d->splitAndTrim(aliases, ",");
  d->DeprecatedLongFlagAliasesAsString = d->DeprecatedLongFlagAliases.join(", ");
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::deprecatedLongFlagAliasesAsString() const
{
  Q_D(const ctkModuleParameter);
  return d->DeprecatedLongFlagAliasesAsString;
}

//----------------------------------------------------------------------------
QStringList ctkModuleParameter::deprecatedLongFlagAliases() const
{
  Q_D(const ctkModuleParameter);
  return d->DeprecatedLongFlagAliases;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setLabel(const QString& label)
{
  Q_D(ctkModuleParameter);
  d->Label = label;
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::label() const
{
  Q_D(const ctkModuleParameter);
  return d->Label;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setConstraints(bool constraints)
{
  Q_D(ctkModuleParameter);
  d->Constraints = constraints;
}

//----------------------------------------------------------------------------
bool ctkModuleParameter::constraints() const
{
  Q_D(const ctkModuleParameter);
  return d->Constraints;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setMaximum(const QString& maximum)
{
  Q_D(ctkModuleParameter);
  d->Maximum = maximum;
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::maximum() const
{
  Q_D(const ctkModuleParameter);
  return d->Maximum;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setMinimum(const QString& minimum)
{
  Q_D(ctkModuleParameter);
  d->Minimum = minimum;
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::minimum() const
{
  Q_D(const ctkModuleParameter);
  return d->Minimum;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setStep(const QString& step)
{
  Q_D(ctkModuleParameter);
  d->Step = step;
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::step() const
{
  Q_D(const ctkModuleParameter);
  return d->Step;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setDescription(const QString& description)
{
  Q_D(ctkModuleParameter);
  d->Description = description;
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::description() const
{
  Q_D(const ctkModuleParameter);
  return d->Description;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setChannel(const QString& channel)
{
  Q_D(ctkModuleParameter);
  d->Channel = channel;
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::channel() const
{
  Q_D(const ctkModuleParameter);
  return d->Channel;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setIndex(int index)
{
  Q_D(ctkModuleParameter);
  d->Index = index;
}

//----------------------------------------------------------------------------
int ctkModuleParameter::index() const
{
  Q_D(const ctkModuleParameter);
  return d->Index;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setDefaultValue(const QString& def)
{
  Q_D(ctkModuleParameter);
  d->Default = def;
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::defaultValue() const
{
  Q_D(const ctkModuleParameter);
  return d->Default;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setFlag(const QString& flag)
{
  Q_D(ctkModuleParameter);
  d->Flag = flag;
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::flag() const
{
  Q_D(const ctkModuleParameter);
  return d->Flag;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setFlagAliasesAsString(const QString& aliases)
{
  Q_D(ctkModuleParameter);
  d->FlagAliases = d->splitAndTrim(aliases, ",");
  d->FlagAliasesAsString = d->FlagAliases.join(", ");
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::flagAliasesAsString() const
{
  Q_D(const ctkModuleParameter);
  return d->FlagAliasesAsString;
}

//----------------------------------------------------------------------------
QStringList ctkModuleParameter::flagAliases() const
{
  Q_D(const ctkModuleParameter);
  return d->FlagAliases;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setDeprecatedFlagAliasesAsString(const QString& aliases)
{
  Q_D(ctkModuleParameter);
  d->DeprecatedFlagAliases = d->splitAndTrim(aliases, ",");
  d->DeprecatedFlagAliasesAsString = d->DeprecatedFlagAliases.join(", ");
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::deprecatedFlagAliasesAsString() const
{
  Q_D(const ctkModuleParameter);
  return d->DeprecatedFlagAliasesAsString;
}

//----------------------------------------------------------------------------
QStringList ctkModuleParameter::deprecatedFlagAliases() const
{
  Q_D(const ctkModuleParameter);
  return d->DeprecatedFlagAliases;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setMultiple(bool multiple)
{
  Q_D(ctkModuleParameter);
  d->Multiple = multiple;
}

//----------------------------------------------------------------------------
bool ctkModuleParameter::multiple() const
{
  Q_D(const ctkModuleParameter);
  return d->Multiple;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setAggregate(const QString& aggregate)
{
  Q_D(ctkModuleParameter);
  d->Aggregate = aggregate;
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::aggregate() const
{
  Q_D(const ctkModuleParameter);
  return d->Aggregate;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setFileExtensionsAsString(const QString& extensions)
{
  Q_D(ctkModuleParameter);
  d->FileExtensions = d->splitAndTrim(extensions, ",");
  d->FileExtensionsAsString = d->FileExtensions.join(",");
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::fileExtensionsAsString() const
{
  Q_D(const ctkModuleParameter);
  return d->FileExtensionsAsString;
}

//----------------------------------------------------------------------------
QStringList ctkModuleParameter::fileExtensions() const
{
  Q_D(const ctkModuleParameter);
  return d->FileExtensions;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setCoordinateSystem(const QString& coordinateSystem)
{
  Q_D(ctkModuleParameter);
  d->CoordinateSystem = coordinateSystem;
}

//----------------------------------------------------------------------------
QString ctkModuleParameter::coordinateSystem() const
{
  Q_D(const ctkModuleParameter);
  return d->CoordinateSystem;
}

//----------------------------------------------------------------------------
void ctkModuleParameter::addElement(const QString &elem)
{
  Q_D(ctkModuleParameter);
  d->Elements.push_back(elem);
}

//----------------------------------------------------------------------------
void ctkModuleParameter::setElements(const QStringList& elems)
{
  Q_D(ctkModuleParameter);
  d->Elements = elems;
}

//----------------------------------------------------------------------------
QStringList ctkModuleParameter::elements() const
{
  Q_D(const ctkModuleParameter);
  return d->Elements;
}

//----------------------------------------------------------------------------
//QStringList& ctkModuleParameter::elements()
//{
//  return d->Elements;
//}

//----------------------------------------------------------------------------
QTextStream& operator<<(QTextStream& os, const ctkModuleParameter& parameter)
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
