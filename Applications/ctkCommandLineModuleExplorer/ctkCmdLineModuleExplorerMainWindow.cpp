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

#include "ctkCmdLineModuleExplorerMainWindow.h"
#include "ui_ctkCmdLineModuleExplorerMainWindow.h"

#include "ctkCmdLineModuleExplorerGeneralModuleSettings.h"
#include "ctkCmdLineModuleExplorerDirectorySettings.h"
#include "ctkCmdLineModuleExplorerModulesSettings.h"
#include "ctkCmdLineModuleExplorerTabList.h"
#include "ctkCmdLineModuleExplorerProgressWidget.h"
#include "ctkCmdLineModuleExplorerConstants.h"
#include "ctkCmdLineModuleExplorerUtils.h"

#include <ctkCmdLineModuleManager.h>
#include <ctkCmdLineModuleConcurrentHelpers.h>
#include <ctkCmdLineModuleDescription.h>
#include <ctkCmdLineModuleFrontendFactoryQtGui.h>
#include <ctkCmdLineModuleFrontendFactoryQtWebKit.h>
#include <ctkCmdLineModuleBackendLocalProcess.h>
#include <ctkCmdLineModuleBackendFunctionPointer.h>
#include <ctkException.h>
#include <ctkCmdLineModuleXmlException.h>

#include <ctkSettingsDialog.h>

#include <QtConcurrentMap>
#include <QDesktopServices>
#include <QMessageBox>
#include <QFutureSynchronizer>
#include <QCloseEvent>
#include <QFileDialog>


ctkCLModuleExplorerMainWindow::ctkCLModuleExplorerMainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::ctkCmdLineModuleExplorerMainWindow),
  defaultModuleFrontendFactory(NULL),
  moduleManager(ctkCmdLineModuleManager::WEAK_VALIDATION, QDesktopServices::storageLocation(QDesktopServices::CacheLocation)),
  directoryWatcher(&moduleManager),
  settingsDialog(NULL)
{
  ui->setupUi(this);

  settings.restoreState(this->objectName(), *this);

  if (!settings.contains(ctkCmdLineModuleExplorerConstants::KEY_MAX_PARALLEL_MODULES))
  {
    settings.setValue(ctkCmdLineModuleExplorerConstants::KEY_MAX_PARALLEL_MODULES, QThread::idealThreadCount());
  }
  QThreadPool::globalInstance()->setMaxThreadCount(settings.value(ctkCmdLineModuleExplorerConstants::KEY_MAX_PARALLEL_MODULES,
                                                                  QThread::idealThreadCount()).toInt());

  // Frontends
  moduleFrontendFactories << new ctkCmdLineModuleFrontendFactoryQtGui;
  moduleFrontendFactories << new ctkCmdLineModuleFrontendFactoryQtWebKit;
  defaultModuleFrontendFactory = moduleFrontendFactories.front();

  ui->modulesTreeWidget->setModuleFrontendFactories(moduleFrontendFactories, moduleFrontendFactories.front());
  ui->modulesSearchBox->setClearIcon(QIcon(":/icons/clear.png"));
  connect(ui->modulesSearchBox, SIGNAL(textChanged(QString)), ui->modulesTreeWidget, SLOT(setFilter(QString)));

  // Backends
  ctkCmdLineModuleBackendFunctionPointer* backendFunctionPointer = new ctkCmdLineModuleBackendFunctionPointer;

  moduleBackends.push_back(new ctkCmdLineModuleBackendLocalProcess);
  moduleBackends.push_back(backendFunctionPointer);
  for(int i = 0; i < moduleBackends.size(); ++i)
  {
    moduleManager.registerBackend(moduleBackends[i]);
  }

  tabList.reset(new ctkCmdLineModuleExplorerTabList(ui->mainTabWidget));

  // If a module is registered via the ModuleManager, add it to the tree
  connect(&moduleManager, SIGNAL(moduleRegistered(ctkCmdLineModuleReference)), ui->modulesTreeWidget, SLOT(addModuleItem(ctkCmdLineModuleReference)));
  connect(&moduleManager, SIGNAL(moduleUnregistered(ctkCmdLineModuleReference)), ui->modulesTreeWidget, SLOT(removeModuleItem(ctkCmdLineModuleReference)));
  // React to specific frontend creations
  connect(ui->modulesTreeWidget, SIGNAL(moduleFrontendCreated(ctkCmdLineModuleFrontend*)), tabList.data(), SLOT(addTab(ctkCmdLineModuleFrontend*)));
  // React to tab-changes
  connect(tabList.data(), SIGNAL(tabActivated(ctkCmdLineModuleFrontend*)), SLOT(moduleTabActivated(ctkCmdLineModuleFrontend*)));
  connect(tabList.data(), SIGNAL(tabActivated(ctkCmdLineModuleFrontend*)), ui->progressListWidget, SLOT(setCurrentProgressWidget(ctkCmdLineModuleFrontend*)));
  connect(tabList.data(), SIGNAL(tabClosed(ctkCmdLineModuleFrontend*)), ui->outputText, SLOT(frontendRemoved(ctkCmdLineModuleFrontend*)));
  connect(tabList.data(), SIGNAL(tabClosed(ctkCmdLineModuleFrontend*)), ui->progressListWidget, SLOT(removeProgressWidget(ctkCmdLineModuleFrontend*)));

  connect(ui->progressListWidget, SIGNAL(progressWidgetClicked(ctkCmdLineModuleFrontend*)), tabList.data(), SLOT(setActiveTab(ctkCmdLineModuleFrontend*)));

  connect(ui->ClearButton, SIGNAL(clicked()), ui->progressListWidget, SLOT(clearList()));

  // Listen to future events for the currently active tab

  // Due to Qt bug 12152, we cannot listen to the "paused" signal because it is
  // not emitted directly when the QFuture is paused. Instead, it is emitted after
  // resuming the future, after the "resume" signal has been emitted... we use
  // a polling aproach instead.
  pollPauseTimer.setInterval(300);
  connect(&pollPauseTimer, SIGNAL(timeout()), SLOT(checkModulePaused()));
  connect(&currentFutureWatcher, SIGNAL(resumed()), SLOT(currentModuleResumed()));
  connect(&currentFutureWatcher, SIGNAL(canceled()), SLOT(currentModuleCanceled()));
  connect(&currentFutureWatcher, SIGNAL(finished()), SLOT(currentModuleFinished()));

  foreach(QUrl fpModule, backendFunctionPointer->registeredFunctionPointers())
  {
    moduleManager.registerModule(fpModule);
  }

  // Register persistent modules
  QFuture<void> future = QtConcurrent::mapped(settings.value(ctkCmdLineModuleExplorerConstants::KEY_REGISTERED_MODULES).toStringList(),
                                              ctkCmdLineModuleConcurrentRegister(&moduleManager, true));

  // Start watching directories
  directoryWatcher.setDebug(true);
  directoryWatcher.setDirectories(settings.value(ctkCmdLineModuleExplorerConstants::KEY_SEARCH_PATHS).toStringList());

  moduleTabActivated(NULL);

  pollPauseTimer.start();

  future.waitForFinished();
}

