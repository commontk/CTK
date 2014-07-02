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
#include "ctkCmdLineModuleExplorerUtils.h"
#include "ctkCmdLineModuleExplorerShowXmlAction.h"
#include "ctkCmdLineModuleUtils.h"

#include "ui_ctkCmdLineModuleExplorerModulesSettings.h"

#include <ctkCmdLineModuleManager.h>
#include <ctkCmdLineModuleConcurrentHelpers.h>

#include <QUrl>
#include <QStandardItem>
#include <QtConcurrentMap>
#include <QFutureSynchronizer>

ctkCmdLineModuleExplorerModulesSettings::ctkCmdLineModuleExplorerModulesSettings(ctkCmdLineModuleManager *moduleManager)
  : ui(new Ui::ctkCmdLineModuleExplorerModulesSettings)
  , ModuleManager(moduleManager)
  , ShowXmlAction(new ctkCmdLineModuleExplorerShowXmlAction(this))
  , ModulesRegistered(false)
{
  ui->setupUi(this);

  ui->PathListButtonsWidget->init(ui->PathListWidget);
  ui->PathListWidget->addAction(this->ShowXmlAction);
  ui->PathListWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

  this->ShowXmlAction->setEnabled(false);

  connect(ui->PathListWidget, SIGNAL(currentPathChanged(QString,QString)), SLOT(pathSelected(QString)));
  connect(ui->PathListWidget, SIGNAL(pathsChanged(QStringList,QStringList)), SLOT(pathsAdded(QStringList)));

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

  this->setCursor(Qt::BusyCursor);

  QFuture<void> future1 = QtConcurrent::mapped(removedModules, ctkCmdLineModuleConcurrentUnRegister(this->ModuleManager));
  QFuture<ctkCmdLineModuleReferenceResult> future2 = QtConcurrent::mapped(addedModules, ctkCmdLineModuleConcurrentRegister(this->ModuleManager, true));

  ctkSettingsPanel::applySettings();

  future1.waitForFinished();

  /*
  QFutureSynchronizer<void> sync;
  sync.addFuture(future1);
  sync.addFuture(future2);
  sync.waitForFinished();
*/

  this->ModulesRegistered = true;
  this->pathsAdded(addedModules);
  this->ModulesRegistered = false;

  this->unsetCursor();

  future2.waitForFinished();
  ctkCmdLineModuleUtils::messageBoxModuleRegistration(future2,
                                                      this->ModuleManager->validationMode());

}

void ctkCmdLineModuleExplorerModulesSettings::pathSelected(const QString &path)
{
  this->ShowXmlAction->setEnabled(!path.isEmpty());
  ctkCmdLineModuleReference moduleRef = this->ModuleManager->moduleReference(QUrl::fromLocalFile(path));
  this->ShowXmlAction->setModuleReference(moduleRef);
}

void ctkCmdLineModuleExplorerModulesSettings::pathsAdded(const QStringList &paths)
{
  // Check the validity of the entries
  foreach(const QString& path, paths)
  {
    ctkCmdLineModuleReference moduleRef = this->ModuleManager->moduleReference(QUrl::fromLocalFile(path));
    if (!moduleRef || !moduleRef.xmlValidationErrorString().isEmpty())
    {
      QStandardItem* item = ui->PathListWidget->item(path);
      if (this->WarningIcon.isNull())
      {
        this->WarningIcon = ctkCmdLineModuleExplorerUtils::createIconOverlay(
              item->icon().pixmap(item->icon().availableSizes().front()),
              QApplication::style()->standardPixmap(QStyle::SP_MessageBoxWarning));
      }

      QString toolTip = path + "\n\n" + tr("Warning") + ":\n\n";
      if (moduleRef)
      {
        item->setIcon(this->WarningIcon);
        toolTip += moduleRef.xmlValidationErrorString();
      }
      else if (this->ModulesRegistered)
      {
        item->setIcon(this->WarningIcon);
        toolTip += tr("No XML output available.");
      }
      else
      {
        toolTip = path;
      }
      item->setToolTip(toolTip);
    }
  }
}
