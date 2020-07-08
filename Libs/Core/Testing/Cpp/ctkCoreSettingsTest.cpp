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

// CTK includes
#include "ctkCoreSettings.h"
#include "ctkTest.h"

// STD includes
#include <iostream>
#include <limits>

// Qt includes
#include <QApplication>

// ----------------------------------------------------------------------------
class ctkCoreSettingsTester: public QObject
{
  Q_OBJECT
private slots:
  void initTestCase();

  void testSinglePath();
  void testSinglePath_data();

  void testMultiplePaths();
  void testMultiplePaths_data();
};

// ----------------------------------------------------------------------------
void ctkCoreSettingsTester::initTestCase()
{
}

// ----------------------------------------------------------------------------
void ctkCoreSettingsTester::testSinglePath()
{
  QFETCH(QString, homeDir);
  QFETCH(QString, inputPath);
  QFETCH(QString, expectedStoredValue);

  ctkCoreSettings settings;
  settings.setApplicationHomeDirectory(homeDir);
  QString key("someKey");
  settings.setPathValue(key, inputPath);

  QCOMPARE(settings.value(key).toString(), expectedStoredValue);
  QCOMPARE(settings.pathValue(key).toString().toLower(), QDir::cleanPath(inputPath).toLower());
}

// ----------------------------------------------------------------------------
void ctkCoreSettingsTester::testSinglePath_data()
{
  QTest::addColumn<QString>("homeDir");
  QTest::addColumn<QString>("inputPath");
  QTest::addColumn<QString>("expectedStoredValue");

  QTest::newRow("windows-style relative 1") << "c:/windows/path"
    << "c:/windows/path/internal/dir" << "<APPLICATION_HOME_DIR>/internal/dir";
  QTest::newRow("windows-style relative 2") << "c:\\windows\\path"
    << "c:/windows/path/internal/subdir/file.txt" << "<APPLICATION_HOME_DIR>/internal/subdir/file.txt";
  QTest::newRow("windows-style relative 3") << "c:/windows/path/"
    << "C:/windows/path/internal/dir" << "<APPLICATION_HOME_DIR>/internal/dir";
  QTest::newRow("windows-style relative 4") << "c:/windows/path/"
    << "c:/windows/path/internal/subdir/file.txt" << "<APPLICATION_HOME_DIR>/internal/subdir/file.txt";
  QTest::newRow("windows-style absolute 1") << "c:/windows/path"
    << "d:/windows/path/internal/dir" << "d:/windows/path/internal/dir";
  QTest::newRow("windows-style absolute 2") << "c:/windows/path"
    << "c:/windows/external/subdir/file.txt" << "c:/windows/external/subdir/file.txt";

  QTest::newRow("linux-style relative 1") << "/linux/path"
    << "/linux/path/internal/dir" << "<APPLICATION_HOME_DIR>/internal/dir";
  QTest::newRow("linux-style relative 2") << "/linux/path/"
    << "/linux/path/internal/dir" << "<APPLICATION_HOME_DIR>/internal/dir";
  QTest::newRow("linux-style relative 3") << "/linux/path/"
    << "/linux/path/internal/subdir/file.txt" << "<APPLICATION_HOME_DIR>/internal/subdir/file.txt";
  QTest::newRow("linux-style absolute 1") << "/linux/path"
    <<  "/xunil/path/internal/dir" << "/xunil/path/internal/dir";
  QTest::newRow("linux-style absolute 2") << "/linux/path"
    << "/linux/external/subdir/file.txt" << "/linux/external/subdir/file.txt";
}

// ----------------------------------------------------------------------------
void ctkCoreSettingsTester::testMultiplePaths()
{
  QFETCH(QString, homeDir);
  QFETCH(QStringList, inputPaths);
  QFETCH(QStringList, expectedStoredValues);

  ctkCoreSettings settings;
  settings.setApplicationHomeDirectory(homeDir);
  QString key("someKey");
  settings.setPathValue(key, inputPaths);

  QCOMPARE(settings.value(key).toStringList(), expectedStoredValues);

  QStringList retrievedPaths = settings.pathValue(key).toStringList();
  QCOMPARE(inputPaths.size(), retrievedPaths.size());
  for (int index=0; index < inputPaths.size(); index++)
    {
    QCOMPARE(retrievedPaths[index].toLower(), QDir::cleanPath(inputPaths[index]).toLower());
    }
}

// ----------------------------------------------------------------------------
void ctkCoreSettingsTester::testMultiplePaths_data()
{
  QTest::addColumn<QString>("homeDir");
  QTest::addColumn<QStringList>("inputPaths");
  QTest::addColumn<QStringList>("expectedStoredValues");

  QTest::newRow("relative paths multiple items") << "c:/windows/path"
    << (QStringList() << "c:/windows/path/internal/dir" << "c:/windows/path/internal/subdir/file.txt")
    << (QStringList() << "<APPLICATION_HOME_DIR>/internal/dir" << "<APPLICATION_HOME_DIR>/internal/subdir/file.txt");

  QTest::newRow("relative paths single item") << "c:/windows/path"
    << (QStringList() << "C:/windows/path/internal/dir")
    << (QStringList() << "<APPLICATION_HOME_DIR>/internal/dir");

  QTest::newRow("absolute paths") << "c:/windows/path"
    << (QStringList() << "d:/windows/path/internal/dir" << "c:/windows/external/subdir/file.txt")
    << (QStringList() << "d:/windows/path/internal/dir" << "c:/windows/external/subdir/file.txt");
}

// ----------------------------------------------------------------------------
int ctkCoreSettingsTest(int argc, char* argv[])
{
  QApplication app(argc, argv);
  app.setOrganizationName("CommonToolkit");
  app.setOrganizationDomain("commontk.org");
  app.setApplicationName("CTK");
  QSettings::setDefaultFormat(QSettings::IniFormat);

  QTEST_DISABLE_KEYPAD_NAVIGATION
  ctkCoreSettingsTester tc;
  return QTest::qExec(&tc, argc, argv);
}

#include "moc_ctkCoreSettingsTest.cpp"
