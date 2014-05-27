/*=========================================================================

  Library:   CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

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
#include <QSpinBox>
#include <QComboBox>
#include <QVariant>

#if (QT_VERSION < QT_VERSION_CHECK(4,7,0))
Q_DECLARE_METATYPE(QVariant)
#endif

// CTK includes
#include "ctkCmdLineModuleManager.h"
#include "ctkCmdLineModuleBackend.h"
#include "ctkCmdLineModuleFrontendQtGui.h"
#include "ctkCmdLineModuleFuture.h"
#include "ctkCmdLineModuleDescription.h"
#include "ctkCmdLineModuleParameter.h"

#include "ctkTest.h"

#if (QT_VERSION < QT_VERSION_CHECK(4,7,0))
extern int qHash(const QUrl& url);
#endif

namespace {

class BackendMockUp : public ctkCmdLineModuleBackend
{

public:

  void addModule(const QUrl& location, const QByteArray& xml)
  {
    this->UrlToXml[location] = xml;
  }

  virtual QString name() const { return "Mockup"; }
  virtual QString description() const { return "Test Mock-up"; }
  virtual QList<QString> schemes() const { return QList<QString>() << "test"; }
  virtual qint64 timeStamp(const QUrl& /*location*/) const { return 0; }
  virtual QByteArray rawXmlDescription(const QUrl& location, int /*timeout*/)
  {
    return UrlToXml[location];
  }

protected:

  virtual ctkCmdLineModuleFuture run(ctkCmdLineModuleFrontend* /*frontend*/)
  {
    return ctkCmdLineModuleFuture();
  }

private:

  QHash<QUrl, QByteArray> UrlToXml;
};

}

// ----------------------------------------------------------------------------
class ctkCmdLineModuleFrontendQtGuiTester: public QObject
{
  Q_OBJECT

private:

  QScopedPointer<ctkCmdLineModuleBackend> Backend;
  ctkCmdLineModuleManager Manager;

  ctkCmdLineModuleReference ModuleRef;

  QString ChangedParameter;
  QVariant ChangedParameterValue;

private Q_SLOTS:

  void valueChanged(const QString& parameter, const QVariant& value);

private Q_SLOTS:

  void initTestCase();

  void init();

  void testValueSetterAndGetter();
  void testValueSetterAndGetter_data();

};

// ----------------------------------------------------------------------------
void ctkCmdLineModuleFrontendQtGuiTester::valueChanged(const QString &parameter, const QVariant &value)
{
  this->ChangedParameter = parameter;
  this->ChangedParameterValue = value;
}

// ----------------------------------------------------------------------------
void ctkCmdLineModuleFrontendQtGuiTester::initTestCase()
{
  BackendMockUp* backend = new BackendMockUp;
  this->Backend.reset(backend);

  QFile xmlFile(":/ctkCmdLineModuleFrontendQtGuiTestModule1.xml");
  QVERIFY(xmlFile.open(QIODevice::ReadOnly));

  backend->addModule(QUrl("test://module1"), xmlFile.readAll());

  this->Manager.registerBackend(backend);

  this->ModuleRef = this->Manager.registerModule(QUrl("test://module1"));
  QVERIFY(this->ModuleRef);
}

// ----------------------------------------------------------------------------
void ctkCmdLineModuleFrontendQtGuiTester::init()
{
  this->ChangedParameter.clear();
  this->ChangedParameterValue.clear();
}

// ----------------------------------------------------------------------------
void ctkCmdLineModuleFrontendQtGuiTester::testValueSetterAndGetter()
{

  QScopedPointer<ctkCmdLineModuleFrontend> frontend(new ctkCmdLineModuleFrontendQtGui(this->ModuleRef));
  // force the creation of the frontend gui
  frontend->guiHandle();

  connect(frontend.data(), SIGNAL(valueChanged(QString,QVariant)), SLOT(valueChanged(QString,QVariant)));

  QFETCH(QString, parameter);
  QFETCH(QVariant, currentValue);
  QFETCH(QVariant, newValue);
  QFETCH(QVariant, expectedValue);
  QFETCH(int, role);

  if (role == -1)
  {
    // test with default role argument
    QCOMPARE(frontend->value(parameter), currentValue);
  }
  else
  {
    QCOMPARE(frontend->value(parameter, role), currentValue);
  }

  // test setting values
  if (newValue.isValid())
  {
    frontend->setValue(parameter, newValue);
    if (role == -1)
    {
      QCOMPARE(frontend->value(parameter), expectedValue);
    }
    else
    {
      QCOMPARE(frontend->value(parameter, role), expectedValue);
      if (role == ctkCmdLineModuleFrontend::DisplayRole)
      {
        QWidget* widget = frontend->guiHandle()->findChild<QWidget*>("parameter:" + parameter);
        QVERIFY(widget != NULL);
        QString tag = this->ModuleRef.description().parameter(parameter).tag();
        if (tag == "integer")
        {
          QSpinBox* spinBox = qobject_cast<QSpinBox*>(widget);
          QVERIFY(spinBox);
          QCOMPARE(spinBox->value(), expectedValue.toInt());
        }
        else if (tag.endsWith("enumeration"))
        {
          QComboBox* comboBox = qobject_cast<QComboBox*>(widget);
          QVERIFY(comboBox);
          QCOMPARE(comboBox->currentText(), expectedValue.toString());
        }
        else
        {
          QFAIL("Missing widget sub-class test code.");
        }
      }
    }

    QCOMPARE(this->ChangedParameter, parameter);
    QCOMPARE(this->ChangedParameterValue, expectedValue);
  }

}

