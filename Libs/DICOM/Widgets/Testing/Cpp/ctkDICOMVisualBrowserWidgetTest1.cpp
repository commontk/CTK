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

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Center for Intelligent Image-guided Interventions (CI3).

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QTimer>

// ctkCore includes
#include <ctkCoreTestingMacros.h>
#include <ctkUtils.h>

// ctkDICOMWidget includes
#include "ctkDICOMVisualBrowserWidget.h"

int ctkDICOMVisualBrowserWidgetTest1(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();

  bool interactive = arguments.removeOne("-I");

  if (arguments.count() != 1)
  {
    std::cerr << "Usage: " << qPrintable(testName)
              << " [-I] <path-to-dicom-directory>" << std::endl;
    return EXIT_FAILURE;
  }

  QString dicomDirectory(arguments.at(0));

  ctkDICOMVisualBrowserWidget browser;

  // Test the default values
  CHECK_QSTRING(browser.storageAETitle(), "CTKSTORE");
  CHECK_INT(browser.storagePort(), 11112);
  CHECK_QSTRING(browser.filteringPatientID(), "");
  CHECK_QSTRING(browser.filteringPatientName(), "");
  CHECK_QSTRING(browser.filteringStudyDescription(), "");
  CHECK_QSTRING(browser.filteringSeriesDescription(), "");
  CHECK_QSTRING(browser.filteringModalities().at(0), "Any");
  CHECK_INT(browser.filteringDate(), ctkDICOMPatientItemWidget::DateType::Any);
  CHECK_INT(browser.numberOfStudiesPerPatient(), 2);
  CHECK_INT(browser.thumbnailSize(), ctkDICOMStudyItemWidget::ThumbnailSizeOption::Medium);
  CHECK_BOOL(browser.isSendActionVisible(), false);
  CHECK_BOOL(browser.isDeleteActionVisible(), true);

  // Test visual browser import functionality
  QFileInfo tempFileInfo(QDir::tempPath() + QString("/ctkDICOMVisualBrowserWidgetTest1-db"));
  QString dbDir = tempFileInfo.absoluteFilePath();
  qDebug().noquote() << "\n\n"
                     << testName << ": Using directory: " << dbDir;
  if (tempFileInfo.exists())
  {
    qDebug().noquote() << "\n\n"
                       << testName << ": Removing directory: " << dbDir;
    ctk::removeDirRecursively(dbDir);
  }
  qDebug().noquote() << "\n\n"
                     << testName << ": Making directory: " << dbDir;
  QDir dir(dbDir);
  dir.mkdir(dbDir);

  browser.setDatabaseDirectory(dbDir);
  browser.show();

  qDebug().noquote() << testName << ": Importing directory " << dicomDirectory;

  // Test import of a few specific files
  QDirIterator it(dicomDirectory, QStringList() << "*.IMA", QDir::Files, QDirIterator::Subdirectories);
  // Skip a few files
  it.next();
  it.next();
  // Add 3 files
  QStringList files;
  files << it.next();
  files << it.next();
  files << it.next();
  browser.importFiles(files);
  browser.waitForImportFinished();

  qDebug().noquote() << testName << ":"
                     << " " << browser.patientsAddedDuringImport()
                     << " " << browser.studiesAddedDuringImport()
                     << " " << browser.seriesAddedDuringImport()
                     << " " << browser.instancesAddedDuringImport();

  CHECK_INT(browser.patientsAddedDuringImport(), 1);
  CHECK_INT(browser.studiesAddedDuringImport(), 1);
  CHECK_INT(browser.seriesAddedDuringImport(), 1);
  CHECK_INT(browser.instancesAddedDuringImport(), 3);

  qDebug().noquote() << "\n\n"
                   << testName << ": Added to database directory: " << files;

  browser.importDirectories(QStringList() << argv[1]);
  browser.waitForImportFinished();

  CHECK_INT(browser.patientsAddedDuringImport(), 0);
  CHECK_INT(browser.studiesAddedDuringImport(), 0);
  CHECK_INT(browser.seriesAddedDuringImport(), 0);
  CHECK_INT(browser.instancesAddedDuringImport(), 97);

  qDebug().noquote() << "\n\n"
                     << testName << ": Added to database directory: " << dbDir;

  // Test setting and getting
  browser.setStorageAETitle("storage");
  CHECK_QSTRING(browser.storageAETitle(), "storage");
  browser.setStoragePort(2014);
  CHECK_INT(browser.storagePort(), 2014);
  browser.setFilteringPatientID("123456");
  CHECK_QSTRING(browser.filteringPatientID(), "123456");
  browser.setFilteringPatientName("Name");
  CHECK_QSTRING(browser.filteringPatientName(), "Name");
  browser.setFilteringStudyDescription("StudyDescription");
  CHECK_QSTRING(browser.filteringStudyDescription(), "StudyDescription");
  browser.setFilteringSeriesDescription("SeriesDescription");
  CHECK_QSTRING(browser.filteringSeriesDescription(), "SeriesDescription");
  QStringList filteringModalities = {"CT"};
  browser.setFilteringModalities(filteringModalities);
  CHECK_QSTRING(browser.filteringModalities().at(0), "CT");
  browser.setFilteringDate(ctkDICOMPatientItemWidget::DateType::LastYear);
  CHECK_INT(browser.filteringDate(), ctkDICOMPatientItemWidget::DateType::LastYear);
  browser.setNumberOfStudiesPerPatient(6);
  CHECK_INT(browser.numberOfStudiesPerPatient(), 6);
  browser.setThumbnailSize(ctkDICOMStudyItemWidget::ThumbnailSizeOption::Small);
  CHECK_INT(browser.thumbnailSize(), ctkDICOMStudyItemWidget::ThumbnailSizeOption::Small);
  browser.setSendActionVisible(true);
  CHECK_BOOL(browser.isSendActionVisible(), true);
  browser.setDeleteActionVisible(false);
  CHECK_BOOL(browser.isDeleteActionVisible(), false);

  if (!interactive)
  {
    QTimer::singleShot(200, &app, SLOT(quit()));
  }

  return app.exec();
}
