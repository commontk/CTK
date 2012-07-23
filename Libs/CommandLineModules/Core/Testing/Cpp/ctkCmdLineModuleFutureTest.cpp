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
#include <ctkCmdLineModuleRunException.h>
#include <ctkCmdLineModuleFuture.h>

#include "ctkCmdLineModuleSignalTester.h"

#include <QVariant>
#include <QCoreApplication>
#include <QDebug>
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
        QVariant value = currentValues[parameter];
        if (!value.isValid())
          return this->moduleReference().description().parameter(parameter).defaultValue();
        return value;
      }

      virtual void setValue(const QString& parameter, const QVariant& value)
      {
        currentValues[parameter] = value;
      }

    private:

      QHash<QString, QVariant> currentValues;
    };

    return new ModuleTestInstance(moduleRef);
  }
};

bool futureTestStartFinish(ctkCmdLineModule* module)
{
  qDebug() << "Testing ctkCmdLineModuleFuture start/finish signals.";

  QList<QString> expectedSignals;
  expectedSignals.push_back("module.started");
  expectedSignals.push_back("module.finished");

  ctkCmdLineModuleSignalTester signalTester;

  QFutureWatcher<ctkCmdLineModuleResult> watcher;
  QObject::connect(&watcher, SIGNAL(started()), &signalTester, SLOT(moduleStarted()));
  QObject::connect(&watcher, SIGNAL(finished()), &signalTester, SLOT(moduleFinished()));

  ctkCmdLineModuleFuture future = module->run();
  watcher.setFuture(future);

  future.waitForFinished();

  // process pending events
  QCoreApplication::processEvents();

  return signalTester.checkSignals(expectedSignals);
}

bool futureTestProgress(ctkCmdLineModule* module)
{
  qDebug() << "Testing ctkCmdLineModuleFuture progress signals.";

  QList<QString> expectedSignals;
  expectedSignals.push_back("module.started");
  // this signal is send when connecting a QFutureWatcher to
  // an already started QFuture
  expectedSignals.push_back("module.progressValueChanged");

  // the following two signals are send when the module reports "filter start"
  expectedSignals.push_back("module.progressValueChanged");
  expectedSignals.push_back("module.progressTextChanged");

  // this signal is send when the module reports progress for "output1"
  expectedSignals.push_back("module.progressValueChanged");

  // the following two signal are sent at the end to report
  // completion and the full standard output text.
  expectedSignals.push_back("module.progressValueChanged");
  expectedSignals.push_back("module.progressTextChanged");
  expectedSignals.push_back("module.finished");

  ctkCmdLineModuleSignalTester signalTester;

  QFutureWatcher<ctkCmdLineModuleResult> watcher;
  QObject::connect(&watcher, SIGNAL(started()), &signalTester, SLOT(moduleStarted()));
  QObject::connect(&watcher, SIGNAL(progressValueChanged(int)), &signalTester, SLOT(moduleProgressValueChanged(int)));
  QObject::connect(&watcher, SIGNAL(progressTextChanged(QString)), &signalTester, SLOT(moduleProgressTextChanged(QString)));
  QObject::connect(&watcher, SIGNAL(finished()), &signalTester, SLOT(moduleFinished()));

  module->setValue("fileVar", "output1");
  ctkCmdLineModuleFuture future = module->run();
  watcher.setFuture(future);

  future.waitForFinished();

  // process pending events
  QCoreApplication::processEvents();

  return signalTester.checkSignals(expectedSignals);
}

bool futureTestError(ctkCmdLineModule* module)
{
  qDebug() << "Testing ctkCmdLineModuleFuture error reporting.";

  module->setValue("fileVar", "output1");
  module->setValue("exitCodeVar", 24);
  module->setValue("errorTextVar", "Some error occured\n");

  QFutureWatcher<ctkCmdLineModuleResult> watcher;
  ctkCmdLineModuleFuture future = module->run();
  watcher.setFuture(future);

  try
  {
    future.waitForFinished();
    return EXIT_FAILURE;
  }
  catch (const ctkCmdLineModuleRunException& e)
  {
    Q_ASSERT_X(e.errorCode() == 24, __FUNCTION__, "Error code mismatch");
    Q_ASSERT_X(e.errorString() == "Some error occured\n", __FUNCTION__, "Error text mismatch");
  }

  // process pending events
  QCoreApplication::processEvents();

  return true;
}

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

  QScopedPointer<ctkCmdLineModule> module(manager.createModule(moduleRef));

  if (!futureTestStartFinish(module.data()))
  {
    return EXIT_FAILURE;
  }

  if (!futureTestProgress(module.data()))
  {
    return EXIT_FAILURE;
  }

  if (!futureTestError(module.data()))
  {
    return EXIT_FAILURE;
  }

//  if (!futureTestPauseAndCancel(module.data()))
//  {
//    return EXIT_FAILURE;
//  }

  //  if (!futureTestResultReady(module.data()))
  //  {
  //    return EXIT_FAILURE;
  //  }

  return EXIT_SUCCESS;
}
