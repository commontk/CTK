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

#ifndef __ctkModuleDescriptionExecutionInterface_h
#define __ctkModuleDescriptionExecutionInterface_h

#include <QVariant>

#include "CTKModuleDescriptionExport.h"
#include "ctkModuleDescription.h"

/** 
 * \brief Interface of ModuleDescription execution
 *
 * This allows to execute a filter using an executable application
 * or a DLL Command Line Plugin
 */
class CTK_MODULDESC_EXPORT ctkModuleDescriptionExecutionInterface : public QObject
{
    Q_OBJECT
public:
  ctkModuleDescriptionExecutionInterface(){};
  ~ctkModuleDescriptionExecutionInterface(){};

  //! Set input Module Description
  virtual void setModuleDescription(const ctkModuleDescription &val) = 0;

  //!
  virtual void Update( ) = 0;
};

#endif
