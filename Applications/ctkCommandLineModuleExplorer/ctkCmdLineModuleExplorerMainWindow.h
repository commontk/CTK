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

#ifndef CTKCLIPLUGINEXPLORERMAINWINDOW_H
#define CTKCLIPLUGINEXPLORERMAINWINDOW_H

#include <ctkCmdLineModuleManager.h>
#include <ctkCmdLineModuleDirectoryWatcher.h>
#include <ctkCmdLineModuleFuture.h>
#include <ctkSettings.h>

#include <QMainWindow>
#include <QTimer>
#include <QFutureWatcher>

class ctkCmdLineModuleExplorerTabList;
class ctkCmdLineModuleReference;
class ctkCmdLineModuleResult;
class ctkSettingsDialog;
class ctkCmdLineModuleBackendXMLChecker;

namespace Ui {
class ctkCmdLineModuleExplorerMainWindow;
}

/**
 * \class ctkCLModuleExplorerMainWindow
 * \brief Example application main window.
 */
class ctkCLModuleExplorerMainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit ctkCLModuleExplorerMainWindow(QWidget *parent = 0);
  ~ctkCLModuleExplorerMainWindow();

  void addModule(const QUrl &location);

protected:

  void closeEvent(QCloseEvent* event);

protected Q_SLOTS:

  void on_actionRun_triggered();
  void on_actionPause_toggled(bool toggled);
  void on_actionCancel_triggered();
  void on_actionOptions_triggered();
  void on_actionLoad_triggered();
  void on_actionQuit_triggered();

  void on_actionReset_triggered();
  void on_actionClear_Cache_triggered();
  void on_actionReload_Modules_triggered();

  void checkModulePaused();
  void currentModuleResumed();
  void currentModuleCanceled();
  void currentModuleFinished();

  void moduleTabActivated(ctkCmdLineModuleFrontend* module);
  void checkXMLPressed();

private:

  QScopedPointer<Ui::ctkCmdLineModuleExplorerMainWindow> ui;
  QScopedPointer<ctkCmdLineModuleExplorerTabList> tabList;

  ctkCmdLineModuleFrontendFactory* defaultModuleFrontendFactory;
  QList<ctkCmdLineModuleFrontendFactory*> moduleFrontendFactories;
  QList<ctkCmdLineModuleBackend*> moduleBackends;

  ctkCmdLineModuleManager moduleManager;

  QTimer pollPauseTimer;
  QFutureWatcher<ctkCmdLineModuleResult> currentFutureWatcher;
  QHash<ctkCmdLineModuleFrontend*, QByteArray> frontendToOutputMap;

  ctkCmdLineModuleDirectoryWatcher directoryWatcher;

  ctkSettings settings;
  ctkSettingsDialog* settingsDialog;
  ctkCmdLineModuleBackendXMLChecker* xmlCheckerBackEnd;
};

#endif // CTKCLIPLUGINEXPLORERMAINWINDOW_H
