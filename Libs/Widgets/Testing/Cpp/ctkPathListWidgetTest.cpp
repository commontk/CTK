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
#include <QCoreApplication>
#include <QSignalSpy>
#include <QTimer>

// CTK includes
#include "ctkPathListWidget.h"
#include "ctkTest.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
class ctkPathListWidgetTester : public QObject
{
  Q_OBJECT

private slots:

  void testDefaults();
  void testChangeOptions();
  void testMode();
  void testRelativeAndAbsolutePaths();
  void testSimilarPaths();
  void testSetPaths();
  void testEditPaths();
};

// ----------------------------------------------------------------------------
void ctkPathListWidgetTester::testDefaults()
{
  ctkPathListWidget pathList;

  QSignalSpy pathListChangedSpy(&pathList, SIGNAL(pathsChanged(QStringList,QStringList)));

  // The default options are Readable and Exists

  QCOMPARE(pathList.fileOptions(), ctkPathListWidget::Exists | ctkPathListWidget::Readable);
  QCOMPARE(pathList.directoryOptions(), ctkPathListWidget::Exists | ctkPathListWidget::Readable);

  QVERIFY(!pathList.addPath("/shouldnotexist/"));
  QVERIFY(!pathList.addPath("/should/also/not/exist"));

  QVERIFY(pathList.addPath(QDir::tempPath()));
  QVERIFY(!pathList.addPath(QDir::tempPath()));

  QVERIFY(!pathList.removePath("/shouldnotexist/"));


  QList<QVariant> expectedArgs;
  expectedArgs.push_back(QStringList(QDir::tempPath()));
  expectedArgs.push_back(QStringList());

  QCOMPARE(pathList.paths(), expectedArgs.front().toStringList());

  QCOMPARE(pathListChangedSpy.count(), 1);
  QCOMPARE(pathListChangedSpy.front(), expectedArgs);
}

