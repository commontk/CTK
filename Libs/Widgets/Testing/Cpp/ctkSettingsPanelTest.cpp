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
#include <QSettings>
#include <QSignalSpy>
#include <QSpinBox>

// CTK includes
#include "ctkSettingsPanel.h"
#include "ctkSettingsPanelTest2Helper.h"
#include "ctkTest.h"

// STD includes
#include <cstdlib>
#include <iostream>


// ----------------------------------------------------------------------------
class ctkSettingsPanelTester: public QObject
{
  Q_OBJECT
private slots:

  void testChangeProperty();
  void testChangeProperty_data();

  void testEmptyQStringList();

  void testResetRestoreReloadSettings();
  void testResetRestoreReloadSettings_data();
};

//-----------------------------------------------------------------------------
void ctkSettingsPanelTester::testChangeProperty()
{
  QFETCH(bool, registerSpecificSettings);
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Common ToolKit", "CTK");
  QSettings specificSettings(QSettings::IniFormat, QSettings::UserScope, "Common ToolKit", "CTK-specific");
  
  // Clear settings
  settings.clear();
  settings.sync();
  specificSettings.clear();
  specificSettings.sync();
  
  QSpinBox spinBox;
  ctkSettingsPanel panel;
  panel.setSettings(&settings);
  spinBox.setValue(1);

  QFETCH(QString, label);
  QFETCH(ctkSettingsPanel::SettingOptions, options);
  panel.registerProperty("property", &spinBox,
                         "value", SIGNAL(valueChanged(int)),
                         label, options, registerSpecificSettings ? &specificSettings : 0);

  QCOMPARE(spinBox.value(), 1);
  QCOMPARE(panel.settingLabel("property"), label);
  QCOMPARE(panel.settingOptions("property"), options);

  qRegisterMetaType<QVariant>("QVariant");
  QSignalSpy spy(&panel, SIGNAL(settingChanged(QString,QVariant)));
  QFETCH(int, value);
  QFETCH(bool, setOnObject);
  if (setOnObject)
    {
    spinBox.setValue(value);
    }
  else
    {
    panel.setSetting("property", QVariant(value));
    }

  QFETCH(int, expectedSettingChangedCount);
  QCOMPARE(spy.count(), expectedSettingChangedCount);
  QFETCH(QStringList, expectedChangedSettings);
  QCOMPARE(panel.changedSettings(), expectedChangedSettings);

  // make sure it didn't change
  QCOMPARE(panel.settingLabel("property"), label);
  QCOMPARE(panel.settingOptions("property"), options);
  
  QString currentSettingContent;
  {
    settings.sync();
    if (QFile::exists(settings.fileName()))
      {
      QFile file(settings.fileName());
      QVERIFY(file.open(QIODevice::ReadOnly));
      currentSettingContent = file.readAll();
      file.close();
      }
  }
  
  QString currentSpecificSettingsContent;
  {
    specificSettings.sync();
    if (QFile::exists(specificSettings.fileName()))
      {
      QFile file(specificSettings.fileName());
      QVERIFY(file.open(QIODevice::ReadOnly));
      currentSpecificSettingsContent = file.readAll();
      file.close();
      }
  }

  QString expectedSettingsContent = QLatin1String("[General]\nproperty=%1\n");
 
  if (expectedChangedSettings.count() > 0)
    {
    if (registerSpecificSettings)
      {
      QCOMPARE(currentSettingContent, QString(""));
      QCOMPARE(currentSpecificSettingsContent, expectedSettingsContent.arg(value));
      }
    else
      {
      QCOMPARE(currentSettingContent, expectedSettingsContent.arg(value));
      QCOMPARE(currentSpecificSettingsContent, QString(""));
      }
    }
  else
    {
    if (registerSpecificSettings)
      {
      QCOMPARE(currentSettingContent, QString(""));
      QCOMPARE(currentSpecificSettingsContent, expectedSettingsContent.arg(1));
      }
    else
      {
      QCOMPARE(currentSettingContent, expectedSettingsContent.arg(1));
      QCOMPARE(currentSpecificSettingsContent, QString(""));
      }
    }

  panel.resetSettings();
}

