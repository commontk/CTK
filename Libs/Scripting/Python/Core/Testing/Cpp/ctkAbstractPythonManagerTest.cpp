
// Qt includes
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
#if QT_VERSION < 0x040700
  Q_DECLARE_METATYPE(QVariant)
#endif

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

  //void testPythonAttributes(); // TODO
};

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

  QTest::newRow("1-helloworld") << QString("print 'Hello world'")
                     << false;

  QTest::newRow("2-syntaxerror") << QString("print '") // SyntaxError
                     << true;

  QTest::newRow("3-check __file__ attribute") << QString("print 'This file is: %s' % __file__")
                     << false;
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkAbstractPythonManagerTest)
#include "moc_ctkAbstractPythonManagerTest.cpp"
