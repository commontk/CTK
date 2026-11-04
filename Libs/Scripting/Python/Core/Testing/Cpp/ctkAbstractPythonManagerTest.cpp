
// Qt includes
#include <QFileInfo>
#include <QTemporaryFile>
#include <QTextStream>

// CTK includes
#include "ctkAbstractPythonManager.h"
#include "ctkTest.h"

// PythonQt includes
#include <PythonQt.h>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ctkAbstractPythonManagerTester: public QObject
{
  Q_OBJECT
private:
  ctkAbstractPythonManager PythonManager;

private Q_SLOTS:

  void testDefaults();

  void testIsPythonInitialized();

  void testSetInitializationFlags();

  void testSetSystemExitExceptionHandlerEnabled();

  void testPythonErrorOccured();
  void testPythonErrorOccured_data();

  void testInitialize();

  void testMainContext();

  void testAddObjectToPythonMain();

  //void testRegisterPythonQtDecorator(); // TODO
  //void testRegisterClassForPythonQt(); // TODO

  void testExecuteString();
  void testExecuteString_data();

  void testExecuteFile();
  void testExecuteFile_data();

  void testExecuteFileDoesNotLeakSysPath();
  void testExecuteFileUsesMainGlobals();

  void testPythonAttributes();
  void testPythonAttributes_data();

  void testPythonAttributeValues();
  void testPythonAttributeValues_data();

  void testPythonModule();
  void testPythonModule_data();

  void testPythonObject();
  void testPythonObject_data();

  void testPythonModuleReferenceCount();
  void testPythonAttributesReferenceCount();
};

