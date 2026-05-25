/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QAtomicInt>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QString>
#include <QThread>
#include <QVariant>

// CTK includes
#include "ctkAbstractPythonManager.h"
#include "ctkTest.h"

// PythonQt includes
#include <PythonQtThreadSupport.h>

//-----------------------------------------------------------------------------
class PythonManagerWorker : public QThread {
public:
  PythonManagerWorker(ctkAbstractPythonManager& manager, QAtomicInt& finished, QAtomicInt& result)
    : Manager(manager), Finished(finished), Result(result) {}

protected:
  void run() override {
    QVariant value = this->Manager.executeString("40 + 2", ctkAbstractPythonManager::EvalInput);
    this->Result.storeRelease(value.toInt() == 42);
    this->Finished.storeRelease(1);
  }

private:
  ctkAbstractPythonManager& Manager;
  QAtomicInt& Finished;
  QAtomicInt& Result;
};

//-----------------------------------------------------------------------------
class ctkAbstractPythonManagerThreadTester : public QObject {
  Q_OBJECT

private Q_SLOTS:
  void initTestCase();
  void testCppThreadWhileMainIdle();
  void testPythonQThread();

private:
  QVariant getVariable(const QString &variable) {
    return this->PythonManager.getVariable(variable);
  }

  void verifyTrue(const QString &variable) {
    QVariant variant = getVariable(variable);
    QVERIFY(variant.isValid() && variant.toBool());
  }

  void verifyFalse(const QString &variable) {
    QVariant variant = getVariable(variable);
    QVERIFY(variant.isValid() && !variant.toBool());
  }

  ctkAbstractPythonManager PythonManager;
};

//-----------------------------------------------------------------------------
void ctkAbstractPythonManagerThreadTester::initTestCase() {
  QVERIFY(this->PythonManager.initialize(true));
  this->PythonManager.setThreadSupportEnabled(false);
  QVERIFY(this->PythonManager.threadSupportEnabled());
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManagerThreadTester::testCppThreadWhileMainIdle() {
  QAtomicInt finished(0);
  QAtomicInt result(0);
  PythonManagerWorker worker(this->PythonManager, finished, result);
  worker.start();

  QElapsedTimer timeout;
  timeout.start();
  while (!finished.loadAcquire() && timeout.elapsed() < 5000) {
    QCoreApplication::processEvents();
    QThread::msleep(1);
  }
  if (!finished.loadAcquire()) {
    this->PythonManager.executeString("import time; time.sleep(0.1)");
  }

  QVERIFY(worker.wait(5000));
  QVERIFY(finished.loadAcquire());
  QVERIFY(result.loadAcquire());
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManagerThreadTester::testPythonQThread() {
  const QString test_path =
      QFileInfo(QString::fromUtf8(__FILE__)).absolutePath() + "/PythonThreading-test.py";
  QVERIFY2(QFileInfo::exists(test_path), qPrintable(test_path));

  this->PythonManager.executeFile(test_path);
  QVERIFY2(
      !this->PythonManager.pythonErrorOccured(),
      qPrintable(QString("Python error while executing %1").arg(test_path)));

  verifyTrue("test_wait_ok");
  verifyFalse("test_is_main_thread");
  verifyFalse("test_is_application_thread");
  verifyTrue("test_python_ran");
}

//-----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkAbstractPythonManagerThreadTest)
#include "ctkAbstractPythonManagerThreadTest.moc"
