/*=============================================================================

  Library: CTK

  Copyright (c) University College London

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

#ifndef CTKCMDLINEMODULEPATHBUILDER_H
#define CTKCMDLINEMODULEPATHBUILDER_H

#include <QStringList>

#include "ctkCommandLineModulesCoreExport.h"

/**
 * \class ctkCmdLineModulePathBuilder
 * \brief Prototype interface for objects that can build
 * up a list of file paths, stored in a QStringList.
 * \author m.clarkson@ucl.ac.uk
 */
struct CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModulePathBuilder
{
  virtual ~ctkCmdLineModulePathBuilder();

  virtual QStringList build() = 0;
};

#endif // CTKCMDLINEMODULEPATHBUILDER_H