// ----------------------------------------------------------------------------
void ctkCmdLineModuleFrontendQtGuiTester::testValueSetterAndGetter_data()
{
  QTest::addColumn<QString>("parameter");
  QTest::addColumn<QVariant>("currentValue");
  QTest::addColumn<QVariant>("newValue");
  QTest::addColumn<QVariant>("expectedValue");
  QTest::addColumn<int>("role");

  QTest::newRow("intParamDefaultDefaultRole") << "intParam" << QVariant(1) << QVariant(2) << QVariant(2) << -1;
  QTest::newRow("intParamDefaultDisplayRole") << "intParam" << QVariant(1) << QVariant(2) << QVariant(2) << static_cast<int>(ctkCmdLineModuleFrontend::DisplayRole);
  QTest::newRow("intParmaDefaultLRRole") << "intParam" << QVariant(1) << QVariant(2) << QVariant(2) << static_cast<int>(ctkCmdLineModuleFrontend::LocalResourceRole);

  // newValue too low
  QTest::newRow("intParamTooLowDefaultRole") << "intParam" << QVariant(1) << QVariant(-6) << QVariant(-5) << -1;
  QTest::newRow("intParamTooLowDisplayRole") << "intParam" << QVariant(1) << QVariant(-6) << QVariant(-5) << static_cast<int>(ctkCmdLineModuleFrontend::DisplayRole);
  QTest::newRow("intParmaTooLowLRRole") << "intParam" << QVariant(1) << QVariant(-6) << QVariant(-5) << static_cast<int>(ctkCmdLineModuleFrontend::LocalResourceRole);

  // newValue too high
  QTest::newRow("intParamTooHighDefaultRole") << "intParam" << QVariant(1) << QVariant(200) << QVariant(60) << -1;
  QTest::newRow("intParamTooHighDisplayRole") << "intParam" << QVariant(1) << QVariant(200) << QVariant(60) << static_cast<int>(ctkCmdLineModuleFrontend::DisplayRole);
  QTest::newRow("intParmaTooHighLRRole") << "intParam" << QVariant(1) << QVariant(200) << QVariant(60) << static_cast<int>(ctkCmdLineModuleFrontend::LocalResourceRole);

  QTest::newRow("stringEnumDefaultRole") << "stringEnumParam" << QVariant("yes") << QVariant("no") << QVariant("no") << -1;
  QTest::newRow("stringEnumDisplayRole") << "stringEnumParam" << QVariant("yes") << QVariant("no") << QVariant("no") << static_cast<int>(ctkCmdLineModuleFrontend::DisplayRole);
  QTest::newRow("stringEnumLRRole") << "stringEnumParam" << QVariant("yes") << QVariant("no") << QVariant("no") << static_cast<int>(ctkCmdLineModuleFrontend::LocalResourceRole);

  QTest::newRow("intOutputParamDefaultRole") << "intOutputParam" << QVariant(0) << QVariant(3) << QVariant(3) << -1;
  QTest::newRow("intOutputParamDisplayRole") << "intOutputParam" << QVariant(0) << QVariant(3) << QVariant(3) << static_cast<int>(ctkCmdLineModuleFrontend::DisplayRole);
  QTest::newRow("intOutputParamLRRole") << "intOutputParam" << QVariant(0) << QVariant(3) << QVariant(3) << static_cast<int>(ctkCmdLineModuleFrontend::LocalResourceRole);
}


// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkCmdLineModuleFrontendQtGuiTest)
#include "moc_ctkCmdLineModuleFrontendQtGuiTest.cpp"
