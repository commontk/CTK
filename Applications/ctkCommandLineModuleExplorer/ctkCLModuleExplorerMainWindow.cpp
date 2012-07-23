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
#include <ctkCmdLineModule.h>
#include <ctkCmdLineModuleFuture.h>
#include <ctkCmdLineModuleFactoryQtGui.h>
#include <ctkException.h>

#include <QDebug>

ctkCLModuleExplorerMainWindow::ctkCLModuleExplorerMainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::ctkCLModuleExplorerMainWindow),
  moduleManager(new ctkCmdLineModuleFactoryQtGui()),
  watcher(NULL)
{
  ui->setupUi(this);
}

ctkCLModuleExplorerMainWindow::~ctkCLModuleExplorerMainWindow()
{
  delete ui;
}

void ctkCLModuleExplorerMainWindow::addModuleTab(const ctkCmdLineModuleReference& moduleRef)
{
  ctkCmdLineModule* module = moduleManager.createModule(moduleRef);
  if (!module) return;

  QObject* guiHandle = module->guiHandle();

  QWidget* widget = qobject_cast<QWidget*>(guiHandle);
  int tabIndex = ui->mainTabWidget->addTab(widget, widget->objectName());
  mapTabToModuleRef[tabIndex] = module;
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
  ctkCmdLineModule* module = mapTabToModuleRef[ui->mainTabWidget->currentIndex()];
  if (!module)
  {
    qWarning() << "Invalid module instance";
    return;
  }

  QStringList cmdLineArgs = module->commandLineArguments();
  qDebug() << cmdLineArgs;

  if (watcher != NULL)
  {
    delete watcher;
  }
  watcher = new QFutureWatcher<ctkCmdLineModuleResult>();
  QObject::connect(watcher, SIGNAL(started()), this, SLOT(moduleStarted()));
  QObject::connect(watcher, SIGNAL(finished()), this, SLOT(moduleFinished()));
  QObject::connect(watcher, SIGNAL(progressValueChanged(int)), this, SLOT(moduleProgressValueChanged(int)));
  QObject::connect(watcher, SIGNAL(progressTextChanged(QString)), this, SLOT(moduleProgressTextChanged(QString)));

  ctkCmdLineModuleFuture future = module->run();
  watcher->setFuture(future);

  try
  {
    future.waitForFinished();
  }
  catch (const ctkException& e)
  {
    qDebug() << e.printStackTrace();
  }
}

void ctkCLModuleExplorerMainWindow::moduleStarted()
{
  qDebug() << "*** moduleStarted";
}

void ctkCLModuleExplorerMainWindow::moduleFinished()
{
  qDebug() << "*** moduleFinished";
}

void ctkCLModuleExplorerMainWindow::moduleProgressValueChanged(int value)
{
  qDebug() << "*** moduleProgressValueChanged int=" << value;
}

void ctkCLModuleExplorerMainWindow::moduleProgressTextChanged(QString value)
{
  qDebug() << "*** moduleProgressTextChanged QString=" << value;
}
