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

#ifndef CTKCMDLINEMODULEFRONTENDFACTORYQTWEBKIT_H
#define CTKCMDLINEMODULEFRONTENDFACTORYQTWEBKIT_H

#include "ctkCommandLineModulesFrontendQtWebKitExport.h"

#include "ctkCmdLineModuleFrontendFactory.h"

/**
 * \class ctkCmdLineModuleFrontendFactoryQtWebKit
 * \brief QtWebKit specific implementation of ctkCmdLineModuleFrontendFactory
 * \ingroup CommandLineModulesFrontendQtWebKit_API
 *
 * \warning This front-end is highly experimental and actually just serves as a
 *          proof-of-concept.
 */
class CTK_CMDLINEMODULEQTWEBKIT_EXPORT ctkCmdLineModuleFrontendFactoryQtWebKit : public ctkCmdLineModuleFrontendFactory
{

public:

  virtual QString name() const;
  virtual QString description() const;

  virtual ctkCmdLineModuleFrontend* create(const ctkCmdLineModuleReference& moduleRef);
};

#endif // CTKCMDLINEMODULEFRONTENDFACTORYQTWEBKIT_H
