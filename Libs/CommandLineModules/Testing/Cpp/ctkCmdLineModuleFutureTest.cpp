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

#include "ctkTest.h"

#include <QVariant>
#include <QCoreApplication>
#include <QDebug>
#include <QFutureWatcher>


//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
class ctkCmdLineModuleFutureTester : public QObject
{
  Q_OBJECT

private Q_SLOTS:

  void initTestCase();

  void init();
  void cleanup();

  void testStartFinish();
  void testProgress();
  void testPauseAndCancel();
  void testError();

private:

  ctkCmdLineModuleFrontendMockupFactory factory;
  ctkCmdLineModuleBackendLocalProcess backend;

  ctkCmdLineModuleManager manager;

  ctkCmdLineModuleReference moduleRef;
  ctkCmdLineModuleFrontend* frontend;
};

//-----------------------------------------------------------------------------
void ctkCmdLineModuleFutureTester::initTestCase()
{
  manager.registerBackend(&backend);

  QUrl moduleUrl = QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + "/ctkCmdLineModuleTestBed");
  moduleRef = manager.registerModule(moduleUrl);
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleFutureTester::init()
{
  frontend = factory.create(moduleRef);
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleFutureTester::cleanup()
{
  delete frontend;
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleFutureTester::testStartFinish()
{
  QList<QString> expectedSignals;
  expectedSignals.push_back("module.started");
  expectedSignals.push_back("module.finished");

  ctkCmdLineModuleSignalTester signalTester;

  QFutureWatcher<ctkCmdLineModuleResult> watcher;
  QObject::connect(&watcher, SIGNAL(started()), &signalTester, SLOT(moduleStarted()));
  QObject::connect(&watcher, SIGNAL(finished()), &signalTester, SLOT(moduleFinished()));

  ctkCmdLineModuleFuture future = manager.run(frontend);
  watcher.setFuture(future);
  future.waitForFinished();

  QCoreApplication::processEvents();
  QVERIFY(signalTester.checkSignals(expectedSignals));
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleFutureTester::testProgress()
{
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
  ctkCmdLineModuleFuture future = manager.run(frontend);
  watcher.setFuture(future);

  future.waitForFinished();

  // process pending events
  QCoreApplication::processEvents();

  QVERIFY(signalTester.checkSignals(expectedSignals));
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleFutureTester::testPauseAndCancel()
{
  ctkCmdLineModuleSignalTester signalTester;

  QFutureWatcher<ctkCmdLineModuleResult> watcher;
  QObject::connect(&watcher, SIGNAL(started()), &signalTester, SLOT(moduleStarted()));
  QObject::connect(&watcher, SIGNAL(paused()), &signalTester, SLOT(modulePaused()));
  QObject::connect(&watcher, SIGNAL(resumed()), &signalTester, SLOT(moduleResumed()));
  QObject::connect(&watcher, SIGNAL(canceled()), &signalTester, SLOT(moduleCanceled()));
  QObject::connect(&watcher, SIGNAL(finished()), &signalTester, SLOT(moduleFinished()));

  frontend->setValue("runtimeVar", 60);
  ctkCmdLineModuleFuture future = manager.run(frontend);
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

  QTest::qWait(100);

  future.pause();
  QTest::qWait(100);

  QVERIFY(future.isRunning());
  if (future.canPause())
  {
    QVERIFY(future.isPaused());
  }

  future.togglePaused();

  QTest::qWait(100);

  QVERIFY(!future.isPaused());
  QVERIFY(future.isRunning());

  future.cancel();
  future.waitForFinished();

  // process pending events
  QCoreApplication::processEvents();

  QVERIFY(future.isCanceled());
  QVERIFY(future.isFinished());

  QVERIFY(signalTester.checkSignals(expectedSignals));
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleFutureTester::testError()
{
  frontend->setValue("fileVar", "output1");
  frontend->setValue("exitCodeVar", 24);
  frontend->setValue("errorTextVar", "Some error occured\n");

  QFutureWatcher<ctkCmdLineModuleResult> watcher;
  ctkCmdLineModuleFuture future = manager.run(frontend);
  watcher.setFuture(future);

  try
  {
    future.waitForFinished();
    QFAIL("Expected exception not thrown.");
  }
  catch (const ctkCmdLineModuleRunException& e)
  {
    QVERIFY2(e.errorCode() == 24, "Test matching error code");
    QVERIFY2(e.errorString() == "Some error occured\n", "Error text mismatch");
  }
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkCmdLineModuleFutureTest)
#include "moc_ctkCmdLineModuleFutureTest.cpp"
