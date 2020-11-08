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
#include <QCheckBox>
#include <QSettings>
#include <QStyle>
#include <QTimer>

// CTK includes
#include "ctkMessageBox.h"
#include "ctkTest.h"

// STD includes
#include <cstdlib>
#include <iostream>

Q_DECLARE_METATYPE(QMessageBox::StandardButton);
Q_DECLARE_METATYPE(QMessageBox::ButtonRole);

// ----------------------------------------------------------------------------
class ctkMessageBoxDontShowAgainTester: public QObject
{
  Q_OBJECT
  void testExecMessageBox(ctkMessageBox& messageBox);

private Q_SLOTS:
  void initTestCase();
  void init();
  void cleanup();

  // Check default values of ctkMessageBox
  void testDefaults();

  // Ensure the "don't show again" checkbox works correctly with 1 button (Ok)
  void testDontShowAgain();
  void testDontShowAgain_data();

  // Ensure the "don't show again" checkbox works correctly with 2 buttons
  // Ok and Cancel
  void testOkCancel();
  void testOkCancel_data();

  // Ensure the "don't show again" checkbox works correctly with 2 buttons
  /// with custom text
  void testDontShowAgainCustomText();
  void testDontShowAgainCustomText_data();

  // Test Settings key with 1 button (Ok)
  void testDontShowAgainSettingsKey();
  void testDontShowAgainSettingsKey_data();

  // Test with 2 buttons (Ok and Cancel)
  void testDontShowAgainSettingsKeyOkCancel();
  void testDontShowAgainSettingsKeyOkCancel_data();

  // Check "Don't show again" before the the dialog is shown.
  // -> doesn't show the dialog.
  void testDontShowAgainSettingsKeySetDontShowAgain();
  void testDontShowAgainSettingsKeySetDontShowAgain_data();

  // After the dialog is shown, simulate a click on the
  // "don't show again" checkbox.
  void testDontShowAgainSettingsKeyClickDontShowAgain();
  void testDontShowAgainSettingsKeyClickDontShowAgain_data();

  // static utility function
  void testConfirmExit();
  void testConfirmExit_data();