// ----------------------------------------------------------------------------
void ctkPathListWidgetTester::testChangeOptions()
{
  ctkPathListWidget pathList;

  QSignalSpy pathListChangedSpy(&pathList, SIGNAL(pathsChanged(QStringList,QStringList)));

  // The default options are Readable and Exists

  QCOMPARE(pathList.fileOptions(), ctkPathListWidget::Exists | ctkPathListWidget::Readable);
  QCOMPARE(pathList.directoryOptions(), ctkPathListWidget::Exists | ctkPathListWidget::Readable);

  QCOMPARE(pathList.mode(), ctkPathListWidget::Any);

  QTemporaryFile tmpFile;
  QVERIFY(tmpFile.open());

  QVERIFY(!tmpFile.permissions().testFlag(QFile::ExeOwner));

  QVERIFY(pathList.addPath(tmpFile.fileName()));
  QCOMPARE(pathList.path(0), tmpFile.fileName());
  pathList.clear();
  QCOMPARE(pathList.count(), 0);
  QCOMPARE(pathList.path(0), QString());

  QCOMPARE(pathListChangedSpy.size(), 2);
  QCOMPARE(pathListChangedSpy.at(0), QList<QVariant>() << (QStringList() << tmpFile.fileName()) << QStringList());
  QCOMPARE(pathListChangedSpy.at(1), QList<QVariant>() << QStringList() << (QStringList() << tmpFile.fileName()));

  pathListChangedSpy.clear();

  // Add another temporary non-executable file
  QTemporaryFile tmpFile2;
  QVERIFY(tmpFile2.open());
  QVERIFY(pathList.addPath(tmpFile2.fileName()));
  QCOMPARE(pathList.count(), 1);
  QCOMPARE(pathList.path(0), tmpFile2.fileName());

  pathListChangedSpy.clear();

  // Changing the file options. This should invalidate tmpFile2 and remove it
  pathList.setFileOptions(pathList.fileOptions() | ctkPathListWidget::Executable);
  QCOMPARE(pathList.fileOptions(), ctkPathListWidget::Exists | ctkPathListWidget::Readable | ctkPathListWidget::Executable);
  pathList.setDirectoryOptions(pathList.directoryOptions() | ctkPathListWidget::Writable);
  QCOMPARE(pathList.directoryOptions(), ctkPathListWidget::Exists | ctkPathListWidget::Readable | ctkPathListWidget::Writable);

  QCOMPARE(pathList.count(), 0);

  QCOMPARE(pathListChangedSpy.count(), 1);
  QCOMPARE(pathListChangedSpy.at(0), QList<QVariant>() << QStringList() << (QStringList() << tmpFile2.fileName()));
  pathListChangedSpy.clear();

  // The tmp file is not executable, so it should not be added now
  QVERIFY(!pathList.addPath(tmpFile.fileName()));
  QVERIFY(pathListChangedSpy.isEmpty());

  QVERIFY(tmpFile.setPermissions(tmpFile.permissions() | QFile::ExeOwner));

  // Try again
  QVERIFY(pathList.addPath(tmpFile.fileName()));
  QCOMPARE(pathList.count(), 1);
  QCOMPARE(pathList.path(0), tmpFile.fileName());

  // Change the file options again. This should not invalidate the executable temporary file
  pathList.setFileOptions(ctkPathListWidget::Exists | ctkPathListWidget::Readable);
  QCOMPARE(pathList.fileOptions(), ctkPathListWidget::Exists | ctkPathListWidget::Readable);
  QCOMPARE(pathList.paths(), QStringList() << tmpFile.fileName());

  // Add the non-executable tmpFile2 again
  pathList.addPath(tmpFile2.fileName());
  QCOMPARE(pathList.count(), 2);
  QCOMPARE(pathList.paths(), QStringList() << tmpFile.fileName() << tmpFile2.fileName());

  QCOMPARE(pathListChangedSpy.count(), 2);
  pathListChangedSpy.clear();

  // Remove all
  pathList.clear();
  QCOMPARE(pathList.count(), 0);
  QCOMPARE(pathListChangedSpy.count(), 1);
  QCOMPARE(pathListChangedSpy.at(0), QList<QVariant>() << QStringList()
           << (QStringList() << tmpFile.fileName() << tmpFile2.fileName()));
  pathListChangedSpy.clear();

  // Add two at once
  pathList.addPaths(QStringList() << tmpFile.fileName() << tmpFile2.fileName());
  QCOMPARE(pathList.count(), 2);
  QCOMPARE(pathList.path(0), tmpFile.fileName());
  QCOMPARE(pathList.path(1), tmpFile2.fileName());

  QCOMPARE(pathListChangedSpy.count(), 1);
  QCOMPARE(pathListChangedSpy.at(0), QList<QVariant>()
           << (QStringList() << tmpFile.fileName() << tmpFile2.fileName())
           << QStringList());
}

// ----------------------------------------------------------------------------
void ctkPathListWidgetTester::testMode()
{
  ctkPathListWidget listWidget;

  listWidget.setFileOptions(ctkPathListWidget::None);
  QVERIFY(listWidget.fileOptions() == ctkPathListWidget::None);
  listWidget.setDirectoryOptions(ctkPathListWidget::None);
  QVERIFY(listWidget.directoryOptions() == ctkPathListWidget::None);

  QVERIFY(listWidget.addPath("/a"));
  QVERIFY(listWidget.addPath("/a/"));

  listWidget.setMode(ctkPathListWidget::FilesOnly);
  QVERIFY(listWidget.addPath("/b"));
  QVERIFY(!listWidget.addPath("/b/"));

  listWidget.setMode(ctkPathListWidget::DirectoriesOnly);
  QVERIFY(!listWidget.addPath("/c"));
  QVERIFY(listWidget.addPath("/c/"));
}

