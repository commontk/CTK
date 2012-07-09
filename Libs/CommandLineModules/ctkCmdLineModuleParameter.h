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

#include <QSharedDataPointer>

class QTextStream;
class QStringList;

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

public:

  ctkCmdLineModuleParameter(const ctkCmdLineModuleParameter& other);
  ~ctkCmdLineModuleParameter();

  ctkCmdLineModuleParameter& operator=(const ctkCmdLineModuleParameter& other);

  QString tag() const;

//  QString cppType() const;

  QString type() const;

  QString reference() const;

  bool hidden() const;

  // Simple return types are parameters on output channel with no
  // flags and without a specified index
  bool isReturnParameter() const;

  // Has a flag or a long flag?
  bool isFlagParameter() const;

  // Is an index type?
  bool isIndexParameter() const;

  QString argType() const;

  //void setStringToType(const QString& stringToType);
  //QString stringToType() const;

  QString name() const;

  QString longFlag() const;

  QString longFlagAliasesAsString() const;
  QStringList longFlagAliases() const;

  QString deprecatedLongFlagAliasesAsString() const;
  QStringList deprecatedLongFlagAliases() const;

  QString label() const;

  bool constraints() const;

  QString maximum() const;

  QString minimum() const;

  QString step() const;

  QString description() const;

  QString channel() const;

  int index() const;

  QString defaultValue() const;

  QString flag() const;

  QString flagAliasesAsString() const;
  QStringList flagAliases() const;

  QString deprecatedFlagAliasesAsString() const;
  QStringList deprecatedFlagAliases() const;

  bool multiple() const;

  QString aggregate() const;

  QString fileExtensionsAsString() const;
  QStringList fileExtensions() const;

  QString coordinateSystem() const;

  QStringList elements() const;

private:

  friend class ctkCmdLineModuleParameterParser;
  friend class ctkCmdLineModuleXmlParser;

  ctkCmdLineModuleParameter();

  QSharedDataPointer<ctkCmdLineModuleParameterPrivate> d;
};

CTK_CMDLINEMODULE_EXPORT QTextStream& operator<<(QTextStream& os, const ctkCmdLineModuleParameter& parameter);

#endif
