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

#include <ctkCmdLineModuleManager.h>
#include <ctkCmdLineModuleFactory.h>
#include <ctkCmdLineModule.h>
#include <ctkCmdLineModuleReference.h>
#include <ctkCmdLineModuleDescription.h>
#include <ctkCmdLineModuleParameter.h>

#include <ctkCmdLineModuleXmlProgressWatcher.h>
#include <QBuffer>

#include "ctkCmdLineModuleSignalTester.h"

#include <QVariant>
#include <QCoreApplication>
#include <QDebug>
#include <QFuture>
#include <QFutureWatcher>

#include <cstdlib>


class ctkCmdLineModuleTestInstanceFactory : public ctkCmdLineModuleFactory
{
public:

  virtual ctkCmdLineModule* create(const ctkCmdLineModuleReference& moduleRef)
  {
    struct ModuleTestInstance : public ctkCmdLineModule
    {
      ModuleTestInstance(const ctkCmdLineModuleReference& moduleRef) : ctkCmdLineModule(moduleRef) {}

      virtual QObject* guiHandle() const { return NULL; }

      virtual QVariant value(const QString& parameter) const
      {
        return this->moduleReference().description().parameter(parameter).defaultValue();
      }

      virtual void setValue(const QString& /*parameter*/, const QVariant& /*value*/)
      {
        // do nothing
      }
    };

    return new ModuleTestInstance(moduleRef);
  }
};


int ctkCmdLineModuleFutureTest(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);

  ctkCmdLineModuleTestInstanceFactory factory;
  ctkCmdLineModuleManager manager(&factory);

  QString moduleFilename = app.applicationDirPath() + "/ctkCmdLineModuleTestBed";
  ctkCmdLineModuleReference moduleRef = manager.registerModule(moduleFilename);
  if (!moduleRef)
  {
    qCritical() << "Module at" << moduleFilename << "could not be registered";
  }

  ctkCmdLineModule* module = manager.createModule(moduleRef);

  QList<QString> expectedSignals;
  expectedSignals.push_back("module.started");
  expectedSignals.push_back("module.finished");

  ctkCmdLineModuleSignalTester signalTester;

  QFutureWatcher<QString> watcher;
  QObject::connect(&watcher, SIGNAL(started()), &signalTester, SLOT(moduleStarted()));
  QObject::connect(&watcher, SIGNAL(finished()), &signalTester, SLOT(moduleFinished()));

  QFuture<QString> future = module->run();
  watcher.setFuture(future);

  future.waitForFinished();

  // process pending events
  QCoreApplication::processEvents();

  delete module;

  if (!signalTester.checkSignals(expectedSignals))
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