// ----------------------------------------------------------------------------
void ctkPathListWidgetTester::testRelativeAndAbsolutePaths()
{
  ctkPathListWidget listWidget;

  listWidget.setFileOptions(ctkPathListWidget::None);
  QVERIFY(listWidget.fileOptions() == ctkPathListWidget::None);
  listWidget.setDirectoryOptions(ctkPathListWidget::None);
  QVERIFY(listWidget.directoryOptions() == ctkPathListWidget::None);

  QStringList paths = QStringList()
      << "/some/absolute/path/to/file"
      << "/some/absolute/path/to/dir/"
      << "relative/path/to/file"
      << "relative/path/to/dir/";

  QStringList resultPaths = QStringList()
      << "/some/absolute/path/to/file"
      << "/some/absolute/path/to/dir/"
      << QDir::currentPath() + "/relative/path/to/file"
      << QDir::currentPath() + "/relative/path/to/dir/";

  QCOMPARE(listWidget.addPaths(paths), resultPaths);

  QCOMPARE(listWidget.path(0), resultPaths[0]);
  QCOMPARE(listWidget.path(1), resultPaths[1]);
  QCOMPARE(listWidget.path(2), resultPaths[2]);
  QCOMPARE(listWidget.path(3), resultPaths[3]);

  QCOMPARE(listWidget.files(), QStringList() << paths[0] << paths[2]);
  QCOMPARE(listWidget.files(true), QStringList() << resultPaths[0] << resultPaths[2]);
  QCOMPARE(listWidget.directories(), QStringList() << paths[1] << paths[3]);
  QCOMPARE(listWidget.directories(true), QStringList() << resultPaths[1] << resultPaths[3]);
}

// ----------------------------------------------------------------------------
void ctkPathListWidgetTester::testSimilarPaths()
{
  ctkPathListWidget listWidget;

  listWidget.setFileOptions(ctkPathListWidget::None);
  listWidget.setDirectoryOptions(ctkPathListWidget::None);

  QVERIFY(listWidget.addPath("/one/path"));
  QVERIFY(listWidget.addPath("/one/path/"));
  QVERIFY(listWidget.addPath("/one"));
  QVERIFY(listWidget.addPath("/one/"));
}

// ----------------------------------------------------------------------------
void ctkPathListWidgetTester::testSetPaths()
{
  ctkPathListWidget listWidget;

  listWidget.setFileOptions(ctkPathListWidget::None);
  QVERIFY(listWidget.fileOptions() == ctkPathListWidget::None);
  listWidget.setDirectoryOptions(ctkPathListWidget::None);
  QVERIFY(listWidget.directoryOptions() == ctkPathListWidget::None);

  QVERIFY(listWidget.addPath("/file/a"));
  QVERIFY(listWidget.addPath("/file/b"));
  QVERIFY(listWidget.addPath("/dir/a/"));

  QSignalSpy pathListChangedSpy(&listWidget, SIGNAL(pathsChanged(QStringList,QStringList)));

  QStringList newPaths = QStringList()
      << "/new/file/x"
      << "/file/b"
      << "/new/dir/x";

  listWidget.setPaths(newPaths);
  QCOMPARE(pathListChangedSpy.count(), 1);
  QCOMPARE(pathListChangedSpy.front().at(0).toStringList(), QStringList() << "/new/file/x" << "/new/dir/x");
}

// ----------------------------------------------------------------------------
void ctkPathListWidgetTester::testEditPaths()
{
  ctkPathListWidget listWidget;

  listWidget.setFileOptions(ctkPathListWidget::None);
  listWidget.setDirectoryOptions(ctkPathListWidget::None);

  QVERIFY(listWidget.addPath("/file/a"));
  QVERIFY(listWidget.addPath("/file/b"));
  QVERIFY(listWidget.addPath("/dir/a/"));

  QSignalSpy pathListChangedSpy(&listWidget, SIGNAL(pathsChanged(QStringList,QStringList)));

  QVERIFY(!listWidget.editPath(QModelIndex(), "bla"));
  QVERIFY(!listWidget.editPath(listWidget.model()->index(0,0), "/new/file/a/"));
  QVERIFY(listWidget.editPath(listWidget.model()->index(0,0), "/new/file/a"));
  QCOMPARE(listWidget.path(0), QString("/new/file/a"));

  QVERIFY(listWidget.editPath("/dir/a/", "/new/dir/a/"));
  QCOMPARE(listWidget.path(2), QString("/new/dir/a/"));

  QCOMPARE(pathListChangedSpy.count(), 2);
  QCOMPARE(pathListChangedSpy.at(0).at(0).toString(), QString("/new/file/a"));
  QCOMPARE(pathListChangedSpy.at(0).at(1).toString(), QString("/file/a"));

  QCOMPARE(pathListChangedSpy.at(1).at(0).toString(), QString("/new/dir/a/"));
  QCOMPARE(pathListChangedSpy.at(1).at(1).toString(), QString("/dir/a/"));
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkPathListWidgetTest)
#include "moc_ctkPathListWidgetTest.cpp"