//-----------------------------------------------------------------------------
void ctkSettingsPanelTester::testChangeProperty_data()
{
  QTest::addColumn<QString>("label");
  QTest::addColumn<ctkSettingsPanel::SettingOptions>("options");
  QTest::addColumn<int>("value");
  QTest::addColumn<bool>("setOnObject");
  QTest::addColumn<bool>("registerSpecificSettings");
  QTest::addColumn<int>("expectedSettingChangedCount");
  QTest::addColumn<QStringList>("expectedChangedSettings");

  // registerSpecificSettings: false
  QTest::newRow("null none changed obj") << QString() << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 2 << true << false << 1 << QStringList("property");
  QTest::newRow("null none changed panel") << QString() << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 2 << false << false << 1 << QStringList("property");
  QTest::newRow("null none unchanged obj") << QString() << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 1 << true << false << 0 << QStringList();
  QTest::newRow("null none unchanged panel") << QString() << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 1 << false << false << 0 << QStringList();
  QTest::newRow("null RequireRestart changed obj") << QString() << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 2 << true << false << 1 << QStringList("property");
  QTest::newRow("null RequireRestart changed panel") << QString() << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 2 << false << false << 1 << QStringList("property");
  QTest::newRow("null RequireRestart unchanged obj") << QString() << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 1 << true << false << 0 << QStringList();
  QTest::newRow("null RequireRestart unchanged panel") << QString() << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 1 << false << false << 0 << QStringList();
  QTest::newRow("empty none changed obj") << QString("") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 2 << true << false << 1 << QStringList("property");
  QTest::newRow("empty none changed panel") << QString("") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 2 << false << false << 1 << QStringList("property");
  QTest::newRow("empty none unchanged obj") << QString("") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 1 << true << false << 0 << QStringList();
  QTest::newRow("empty none unchanged panel") << QString("") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 1 << false << false << 0 << QStringList();
  QTest::newRow("empty RequireRestart changed obj") << QString("") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 2 << true << false << 1 << QStringList("property");
  QTest::newRow("empty RequireRestart changed panel") << QString("") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 2 << false << false << 1 << QStringList("property");
  QTest::newRow("empty RequireRestart unchanged obj") << QString("") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 1 << true << false << 0 << QStringList();
  QTest::newRow("empty RequireRestart unchanged panel") << QString("") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 1 << false << false << 0 << QStringList();
  QTest::newRow("label none changed obj") << QString("label") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 2 << true << false << 1 << QStringList("property");
  QTest::newRow("label none changed panel") << QString("label") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 2 << false << false << 1 << QStringList("property");
  QTest::newRow("label none unchanged obj") << QString("label") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 1 << true << false << 0 << QStringList();
  QTest::newRow("label none unchanged panel") << QString("label") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 1 << false << false << 0 << QStringList();
  QTest::newRow("label RequireRestart changed obj") << QString("label") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 2 << true << false << 1 << QStringList("property");
  QTest::newRow("label RequireRestart changed panel") << QString("label") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 2 << false << false << 1 << QStringList("property");
  QTest::newRow("label RequireRestart unchanged obj") << QString("label") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 1 << true << false << 0 << QStringList();
  QTest::newRow("label RequireRestart unchanged panel") << QString("label") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 1 << false << false << 0 << QStringList();
  
  // registerSpecificSettings: true
  QTest::newRow("null none changed obj") << QString() << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 2 << true << true << 1 << QStringList("property");
  QTest::newRow("null none changed panel") << QString() << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 2 << false << true << 1 << QStringList("property");
  QTest::newRow("null none unchanged obj") << QString() << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 1 << true << true << 0 << QStringList();
  QTest::newRow("null none unchanged panel") << QString() << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 1 << false << true << 0 << QStringList();
  QTest::newRow("null RequireRestart changed obj") << QString() << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 2 << true << true << 1 << QStringList("property");
  QTest::newRow("null RequireRestart changed panel") << QString() << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 2 << false << true << 1 << QStringList("property");
  QTest::newRow("null RequireRestart unchanged obj") << QString() << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 1 << true << true << 0 << QStringList();
  QTest::newRow("null RequireRestart unchanged panel") << QString() << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 1 << false << true << 0 << QStringList();
  QTest::newRow("empty none changed obj") << QString("") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 2 << true << true << 1 << QStringList("property");
  QTest::newRow("empty none changed panel") << QString("") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 2 << false << true << 1 << QStringList("property");
  QTest::newRow("empty none unchanged obj") << QString("") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 1 << true << true << 0 << QStringList();
  QTest::newRow("empty none unchanged panel") << QString("") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 1 << false << true << 0 << QStringList();
  QTest::newRow("empty RequireRestart changed obj") << QString("") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 2 << true << true << 1 << QStringList("property");
  QTest::newRow("empty RequireRestart changed panel") << QString("") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 2 << false << true << 1 << QStringList("property");
  QTest::newRow("empty RequireRestart unchanged obj") << QString("") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 1 << true << true << 0 << QStringList();
  QTest::newRow("empty RequireRestart unchanged panel") << QString("") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 1 << false << true << 0 << QStringList();
  QTest::newRow("label none changed obj") << QString("label") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 2 << true << true << 1 << QStringList("property");
  QTest::newRow("label none changed panel") << QString("label") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 2 << false << true << 1 << QStringList("property");
  QTest::newRow("label none unchanged obj") << QString("label") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 1 << true << true << 0 << QStringList();
  QTest::newRow("label none unchanged panel") << QString("label") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionNone) << 1 << false << true << 0 << QStringList();
  QTest::newRow("label RequireRestart changed obj") << QString("label") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 2 << true << true << 1 << QStringList("property");
  QTest::newRow("label RequireRestart changed panel") << QString("label") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 2 << false << true << 1 << QStringList("property");
  QTest::newRow("label RequireRestart unchanged obj") << QString("label") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 1 << true << true << 0 << QStringList();
  QTest::newRow("label RequireRestart unchanged panel") << QString("label") << ctkSettingsPanel::SettingOptions(ctkSettingsPanel::OptionRequireRestart) << 1 << false << true << 0 << QStringList();
}

