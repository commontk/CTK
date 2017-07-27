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
#include <QSignalSpy>
#include <QTimer>
#include <QVBoxLayout>

// CTK includes
#include "ctkLanguageComboBox.h"
#include "ctkTest.h"

// STD includes
#include <cstdlib>
#include <iostream>

// ----------------------------------------------------------------------------
class ctkLanguageComboBoxTester: public QObject
{
  Q_OBJECT
private slots:

  void testDefaults();

  void testSetDefaultLanguage();
  void testSetDefaultLanguage_data();
};

// ----------------------------------------------------------------------------
void ctkLanguageComboBoxTester::testDefaults()
{
  ctkLanguageComboBox languageComboBox;
  QCOMPARE(languageComboBox.currentLanguage(), languageComboBox.defaultLanguage());
  QCOMPARE(languageComboBox.defaultLanguage(), QString());
  QCOMPARE(languageComboBox.directory(), QString());
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBoxTester::testSetDefaultLanguage()
{
  ctkLanguageComboBox languageComboBox;
  QFETCH(QString, defaultLanguage);
  languageComboBox.setDefaultLanguage(defaultLanguage);

  // Default language
  QFETCH(QString, expectedDefaultLanguage);
  QCOMPARE(languageComboBox.defaultLanguage(), expectedDefaultLanguage);
  if (!expectedDefaultLanguage.isEmpty())
    {
    QCOMPARE(languageComboBox.count(), 1);
    QCOMPARE(languageComboBox.itemData(0).toString(), expectedDefaultLanguage);
    QCOMPARE(languageComboBox.itemText(0),
             QLocale::languageToString(QLocale(expectedDefaultLanguage).language()));
    }

  // Current language
  QFETCH(QString, expectedCurrentLanguage);
  QCOMPARE(languageComboBox.currentLanguage(), expectedCurrentLanguage);
  QCOMPARE(languageComboBox.currentIndex(), expectedCurrentLanguage.isEmpty() ? -1 : 0);
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBoxTester::testSetDefaultLanguage_data()
{
  QTest::addColumn<QString>("defaultLanguage");
  QTest::addColumn<QString>("expectedDefaultLanguage");
  QTest::addColumn<QString>("expectedCurrentLanguage");

  QTest::newRow("invalid") << QString() << QString() << QString();
  QTest::newRow("empty") << QString("") << QString("") << QString("");
  QTest::newRow("en") << QString("en") << QString("en_US") << QString("en_US");
  // ISO_3166 says that The United Kingdom is GB not UK
  QTest::newRow("en_UK") << QString("en_UK") << QString("en_US") << QString("en_US");
  QTest::newRow("en_GB") << QString("en_GB") << QString("en_GB") << QString("en_GB");
  QTest::newRow("fr") << QString("fr") << QString("fr_FR") << QString("fr_FR");
  QTest::newRow("de_CH") << QString("de_CH") << QString("de_CH") << QString("de_CH");
  QTest::newRow("de_DE") << QString("de_DE") << QString("de_DE") << QString("de_DE");
  QTest::newRow("xx") << QString("xx") << QString() << QString();
  QTest::newRow("xx_yy") << QString("xx_yy") << QString() << QString();
  QTest::newRow("_en") << QString("_en") << QString() << QString();
  QTest::newRow("fr_") << QString("fr_") << QString("fr_FR") << QString("fr_FR");
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkLanguageComboBoxTest)
#include "moc_ctkLanguageComboBoxTest.cpp"
