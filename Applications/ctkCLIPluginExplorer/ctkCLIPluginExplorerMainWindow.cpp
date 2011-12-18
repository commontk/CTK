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

#include "ctkCLIPluginExplorerMainWindow.h"
#include "ui_ctkCLIPluginExplorerMainWindow.h"

#include <ctkModuleDescriptionValidator.h>
#include <ctkModuleManager.h>
#include <ctkModuleProcessFuture.h>

#include <QFile>
#include <QBuffer>
#include <QUiLoader>
#include <QDebug>

class ctkModuleDescriptionDefaultFactory : public ctkModuleDescriptionFactory
{
public:

  QObject* createObjectRepresentationFromXML(const QByteArray &xmlDescription)
  {
    return cachedObjectTree(xmlDescription);
  }

  QObject* createGUIFromXML(const QByteArray &xmlDescription)
  {
    return cachedObjectTree(xmlDescription);
  }

private:

  QObject* cachedObjectTree(const QByteArray& xmlDescription)
  {
    QObject* root = cache[xmlDescription];
    if (root != 0) return root;

    QBuffer input;
    input.setData(xmlDescription);
    input.open(QIODevice::ReadOnly);

    ctkModuleDescriptionValidator validator(&input);
    if (!validator.validateXSLTOutput())
    {
      qCritical() << validator.errorString();
      return 0;
    }

    QUiLoader uiLoader;
    QByteArray uiBlob;
    uiBlob.append(validator.output());
    qDebug() << validator.output();
    QBuffer uiForm(&uiBlob);

    root = uiLoader.load(&uiForm);
    cache[xmlDescription] = root;
    return root;
  }

  // TODO: remove entry if QObject was deleted
  QHash<QByteArray, QObject*> cache;
};


ctkCLIPluginExplorerMainWindow::ctkCLIPluginExplorerMainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::ctkCLIPluginExplorerMainWindow),
  factory(new ctkModuleDescriptionDefaultFactory),
  moduleManager(factory)
{
  ui->setupUi(this);
}

ctkCLIPluginExplorerMainWindow::~ctkCLIPluginExplorerMainWindow()
{
  delete ui;
  delete factory;
}

void ctkCLIPluginExplorerMainWindow::addModuleTab(const ctkModuleReference& moduleRef)
{
  if (moduleRef.widgetTree() == 0) return;

  QWidget* widget = qobject_cast<QWidget*>(moduleRef.widgetTree());
  int tabIndex = ui->mainTabWidget->addTab(widget, widget->objectName());
  mapTabToModuleRef[tabIndex] = moduleRef;
}

void ctkCLIPluginExplorerMainWindow::addModule(const QString &location)
{
  ctkModuleReference ref = moduleManager.addModule(location);
  if (ref.isValid())
  {
    addModuleTab(ref);
  }
}

void ctkCLIPluginExplorerMainWindow::testModuleXML(const QByteArray &xml)
{
  QObject* root = factory->createGUIFromXML(xml);
  if (root)
  {
    QWidget* widget = qobject_cast<QWidget*>(root);
    int tabIndex = ui->mainTabWidget->addTab(widget, widget->objectName());
    //mapTabToModuleRef[tabIndex] = moduleRef;
  }
}

void ctkCLIPluginExplorerMainWindow::on_actionRun_triggered()
{
  qDebug() << "Creating module command line...";

  QStringList cmdLineArgs = ctkModuleManager::createCommandLineArgs(ui->mainTabWidget->currentWidget());
  qDebug() << cmdLineArgs;

  ctkModuleReference moduleRef = mapTabToModuleRef[ui->mainTabWidget->currentIndex()];
  if (!moduleRef.isValid())
  {
    qWarning() << "Invalid module reference";
    return;
  }

  connect(&futureWatcher, SIGNAL(finished()), this, SLOT(futureFinished()));
  ctkModuleProcessFuture future = moduleManager.run(moduleRef);
  futureWatcher.setFuture(future);
}

void ctkCLIPluginExplorerMainWindow::futureFinished()
{
  qDebug() << "*** Future finished";
  qDebug() << "stdout:" << futureWatcher.future().standardOutput();
  qDebug() << "stderr:" << futureWatcher.future().standardError();
}

ctkModuleReference ctkCLIPluginExplorerMainWindow::moduleReference(int tabIndex)
{
  return mapTabToModuleRef[tabIndex];
}
