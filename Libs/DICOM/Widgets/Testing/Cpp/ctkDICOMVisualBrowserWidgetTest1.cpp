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
  and development was supported by the Program for Intelligent Image-Guided Interventions (PI3).

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QCheckBox>
#include <QGridLayout>
#include <QDir>
#include <QDirIterator>
#include <QSettings>
#include <QTimer>

// ctkCore includes
#include <ctkCoreTestingMacros.h>
#include <ctkUtils.h>

// ctkWidgets includes
#include <ctkCheckBox.h>
#include <ctkCollapsibleGroupBox.h>

// ctkDICOMWidget includes
#include "ctkDICOMServerNodeWidget2.h"
#include "ctkDICOMVisualBrowserWidget.h"

namespace
{

/// Return the name of two widgets of the layout that are drawn on top of each other,
/// or an empty string when every widget has its own cells.
QString overlappingWidgets(QGridLayout* layout)
{
  if (!layout)
  {
    return QString();
  }

  for (int i = 0; i < layout->count(); ++i)
  {
    int row = 0, column = 0, rowSpan = 0, columnSpan = 0;
    layout->getItemPosition(i, &row, &column, &rowSpan, &columnSpan);
    const QRect cellsOfI(column, row, columnSpan, rowSpan);
    for (int j = i + 1; j < layout->count(); ++j)
    {
      int otherRow = 0, otherColumn = 0, otherRowSpan = 0, otherColumnSpan = 0;
      layout->getItemPosition(j, &otherRow, &otherColumn, &otherRowSpan, &otherColumnSpan);
      const QRect cellsOfJ(otherColumn, otherRow, otherColumnSpan, otherRowSpan);
      if (cellsOfI.intersects(cellsOfJ))
      {
        QWidget* widgetOfI = layout->itemAt(i)->widget();
        QWidget* widgetOfJ = layout->itemAt(j)->widget();
        return QString("%1 and %2").arg(widgetOfI ? widgetOfI->objectName() : QString("item"),
                                        widgetOfJ ? widgetOfJ->objectName() : QString("item"));
      }
    }
  }

  return QString();
}

} // end of anonymous namespace

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

  // The browser persists some of its preferences in QSettings: keep the test out of
  // the settings of the user running it, and make the defaults below deterministic.
  QCoreApplication::setOrganizationName("CTK");
  QCoreApplication::setApplicationName("ctkDICOMVisualBrowserWidgetTest1");
  QSettings().clear();

  ctkDICOMVisualBrowserWidget browser;

  // Test the default values
  CHECK_QSTRING(browser.storageAETitle(), "CTKSTORE");
  CHECK_INT(browser.storagePort(), 11112);
  CHECK_QSTRING(browser.filteringPatientID(), "");
  CHECK_QSTRING(browser.filteringPatientName(), "");
  CHECK_QSTRING(browser.filteringStudyDescription(), "");
  CHECK_QSTRING(browser.filteringSeriesDescription(), "");
  CHECK_QSTRING(browser.filteringModalities().at(0), "Any");
  CHECK_INT(browser.filteringDate(), ctkDICOMVisualBrowserWidget::DateType::Any);
  CHECK_INT(browser.numberOfOpenedStudiesPerPatient(), 2);
  CHECK_INT(browser.thumbnailSizePreset(), ctkDICOMVisualBrowserWidget::ThumbnailSizePresetOption::Small);
  CHECK_BOOL(browser.isSendActionVisible(), false);
  CHECK_BOOL(browser.isDeleteActionVisible(), true);
  // All the frames of a series are retrieved as soon as its thumbnail is shown
  CHECK_BOOL(browser.autoRetrieveFullSeries(), true);

  // Test the automatic prefetch of the full series: the check box follows the
  // property, and the preference is remembered by the next browser
  QCheckBox* autoRetrieveFullSeriesCheckBox =
    browser.findChild<QCheckBox*>("AutoRetrieveFullSeriesCheckBox");
  CHECK_NOT_NULL(autoRetrieveFullSeriesCheckBox);
  CHECK_BOOL(autoRetrieveFullSeriesCheckBox->isChecked(), true);

  browser.setAutoRetrieveFullSeries(false);
  CHECK_BOOL(browser.autoRetrieveFullSeries(), false);
  CHECK_BOOL(autoRetrieveFullSeriesCheckBox->isChecked(), false);

  browser.setAutoRetrieveFullSeries(false);
  {
    ctkDICOMVisualBrowserWidget browserWithSavedSettings;
    CHECK_BOOL(browserWithSavedSettings.autoRetrieveFullSeries(), false);
  }
  browser.setAutoRetrieveFullSeries(true);

  // Test the send action of the context menus: hidden by default, and the check box
  // and the property follow each other, since the application can set it as well
  CHECK_BOOL(browser.isSendActionVisible(), false);
  QCheckBox* sendActionVisibleCheckBox =
    browser.findChild<QCheckBox*>("SendActionVisibleCheckBox");
  CHECK_NOT_NULL(sendActionVisibleCheckBox);
  CHECK_BOOL(sendActionVisibleCheckBox->isChecked(), false);

  browser.setSendActionVisible(true);
  CHECK_BOOL(sendActionVisibleCheckBox->isChecked(), true);
  browser.setSendActionVisible(false);
  CHECK_BOOL(sendActionVisibleCheckBox->isChecked(), false);

  // The operations share the Apply and Discard buttons of the server settings: the
  // check boxes are pending changes, like the settings they sit with, and the browser
  // follows them only once they are applied.
  ctkDICOMServerNodeWidget2* serverNodeWidget = browser.findChild<ctkDICOMServerNodeWidget2*>();
  CHECK_NOT_NULL(serverNodeWidget);

  autoRetrieveFullSeriesCheckBox->setChecked(false);
  sendActionVisibleCheckBox->setChecked(true);
  CHECK_BOOL(browser.autoRetrieveFullSeries(), true);
  CHECK_BOOL(browser.isSendActionVisible(), false);

  serverNodeWidget->saveSettings();
  CHECK_BOOL(browser.autoRetrieveFullSeries(), false);
  CHECK_BOOL(browser.isSendActionVisible(), true);

  // Discarding puts the check boxes back to what the browser is doing
  autoRetrieveFullSeriesCheckBox->setChecked(true);
  sendActionVisibleCheckBox->setChecked(false);
  serverNodeWidget->readSettings();
  CHECK_BOOL(autoRetrieveFullSeriesCheckBox->isChecked(), false);
  CHECK_BOOL(sendActionVisibleCheckBox->isChecked(), true);
  CHECK_BOOL(browser.autoRetrieveFullSeries(), false);
  CHECK_BOOL(browser.isSendActionVisible(), true);

  // Back to the defaults for the rest of the test
  browser.setAutoRetrieveFullSeries(true);
  browser.setSendActionVisible(false);

  // The operations of the browser and the storage settings are shown as one section,
  // which follows the list of servers
  CHECK_NULL(browser.findChild<ctkCollapsibleGroupBox*>("OperationsCollapsibleGroupBox"));
  ctkCollapsibleGroupBox* operationsGroupBox =
    browser.findChild<ctkCollapsibleGroupBox*>("StorageCollapsibleGroupBox");
  CHECK_NOT_NULL(operationsGroupBox);
  CHECK_QSTRING(operationsGroupBox->title(), "Operations");
  CHECK_BOOL(operationsGroupBox->isAncestorOf(autoRetrieveFullSeriesCheckBox), true);
  CHECK_BOOL(operationsGroupBox->isAncestorOf(sendActionVisibleCheckBox), true);

  // The check box says what it enables, instead of being labelled "Enable:"
  ctkCheckBox* storageEnabledCheckBox = browser.findChild<ctkCheckBox*>("StorageEnabledCheckBox");
  CHECK_NOT_NULL(storageEnabledCheckBox);
  CHECK_QSTRING(storageEnabledCheckBox->text(), "Storage");
  CHECK_NULL(browser.findChild<QWidget*>("StorageEnabledLabel"));

  // The operations and the storage settings are laid out one above the other as soon
  // as the browser is built, and not only once it is resized
  QGridLayout* operationsLayout = qobject_cast<QGridLayout*>(operationsGroupBox->layout());
  CHECK_NOT_NULL(operationsLayout);
  int prefetchRow = -1;
  int sendRow = -1;
  int storageRow = -1;
  int column = -1;
  int rowSpan = -1;
  int columnSpan = -1;
  const int prefetchIndex = operationsLayout->indexOf(autoRetrieveFullSeriesCheckBox);
  const int sendIndex = operationsLayout->indexOf(sendActionVisibleCheckBox);
  const int storageIndex = operationsLayout->indexOf(storageEnabledCheckBox);
  CHECK_BOOL(prefetchIndex >= 0 && sendIndex >= 0 && storageIndex >= 0, true);
  operationsLayout->getItemPosition(prefetchIndex, &prefetchRow, &column, &rowSpan, &columnSpan);
  operationsLayout->getItemPosition(sendIndex, &sendRow, &column, &rowSpan, &columnSpan);
  operationsLayout->getItemPosition(storageIndex, &storageRow, &column, &rowSpan, &columnSpan);
  CHECK_BOOL(prefetchRow < sendRow, true);
  CHECK_BOOL(sendRow < storageRow, true);

  // and they stay that way once the browser is shown and resized, in both the wide
  // and the narrow arrangement
  browser.show();
  const QList<QSize> sizesToCheck = QList<QSize>()
    << QSize(1200, 800)   // wide: the storage settings go on one row
    << QSize(600, 800);   // narrow: they are stacked
  foreach (const QSize& size, sizesToCheck)
  {
    browser.resize(size);
    QCoreApplication::processEvents();

    operationsLayout->getItemPosition(operationsLayout->indexOf(autoRetrieveFullSeriesCheckBox),
                                      &prefetchRow, &column, &rowSpan, &columnSpan);
    operationsLayout->getItemPosition(operationsLayout->indexOf(sendActionVisibleCheckBox),
                                      &sendRow, &column, &rowSpan, &columnSpan);
    operationsLayout->getItemPosition(operationsLayout->indexOf(storageEnabledCheckBox),
                                      &storageRow, &column, &rowSpan, &columnSpan);
    if (prefetchRow >= sendRow || sendRow >= storageRow)
    {
      std::cerr << "Line " << __LINE__ << " - the operations overlap the storage"
                << " settings at " << size.width() << "x" << size.height()
                << ": rows " << prefetchRow << ", " << sendRow << ", " << storageRow
                << std::endl;
      return EXIT_FAILURE;
    }

    // The buttons of the actions are rearranged with the width as well, and a button
    // left out of the rearrangement is drawn on top of another one
    QWidget* actionsGroupBox = browser.findChild<QWidget*>("ActionsCollapsibleGroupBox");
    CHECK_NOT_NULL(actionsGroupBox);
    const QString overlapping = overlappingWidgets(qobject_cast<QGridLayout*>(actionsGroupBox->layout()));
    if (!overlapping.isEmpty())
    {
      std::cerr << "Line " << __LINE__ << " - the actions overlap at "
                << size.width() << "x" << size.height() << ": "
                << qPrintable(overlapping) << std::endl;
      return EXIT_FAILURE;
    }
  }

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
  browser.setFilteringDate(ctkDICOMVisualBrowserWidget::DateType::LastYear);
  CHECK_INT(browser.filteringDate(), ctkDICOMVisualBrowserWidget::DateType::LastYear);
  browser.setNumberOfOpenedStudiesPerPatient(6);
  CHECK_INT(browser.numberOfOpenedStudiesPerPatient(), 6);
  browser.setThumbnailSizePreset(ctkDICOMVisualBrowserWidget::ThumbnailSizePresetOption::Small);
  CHECK_INT(browser.thumbnailSizePreset(), ctkDICOMVisualBrowserWidget::ThumbnailSizePresetOption::Small);
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