ctkCLModuleExplorerMainWindow::~ctkCLModuleExplorerMainWindow()
{
  qDeleteAll(moduleBackends);
  qDeleteAll(moduleFrontendFactories);

  settings.saveState(*this, this->objectName());
  if (settingsDialog)
  {
    settings.saveState(*settingsDialog, settingsDialog->objectName());
  }
}

void ctkCLModuleExplorerMainWindow::addModule(const QUrl &location)
{
  moduleManager.registerModule(location);
}

void ctkCLModuleExplorerMainWindow::closeEvent(QCloseEvent *event)
{
  QList<ctkCmdLineModuleFrontend*> runningFrontends;
  foreach (ctkCmdLineModuleFrontend* frontend, this->tabList->tabs())
  {
    if (frontend->isRunning())
    {
      runningFrontends << frontend;
    }
  }

  if (!runningFrontends.empty())
  {
    QMessageBox::StandardButton button =
        QMessageBox::warning(QApplication::activeWindow(),
                             QString("Closing %1 running modules").arg(runningFrontends.size()),
                             "Some modules are still running.\n"
                             "Closing the application will cancel all current computations.",
                             QMessageBox::Ok | QMessageBox::Cancel);
    if (button == QMessageBox::Ok)
    {
      QFutureSynchronizer<void> futureSync;
      futureSync.setCancelOnWait(true);
      foreach(ctkCmdLineModuleFrontend* frontend, runningFrontends)
      {
        if (frontend->future().canCancel())
        {
          futureSync.addFuture(frontend->future());
        }
      }
      futureSync.waitForFinished();
      event->accept();
      QMainWindow::closeEvent(event);
      return;
    }
    else
    {
      event->ignore();
      return;
    }
  }
  event->accept();
}

void ctkCLModuleExplorerMainWindow::on_actionRun_triggered()
{
  ctkCmdLineModuleFrontend* moduleFrontend = this->tabList->activeTab();
  Q_ASSERT(moduleFrontend);

  ui->actionRun->setEnabled(false);
  qobject_cast<QWidget*>(moduleFrontend->guiHandle())->setEnabled(false);

  ctkCmdLineModuleFuture future = moduleManager.run(moduleFrontend);

  ui->progressListWidget->addProgressWidget(moduleFrontend, future);
  ui->progressListWidget->setCurrentProgressWidget(moduleFrontend);

  ui->actionPause->setEnabled(future.canPause() && future.isRunning());
  ui->actionPause->setChecked(future.isPaused());
  ui->actionCancel->setEnabled(future.canCancel() && future.isRunning());

  this->currentFutureWatcher.setFuture(future);
}

