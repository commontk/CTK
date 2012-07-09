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

#ifndef __ctkCmdLineModuleParameter_h
#define __ctkCmdLineModuleParameter_h

#include "ctkCommandLineModulesExport.h"

#include <QTextStream>

class ctkCmdLineModuleParameterPrivate;

/** 
 *  \brief Single parameter to a module, like a threshold of a filter.
 *
 * ctkCmdLineModuleParameter describes a single parameters to a
 * module. Information on the parameter type, name, flag, label,
 * description, channel, index, default, and constraints can be
 * stored.
 *
 */
class CTK_CMDLINEMODULE_EXPORT ctkCmdLineModuleParameter
{
  Q_DECLARE_PRIVATE(ctkCmdLineModuleParameter)

public:

  ctkCmdLineModuleParameter();
  ~ctkCmdLineModuleParameter();

  void setTag(const QString& tag);
  QString tag() const;

//  void setCPPType(const QString& type);
//  QString cppType() const;

  void setType(const QString& type);
  QString type() const;

  void setReference(const QString& ref);
  QString reference() const;

  void setHidden(bool hidden);
  bool hidden() const;

  // Simple return types are parameters on output channel with no
  // flags and without a specified index
  bool isReturnParameter() const;

  // Has a flag or a long flag?
  bool isFlagParameter() const;

  // Is an index type?
  bool isIndexParameter() const;

  void setArgType(const QString& argType);
  QString argType() const;

  //void setStringToType(const QString& stringToType);
  //QString stringToType() const;

  void setName(const QString& name);
  QString name() const;

  void setLongFlag(const QString& longFlag);
  QString longFlag() const;

  void setLongFlagAliasesAsString(const QString& aliases);
  QString longFlagAliasesAsString() const;
  QStringList longFlagAliases() const;

  void setDeprecatedLongFlagAliasesAsString(const QString& aliases);
  QString deprecatedLongFlagAliasesAsString() const;
  QStringList deprecatedLongFlagAliases() const;

  void setLabel(const QString& label);
  QString label() const;

  void setConstraints(bool constraints);
  bool constraints() const;

  void setMaximum(const QString& maximum);
  QString maximum() const;

  void setMinimum(const QString& minimum);
  QString minimum() const;

  void setStep(const QString& step);
  QString step() const;

  void setDescription(const QString& description);
  QString description() const;

  void setChannel(const QString& channel);
  QString channel() const;

  void setIndex(int index);
  int index() const;

  void setDefaultValue(const QString& def);
  QString defaultValue() const;

  void setFlag(const QString& flag);
  QString flag() const;

  void setFlagAliasesAsString(const QString& aliases);
  QString flagAliasesAsString() const;
  QStringList flagAliases() const;

  void setDeprecatedFlagAliasesAsString(const QString& aliases);
  QString deprecatedFlagAliasesAsString() const;
  QStringList deprecatedFlagAliases() const;

  void setMultiple(bool multiple);
  bool multiple() const;

  void setAggregate(const QString& aggregate);
  QString aggregate() const;

  void setFileExtensionsAsString(const QString& extensions);
  QString fileExtensionsAsString() const;
  QStringList fileExtensions() const;

  void setCoordinateSystem(const QString& coordinateSystem);
  QString coordinateSystem() const;

  void addElement(const QString& elem);
  void setElements(const QStringList& elems);
  QStringList elements() const;

private:

  Q_DISABLE_COPY(ctkCmdLineModuleParameter)

  ctkCmdLineModuleParameterPrivate * const d_ptr;
};

CTK_CMDLINEMODULE_EXPORT QTextStream& operator<<(QTextStream& os, const ctkCmdLineModuleParameter& parameter);

#endif
