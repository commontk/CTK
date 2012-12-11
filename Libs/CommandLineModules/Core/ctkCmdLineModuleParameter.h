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

#include "ctkCommandLineModulesCoreExport.h"

#include <QSharedDataPointer>

class QTextStream;
class QStringList;

struct ctkCmdLineModuleParameterPrivate;

/** 
 * \class ctkCmdLineModuleParameter
 * \brief Single parameter to a module, like a threshold of a filter.
 * \ingroup CommandLineModulesCore_API
 *
 *
 * ctkCmdLineModuleParameter describes a single parameter for a
 * module. Information on the parameter type, name, flag, label,
 * description, channel, index, default, and constraints can be
 * retrieved.
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleParameter
{

public:

  ctkCmdLineModuleParameter(const ctkCmdLineModuleParameter& other);
  ~ctkCmdLineModuleParameter();

  ctkCmdLineModuleParameter& operator=(const ctkCmdLineModuleParameter& other);

  /**
   * @return The tag name, e.g. "integer" or "image".
   */
  QString tag() const;

  /**
   * @return The type of the parameter, e.g. "scalar" or "vector" for image parameters.
   */
  QString type() const;

  /**
   * @return <code>true</code> if the parameter is declared hidden, <code>false</code>
   *         otherwise.
   */
  bool hidden() const;

  /**
   * Simple return types are parameters on the output channel with no
   * flags and with a dummy index of 1000.
   *
   * @return <code>true</code> if the parameter is a simple return type,
   *         <code>false</code> otherwise.
   */
  bool isReturnParameter() const;

  /**
   * @return <code>true</code> if the parameter has a flag (long or short),
   *         <code>false</code> otherwise.
   */
  bool isFlagParameter() const;

  /**
   * @return <code>true</code> if the parameter has an index, <code>false</code> otherwise.
   */
  bool isIndexParameter() const;

  /**
   * @return The parameter name.
   */
  QString name() const;

  /**
   * @return The (possibly empty) long flag for this parameter.
   */
  QString longFlag() const;

  /**
   * @return A (possibly empty) comma separated string of aliases for the long flag.
   */
  QString longFlagAliasesAsString() const;

  /**
   * @return A (possibly empty) list of long flag aliases.
   */
  QStringList longFlagAliases() const;

  /**
   * @return A (possibly empty) comma separated string of deprecated long flag aliases.
   */
  QString deprecatedLongFlagAliasesAsString() const;

  /**
   * @return A (possibly empty) list of deprectated long flag aliases.
   */
  QStringList deprecatedLongFlagAliases() const;

  /**
   * @return The human-readable name of this parameter.
   */
  QString label() const;

  /**
   * @return <code>true</code> if this parameter imposes constraints on the set of allowed values.
   */
  bool constraints() const;

  /**
   * @return The maximum value constraint.
   */
  QString maximum() const;

  /**
   * @return The minimum value constraint.
   */
  QString minimum() const;

  /**
   * @return The value step size constraint.
   */
  QString step() const;

  /**
   * @return A longer description of this parameter.
   */
  QString description() const;

  /**
   * @return The string "input" for input parameters and "output" for output parameters.
   */
  QString channel() const;

  /**
   * @return The parameter index or <code>-1</code> if this is not an indexed parameter.
   * @sa flag()
   * @sa longFlag()
   */
  int index() const;

  /**
   * @return The default value.
   */
  QString defaultValue() const;

  /**
   * @return The (possibly empty) flag for this parameter.
   * @sa index()
   */
  QString flag() const;

  /**
   * @return A (possibly empty) comma separated string of flag aliases.
   */
  QString flagAliasesAsString() const;

  /**
   * @return A (possibly empty) list of flag aliases.
   */
  QStringList flagAliases() const;

  /**
   * @return A (possibly empty) comma separated string of deprecated flag aliases.
   */
  QString deprecatedFlagAliasesAsString() const;

  /**
   * @return A (possibly empty) list of deprecated flag aliases.
   */
  QStringList deprecatedFlagAliases() const;

  /**
   * @return <code>true</code> if this parameter can appear multiple time in the argument list,
   *         <code>false</code> otherwise.
   */
  bool multiple() const;

  /**
   * @return A (possibly empty) comma separated list of file extensions (e.g. "*.nrrd,*.mhd") for
   *         the "file", "image", or "geometry" parameter tags.
   */
  QString fileExtensionsAsString() const;

  /**
   * @return A (possibly empty) list of file extensions.
   * @sa fileExtensionsAsString()
   */
  QStringList fileExtensions() const;

  /**
   * @return The coordinate system (either "lps", "ras", oder "ijk") for the "point" or "region"
   *         parameter tags.
   */
  QString coordinateSystem() const;

  /**
   * @return The list of valid elements for enumeration parameters (e.g. "string-enumeration").
   */
  QStringList elements() const;

private:

  friend struct ctkCmdLineModuleParameterParser;
  friend class ctkCmdLineModuleXmlParser;

  ctkCmdLineModuleParameter();

  QSharedDataPointer<ctkCmdLineModuleParameterPrivate> d;
};

CTK_CMDLINEMODULECORE_EXPORT QTextStream& operator<<(QTextStream& os, const ctkCmdLineModuleParameter& parameter);

#endif
