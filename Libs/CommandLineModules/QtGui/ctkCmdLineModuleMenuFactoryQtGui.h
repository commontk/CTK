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

#ifndef CTKCMDLINEMODULEMENUFACTORYQTGUI_H
#define CTKCMDLINEMODULEMENUFACTORYQTGUI_H

#include <QMenu>
#include <QHash>
#include "ctkCmdLineModuleReference.h"
#include "ctkCommandLineModulesQtGuiExport.h"

/**
 * \class ctkCmdLineModuleMenuFactoryQtGui
 * \brief Takes a QHash of filename and ctkCmdLineModuleReference and produces a QMenu.
 * \author m.clarkson@ucl.ac.uk
 */
class CTK_CMDLINEMODULEQTGUI_EXPORT ctkCmdLineModuleMenuFactoryQtGui
{
public:

  ctkCmdLineModuleMenuFactoryQtGui();
  virtual ~ctkCmdLineModuleMenuFactoryQtGui();

  /**
   * \brief Constructs a menu, for all the items in the QHash.
   * \param hashMap Hash map of filename to reference.
   * \return QMenu* a menu.
   */
  QMenu* create(const QHash<QString, ctkCmdLineModuleReference>& hashMap);
};

#endif // CTKCMDLINEMODULEINSTANCEFACTORYQTGUI_H
