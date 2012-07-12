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

#include "ctkCLModuleExplorerMainWindow.h"
#include "ui_ctkCLModuleExplorerMainWindow.h"

#include <ctkCmdLineModuleXmlValidator.h>
#include <ctkCmdLineModuleManager.h>
#include <ctkCmdLineModuleInstance.h>
#include <ctkCmdLineModuleInstanceFactoryQtGui.h>
//#include <ctkCmdLineModuleFuture.h>

#include <QDebug>

ctkCLModuleExplorerMainWindow::ctkCLModuleExplorerMainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::ctkCLModuleExplorerMainWindow),
  moduleManager(new ctkCmdLineModuleInstanceFactoryQtGui())
{
  ui->setupUi(this);
}

ctkCLModuleExplorerMainWindow::~ctkCLModuleExplorerMainWindow()
{
  delete ui;
}

void ctkCLModuleExplorerMainWindow::addModuleTab(const ctkCmdLineModuleReference& moduleRef)
{
  ctkCmdLineModuleInstance* moduleInstance = moduleManager.createModuleInstance(moduleRef);
  if (!moduleInstance) return;

  QObject* guiHandle = moduleInstance->guiHandle();

  QWidget* widget = qobject_cast<QWidget*>(guiHandle);
  int tabIndex = ui->mainTabWidget->addTab(widget, widget->objectName());
  mapTabToModuleRef[tabIndex] = moduleInstance;
}

void ctkCLModuleExplorerMainWindow::addModule(const QString &location)
{
  ctkCmdLineModuleReference ref = moduleManager.registerModule(location);
  if (ref)
  {
    addModuleTab(ref);
  }
}

void ctkCLModuleExplorerMainWindow::on_actionRun_triggered()
{
  qDebug() << "Creating module command line...";

//  QStringList cmdLineArgs = ctkCmdLineModuleManager::createCommandLineArgs(ui->mainTabWidget->currentWidget());
//  qDebug() << cmdLineArgs;

  ctkCmdLineModuleInstance* moduleInstance = mapTabToModuleRef[ui->mainTabWidget->currentIndex()];
  if (!moduleInstance)
  {
    qWarning() << "Invalid module instance";
    return;
  }

  //ctkCmdLineModuleProcessFuture future = moduleInstance->run();
  //future.waitForFinished();
  //qDebug() << future.standardOutput();

//  connect(&futureWatcher, SIGNAL(finished()), this, SLOT(futureFinished()));
//  ctkCmdLineModuleProcessFuture future = moduleManager.run(moduleRef);
//  futureWatcher.setFuture(future);
}

void ctkCLModuleExplorerMainWindow::futureFinished()
{
  qDebug() << "*** Future finished";
  //qDebug() << "stdout:" << futureWatcher.future().standardOutput();
  //qDebug() << "stderr:" << futureWatcher.future().standardError();
}

//ctkCmdLineModuleReference ctkCLModuleExplorerMainWindow::moduleReference(int tabIndex)
//{
//  return mapTabToModuleRef[tabIndex];
//}
