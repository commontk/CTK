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

#ifndef CTKCMDLINEMODULEEXPLORERMODULESSETTINGS_H
#define CTKCMDLINEMODULEEXPLORERMODULESSETTINGS_H

#include <ctkSettingsPanel.h>

#include <QIcon>

class ctkCmdLineModuleManager;
class ctkCmdLineModuleExplorerShowXmlAction;

namespace Ui {
class ctkCmdLineModuleExplorerModulesSettings;
}

class ctkCmdLineModuleExplorerModulesSettings : public ctkSettingsPanel
{
  Q_OBJECT
  
public:
  explicit ctkCmdLineModuleExplorerModulesSettings(ctkCmdLineModuleManager* moduleManager);
  ~ctkCmdLineModuleExplorerModulesSettings();

  void applySettings();

private:

  Q_SLOT void pathSelected(const QString& path);

  Q_SLOT void pathsAdded(const QStringList& paths);

  Ui::ctkCmdLineModuleExplorerModulesSettings *ui;

  ctkCmdLineModuleManager* ModuleManager;
  ctkCmdLineModuleExplorerShowXmlAction* ShowXmlAction;
  QIcon WarningIcon;

  bool ModulesRegistered;
};

#endif // CTKCMDLINEMODULEEXPLORERMODULESSETTINGS_H
