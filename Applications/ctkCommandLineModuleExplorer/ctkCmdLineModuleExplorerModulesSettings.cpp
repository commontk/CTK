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

#include "ctkCmdLineModuleExplorerModulesSettings.h"
#include "ctkCmdLineModuleExplorerConstants.h"

#include "ui_ctkCmdLineModuleExplorerModulesSettings.h"

#include <ctkCmdLineModuleManager.h>
#include <ctkCmdLineModuleConcurrentHelpers.h>

#include <QUrl>
#include <QtConcurrentMap>

ctkCmdLineModuleExplorerModulesSettings::ctkCmdLineModuleExplorerModulesSettings(ctkCmdLineModuleManager *moduleManager)
  : ui(new Ui::ctkCmdLineModuleExplorerModulesSettings)
  , ModuleManager(moduleManager)
{
  ui->setupUi(this);

  ui->PathListButtonsWidget->init(ui->PathListWidget);

  this->registerProperty(ctkCmdLineModuleExplorerConstants::KEY_REGISTERED_MODULES,
                         ui->PathListWidget, "paths", SIGNAL(pathsChanged(QStringList,QStringList)));
}

ctkCmdLineModuleExplorerModulesSettings::~ctkCmdLineModuleExplorerModulesSettings()
{
  delete ui;
}

void ctkCmdLineModuleExplorerModulesSettings::applySettings()
{
  QStringList oldModules = this->previousPropertyValue(ctkCmdLineModuleExplorerConstants::KEY_REGISTERED_MODULES).toStringList();
  QStringList newModules = this->propertyValue(ctkCmdLineModuleExplorerConstants::KEY_REGISTERED_MODULES).toStringList();

  QStringList removedModules;
  QStringList addedModules = newModules;
  foreach(const QString& oldModule, oldModules)
  {
    if (!newModules.contains(oldModule))
    {
      removedModules << oldModule;
    }
    else
    {
      addedModules.removeAll(oldModule);
    }
  }

  QtConcurrent::mapped(removedModules, ctkCmdLineModuleConcurrentUnRegister(this->ModuleManager));
  QtConcurrent::mapped(addedModules, ctkCmdLineModuleConcurrentRegister(this->ModuleManager));

  ctkSettingsPanel::applySettings();
}
