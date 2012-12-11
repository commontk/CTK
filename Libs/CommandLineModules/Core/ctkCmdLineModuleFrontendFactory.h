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

#ifndef CTKCMDLINEMODULEFRONTENDFACTORY_H
#define CTKCMDLINEMODULEFRONTENDFACTORY_H

#include "ctkCommandLineModulesCoreExport.h"

class ctkCmdLineModuleFrontend;
class ctkCmdLineModuleReference;

/**
 * \class ctkCmdLineModuleFrontendFactory
 * \brief Factory class to create new front-ends.
 * \ingroup CommandLineModulesCore_API
 *
 * Front-end implementors are advised to create and export a sub-class of
 * this class to unify the creation process of front-ends.
 *
 * \see ctkCmdLineModuleFrontend
 */
struct CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleFrontendFactory
{
  virtual ~ctkCmdLineModuleFrontendFactory();

  /**
   * @brief Get the name of this factory.
   * @return The factory name.
   */
  virtual QString name() const = 0;

  /**
   * @brief Get the description for this factory.
   * @return A factory description.
   */
  virtual QString description() const = 0;

  /**
   * @brief Creates front-end instances.
   * @param moduleRef The module reference for which to create a front-end.
   * @return The created front-end or NULL if creation failed.
   */
  virtual ctkCmdLineModuleFrontend* create(const ctkCmdLineModuleReference& moduleRef) = 0;
};

#endif // CTKCMDLINEMODULEFRONTENDFACTORY_H
