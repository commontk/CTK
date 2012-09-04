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

#ifndef CTKCMDLINEMODULEFRONTENDFACTORYQTGUI_H
#define CTKCMDLINEMODULEFRONTENDFACTORYQTGUI_H

#include "ctkCommandLineModulesFrontendQtGuiExport.h"

#include "ctkCmdLineModuleFrontendFactory.h"
#include "ctkCmdLineModuleFrontendQtGui.h"

/**
 * \class ctkCmdLineModuleFrontendFactoryQtGui
 * \brief Factory class to instantiate Qt widget based front-ends.
 * \ingroup CommandLineModulesFrontendQtGui_API
 *
 * The created front-end instances assume that the CTKWidgetPlugin library (a Qt Designer plug-in)
 * is available in the applications search path. See also ctkCmdLineModuleFrontendQtGui.
 *
 * @see ctkCmdLineModuleFrontendQtGui
 */
class CTK_CMDLINEMODULEQTGUI_EXPORT ctkCmdLineModuleFrontendFactoryQtGui : public ctkCmdLineModuleFrontendFactory
{

public:

  virtual QString name() const;
  virtual QString description() const;

  virtual ctkCmdLineModuleFrontendQtGui* create(const ctkCmdLineModuleReference& moduleRef);
};

#endif // CTKCMDLINEMODULEFRONTENDFACTORYQTGUI_H