namespace
{
//-----------------------------------------------------------------------------
class ctkSettingsPanelForTest : public ctkSettingsPanel
{
public:
  QVariant myDefaultPropertyValue(const QString& key) const
    {
    return this->defaultPropertyValue(key);
    }
  QVariant myPreviousPropertyValue(const QString& key) const
    {
    return this->previousPropertyValue(key);
    }
  QVariant myPropertyValue(const QString& key) const
    {
    return this->propertyValue(key);
    }
};

} // end of anonymous namespace

//-----------------------------------------------------------------------------
void ctkSettingsPanelTester::testEmptyQStringList()
{
  {
    // When QSettings goes out of scope, we are the settings file is up-to-date
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Common ToolKit", "CTK");
    settings.clear();
    settings.setValue("list", QVariant(QStringList()));
  }

  // Regression: Reading empty QStringList property from settings should be handled properly
  // See issue #646

  ctkSettingsPanelForTest settingsPanel;
  ctkSettingsPanelTest2Helper * list = new ctkSettingsPanelTest2Helper(&settingsPanel);
  settingsPanel.registerProperty("list", list, "list", SIGNAL(listChanged()));
  QSettings settings2(QSettings::IniFormat, QSettings::UserScope, "Common ToolKit", "CTK");
  settingsPanel.setSettings(&settings2);

  QVariant listVal = settings2.value("list");
#if QT_VERSION < QT_VERSION_CHECK(5,4,0)
  QCOMPARE(listVal.isValid(), false); // See issue #646
  QCOMPARE(listVal, QVariant()); // See issue #646
#else
  QCOMPARE(listVal.isValid(), true);
  QCOMPARE(listVal, QVariant(QStringList()));
#endif
  QCOMPARE(listVal.toStringList(), QStringList());
  QCOMPARE(settingsPanel.myPreviousPropertyValue("list").toStringList(), QStringList());
  QCOMPARE(settingsPanel.myDefaultPropertyValue("list").toStringList(), QStringList());
  QCOMPARE(settingsPanel.myPropertyValue("list").toStringList(), QStringList());
}

//-----------------------------------------------------------------------------
void ctkSettingsPanelTester::testResetRestoreReloadSettings()
{
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Common ToolKit", "CTK");
  // Clear settings
  settings.clear();
  settings.sync();

  int settingsValue = 1;
  int initValue = 2;
  int newValue = 3;

  QFETCH(bool, setSettingsValue);
  if (setSettingsValue)
    {
    settings.setValue("key 1", settingsValue);
    }

  QSpinBox spinBox;
  spinBox.setValue(initValue);

  ctkSettingsPanel settingsPanel;
  settingsPanel.setSettings(&settings);
  settingsPanel.registerProperty("key 1", &spinBox, "value",
                                 SIGNAL(valueChanged(int)));

  QFETCH(bool, setSpinBoxValue);
  if (setSpinBoxValue)
    {
    spinBox.setValue(newValue);
    }

  QFETCH(bool, applySettings);
  if (applySettings)
    {
    settingsPanel.applySettings();
    }

  QFETCH(bool, resetSettings);
  if (resetSettings)
    {
    settingsPanel.resetSettings();
    }

  QFETCH(bool, restoreDefaultSettings);
  if (restoreDefaultSettings)
    {
    settingsPanel.restoreDefaultSettings();
    }

  QFETCH(bool, reloadSettings);
  if (reloadSettings)
    {
    settingsPanel.reloadSettings();
    }

  QFETCH(int, finalValue);
  QCOMPARE(settings.value("key 1").toInt(), finalValue);
}

