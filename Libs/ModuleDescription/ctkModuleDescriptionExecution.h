/*=============================================================================

Library: CTK

Copyright (c) 2010 CISTIB - Universitat Pompeu Fabra

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

#ifndef __ctkModuleDescriptionExecution_h
#define __ctkModuleDescriptionExecution_h

#include <QString>

#include "ctkModuleDescriptionExport.h"
#include "ctkModuleDescriptionExecutionInterface.h"

/** 
 * \brief Base Reader of ModuleDescription
 *
 * This is the base interface
 */
class CTK_MODULDESC_EXPORT ctkModuleDescriptionExecution :
  virtual public ctkModuleDescriptionExecutionInterface
{
  Q_OBJECT
public:
  ctkModuleDescriptionExecution();
  ~ctkModuleDescriptionExecution();

  ///
  virtual void setModuleDescription(const ctkModuleDescription &val);

protected:
  ///
  ctkModuleDescription ModuleDescription;
};

#endif