  // test the visibility cases();
  void testVisible();
  //void testCustomButton();
};

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::initTestCase()
{
  // Setup QApplication for settings
  qApp->setOrganizationName("CommonToolKit");
  qApp->setOrganizationDomain("commontk.org");
  qApp->setApplicationName("CTK");
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::init()
{
  QSettings settings;
  settings.setValue("Show", int(QMessageBox::InvalidRole));
  settings.setValue("DontShowWithOkButton", int(QMessageBox::Ok));
  settings.setValue("DontShowWithAcceptRole", int(QMessageBox::AcceptRole));

  settings.setValue("DontShowWithCancelButton", int(QMessageBox::Cancel));
  settings.setValue("DontShowWithRejectRole", int(QMessageBox::RejectRole));
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::cleanup()
{
  QSettings settings;
  settings.clear();
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testDefaults()
{
  ctkMessageBox messageBox;

  QCOMPARE(messageBox.isDontShowAgainVisible(), false);
  QCOMPARE(messageBox.dontShowAgain(), false);
  QCOMPARE(messageBox.dontShowAgainSettingsKey(), QString());
  QCOMPARE(messageBox.buttons().size(), 0);
  QCOMPARE(messageBox.standardButtons(), QMessageBox::NoButton);

  messageBox.show();
  qApp->processEvents();

  QCOMPARE(messageBox.buttons().size(), 1);
  QCOMPARE(messageBox.standardButtons(), QMessageBox::Ok);
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testExecMessageBox(ctkMessageBox& messageBox)
{
  QFETCH(int, buttonOrRole);
  if (buttonOrRole != QMessageBox::InvalidRole)
    {
    if (messageBox.standardButtons() == QMessageBox::NoButton &&
        messageBox.buttons().size() == 0)
      {
      messageBox.addButton(QMessageBox::Ok);
      }
    if (messageBox.standardButtons() & buttonOrRole)
      {
      QAbstractButton* button = messageBox.button(
        static_cast<QMessageBox::StandardButton>(buttonOrRole));
      QVERIFY(button);
      QTimer::singleShot(0, button, SLOT(click()));
      }
    else
      {
      const char* slot = 0;
      if (buttonOrRole == QMessageBox::AcceptRole)
        {
        slot = SLOT(accept());
        }
      else if (buttonOrRole == QMessageBox::RejectRole)
        {
        slot = SLOT(reject());
        }
      QTimer::singleShot(0, &messageBox, slot);
      }
    }
  // shouldn't hang
  int execResult = messageBox.exec();
  QFETCH(int, result);
  QCOMPARE(execResult, result);

  QFETCH(QMessageBox::ButtonRole, resultRole);
  QCOMPARE(messageBox.buttonRole(messageBox.clickedButton()), resultRole);
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testDontShowAgain()
{
  ctkMessageBox messageBox;

  QFETCH(bool, visible);
  messageBox.setDontShowAgainVisible(visible);

  QFETCH(bool, dontShowAgain);
  messageBox.setDontShowAgain(dontShowAgain);

  this->testExecMessageBox(messageBox);
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testDontShowAgain_data()
{
  QTest::addColumn<bool>("visible");
  QTest::addColumn<bool>("dontShowAgain");
  QTest::addColumn<int>("buttonOrRole");
  QTest::addColumn<int>("result");
  QTest::addColumn<QMessageBox::ButtonRole>("resultRole");

  QTest::newRow("invisible dont show") << false << true << int(QMessageBox::InvalidRole) << int(QMessageBox::Ok) << QMessageBox::AcceptRole;
  QTest::newRow("visible dont show") << true << true << int(QMessageBox::InvalidRole) << int(QMessageBox::Ok) << QMessageBox::AcceptRole;

  QTest::newRow("invisible show click ok") << false << false << int(QMessageBox::Ok) << int(QMessageBox::Ok) << QMessageBox::AcceptRole;
  QTest::newRow("visible show click ok") << true << false << int(QMessageBox::Ok) << int(QMessageBox::Ok) << QMessageBox::AcceptRole;
  QTest::newRow("invisible show accept") << false << false << int(QMessageBox::AcceptRole) << int(QDialog::Accepted) << QMessageBox::InvalidRole;
  QTest::newRow("visible show accept") << true << false << int(QMessageBox::AcceptRole) << int(QDialog::Accepted) << QMessageBox::InvalidRole;
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testOkCancel()
{
  ctkMessageBox messageBox;
  messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

  QFETCH(bool, visible);
  messageBox.setDontShowAgainVisible(visible);

  QFETCH(bool, dontShowAgain);
  messageBox.setDontShowAgain(dontShowAgain);

  this->testExecMessageBox(messageBox);
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testOkCancel_data()
{
  this->testDontShowAgain_data();

  QTest::newRow("invisible show click cancel") << false << false << int(QMessageBox::Cancel) << int(QMessageBox::Cancel) << QMessageBox::RejectRole;
  QTest::newRow("visible show click cancel") << true << false << int(QMessageBox::Cancel) << int(QMessageBox::Cancel) << QMessageBox::RejectRole;
  QTest::newRow("invisible show reject") << false << false << int(QMessageBox::RejectRole) << int(QMessageBox::Rejected) << QMessageBox::InvalidRole;
  QTest::newRow("visible show click cancel") << true << false << int(QMessageBox::RejectRole) << int(QMessageBox::Rejected) << QMessageBox::InvalidRole;
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testDontShowAgainCustomText()
{
  ctkMessageBox messageBox;

  messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

  QAbstractButton *okButton = messageBox.button(QMessageBox::Ok);
  okButton->setText("SaveData");
  QAbstractButton *cancelButton = messageBox.button(QMessageBox::Cancel);
  cancelButton->setText("Discard Data");

  QFETCH(bool, visible);
  messageBox.setDontShowAgainVisible(visible);

  QFETCH(bool, dontShowAgain);
  messageBox.setDontShowAgain(dontShowAgain);

  this->testExecMessageBox(messageBox);

  // check that the don't show again text has been updated
  QCheckBox *checkBox = messageBox.findChild<QCheckBox*>("ctk_msgbox_dontshowcheckbox");
  QString dontShowAgainText = checkBox->text();

  if (visible)
    {
    // the custom text was set from the Ok box as it has the accept role
    QString expectedString = QString("Don't show this message again and always ")
      + okButton->text();
    QCOMPARE(dontShowAgainText, expectedString);
    }
  else
    {
    // the custom text was not added to the end of the standard message
    QString expectedString = QString("Don't show this message again");
    QCOMPARE(dontShowAgainText, expectedString);
    }
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testDontShowAgainCustomText_data()
{
  // use the basic test set up
  this->testDontShowAgain_data();
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testDontShowAgainSettingsKey()
{
  ctkMessageBox messageBox;

  QFETCH(bool, visible);
  messageBox.setDontShowAgain(visible);

  QFETCH(QString, key);
  messageBox.setDontShowAgainSettingsKey(key);
  QCOMPARE(messageBox.dontShowAgainSettingsKey(), key);

  QFETCH(bool, dontShowAgain);
  QCOMPARE(messageBox.dontShowAgain(), dontShowAgain);

  this->testExecMessageBox(messageBox);

  QSettings settings;
  QFETCH(int, keyValue);
  QCOMPARE(settings.value(key).toInt(), keyValue);
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testDontShowAgainSettingsKey_data()
{
  QTest::addColumn<bool>("visible");
  QTest::addColumn<QString>("key");
  QTest::addColumn<bool>("dontShowAgain");
  QTest::addColumn<int>("buttonOrRole");
  QTest::addColumn<int>("result");
  QTest::addColumn<QMessageBox::ButtonRole>("resultRole");
  QTest::addColumn<int>("keyValue");

  QTest::newRow("invisible NonExistingKey")
    << false << "NonExistingKey" << false << int(QMessageBox::Ok)
    << int(QMessageBox::Ok) << QMessageBox::AcceptRole
    << int(QMessageBox::InvalidRole);
  QTest::newRow("visible NonExistingKey")
    << true << "NonExistingKey" << false << int(QMessageBox::Ok)
    << int(QMessageBox::Ok) << QMessageBox::AcceptRole
    << int(QMessageBox::InvalidRole);

  QTest::newRow("invisible Show")
    << false << "Show" << false << int(QMessageBox::Ok)
    << int(QMessageBox::Ok) << QMessageBox::AcceptRole
    << int(QMessageBox::InvalidRole);
  QTest::newRow("visible Show")
    << true << "Show" << false << int(QMessageBox::Ok)
    << int(QMessageBox::Ok) << QMessageBox::AcceptRole
    << int(QMessageBox::InvalidRole);

  QTest::newRow("invisible DontShow Ok")
    << false << "DontShowWithOkButton" << true << int(QMessageBox::InvalidRole)
    << int(QMessageBox::Ok) << QMessageBox::AcceptRole
    << int(QMessageBox::Ok);
  QTest::newRow("visible DontShow Ok")
    << true << "DontShowWithOkButton" << true << int(QMessageBox::InvalidRole)
    << int(QMessageBox::Ok) << QMessageBox::AcceptRole
    << int(QMessageBox::Ok);

  QTest::newRow("invisible DontShow Accept")
    << false << "DontShowWithAcceptRole" << true << int(QMessageBox::InvalidRole)
    << int(QMessageBox::Ok) << QMessageBox::AcceptRole
    << int(QMessageBox::Ok);
  QTest::newRow("visible DontShow Accept")
    << true << "DontShowWithAcceptRole" << true << int(QMessageBox::InvalidRole)
    << int(QMessageBox::Ok) << QMessageBox::AcceptRole
    << int(QMessageBox::Ok);
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testDontShowAgainSettingsKeyOkCancel()
{
  ctkMessageBox messageBox;
  messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

  QFETCH(bool, visible);
  messageBox.setDontShowAgain(visible);

  QFETCH(QString, key);
  messageBox.setDontShowAgainSettingsKey(key);
  QCOMPARE(messageBox.dontShowAgainSettingsKey(), key);

  QFETCH(bool, dontShowAgain);
  QCOMPARE(messageBox.dontShowAgain(), dontShowAgain);

  this->testExecMessageBox(messageBox);

  QSettings settings;
  QFETCH(int, keyValue);
  QCOMPARE(settings.value(key).toInt(), keyValue);
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testDontShowAgainSettingsKeyOkCancel_data()
{
  this->testDontShowAgainSettingsKey_data();

  QTest::newRow("invisible DontShow Cancel")
    << false << "DontShowWithCancelButton" << true << int(QMessageBox::InvalidRole)
    << int(QMessageBox::Cancel) << QMessageBox::RejectRole
    << int(QMessageBox::Cancel);
  QTest::newRow("visible DontShow Cancel")
    << true << "DontShowWithCancelButton" << true << int(QMessageBox::InvalidRole)
    << int(QMessageBox::Cancel) << QMessageBox::RejectRole
    << int(QMessageBox::Cancel);

  QTest::newRow("invisible DontShow Reject")
    << false << "DontShowWithRejectRole" << true << int(QMessageBox::InvalidRole)
    << int(QMessageBox::Cancel) << QMessageBox::RejectRole
    << int(QMessageBox::RejectRole);
  QTest::newRow("visible DontShow Reject")
    << true << "DontShowWithRejectRole" << true << int(QMessageBox::InvalidRole)
    << int(QMessageBox::Cancel) << QMessageBox::RejectRole
    << int(QMessageBox::RejectRole);
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testDontShowAgainSettingsKeySetDontShowAgain()
{
  ctkMessageBox messageBox;
  messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

  QFETCH(QString, key);
  messageBox.setDontShowAgainSettingsKey(key);
  QCOMPARE(messageBox.dontShowAgainSettingsKey(), key);

  QFETCH(bool, dontShowAgain);
  messageBox.setDontShowAgain(dontShowAgain);

  this->testExecMessageBox(messageBox);

  QSettings settings;
  QFETCH(int, keyValue);
  QCOMPARE(settings.value(key).toInt(), keyValue);
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testDontShowAgainSettingsKeySetDontShowAgain_data()
{
  QTest::addColumn<QString>("key");
  QTest::addColumn<bool>("dontShowAgain");
  QTest::addColumn<int>("buttonOrRole");
  QTest::addColumn<int>("result");
  QTest::addColumn<QMessageBox::ButtonRole>("resultRole");
  QTest::addColumn<int>("keyValue");

  // Doesn't have time to click on anything, the dialog is already skipped
  QTest::newRow("NonExistingKey dont show again")
    << "NonExistingKey" << true << int(QMessageBox::InvalidRole)
    << int(QMessageBox::Ok) << QMessageBox::AcceptRole
    << int(QMessageBox::Ok);

  QTest::newRow("Show dont show again accept")
    << "Show" << true << int(QMessageBox::InvalidRole)
    << int(QMessageBox::Ok) << QMessageBox::AcceptRole
    << int(QMessageBox::Ok);

  QTest::newRow("NonExistingKey show again accept")
    << "NonExistingKey" << false << int(QMessageBox::Ok)
    << int(QMessageBox::Ok) << QMessageBox::AcceptRole
    << int(QMessageBox::InvalidRole);
  // ctkMessageBox never write the settings.
  QTest::newRow("NonExistingKey show again reject")
    << "NonExistingKey" << false << int(QMessageBox::Cancel)
    << int(QMessageBox::Cancel) << QMessageBox::RejectRole
    << QVariant().toInt();

  QTest::newRow("Show show again accept")
    << "Show" << false << int(QMessageBox::Ok)
    << int(QMessageBox::Ok) << QMessageBox::AcceptRole
    << int(QMessageBox::InvalidRole);
  // ctkMessageBox never write the settings but the settings exists.
  QTest::newRow("Show dont show reject")
    << "Show" << false << int(QMessageBox::Cancel)
    << int(QMessageBox::Cancel) << QMessageBox::RejectRole
    << int(QMessageBox::InvalidRole);

  QTest::newRow("Show show again accept")
    << "DontShowWithOkButton" << false << int(QMessageBox::Ok)
    << int(QMessageBox::Ok) << QMessageBox::AcceptRole
    << int(QMessageBox::InvalidRole);
  QTest::newRow("Show dont show reject")
    << "DontShowWithOkButton" << false << int(QMessageBox::Cancel)
    << int(QMessageBox::Cancel) << QMessageBox::RejectRole
    << int(QMessageBox::Ok);

}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testDontShowAgainSettingsKeyClickDontShowAgain()
{
  ctkMessageBox messageBox;
  messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

  messageBox.addDontShowAgainButtonRole(QMessageBox::YesRole);
  messageBox.addDontShowAgainButtonRole(QMessageBox::NoRole);

  QFETCH(QString, key);
  messageBox.setDontShowAgainSettingsKey(key);
  QCOMPARE(messageBox.dontShowAgainSettingsKey(), key);

  QFETCH(bool, dontShowAgain);
  QCheckBox* checkBox =
    messageBox.findChild<QCheckBox*>("ctk_msgbox_dontshowcheckbox");
  QMetaObject::invokeMethod(checkBox, "setChecked", Qt::QueuedConnection,
                            Q_ARG(bool, dontShowAgain));
  this->testExecMessageBox(messageBox);

  QSettings settings;
  QFETCH(int, keyValue);
  QCOMPARE(settings.value(key).toInt(), keyValue);
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testDontShowAgainSettingsKeyClickDontShowAgain_data()
{
  QTest::addColumn<QString>("key");
  QTest::addColumn<bool>("dontShowAgain");
  QTest::addColumn<int>("buttonOrRole");
  QTest::addColumn<int>("result");
  QTest::addColumn<QMessageBox::ButtonRole>("resultRole");
  QTest::addColumn<int>("keyValue");

  QTest::newRow("NonExistingKey dont show again ok")
    << "NonExistingKey" << true << int(QMessageBox::Ok)
    << int(QMessageBox::Ok) << QMessageBox::AcceptRole
    << int(QMessageBox::Ok);
  QTest::newRow("NonExistingKey dont show again no")
    << "NonExistingKey" << true << int(QMessageBox::No)
    << int(QMessageBox::No) << QMessageBox::NoRole
    << int(QMessageBox::No);
  QTest::newRow("NonExistingKey dont show again cancel")
    << "NonExistingKey" << true << int(QMessageBox::Cancel)
    << int(QMessageBox::Cancel) << QMessageBox::RejectRole
    << QVariant().toInt();

  QTest::newRow("Show dont show again accept")
    << "Show" << true << int(QMessageBox::Ok)
    << int(QMessageBox::Ok) << QMessageBox::AcceptRole
    << int(QMessageBox::Ok);
  QTest::newRow("Show dont show again no")
    << "Show" << true << int(QMessageBox::No)
    << int(QMessageBox::No) << QMessageBox::NoRole
    << int(QMessageBox::No);
  QTest::newRow("Show dont show again cancel")
    << "Show" << true << int(QMessageBox::Cancel)
    << int(QMessageBox::Cancel) << QMessageBox::RejectRole
    << int(QMessageBox::InvalidRole);

  QTest::newRow("NonExistingKey show again accept")
    << "NonExistingKey" << false << int(QMessageBox::Ok)
    << int(QMessageBox::Ok) << QMessageBox::AcceptRole
    << int(QMessageBox::InvalidRole);
  QTest::newRow("NonExistingKey show again no")
    << "NonExistingKey" << false << int(QMessageBox::No)
    << int(QMessageBox::No) << QMessageBox::NoRole
    << int(QMessageBox::InvalidRole);
  QTest::newRow("NonExistingKey show again reject")
    << "NonExistingKey" << false << int(QMessageBox::Cancel)
    << int(QMessageBox::Cancel) << QMessageBox::RejectRole
    << QVariant().toInt();

  QTest::newRow("Show show again accept")
    << "Show" << false << int(QMessageBox::Ok)
    << int(QMessageBox::Ok) << QMessageBox::AcceptRole
    << int(QMessageBox::InvalidRole);
  QTest::newRow("Show show again no")
    << "Show" << false << int(QMessageBox::No)
    << int(QMessageBox::No) << QMessageBox::NoRole
    << int(QMessageBox::InvalidRole);
  QTest::newRow("Show dont show reject")
    << "Show" << false << int(QMessageBox::Cancel)
    << int(QMessageBox::Cancel) << QMessageBox::RejectRole
    << int(QMessageBox::InvalidRole);
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testConfirmExit()
{
  QFETCH(QString, key);
  bool res = ctkMessageBox::confirmExit(key);

  QFETCH(bool, expectedRes);
  QCOMPARE(res, expectedRes);
}

// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testConfirmExit_data()
{
  QTest::addColumn<QString>("key");
  QTest::addColumn<bool>("expectedRes");

  QTest::newRow("DontShowWithOkButton") << "DontShowWithOkButton" << true;
  QTest::newRow("DontShowWithAcceptRole") << "DontShowWithAcceptRole" << true;
  QTest::newRow("DontShowWithCancelButton") << "DontShowWithCancelButton" << false;
  QTest::newRow("DontShowWithRejectRole") << "DontShowWithRejectRole" << false;
}


// ----------------------------------------------------------------------------
void ctkMessageBoxDontShowAgainTester::testVisible()
{
  ctkMessageBox messageBox;
  QCOMPARE(messageBox.isDontShowAgainVisible(), false);

  messageBox.setDontShowAgainSettingsKey("Non Empty key");
  QCOMPARE(messageBox.isDontShowAgainVisible(), true);

  messageBox.setDontShowAgainSettingsKey(QString());
  QCOMPARE(messageBox.isDontShowAgainVisible(), false);
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkMessageBoxDontShowAgainTest)
#include "moc_ctkMessageBoxDontShowAgainTest.cpp"
