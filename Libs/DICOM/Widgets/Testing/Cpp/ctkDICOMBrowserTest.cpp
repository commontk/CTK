/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QTest>

// CTK includes
#include "ctkDICOMBrowser.h"
#include "ctkScopedCurrentDir.h"
#include "ctkTest.h"
#include "ctkUtils.h"

//-----------------------------------------------------------------------------
class ctkDICOMBrowserTester: public QObject
{
  Q_OBJECT
private slots:

  void testDefaults();
};

// ----------------------------------------------------------------------------
void ctkDICOMBrowserTester::testDefaults()
{
  ctkScopedCurrentDir scopedCurrentDir(QDir::tempPath());

  // If any, remove ./ctkDICOM-Database directory
  ctk::removeDirRecursively("./ctkDICOM-Database");
  QCOMPARE(QFileInfo("./ctkDICOM-Database").isDir(), false);

  ctkDICOMBrowser browser;

  QVERIFY(QFileInfo("./ctkDICOM-Database").isDir());
  QVERIFY(QFileInfo("./ctkDICOM-Database/ctkDICOM.sql").isFile());
  QVERIFY(QFileInfo("./ctkDICOM-Database/ctkDICOMTagCache.sql").isFile());

  QCOMPARE(ctkDICOMBrowser::databaseDirectorySettingsKey(), QString("DatabaseDirectory"));

  QCOMPARE(browser.databaseDirectory(), QString("./ctkDICOM-Database"));
  QVERIFY(browser.database() != 0);

  QCOMPARE(browser.tagsToPrecache(), QStringList());

  QVERIFY(browser.dicomTableManager() != 0);

  QCOMPARE(browser.displayImportSummary(), true);
  QCOMPARE(browser.patientsAddedDuringImport(), 0);
  QCOMPARE(browser.studiesAddedDuringImport(), 0);
  QCOMPARE(browser.seriesAddedDuringImport(), 0);
  QCOMPARE(browser.instancesAddedDuringImport(), 0);

  QCOMPARE(browser.importDirectoryMode(), ctkDICOMBrowser::ImportDirectoryAddLink);
  QCOMPARE(browser.skipConfirmImportDirectory(), false);
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkDICOMBrowserTest)
#include "moc_ctkDICOMBrowserTest.cpp"
