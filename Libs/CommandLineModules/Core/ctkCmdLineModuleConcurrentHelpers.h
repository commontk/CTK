/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

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

#ifndef CTKCMDLINEMODULECONCURRENTHELPERS_H
#define CTKCMDLINEMODULECONCURRENTHELPERS_H

#include "ctkCommandLineModulesCoreExport.h"

#include "ctkCmdLineModuleReferenceResult.h"

class ctkCmdLineModuleManager;

/**
 * \ingroup CommandLineModulesCore_API
 *
 * \brief A function object for concurrently adding modules.
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleConcurrentRegister
{

public:

  typedef ctkCmdLineModuleReferenceResult result_type;

  ctkCmdLineModuleConcurrentRegister(ctkCmdLineModuleManager* manager, bool debug = false);
  result_type operator()(const QString& moduleLocation);
  result_type operator()(const QUrl& moduleUrl);

private:

  ctkCmdLineModuleManager* ModuleManager;
  bool Debug;
};

/**
 * \ingroup CommandLineModulesCore_API
 *
 * \brief A function object for concurrently removing modules.
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleConcurrentUnRegister
{

public:

  typedef bool result_type;

  ctkCmdLineModuleConcurrentUnRegister(ctkCmdLineModuleManager* manager);

  bool operator()(const QString& moduleLocation);
  bool operator()(const QUrl& moduleUrl);
  bool operator()(const ctkCmdLineModuleReference& moduleRef);

private:

  ctkCmdLineModuleManager* ModuleManager;
};


#endif // CTKCMDLINEMODULECONCURRENTHELPERS_H
