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

#ifndef __ctkModuleValues_h
#define __ctkModuleValues_h

// Qt includes
#include <QVector>

// Module parameter
#include "ctkModuleParameterGroupValue.h"

/**
* Description of the parameters values of a module
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
class CTK_MODULDESC_EXPORT ctkModuleValues
{
public:

  const QVector<ctkModuleParameterGroupValue*>& valueGroups() const;
  
  // Return the group that contain the parameter value associated to the name
  ctkModuleParameterGroupValue* valueGroup(const QString& parameterName) const;
  // Return the first parameter value corresponding to the name from any group
  ctkModuleParameterValue* value(const QString& parameterName) const;
  
private:
  /// Groups of parameters
  QVector<ctkModuleParameterGroupValue*> ValueGroups;
};

#endif
