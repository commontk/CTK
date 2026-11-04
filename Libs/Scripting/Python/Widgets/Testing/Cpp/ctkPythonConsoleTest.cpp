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
#include <QApplication>
#include <QPushButton>
#include <QTimer>

// CTK includes
#include "ctkAbstractPythonManager.h"
#include "ctkCompleter.h"
#include "ctkPythonConsole.h"
#include "ctkTest.h"

// PythonQt includes
#include <PythonQt.h>

// STD includes
#include <cstdlib>
#include <iostream>

// ----------------------------------------------------------------------------
class ctkPythonConsoleTester : public QObject
{
  Q_OBJECT

private Q_SLOTS:
  void testShow();
  void testCompleterShortcuts();
  void testCompleterUserFunctionReferenceCount();
  void testCompleterClassMethodReferenceCount();
};

// ----------------------------------------------------------------------------
void ctkPythonConsoleTester::testShow()
{
  ctkPythonConsole pythonConsole;
  ctkAbstractPythonManager pythonManager;
  pythonConsole.initialize(&pythonManager);

  pythonConsole.show();
  bool result = QTest::qWaitForWindowActive(&pythonConsole);
  Q_UNUSED(result);
}

// ----------------------------------------------------------------------------
void ctkPythonConsoleTester::testCompleterShortcuts()
{
  ctkPythonConsole pythonConsole;
  ctkAbstractPythonManager pythonManager;
  pythonConsole.initialize(&pythonManager);

  QList<QKeySequence> otherShortcuts;
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
  otherShortcuts << QKeySequence(Qt::CTRL | Qt::Key_Space);
#else
  otherShortcuts << QKeySequence(Qt::CTRL + Qt::Key_Space);
#endif
  otherShortcuts << Qt::Key_F1;
  pythonConsole.setCompleterShortcuts(otherShortcuts);
  pythonConsole.addCompleterShortcut(Qt::Key_Tab);

  QCOMPARE(pythonConsole.completerShortcuts().size(), 3);
}

// ----------------------------------------------------------------------------
void ctkPythonConsoleTester::testCompleterUserFunctionReferenceCount()
{
  ctkAbstractPythonManager pythonManager;
  ctkPythonConsoleCompleter completer(pythonManager);

  pythonManager.executeString(
    "def user_func(a, b, c):\n"
    "    return a + b + c\n"
  );

  PyObject* funcObj = pythonManager.pythonModule("__main__.user_func");
  QVERIFY(funcObj != nullptr);
  Py_ssize_t funcRefBefore = Py_REFCNT(funcObj);
  Py_DECREF(funcObj);

  for (int i = 0; i < 10; ++i)
  {
    int offset = completer.cursorOffset("user_func()");
    QCOMPARE(offset, 1);
  }

  funcObj = pythonManager.pythonModule("__main__.user_func");
  QVERIFY(funcObj != nullptr);
  Py_ssize_t funcRefAfter = Py_REFCNT(funcObj);
  Py_DECREF(funcObj);

  QCOMPARE(funcRefAfter, funcRefBefore);
}

// ----------------------------------------------------------------------------
void ctkPythonConsoleTester::testCompleterClassMethodReferenceCount()
{
  ctkAbstractPythonManager pythonManager;
  ctkPythonConsoleCompleter completer(pythonManager);

  pythonManager.executeString(
    "class MyClass:\n"
    "    def class_method(self, x, y):\n"
    "        return x + y\n"
    "\n"
    "my_instance = MyClass()\n"
  );

  PyObject* methodObj = pythonManager.pythonObject("__main__.my_instance.class_method");
  QVERIFY(methodObj != nullptr);
  Py_ssize_t methodRefBefore = Py_REFCNT(methodObj);
  Py_DECREF(methodObj);

  for (int i = 0; i < 10; ++i)
  {
    int offset = completer.cursorOffset("my_instance.class_method()");
    QCOMPARE(offset, 1);
  }

  methodObj = pythonManager.pythonObject("__main__.my_instance.class_method");
  QVERIFY(methodObj != nullptr);
  Py_ssize_t methodRefAfter = Py_REFCNT(methodObj);
  Py_DECREF(methodObj);

  QCOMPARE(methodRefAfter, methodRefBefore);
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkPythonConsoleTest)
#include "ctkPythonConsoleTest.moc"
