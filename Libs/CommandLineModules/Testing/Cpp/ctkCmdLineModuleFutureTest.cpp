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
#include <ctkCmdLineModuleFrontendFactory.h>
#include <ctkCmdLineModuleFrontend.h>
#include <ctkCmdLineModuleReference.h>
#include <ctkCmdLineModuleDescription.h>
#include <ctkCmdLineModuleParameter.h>
#include <ctkCmdLineModuleRunException.h>
#include <ctkCmdLineModuleFuture.h>

#include "ctkCmdLineModuleSignalTester.h"

#include "ctkCmdLineModuleBackendLocalProcess.h"

#include <QVariant>
#include <QCoreApplication>
#include <QDebug>
#include <QFutureWatcher>

#include <cstdlib>


#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <time.h>
#endif

void sleep_ms(int ms)
{
#ifdef Q_OS_WIN
  Sleep(ms);
#else
  struct timespec nanostep;
  nanostep.tv_sec = ms / 1000;
  nanostep.tv_nsec = ((ms % 1000) * 1000.0 * 1000.0);
  nanosleep(&nanostep, NULL);
#endif
}

class ctkCmdLineModuleFrontendMockupFactory : public ctkCmdLineModuleFrontendFactory
{
public:

  virtual ctkCmdLineModuleFrontend* create(const ctkCmdLineModuleReference& moduleRef)
  {
    struct ModuleFrontendMockup : public ctkCmdLineModuleFrontend
    {
      ModuleFrontendMockup(const ctkCmdLineModuleReference& moduleRef)
        : ctkCmdLineModuleFrontend(moduleRef) {}

      virtual QObject* guiHandle() const { return NULL; }

      virtual QVariant value(const QString& parameter, int role) const
      {
        Q_UNUSED(role)
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

    return new ModuleFrontendMockup(moduleRef);
  }

  virtual QString name() const { return "Mock-up"; }
  virtual QString description() const { return "A mock-up factory for testing."; }
};

bool futureTestStartFinish(ctkCmdLineModuleManager* manager, ctkCmdLineModuleFrontend* frontend)
{
  qDebug() << "Testing ctkCmdLineModuleFuture start/finish signals.";

  QList<QString> expectedSignals;
  expectedSignals.push_back("module.started");
  expectedSignals.push_back("module.finished");

  ctkCmdLineModuleSignalTester signalTester;

  QFutureWatcher<ctkCmdLineModuleResult> watcher;
  QObject::connect(&watcher, SIGNAL(started()), &signalTester, SLOT(moduleStarted()));
  QObject::connect(&watcher, SIGNAL(finished()), &signalTester, SLOT(moduleFinished()));

  ctkCmdLineModuleFuture future = manager->run(frontend);
  watcher.setFuture(future);

  try
  {
    future.waitForFinished();
  }
  catch (const ctkCmdLineModuleRunException& e)
  {
    qDebug() << e;
    return false;
  }

  // process pending events
  QCoreApplication::processEvents();

  return signalTester.checkSignals(expectedSignals);
}

bool futureTestProgress(ctkCmdLineModuleManager* manager, ctkCmdLineModuleFrontend* frontend)
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

  frontend->setValue("numOutputsVar", 1);
  ctkCmdLineModuleFuture future = manager->run(frontend);
  watcher.setFuture(future);

  try
  {
    future.waitForFinished();
  }
  catch (const ctkCmdLineModuleRunException& e)
  {
    qDebug() << e;
    return false;
  }

  // process pending events
  QCoreApplication::processEvents();

  return signalTester.checkSignals(expectedSignals);
}

bool futureTestPauseAndCancel(ctkCmdLineModuleManager* manager, ctkCmdLineModuleFrontend* frontend)
{
  qDebug() << "Testing ctkCmdLineModuleFuture pause and cancel capabilities";


  ctkCmdLineModuleSignalTester signalTester;

  QFutureWatcher<ctkCmdLineModuleResult> watcher;
  QObject::connect(&watcher, SIGNAL(started()), &signalTester, SLOT(moduleStarted()));
  QObject::connect(&watcher, SIGNAL(paused()), &signalTester, SLOT(modulePaused()));
  QObject::connect(&watcher, SIGNAL(resumed()), &signalTester, SLOT(moduleResumed()));
  QObject::connect(&watcher, SIGNAL(canceled()), &signalTester, SLOT(moduleCanceled()));
  QObject::connect(&watcher, SIGNAL(finished()), &signalTester, SLOT(moduleFinished()));

  frontend->setValue("runtimeVar", 60);
  ctkCmdLineModuleFuture future = manager->run(frontend);
  watcher.setFuture(future);

  QList<QString> expectedSignals;
  expectedSignals.push_back("module.started");
  if (future.canPause())
  {
    // Due to Qt bug 12152, these two signals are reversed
    expectedSignals.push_back("module.resumed");
    expectedSignals.push_back("module.paused");
  }

  if (future.canCancel())
  {
    expectedSignals.push_back("module.canceled");
  }
  expectedSignals.push_back("module.finished");

  sleep_ms(500);

  QCoreApplication::processEvents();
  future.pause();
  sleep_ms(500);
  QCoreApplication::processEvents();

  if (future.canPause())
  {
    if (!(future.isPaused() && future.isRunning()))
    {
      qDebug() << "Pause state wrong";
      future.setPaused(false);
      future.cancel();
      QCoreApplication::processEvents();
      future.waitForFinished();
      return false;
    }
  }

  future.togglePaused();
  QCoreApplication::processEvents();

  sleep_ms(500);

  if (future.isPaused() && future.isRunning())
  {
    qDebug() << "Pause state wrong (module is paused, but it shouldn't be)";
    future.cancel();
    QCoreApplication::processEvents();
    future.waitForFinished();
    return false;
  }

  try
  {
    future.cancel();
    QCoreApplication::processEvents();
    future.waitForFinished();
  }
  catch (const ctkCmdLineModuleRunException& e)
  {
    qDebug() << e;
    return false;
  }

  // process pending events
  QCoreApplication::processEvents();

  if (!signalTester.checkSignals(expectedSignals))
  {
    return false;
  }

  if (!(future.isCanceled() && future.isFinished()))
  {
    qDebug() << "Cancel state wrong";
    return false;
  }
  return true;
}

bool futureTestError(ctkCmdLineModuleManager* manager, ctkCmdLineModuleFrontend* frontend)
{
  qDebug() << "Testing ctkCmdLineModuleFuture error reporting.";

  frontend->setValue("fileVar", "output1");
  frontend->setValue("exitCodeVar", 24);
  frontend->setValue("errorTextVar", "Some error occured\n");

  QFutureWatcher<ctkCmdLineModuleResult> watcher;
  ctkCmdLineModuleFuture future = manager->run(frontend);
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

  ctkCmdLineModuleFrontendMockupFactory factory;
  ctkCmdLineModuleBackendLocalProcess backend;

  ctkCmdLineModuleManager manager;
  manager.registerBackend(&backend);

  QUrl moduleUrl = QUrl::fromLocalFile(app.applicationDirPath() + "/ctkCmdLineModuleTestBed");
  ctkCmdLineModuleReference moduleRef;
  try
  {
    moduleRef = manager.registerModule(moduleUrl);
  }
  catch (const ctkException& e)
  {
    qCritical() << "Module at" << moduleUrl << "could not be registered: " << e;
  }

  {
    QScopedPointer<ctkCmdLineModuleFrontend> frontend(factory.create(moduleRef));
    if (!futureTestStartFinish(&manager, frontend.data()))
    {
      return EXIT_FAILURE;
    }
  }

  {
    QScopedPointer<ctkCmdLineModuleFrontend> frontend(factory.create(moduleRef));
    if (!futureTestProgress(&manager, frontend.data()))
    {
      return EXIT_FAILURE;
    }
  }

  {
    QScopedPointer<ctkCmdLineModuleFrontend> frontend(factory.create(moduleRef));
    if (!futureTestError(&manager, frontend.data()))
    {
      return EXIT_FAILURE;
    }
  }

  {
    QScopedPointer<ctkCmdLineModuleFrontend> frontend(factory.create(moduleRef));
    if (!futureTestPauseAndCancel(&manager, frontend.data()))
    {
      return EXIT_FAILURE;
    }
  }

  //  if (!futureTestResultReady(frontend.data()))
  //  {
  //    return EXIT_FAILURE;
  //  }

  return EXIT_SUCCESS;
}
