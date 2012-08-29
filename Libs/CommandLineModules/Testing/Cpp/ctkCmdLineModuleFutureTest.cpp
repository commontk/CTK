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
#include <ctkCmdLineModuleFutureWatcher.h>

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

public Q_SLOTS:

  void ouputDataReady();
  void errorDataReady();

private Q_SLOTS:

  void initTestCase();

  void init();
  void cleanup();

  void testStartFinish();
  void testProgress();
  void testPauseAndCancel();
  void testOutput();
  void testError();

private:

  QByteArray outputData;
  QByteArray errorData;

  ctkCmdLineModuleFutureWatcher* currentWatcher;

  ctkCmdLineModuleFrontendMockupFactory factory;
  ctkCmdLineModuleBackendLocalProcess backend;

  ctkCmdLineModuleManager manager;

  ctkCmdLineModuleReference moduleRef;
  ctkCmdLineModuleFrontend* frontend;
};

//-----------------------------------------------------------------------------
void ctkCmdLineModuleFutureTester::ouputDataReady()
{
  if (this->currentWatcher)
  {
    outputData.append(currentWatcher->readPendingOutputData());
  }
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleFutureTester::errorDataReady()
{
  if (this->currentWatcher)
  {
    errorData.append(currentWatcher->readPendingErrorData());
  }
}

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
  currentWatcher = 0;
  frontend = factory.create(moduleRef);
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleFutureTester::cleanup()
{
  delete frontend;
  outputData.clear();
  errorData.clear();
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleFutureTester::testStartFinish()
{
  QList<QString> expectedSignals;
  expectedSignals << "module.started"
                  << "module.progressRangeChanged(0,0)"
                  << "module.progressValueChanged(0)"
                  << "module.progressRangeChanged(0,1000)"
                  << "module.errorReady"
                  << "module.progressValueChanged(1000)"
                  << "module.finished";

  ctkCmdLineModuleSignalTester signalTester;

  ctkCmdLineModuleFuture future = manager.run(frontend);
  signalTester.setFuture(future);
  future.waitForFinished();

  QCoreApplication::processEvents();
  QVERIFY(signalTester.checkSignals(expectedSignals));
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleFutureTester::testProgress()
{
  QList<QString> expectedSignals;
  expectedSignals << "module.started"
                     // the following signals are send when connecting a QFutureWatcher to
                     // an already started QFuture
                  << "module.progressRangeChanged(0,0)"
                  << "module.progressValueChanged(0)"
                  << "module.progressRangeChanged(0,1000)"

                     // the test module always reports error data when starting
                  << "module.errorReady"

                     // the following two signals are send when the module reports "filter start"
                  << "module.progressValueChanged(1)"
                  << "module.progressTextChanged(Test Filter)"

                     // this signal is send when the module reports progress for "output1"
                  << "module.progressValueChanged(999)"

                     // the output data (the order is not really deterministic here...)
                  << "module.outputReady"

                     // the following signal is sent at the end to report completion
                  << "module.progressValueChanged(1000)"
                  << "module.finished";

  ctkCmdLineModuleSignalTester signalTester;

  frontend->setValue("numOutputsVar", 1);
  ctkCmdLineModuleFuture future = manager.run(frontend);
  signalTester.setFuture(future);

  future.waitForFinished();

  // process pending events
  QCoreApplication::processEvents();

  QVERIFY(signalTester.checkSignals(expectedSignals));
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleFutureTester::testPauseAndCancel()
{
  ctkCmdLineModuleSignalTester signalTester;

  frontend->setValue("runtimeVar", 60);
  ctkCmdLineModuleFuture future = manager.run(frontend);
  signalTester.setFuture(future);

  QList<QString> expectedSignals;
  expectedSignals << "module.started"
                  << "module.progressRangeChanged(0,0)"
                  << "module.progressValueChanged(0)"
                  << "module.progressRangeChanged(0,1000)"
                  << "module.errorReady";

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
void ctkCmdLineModuleFutureTester::testOutput()
{
  ctkCmdLineModuleSignalTester signalTester;

  connect(signalTester.watcher(), SIGNAL(outputDataReady()), SLOT(ouputDataReady()));
  connect(signalTester.watcher(), SIGNAL(errorDataReady()), SLOT(errorDataReady()));

  this->currentWatcher = signalTester.watcher();

  frontend->setValue("numOutputsVar", 2);
  frontend->setValue("errorTextVar", "Final error msg.");
  ctkCmdLineModuleFuture future = manager.run(frontend);
  signalTester.setFuture(future);

  future.waitForFinished();

  // process pending events
  QCoreApplication::processEvents();

  QVERIFY(future.isFinished());

  QList<QString> expectedSignals;
  expectedSignals << "module.started"
                  << "module.progressRangeChanged(0,0)"
                  << "module.progressValueChanged(0)"
                  << "module.progressRangeChanged(0,1000)"
                  << "module.errorReady"
                  << "module.progressValueChanged(1)"
                  << "module.progressTextChanged(Test Filter)"
                  << "module.progressValueChanged(500)"
                  << "module.outputReady"
                  << "module.progressValueChanged(999)"
                  << "module.outputReady"
                  << "module.errorReady"
                  << "module.progressValueChanged(1000)"
                  << "module.finished";

  QVERIFY(signalTester.checkSignals(expectedSignals));

  const char* expectedOutput = "Output 1\nOutput 2\n";
  const char* expectedError = "A superficial error message.\nFinal error msg.";

  QCOMPARE(this->outputData.data(), expectedOutput);
  QCOMPARE(this->errorData.data(), expectedError);

  QCOMPARE(future.readAllOutputData().data(), expectedOutput);
  QCOMPARE(future.readAllErrorData().data(), expectedError);
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleFutureTester::testError()
{
  frontend->setValue("fileVar", "output1");
  frontend->setValue("exitCodeVar", 24);
  frontend->setValue("errorTextVar", "Some error occured\n");

  ctkCmdLineModuleFuture future = manager.run(frontend);

  try
  {
    future.waitForFinished();
    QFAIL("Expected exception not thrown.");
  }
  catch (const ctkCmdLineModuleRunException& e)
  {
    QVERIFY2(e.errorCode() == 24, "Test matching error code");
    QCOMPARE(future.readAllErrorData().data(), "A superficial error message.\nSome error occured\n");
  }
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkCmdLineModuleFutureTest)
#include "moc_ctkCmdLineModuleFutureTest.cpp"
