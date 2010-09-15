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

#ifndef __ctkModuleDescription_h
#define __ctkModuleDescription_h

#include "QHash"
#include "QIcon"

#include "ctkModuleParameterGroup.h"

/**
* Description of the parameters of a module
*
* The parameters can be used for automated GUI generation or execution
* of the module.
*
* For example:
* - Target: This is the entry point for a shared object module and the full 
* command (with path) for an executable.
* - Type: Unknown, SharedObjectModule, CommandLineModule
* - AlternativeTarget: This is the entry
* point for a shared object module and the full command (with path)
* for an executable. The alternative target is used for a second version
* of a module (whose type differs from the primary target,
* executable verses shared object).
* - Location: This is path to the file (shared
* object or executable) for the module
* - AlternativeLocation: This is path to the
* file (shared object or executable) for a second version of the
* module (usually a different type from the primary).
*/
class CTK_MODULDESC_EXPORT ctkModuleDescription : public QHash<QString, QString>
{
public:
  ctkModuleDescription();
  ctkModuleDescription(const ctkModuleDescription &md);

  void operator=(const ctkModuleDescription &md);

  
  void setLogo(const QIcon& logo);
  const QIcon& logo() const;
  
  void addParameterGroup(const ctkModuleParameterGroup &group);

  const QVector<ctkModuleParameterGroup>& parameterGroups() const;

  QVector<ctkModuleParameterGroup>& parameterGroups();
  
  void setParameterGroups(const QVector<ctkModuleParameterGroup>& groups);

  // Does the module have any simple (primitive) return types?
  bool hasReturnParameters() const;

  bool setParameterDefaultValue(const QString& name,
                                const QString& value);

  ctkModuleParameter* parameter(const QString& name);

  ///
  /// Read a parameter file. Syntax of file is "name: value" for each
  /// parameter. Returns a bool indicating whether any parameter value
  /// was modified.
  bool readParameterFile(const QString& filename);

  ///
  /// Write a parameter file. By default, the method writes out all
  /// the parameters.  The "withHandlesToBulkParameters" parameter
  /// controls whether the handles to the bulk parameters (image,
  /// geometry, etc.) are written to the file.
  bool writeParameterFile(const QString& filename, bool withHandlesToBulkParameters = true);


private:

  /// Groups of parameters
  QVector<ctkModuleParameterGroup> ParameterGroups;
  /// Logo
  QIcon Logo;
};

CTK_MODULDESC_EXPORT QTextStream & operator<<(QTextStream &os, const ctkModuleDescription &module);

#endif