//-----------------------------------------------------------------------------
void ctkSettingsPanelTester::testResetRestoreReloadSettings_data()
{
  QTest::addColumn<bool>("setSettingsValue");
  QTest::addColumn<bool>("setSpinBoxValue");
  QTest::addColumn<bool>("applySettings");
  QTest::addColumn<bool>("resetSettings");
  QTest::addColumn<bool>("restoreDefaultSettings");
  QTest::addColumn<bool>("reloadSettings");
  QTest::addColumn<int>("finalValue");

  int settingsValue = 1;
  int initValue = 2;
  int newValue = 3;

  //   settings spinbox apply    reset   restore  reload  final
  QTest::newRow("set value")
    << false << true << false << false << false << false << newValue;
  QTest::newRow("reload")
    << false << true << false << false << false << true << newValue;
  QTest::newRow("restore")
    << false << true << false << false << true << false << initValue;
  QTest::newRow("restore and reload")
    << false << true << false << false << true << true << initValue;
  QTest::newRow("reset")
    << false << true << false << true << false << false << initValue;
  QTest::newRow("reset and reload")
    << false << true << false << true << false << true << initValue;
  QTest::newRow("reset and restore")
    << false << true << false << true << true << false << initValue;
  QTest::newRow("reset, restore and reload")
    << false << true << false << true << true << true << initValue;
  QTest::newRow("apply")
    << false << true << true << false << false << false << newValue;
  QTest::newRow("apply and reload")
    << false << true << true << false << false << true << newValue;
  QTest::newRow("apply and restore")
    << false << true << true << false << true << false << initValue;
  QTest::newRow("apply, restore and reload")
    << false << true << true << false << true << true << initValue;
  QTest::newRow("apply and reset")
    << false << true << true << true << false << false << newValue;
  QTest::newRow("apply, reset and reload")
    << false << true << true << true << false << true << newValue;
  QTest::newRow("apply, reset and restore")
    << false << true << true << true << true << false << initValue;
  QTest::newRow("apply, reset, restore and reload")
    << false << true << true << true << true << true << initValue;

  // with existing settings
  //   settings spinbox apply    reset   restore  reload  final
  QTest::newRow("set value with settings")
    << true << true << false << false << false << false << newValue;
  QTest::newRow("reload with settings")
    << true << true << false << false << false << true << newValue;
  QTest::newRow("restore with settings")
    << true << true << false << false << true << false << initValue;
  QTest::newRow("restore and reload with settings")
    << true << true << false << false << true << true << initValue;
  QTest::newRow("reset with settings")
    << true << true << false << true << false << false << settingsValue;
  QTest::newRow("reset and reload with settings")
    << true << true << false << true << false << true << settingsValue;
  QTest::newRow("reset and restore with settings")
    << true << true << false << true << true << false << initValue;
  QTest::newRow("reset, restore and reload with settings")
    << true << true << false << true << true << true << initValue;
  QTest::newRow("apply with settings")
    << true << true << true << false << false << false << newValue;
  QTest::newRow("apply and reload with settings")
    << true << true << true << false << false << true << newValue;
  QTest::newRow("apply and restore with settings")
    << true << true << true << false << true << false << initValue;
  QTest::newRow("apply, restore and reload with settings")
    << true << true << true << false << true << true << initValue;
  QTest::newRow("apply and reset with settings")
    << true << true << true << true << false << false << newValue;
  QTest::newRow("apply, reset and reload with settings")
    << true << true << true << true << false << true << newValue;
  QTest::newRow("apply, reset and restore with settings")
    << true << true << true << true << true << false << initValue;
  QTest::newRow("apply, reset, restore and reload with settings")
    << true << true << true << true << true << true << initValue;

}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkSettingsPanelTest)
#include "moc_ctkSettingsPanelTest.cpp"