void ctkCLModuleExplorerMainWindow::on_actionPause_toggled(bool toggled)
{
  this->currentFutureWatcher.setPaused(toggled);
}

void ctkCLModuleExplorerMainWindow::on_actionCancel_triggered()
{
  this->currentFutureWatcher.cancel();
}

void ctkCLModuleExplorerMainWindow::on_actionOptions_triggered()
{
  if (settingsDialog == NULL)
  {
    settingsDialog = new ctkSettingsDialog(this);
    settings.restoreState(settingsDialog->objectName(), *settingsDialog);
    settingsDialog->setSettings(&settings);
    ctkSettingsPanel* generalModulePanel = new ctkCmdLineModuleExplorerGeneralModuleSettings();
    settingsDialog->addPanel(generalModulePanel);
    settingsDialog->addPanel(new ctkCmdLineModuleExplorerDirectorySettings(&directoryWatcher), generalModulePanel);
    settingsDialog->addPanel(new ctkCmdLineModuleExplorerModulesSettings(&moduleManager), generalModulePanel);
  }

  settingsDialog->exec();
}

void ctkCLModuleExplorerMainWindow::on_actionLoad_triggered()
{
  QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Load modules..."));

  this->setCursor(Qt::BusyCursor);
  QFuture<ctkCmdLineModuleReference> future = QtConcurrent::mapped(fileNames, ctkCmdLineModuleConcurrentRegister(&this->moduleManager));
  future.waitForFinished();
  this->unsetCursor();

  ctkCmdLineModuleExplorerUtils::messageBoxModuleRegistration(fileNames, future.results(),
                                                              this->moduleManager.validationMode());
}

void ctkCLModuleExplorerMainWindow::on_actionQuit_triggered()
{
  this->close();
}

void ctkCLModuleExplorerMainWindow::on_actionReset_triggered()
{
  this->tabList->activeTab()->resetValues();
}

void ctkCLModuleExplorerMainWindow::checkModulePaused()
{
  if (this->currentFutureWatcher.future().isPaused())
  {
    if (!ui->actionPause->isChecked())
    {
      ui->actionPause->setChecked(true);
    }
  }
  else
  {
    if (ui->actionPause->isChecked())
    {
      ui->actionPause->setChecked(false);
    }
  }
}

void ctkCLModuleExplorerMainWindow::currentModuleResumed()
{
  ui->actionPause->setChecked(false);
}

void ctkCLModuleExplorerMainWindow::currentModuleCanceled()
{
  ctkCmdLineModuleFrontend* frontend = this->tabList->activeTab();
  if (frontend)
  {
    ui->actionCancel->setEnabled(false);
    ui->actionPause->setEnabled(false);
    ui->actionRun->setEnabled(true);
    QWidget* widget = qobject_cast<QWidget*>(frontend->guiHandle());
    if (widget)
    {
      widget->setEnabled(true);
    }
  }
}

void ctkCLModuleExplorerMainWindow::currentModuleFinished()
{
  ctkCmdLineModuleFrontend* frontend = this->tabList->activeTab();
  if (frontend)
  {
    ui->actionCancel->setEnabled(false);
    ui->actionPause->setEnabled(false);
    ui->actionRun->setEnabled(true);
    QWidget* widget = qobject_cast<QWidget*>(frontend->guiHandle());
    if (widget)
    {
      widget->setEnabled(true);
    }
  }
}

void ctkCLModuleExplorerMainWindow::moduleTabActivated(ctkCmdLineModuleFrontend *module)
{
  if (module == NULL)
  {
    ui->actionRun->setEnabled(false);
    ui->actionPause->setEnabled(false);
    ui->actionCancel->setEnabled(false);
    ui->actionReset->setEnabled(false);
    ui->outputText->setActiveFrontend(NULL);
    currentFutureWatcher.setFuture(ctkCmdLineModuleFuture());
  }
  else
  {
    ui->actionRun->setEnabled(!module->isRunning());
    ui->actionPause->setEnabled(module->future().canPause() && module->isRunning());
    ui->actionPause->blockSignals(true);
    ui->actionPause->setChecked(module->isPaused());
    ui->actionPause->blockSignals(false);
    ui->actionCancel->setEnabled(module->future().canCancel() && module->isRunning());
    ui->actionReset->setEnabled(true);
    ui->outputText->setActiveFrontend(module);
    currentFutureWatcher.setFuture(module->future());
  }
}