Q_DECLARE_METATYPE(PyObject*)

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testDefaults()
{
  QCOMPARE(this->PythonManager.pythonErrorOccured(), false);

  this->PythonManager.resetErrorFlag();
  this->PythonManager.registerPythonQtDecorator(0);
  this->PythonManager.registerClassForPythonQt(0);
  this->PythonManager.registerCPPClassForPythonQt(0);
  this->PythonManager.addWrapperFactory(0);
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testIsPythonInitialized()
{
  QCOMPARE(this->PythonManager.isPythonInitialized(), false);
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testSetInitializationFlags()
{
  QCOMPARE(this->PythonManager.initializationFlags(), PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut);

  int flagsToSetPreInit = PythonQt::RedirectStdOut;
  int expectedFlagsPreInit = PythonQt::RedirectStdOut;

  this->PythonManager.setInitializationFlags(flagsToSetPreInit);
  QCOMPARE(this->PythonManager.initializationFlags(), expectedFlagsPreInit);

  this->PythonManager.mainContext();

  int flagsToSetPostInit = 0;
  int expectedFlagsPostInit = PythonQt::RedirectStdOut;

  this->PythonManager.setInitializationFlags(flagsToSetPostInit);
  QCOMPARE(this->PythonManager.initializationFlags(), expectedFlagsPostInit);

  QCOMPARE(this->PythonManager.isPythonInitialized(), true);
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testSetSystemExitExceptionHandlerEnabled()
{
  QCOMPARE(this->PythonManager.systemExitExceptionHandlerEnabled(), false);
  this->PythonManager.setSystemExitExceptionHandlerEnabled(true);
  QCOMPARE(this->PythonManager.systemExitExceptionHandlerEnabled(), true);
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testInitialize()
{
  QVERIFY(this->PythonManager.initialize());

  this->testDefaults();
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testMainContext()
{
  QVERIFY(this->PythonManager.mainContext());

  this->testDefaults();
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testPythonErrorOccured()
{
  QFETCH(QString, pythonCode);
  QFETCH(bool, errorOccured);

  this->PythonManager.executeString(pythonCode);

  QCOMPARE(this->PythonManager.pythonErrorOccured(), errorOccured);

  if(errorOccured)
  {
    this->PythonManager.resetErrorFlag();
  }
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testPythonErrorOccured_data()
{
  QTest::addColumn<QString>("pythonCode");
  QTest::addColumn<bool>("errorOccured");

  QTest::newRow("0") << QString("2 + 2") << false;

  QTest::newRow("1") << QString("raise Exception('This exception is expected')") << true;
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testAddObjectToPythonMain()
{
  QObject * object = new QObject(this);
  object->setProperty("happy", true);
  this->PythonManager.addObjectToPythonMain("testAddObjectToPythonMain", object);
  QVariant returnValue = this->PythonManager.executeString("testAddObjectToPythonMain.happy",
                                                           ctkAbstractPythonManager::EvalInput);
  this->PythonManager.resetErrorFlag();
  QCOMPARE(returnValue, QVariant(true));
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testExecuteString()
{
  QFETCH(QString, stringToExecute);
  QFETCH(int, executeStringMode);
  QFETCH(bool, errorOccured);
  QFETCH(QVariant, expectedReturnValue);
  QFETCH(QString, expectedVariableName);
  QFETCH(QVariant, expectedVariableValue);

  QVariant returnValue = this->PythonManager.executeString(
        stringToExecute,
        static_cast<ctkAbstractPythonManager::ExecuteStringMode>(executeStringMode));

  QCOMPARE(this->PythonManager.pythonErrorOccured(), errorOccured);
  if (errorOccured)
  {
    this->PythonManager.resetErrorFlag();
    return;
  }
  QCOMPARE(returnValue, expectedReturnValue);
  QCOMPARE(this->PythonManager.getVariable(expectedVariableName), expectedVariableValue);
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testExecuteString_data()
{
  QTest::addColumn<QString>("stringToExecute");
  QTest::addColumn<int>("executeStringMode");
  QTest::addColumn<bool>("errorOccured");
  QTest::addColumn<QVariant>("expectedReturnValue");
  QTest::addColumn<QString>("expectedVariableName");
  QTest::addColumn<QVariant>("expectedVariableValue");

  QTest::newRow("0") << QString("a = 6542")
                     << static_cast<int>(ctkAbstractPythonManager::FileInput)
                     << false
                     << QVariant() << QString("a") << QVariant(6542);

  QTest::newRow("1") << QString("6543")
                     << static_cast<int>(ctkAbstractPythonManager::FileInput)
                     << false
                     << QVariant() << QString("a") << QVariant(6542);

  QTest::newRow("2") << QString("b = 6544")
                     << static_cast<int>(ctkAbstractPythonManager::EvalInput)
                     << true
                     << QVariant() << QString("b") << QVariant();

  QTest::newRow("3") << QString("7")
                     << static_cast<int>(ctkAbstractPythonManager::EvalInput)
                     << false
                     << QVariant(7) << QString("b") << QVariant();

  QTest::newRow("4") << QString("sys.getrecursionlimit()")
                     << static_cast<int>(ctkAbstractPythonManager::FileInput)
                     << false
                     << QVariant() << QString() << QVariant();

  // This assume the default 'recursionlimit' has not been changed
  QTest::newRow("5") << QString("sys.getrecursionlimit()")
                     << static_cast<int>(ctkAbstractPythonManager::EvalInput)
                     << false
                     << QVariant(1000) << QString() << QVariant();
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testExecuteFile()
{
  QFETCH(QString, stringToExecute);
  QFETCH(bool, pythonErrorExpected);

  QTemporaryFile pythonFile("testExecuteFile-XXXXXX.py");
  QVERIFY(pythonFile.open());
  QTextStream out(&pythonFile);
  out << stringToExecute;
  pythonFile.close();

  this->PythonManager.executeFile(pythonFile.fileName());

  QCOMPARE(this->PythonManager.pythonErrorOccured(), pythonErrorExpected);
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testExecuteFile_data()
{
  QTest::addColumn<QString>("stringToExecute");
  QTest::addColumn<bool>("pythonErrorExpected");

  QTest::newRow("0-emptyfile") << QString("")
                     << false;

  QTest::newRow("1-helloworld") << QString("print('Hello world')")
                     << false;

  QTest::newRow("2-syntaxerror") << QString("print '") // SyntaxError
                     << true;

  QTest::newRow("3-check __file__ attribute") << QString("print('This file is: %s' % __file__)")
                     << false;
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testExecuteFileDoesNotLeakSysPath()
{
  QTemporaryFile pythonFile("testExecuteFileLeak-XXXXXX.py");
  QVERIFY(pythonFile.open());
  QTextStream out(&pythonFile);
  out << "pass\n";
  pythonFile.close();

  const QString scriptDir = QFileInfo(pythonFile.fileName()).absolutePath();
  const QString inSysPathExpr =
      QString("%1 in sys.path").arg(ctkAbstractPythonManager::toPythonStringLiteral(scriptDir));

  this->PythonManager.executeString("import sys");

  QVariant beforeContains = this->PythonManager.executeString(
        inSysPathExpr, ctkAbstractPythonManager::EvalInput);
  QCOMPARE(beforeContains, QVariant(false));

  this->PythonManager.executeFile(pythonFile.fileName());
  QCOMPARE(this->PythonManager.pythonErrorOccured(), false);

  QVariant afterContains = this->PythonManager.executeString(
        inSysPathExpr, ctkAbstractPythonManager::EvalInput);
  QCOMPARE(afterContains, QVariant(false));

  QVariant helperExists = this->PythonManager.executeString(
        "'_ctk_executefile_dir' in globals()",
        ctkAbstractPythonManager::EvalInput);
  QCOMPARE(helperExists, QVariant(false));

  QVariant updatedGlobalsExists = this->PythonManager.executeString(
        "'_updated_globals' in globals()",
        ctkAbstractPythonManager::EvalInput);
  QCOMPARE(updatedGlobalsExists, QVariant(false));
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testExecuteFileUsesMainGlobals()
{
  QTemporaryFile pythonFile("testExecuteFileGlobals-XXXXXX.py");
  QVERIFY(pythonFile.open());
  QTextStream out(&pythonFile);
  out << "execfile_counter = 0\n"
         "def execfile_bump():\n"
         "    global execfile_counter\n"
         "    execfile_counter += 1\n"
         "del execfile_removed\n";
  pythonFile.close();

  this->PythonManager.executeString("execfile_removed = 1");
  this->PythonManager.executeFile(pythonFile.fileName());
  QCOMPARE(this->PythonManager.pythonErrorOccured(), false);

  this->PythonManager.executeString("execfile_bump()");
  QCOMPARE(this->PythonManager.getVariable("execfile_counter"), QVariant(1));
  QCOMPARE(this->PythonManager.executeString("'execfile_removed' in globals()",
                                             ctkAbstractPythonManager::EvalInput), QVariant(false));
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testPythonAttributes()
{
  QFETCH(QString, pythonVariableName);
  QFETCH(QStringList, expectedAttributeList);

  QString test_path = QFileInfo(QString::fromUtf8(__FILE__)).absolutePath()
      + "/PythonAttributes-test.py";
  this->PythonManager.executeFile(test_path);

  QStringList AttributeList = this->PythonManager.pythonAttributes(pythonVariableName, QString("__main__").toLatin1(), false);

  foreach (const QString& expectedAttribute, expectedAttributeList)
  {
    QVERIFY(AttributeList.contains(expectedAttribute));
  }
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testPythonAttributes_data()
{
  QTest::addColumn<QString>("pythonVariableName");
  QTest::addColumn<QStringList>("expectedAttributeList");

  QTest::newRow("d.foo_class()")
                     << "d.foo_class()"
                     << (QStringList()
                         << "FOO_CLASS_MEMBER"
                         << "foo_class_method"
                         << "foo_instance_member"
                         << "foo_instance_method"
                         << "instantiate_bar");

  QTest::newRow("d.foo_class().instantiate_bar()")
                     << "d.foo_class().instantiate_bar()"
                     << (QStringList()
                         << "BAR_CLASS_MEMBER"
                         << "bar_class_method"
                         << "bar_instance_member"
                         << "bar_instance_method");

  QTest::newRow("d.foo_class().instantiate_bar().bar_maths(5)")
                     << "d.foo_class().instantiate_bar().bar_maths(5)"
                     << (QStringList()
                         << "MATHS_CLASS_MEMBER"
                         << "maths_instance_member"
                         << "maths_instance_method");

  QTest::newRow("MultipleArg( 5 + 5 , '(')")
                     << "MultipleArg( 5 + 5 , '(')"
                     << (QStringList()
                         << "multipleArg_instance_member_num"
                         << "multipleArg_instance_member_str"
                         << "multipleArg_instance_member_other");

  QTest::newRow("MultipleArg( 5 % 5 + 1, '\"', 0.1)")
                     << "MultipleArg( 5 % 5 + 1, '\"', 0.1)"
                     << (QStringList()
                         << "multipleArg_instance_member_num"
                         << "multipleArg_instance_member_str"
                         << "multipleArg_instance_member_other");

}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testPythonAttributeValues()
{
  QFETCH(QString, code);
  QFETCH(QVariant, expectedValue);

  QString test_path = QFileInfo(QString::fromUtf8(__FILE__)).absolutePath()
      + "/PythonAttributes-test.py";

  this->PythonManager.executeFile(test_path);
  this->PythonManager.executeString(QString("value=%1").arg(code));

  QCOMPARE(
        this->PythonManager.getVariable("value"),
        expectedValue);
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testPythonAttributeValues_data()
{
  QTest::addColumn<QString>("code");
  QTest::addColumn<QVariant>("expectedValue");

  QString code;

  code = "d.foo_class().instantiate_bar().bar_maths(5).MATHS_CLASS_MEMBER";
  QTest::newRow(code.toLatin1())
      << code
      << QVariant(0.1);

  code = "d.foo_class().instantiate_bar().bar_maths(5).maths_instance_member";
  QTest::newRow(code.toLatin1())
      << code
      << QVariant(5);

  code = "MultipleArg( 5 + 5 , '(').multipleArg_instance_member_num";
  QTest::newRow(code.toLatin1())
      << code
      << QVariant(10);

  code = "MultipleArg( 5 + 5 , '(').multipleArg_instance_member_str";
  QTest::newRow(code.toLatin1())
      << code
      << QVariant("(");

  code = "MultipleArg( 5 + 5 , '(').multipleArg_instance_member_other";
  QTest::newRow(code.toLatin1())
      << code
      << QVariant(0);

  code = "MultipleArg( 5 % 5 + 1, '\"', 0.1).multipleArg_instance_member_num";
  QTest::newRow(code.toLatin1())
      << code
      << QVariant(1.1);

  code = "MultipleArg( 5 % 5 + 1, '\"', 0.1).multipleArg_instance_member_str";
  QTest::newRow(code.toLatin1())
      << code
      << QVariant("\"");

  code = "MultipleArg( 5 % 5 + 1, '\"', 0.1).multipleArg_instance_member_other";
  QTest::newRow(code.toLatin1())
      << code
      << QVariant(0.1);
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testPythonModule()
{
  QFETCH(QString, pythonCode);
  QFETCH(QString, inputModuleList);
  QFETCH(QString, expectedReturnedString);

  this->PythonManager.executeString(pythonCode);
  PyObject* returnedPyObject = this->PythonManager.pythonModule(inputModuleList);
  PyObject* returnedPyString;
  if(returnedPyObject)
  {
    returnedPyString = PyObject_GetAttrString(returnedPyObject, "__name__");
  }
  else
  {
    returnedPyString = PyUnicode_FromString("");
  }
  QString returnedString = PyUnicode_AsUTF8(returnedPyString);
  QCOMPARE(returnedString, expectedReturnedString);
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testPythonModule_data()
{
  QTest::addColumn<QString>("pythonCode");
  QTest::addColumn<QString>("inputModuleList");
  QTest::addColumn<QString>("expectedReturnedString");

  QTest::newRow("0") << ""
                     << "__main__"
                     << "__main__";

  QTest::newRow("1") << ""
                     << "__main__.__builtins__"
                     << "builtins";

  QTest::newRow("2") << "class foo: pass"
                     << "__main__.foo"
                     << "foo";

  QTest::newRow("3") << ""
                     << "__main__.NOT_A_MODULE"
                     << "";
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testPythonModuleReferenceCount()
{
  this->PythonManager.executeString(
    "class RootModule:\n"
    "    class SubModule:\n"
    "        class LeafModule:\n"
    "            pass\n"
  );

  PyObject* rootObj = this->PythonManager.pythonModule("__main__.RootModule");
  QVERIFY(rootObj != nullptr);
  Py_ssize_t rootRefBefore = Py_REFCNT(rootObj);
  Py_DECREF(rootObj);

  PyObject* subObj = this->PythonManager.pythonModule("__main__.RootModule.SubModule");
  QVERIFY(subObj != nullptr);
  Py_ssize_t subRefBefore = Py_REFCNT(subObj);
  Py_DECREF(subObj);

  // Look up dotted module multiple times and ensure intermediate and leaf references do not leak
  for (int i = 0; i < 10; ++i)
  {
    PyObject* leafObj = this->PythonManager.pythonModule("__main__.RootModule.SubModule.LeafModule");
    QVERIFY(leafObj != nullptr);
    Py_DECREF(leafObj);
  }

  rootObj = this->PythonManager.pythonModule("__main__.RootModule");
  QVERIFY(rootObj != nullptr);
  Py_ssize_t rootRefAfter = Py_REFCNT(rootObj);
  Py_DECREF(rootObj);

  subObj = this->PythonManager.pythonModule("__main__.RootModule.SubModule");
  QVERIFY(subObj != nullptr);
  Py_ssize_t subRefAfter = Py_REFCNT(subObj);
  Py_DECREF(subObj);

  QCOMPARE(rootRefAfter, rootRefBefore);
  QCOMPARE(subRefAfter, subRefBefore);
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testPythonAttributesReferenceCount()
{
  this->PythonManager.executeString(
    "class AttrTestModule:\n"
    "    member_var = 42\n"
    "    def member_func(self):\n"
    "        pass\n"
    "attr_test = AttrTestModule()\n"
  );

  PyObject* modObj = this->PythonManager.pythonObject("__main__.attr_test.member_var");
  QVERIFY(modObj != nullptr);
  Py_ssize_t modRefBefore = Py_REFCNT(modObj);
  Py_DECREF(modObj);

  for (int i = 0; i < 10; ++i)
  {
    QStringList attrs = this->PythonManager.pythonAttributes("attr_test", "__main__", false);
    QVERIFY(!attrs.isEmpty());
    QVERIFY(attrs.contains("member_var"));
    QVERIFY(attrs.contains("member_func"));
  }

  modObj = this->PythonManager.pythonObject("__main__.attr_test.member_var");
  QVERIFY(modObj != nullptr);
  Py_ssize_t modRefAfter = Py_REFCNT(modObj);
  Py_DECREF(modObj);

  QCOMPARE(modRefAfter, modRefBefore);
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testPythonObject()
{
  QFETCH(QString, pythonCode);
  QFETCH(QString, inputPythonVariableNameAndFunction);
  QFETCH(QString, expectedReturnedString);

  this->PythonManager.executeString(pythonCode);
  PyObject* returnedPyObject = this->PythonManager.pythonObject(inputPythonVariableNameAndFunction);
  PyObject* returnedPyObjectString;
  if (returnedPyObject)
  {
    returnedPyObjectString = PyObject_GetAttrString(returnedPyObject, "__name__");
  }
  else
  {
    returnedPyObjectString = PyUnicode_FromString("");
  }
  QString returnedString = PyUnicode_AsUTF8(returnedPyObjectString);
  QCOMPARE(returnedString, expectedReturnedString);
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testPythonObject_data()
{
  QTest::addColumn<QString>("pythonCode");
  QTest::addColumn<QString>("inputPythonVariableNameAndFunction");
  QTest::addColumn<QString>("expectedReturnedString");

  QTest::newRow("0") << "foo = []"
                     << "__main__.foo.append"
                     << "append";

  QTest::newRow("1") << ""
                     << "__main__.__builtins__.dir"
                     << "dir";

  QTest::newRow("2") << "class foo: bar = []"
                     << "__main__.foo.bar.reverse"
                     << "reverse";

  QTest::newRow("3") << ""
                     << "__main__.__builtins__.NOT_A_FUNCTION"
                     << "";
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkAbstractPythonManagerTest)
#include "ctkAbstractPythonManagerTest.moc"
