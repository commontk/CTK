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
#include <QString>
#include <QTimer>

// CTK includes
#include "ctkConfig.h"
#include "ctkConsole.h"
#include "ctkTest.h"

// ----------------------------------------------------------------------------
class ctkConsoleTester: public QObject
{
  Q_OBJECT
private slots:
  void testShow();

  void testRunFile();
  void testRunFile_data();
};

// ----------------------------------------------------------------------------
void ctkConsoleTester::testShow()
{
  ctkConsole console;
  console.show();
#if (QT_VERSION >= 0x50000)
  bool result = QTest::qWaitForWindowActive(&console);
  Q_UNUSED(result);
#else
  QTest::qWaitForWindowShown(&console);
#endif
}

// ----------------------------------------------------------------------------
void ctkConsoleTester::testRunFile()
{
  ctkConsole console;
  QFETCH(QString, file);
  console.runFile(file);
}

// ----------------------------------------------------------------------------
void ctkConsoleTester::testRunFile_data()
{
  QTest::addColumn<QString>("file");
  QTest::newRow("null") << QString();
  QTest::newRow("empty") << "";
  QTest::newRow("invalid") << " ";
  QTest::newRow(".") << ".";
  QTest::newRow("file")
    << QFileInfo(QDir(CTK_SOURCE_DIR), "README").absoluteFilePath();
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkConsoleTest)
#include "moc_ctkConsoleTest.cpp"
