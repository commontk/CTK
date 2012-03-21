
// CTK includes
#include "ctkAbstractPythonManager.h"
#include "ctkTest.h"

// PythonQt includes
#include <PythonQt.h>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
class ctkAbstractPythonManagerTester: public QObject
{
  Q_OBJECT
private:
  ctkAbstractPythonManager PythonManager;

private Q_SLOTS:

  void testIsPythonInitialized();

  void testSetInitializationFlags();

  void testPythonErrorOccured();
  void testPythonErrorOccured_data();

  void testMainContext();

  void testAddObjectToPythonMain();

  //void testRegisterPythonQtDecorator(); // TODO
  //void testRegisterClassForPythonQt(); // TODO

  void testExecuteString();
  void testExecuteString_data();

  //void testExecuteFile(); // TODO

  //void testPythonAttributes(); // TODO
};

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
void ctkAbstractPythonManagerTester::testMainContext()
{
  QVERIFY(this->PythonManager.mainContext());
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testPythonErrorOccured()
{
  QFETCH(QString, pythonCode);
  QFETCH(bool, errorOccured);

  this->PythonManager.executeString(pythonCode);

  QCOMPARE(this->PythonManager.pythonErrorOccured(), errorOccured);
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testPythonErrorOccured_data()
{
  QTest::addColumn<QString>("pythonCode");
  QTest::addColumn<bool>("errorOccured");

  QTest::newRow("0") << QString("2 + 2") << false;

  QTest::newRow("1") << QString("raise Exception('This is exception is expected')") << true;
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testAddObjectToPythonMain()
{
  QObject * object = new QObject(this);
  object->setProperty("happy", true);
  this->PythonManager.addObjectToPythonMain("testAddObjectToPythonMain", object);
  QVariant returnValue = this->PythonManager.executeString("testAddObjectToPythonMain.happy",
                                                           ctkAbstractPythonManager::EvalInput);
  QCOMPARE(returnValue, QVariant(true));
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testExecuteString()
{
  QFETCH(QString, stringToExecute);
  QFETCH(int, executeStringMode);
  QFETCH(QVariant, expectedReturnValue);
  QFETCH(QString, expectedVariableName);
  QFETCH(QVariant, expectedVariableValue);

  QVariant returnValue = this->PythonManager.executeString(
        stringToExecute,
        static_cast<ctkAbstractPythonManager::ExecuteStringMode>(executeStringMode));

  QCOMPARE(returnValue, expectedReturnValue);
  QCOMPARE(this->PythonManager.getVariable(expectedVariableName), expectedVariableValue);
}

// ----------------------------------------------------------------------------
void ctkAbstractPythonManagerTester::testExecuteString_data()
{
  QTest::addColumn<QString>("stringToExecute");
  QTest::addColumn<int>("executeStringMode");
  QTest::addColumn<QVariant>("expectedReturnValue");
  QTest::addColumn<QString>("expectedVariableName");
  QTest::addColumn<QVariant>("expectedVariableValue");

  QTest::newRow("0") << QString("a = 6542")
                     << static_cast<int>(ctkAbstractPythonManager::FileInput)
                     << QVariant() << QString("a") << QVariant(6542);

  QTest::newRow("1") << QString("6543")
                     << static_cast<int>(ctkAbstractPythonManager::FileInput)
                     << QVariant() << QString("a") << QVariant(6542);

  QTest::newRow("2") << QString("b = 6544")
                     << static_cast<int>(ctkAbstractPythonManager::EvalInput)
                     << QVariant() << QString("b") << QVariant();

  QTest::newRow("3") << QString("7")
                     << static_cast<int>(ctkAbstractPythonManager::EvalInput)
                     << QVariant(7) << QString("b") << QVariant();
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkAbstractPythonManagerTest)
#include "moc_ctkAbstractPythonManagerTest.cpp"
