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

// Regression tests of the interactions with the visual DICOM browser: what is painted
// and what receives the clicks, with a patient that has hundreds of studies and with
// several patients shown as tabs.

// Qt includes
#include <QApplication>
#include <QDate>
#include <QDir>
#include <QElapsedTimer>
#include <QImage>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollBar>
#include <QDateTime>
#include <QSettings>
#include <QSqlQuery>
#include <QSignalSpy>
#include <QTest>
#include <QTimer>
#include <QWheelEvent>
#include <QWindow>

// ctkCore includes
#include <ctkCoreTestingMacros.h>
#include <ctkUtils.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMPatientModel.h"
#include "ctkDICOMSeriesModel.h"
#include "ctkDICOMServer.h"

// ctkDICOMWidgets includes
#include "ctkDICOMPatientDelegate.h"
#include "ctkDICOMPatientView.h"
#include "ctkDICOMSeriesTableView.h"
#include "ctkDICOMStudyListView.h"
#include "ctkDICOMVisualBrowserWidget.h"

// DCMTK includes
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcuid.h>

// STD includes
#include <iostream>

namespace
{

//------------------------------------------------------------------------------
/// Insert studies of one series and one instance, without files: enough for the
/// browser to show them.
void insertStudies(ctkDICOMDatabase* database, int patientNumber, const QString& patientName,
                   int numberOfStudies)
{
  const QString patientID = QString("PATIENT%1").arg(patientNumber);
  for (int study = 0; study < numberOfStudies; ++study)
  {
    const QString studyUID = QString("1.2.826.0.1.3680043.10.1.%1.%2").arg(patientNumber).arg(study + 1);
    DcmDataset dataset;
    dataset.putAndInsertString(DCM_PatientName, patientName.toLatin1().constData());
    dataset.putAndInsertString(DCM_PatientID, patientID.toLatin1().constData());
    dataset.putAndInsertString(DCM_PatientBirthDate, "19700101");
    dataset.putAndInsertString(DCM_StudyInstanceUID, studyUID.toLatin1().constData());
    dataset.putAndInsertString(DCM_StudyDate,
      QDate(2000, 1, 1).addDays(study).toString("yyyyMMdd").toLatin1().constData());
    dataset.putAndInsertString(DCM_StudyTime, "120000");
    dataset.putAndInsertString(DCM_StudyID, QString::number(study + 1).toLatin1().constData());
    dataset.putAndInsertString(DCM_StudyDescription,
      QString("Study %1").arg(study + 1).toLatin1().constData());
    dataset.putAndInsertString(DCM_SeriesInstanceUID, (studyUID + ".1").toLatin1().constData());
    dataset.putAndInsertString(DCM_SeriesNumber, "1");
    dataset.putAndInsertString(DCM_SeriesDescription, "Series");
    dataset.putAndInsertString(DCM_Modality, "MR");
    dataset.putAndInsertString(DCM_SOPClassUID, UID_MRImageStorage);
    dataset.putAndInsertString(DCM_SOPInstanceUID, (studyUID + ".1.1").toLatin1().constData());
    dataset.putAndInsertString(DCM_InstanceNumber, "1");
    database->insert(&dataset, /*storeFile=*/false, /*generateThumbnail=*/false);
  }
}

//------------------------------------------------------------------------------
/// Add an instance found by a query of the series: it has no file until it is retrieved
bool insertInstanceWithoutFile(ctkDICOMDatabase* database, const QString& seriesInstanceUID,
                               const QString& sopInstanceUID)
{
  QSqlQuery insertImage(database->database());
  insertImage.prepare("INSERT INTO Images ( 'SOPInstanceUID', 'Filename', 'URL', 'SeriesInstanceUID', 'InsertTimestamp' ) VALUES ( ?, ?, ?, ?, ? )");
  insertImage.addBindValue(sopInstanceUID);
  insertImage.addBindValue(QString(""));
  insertImage.addBindValue(QString("dimse://Unreachable/") + sopInstanceUID);
  insertImage.addBindValue(seriesInstanceUID);
  insertImage.addBindValue(QDateTime::currentDateTime());
  return insertImage.exec();
}

//------------------------------------------------------------------------------
/// The warnings logged by the browser, which ctkLogger prints with qWarning()
QStringList LoggedWarnings;
QtMessageHandler PreviousMessageHandler = nullptr;
void recordWarnings(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
  if (type == QtWarningMsg)
  {
    LoggedWarnings.append(message);
  }
  if (PreviousMessageHandler)
  {
    PreviousMessageHandler(type, context, message);
  }
}

//------------------------------------------------------------------------------
/// Load the series like a click on Load does, and return the warning that was logged
QString loadSeriesAndLogWarning(ctkDICOMVisualBrowserWidget& browser, const QStringList& seriesInstanceUIDs)
{
  LoggedWarnings.clear();
  QMetaObject::invokeMethod(&browser, "onLoadSeries", Qt::DirectConnection,
                            Q_ARG(QStringList, seriesInstanceUIDs));
  QStringList loadWarnings = LoggedWarnings.filter("loaded");
  return loadWarnings.isEmpty() ? QString() : loadWarnings.last();
}

//------------------------------------------------------------------------------
void processEvents(int msec = 100)
{
  QTest::qWait(msec);
  QApplication::processEvents();
}

//------------------------------------------------------------------------------
/// Collect the regions repainted in a widget
class PaintedRegionRecorder : public QObject
{
public:
  QRegion PaintedRegion;
  bool eventFilter(QObject* object, QEvent* event) override
  {
    if (event->type() == QEvent::Paint)
    {
      this->PaintedRegion += static_cast<QPaintEvent*>(event)->region();
    }
    return QObject::eventFilter(object, event);
  }
};

//------------------------------------------------------------------------------
/// Move the mouse like a user does, through the window, so that the widgets receive
/// the enter, leave and hover events as well.
void moveMouse(QWidget* widget, const QPoint& pos)
{
  QWidget* window = widget->window();
  QTest::mouseMove(window->windowHandle(), widget->mapTo(window, pos));
  processEvents(50);
}

//------------------------------------------------------------------------------
void clickMouse(QWidget* widget, const QPoint& pos)
{
  QWidget* window = widget->window();
  QTest::mouseClick(window->windowHandle(), Qt::LeftButton, Qt::NoModifier, widget->mapTo(window, pos));
  processEvents(50);
}

//------------------------------------------------------------------------------
QRect tabRect(ctkDICOMPatientView* patientView, const QModelIndex& index)
{
  ctkDICOMPatientDelegate* delegate = qobject_cast<ctkDICOMPatientDelegate*>(patientView->itemDelegate());
  QStyleOptionViewItem option;
  option.initFrom(patientView);
  option.widget = patientView;
  option.font = patientView->font();
  option.rect = patientView->visualRect(index);
  return delegate->patientsRect(option.rect, option, index);
}

} // end of anonymous namespace

//------------------------------------------------------------------------------
int ctkDICOMVisualBrowserWidgetTest2(int argc, char* argv[])
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

  // Keep the test out of the settings of the user running it
  QCoreApplication::setOrganizationName("CTK");
  QCoreApplication::setApplicationName("ctkDICOMVisualBrowserWidgetTest2");
  QSettings().clear();

  QString dbDir = QDir::tempPath() + "/ctkDICOMVisualBrowserWidgetTest2-db";
  if (QFileInfo::exists(dbDir))
  {
    ctk::removeDirRecursively(dbDir);
  }
  QDir().mkpath(dbDir);

  ctkDICOMVisualBrowserWidget browser;
  browser.setDatabaseDirectory(dbDir);
  browser.setPatientLayoutMode(ctkDICOMVisualBrowserWidget::PatientLayoutTabs);
  browser.resize(1200, 900);
  browser.show();
  CHECK_BOOL(QTest::qWaitForWindowExposed(&browser), true);

  // A patient with more studies than the list view lays out in one batch, a few
  // patients with a couple of studies, and a series with files that can be loaded
  ctkDICOMDatabase* database = browser.dicomDatabase();
  CHECK_NOT_NULL(database);
  insertStudies(database, 1, "Many^Studies", 150);
  for (int patient = 2; patient <= 4; ++patient)
  {
    insertStudies(database, patient, QString("Patient^%1").arg(patient), 2);
  }
  browser.importDirectories(QStringList() << dicomDirectory);
  browser.waitForImportFinished();
  browser.refreshBrowser();
  processEvents(500);

  ctkDICOMPatientView* patientView = browser.patientView();
  CHECK_NOT_NULL(patientView);
  CHECK_INT(patientView->displayMode(), ctkDICOMPatientView::TabMode);
  QAbstractItemModel* patientModel = patientView->model();
  CHECK_INT(patientModel->rowCount(), 5);
  ctkDICOMStudyListView* studyListView = patientView->studyListView();
  CHECK_NOT_NULL(studyListView);

  QString manyStudiesPatientUID;
  foreach (const QString& patientUID, database->patients())
  {
    if (database->studiesForPatient(patientUID).count() == 150)
    {
      manyStudiesPatientUID = patientUID;
    }
  }
  CHECK_BOOL(manyStudiesPatientUID.isEmpty(), false);

  //
  // The studies are still painted when their layout is requested again while the
  // previous one is in progress: a batched layout painted none of them, which made
  // the list flicker while a patient with hundreds of studies was queried.
  //
  patientView->selectPatientUID(manyStudiesPatientUID);
  processEvents(500);
  CHECK_INT(studyListView->model()->rowCount(), 150);

  QImage studiesBefore = studyListView->viewport()->grab().toImage();
  studyListView->doItemsLayout();
  studyListView->doItemsLayout();
  QImage studiesDuringLayout = studyListView->viewport()->grab().toImage();
  if (studiesDuringLayout != studiesBefore)
  {
    std::cerr << "Line " << __LINE__ << " - the studies are not painted while they are laid out again"
              << std::endl;
    return EXIT_FAILURE;
  }

  //
  // The load button gets the click even where a series view lies below it: the series
  // views are raised each time the studies are laid out, and the button, still drawn
  // on top of the transparent series views, ignored the clicks.
  //
  // Open the studies, so that their series cover the bottom of the list
  studyListView->expandAllStudies();
  processEvents(500);
  ctkDICOMSeriesTableView* seriesView = nullptr;
  foreach (ctkDICOMSeriesTableView* view, studyListView->getAllSeriesView())
  {
    if (view->isVisible() && view->model() && view->model()->rowCount() > 0)
    {
      seriesView = view;
      break;
    }
  }
  CHECK_NOT_NULL(seriesView);
  QString selectedSeriesInstanceUID = seriesView->model()->index(0, 0).data(ctkDICOMSeriesModel::SeriesInstanceUIDRole).toString();
  CHECK_BOOL(selectedSeriesInstanceUID.isEmpty(), false);
  studyListView->selectSeriesInstanceUIDs(QStringList() << selectedSeriesInstanceUID);
  processEvents();

  QPushButton* loadButton = studyListView->findChild<QPushButton*>("LoadSeriesButton");
  CHECK_NOT_NULL(loadButton);
  CHECK_BOOL(loadButton->isVisible(), true);

  QWidget* viewport = studyListView->viewport();
  auto isSeriesViewBelowLoadButton = [&]()
  {
    foreach (ctkDICOMSeriesTableView* view, studyListView->getAllSeriesView())
    {
      if (view->isVisible() && view->geometry().contains(loadButton->geometry().center()))
      {
        return true;
      }
    }
    return false;
  };
  // Scroll the studies until series lie below the button: where it falls depends on
  // the size of the list, which can show the header of a study or the gap between
  // two studies there.
  QScrollBar* studiesScrollBar = studyListView->verticalScrollBar();
  for (int value = studiesScrollBar->minimum();
       !isSeriesViewBelowLoadButton() && value <= studiesScrollBar->maximum(); value += 10)
  {
    studiesScrollBar->setValue(value);
    processEvents(20);
  }

  // Lay out the studies again, as when a thumbnail or the status of a job changes
  studyListView->refreshLayout();
  processEvents();

  QPoint loadButtonCenter = loadButton->geometry().center();
  // otherwise this part of the test would not test anything
  CHECK_BOOL(isSeriesViewBelowLoadButton(), true);
  CHECK_POINTER(viewport->childAt(loadButtonCenter), loadButton);

  QSignalSpy loadSeriesRequestedSpy(studyListView, SIGNAL(loadSeriesRequested(QStringList)));
  // The browser loads the series it is asked to: the signal is enough here
  QObject::disconnect(studyListView, SIGNAL(loadSeriesRequested(QStringList)), &browser, nullptr);
  clickMouse(viewport, loadButtonCenter);
  CHECK_INT(loadSeriesRequestedSpy.count(), 1);
  CHECK_QSTRING(loadSeriesRequestedSpy.at(0).at(0).toStringList().join(","), selectedSeriesInstanceUID);
  studyListView->clearSelection();

  //
  // Patient tabs
  //
  QWidget* patientViewport = patientView->viewport();
  QModelIndex firstPatientIndex = patientModel->index(0, 0);
  QModelIndex secondPatientIndex = patientModel->index(1, 0);
  QModelIndex lastPatientIndex = patientModel->index(patientModel->rowCount() - 1, 0);
  patientView->setCurrentIndex(firstPatientIndex);
  processEvents(300);
  QRect secondTabRect = tabRect(patientView, secondPatientIndex);
  QRect lastTabRect = tabRect(patientView, lastPatientIndex);

  // The item of a patient spans the column below its tab, but the patient is hovered
  // only over its tab
  moveMouse(patientViewport, QPoint(secondTabRect.center().x(), 1));
  CHECK_BOOL(patientView->isIndexHovered(secondPatientIndex), false);
  moveMouse(patientViewport, QPoint(1, tabRect(patientView, firstPatientIndex).center().y()));
  CHECK_BOOL(patientView->isIndexHovered(firstPatientIndex), false);

  // A tab stops being painted as hovered whatever the direction the mouse leaves it:
  // moving from a tab to the empty area right of the tabs left it hovered.
  PaintedRegionRecorder paintedRegionRecorder;
  patientViewport->installEventFilter(&paintedRegionRecorder);
  moveMouse(patientViewport, secondTabRect.center());
  CHECK_BOOL(patientView->isIndexHovered(secondPatientIndex), true);
  paintedRegionRecorder.PaintedRegion = QRegion();
  moveMouse(patientViewport, QPoint(lastTabRect.right() + 40, secondTabRect.center().y()));
  CHECK_BOOL(patientView->isIndexHovered(secondPatientIndex), false);
  CHECK_BOOL(paintedRegionRecorder.PaintedRegion.contains(secondTabRect), true);
  patientViewport->removeEventFilter(&paintedRegionRecorder);

  // The header of the current patient is painted across the columns of the other
  // patients: repainting only the column of another patient (as Qt does when the
  // hover or the status of a patient changes) erased a strip of the header.
  QRect secondPatientColumn = patientView->visualRect(secondPatientIndex);
  CHECK_BOOL(secondPatientColumn.intersects(patientView->visualRect(firstPatientIndex)), false);
  QImage fullyPainted(patientViewport->size(), QImage::Format_ARGB32);
  fullyPainted.fill(Qt::magenta);
  patientViewport->render(&fullyPainted);
  QImage partiallyPainted(patientViewport->size(), QImage::Format_ARGB32);
  partiallyPainted.fill(Qt::magenta);
  // only the region is repainted, like with QWidget::update(rect)
  patientViewport->render(&partiallyPainted, secondPatientColumn.topLeft(), QRegion(secondPatientColumn));
  int differentPixels = 0;
  QRect comparedRect = secondPatientColumn.intersected(patientViewport->rect());
  for (int y = comparedRect.top(); y <= comparedRect.bottom(); ++y)
  {
    for (int x = comparedRect.left(); x <= comparedRect.right(); ++x)
    {
      if (fullyPainted.pixel(x, y) != partiallyPainted.pixel(x, y))
      {
        ++differentPixels;
      }
    }
  }
  if (differentPixels != 0)
  {
    std::cerr << "Line " << __LINE__ << " - repainting the column of another patient paints "
              << differentPixels << " pixels differently" << std::endl;
    return EXIT_FAILURE;
  }

  // A click next to a tab does not select its patient
  clickMouse(patientViewport, QPoint(secondTabRect.center().x(), 1));
  CHECK_BOOL(patientView->currentIndex() == firstPatientIndex, true);
  clickMouse(patientViewport, secondTabRect.center());
  CHECK_BOOL(patientView->currentIndex() == secondPatientIndex, true);

  //
  // Load
  //
  QString loadableSeriesInstanceUID;
  foreach (const QString& patientUID, database->patients())
  {
    foreach (const QString& studyInstanceUID, database->studiesForPatient(patientUID))
    {
      foreach (const QString& seriesInstanceUID, database->seriesForStudy(studyInstanceUID))
      {
        if (database->filesForSeries(seriesInstanceUID).count() > 1)
        {
          loadableSeriesInstanceUID = seriesInstanceUID;
        }
      }
    }
  }
  CHECK_BOOL(loadableSeriesInstanceUID.isEmpty(), false);

  // A series found by a query, whose images have not been retrieved yet
  const QString missingSeriesInstanceUID("1.2.826.0.1.3680043.10.1.2.1.1");
  CHECK_BOOL(insertInstanceWithoutFile(database, missingSeriesInstanceUID, missingSeriesInstanceUID + ".1"), true);
  CHECK_INT(database->instancesForSeries(missingSeriesInstanceUID).count(), 1);
  CHECK_QSTRING(database->filesForSeries(missingSeriesInstanceUID).join(""), "");

  PreviousMessageHandler = qInstallMessageHandler(recordWarnings);
  QPushButton* warningButton = browser.findChild<QPushButton*>("WarningPushButton");
  CHECK_NOT_NULL(warningButton);
  QSignalSpy seriesRetrievedSpy(&browser, SIGNAL(seriesRetrieved(QStringList)));

  // A server that does not answer
  ctkDICOMServer* unreachableServer = new ctkDICOMServer(&browser);
  unreachableServer->setConnectionName("Unreachable");
  unreachableServer->setCallingAETitle("CTK");
  unreachableServer->setCalledAETitle("UNREACHABLE");
  unreachableServer->setHost("127.0.0.1");
  unreachableServer->setPort(1);
  unreachableServer->setQueryRetrieveEnabled(true);
  unreachableServer->setTrustedEnabled(true);
  unreachableServer->setConnectionTimeout(1);
  unreachableServer->setMaximumRetryWait(0);
  browser.addServer(unreachableServer);

  // The load of a series that has to be retrieved gives up once the retrieve failed,
  // instead of waiting for the series forever. A click on Load meanwhile does not
  // start another load: the events are processed while the series are retrieved, and
  // the nested load handed series over to the application in the middle of the first.
  warningButton->hide();
  QTimer::singleShot(0, &browser, [&browser, loadableSeriesInstanceUID]()
    {
      QMetaObject::invokeMethod(&browser, "onLoadSeries", Qt::DirectConnection,
                                Q_ARG(QStringList, QStringList() << loadableSeriesInstanceUID));
    });
  QElapsedTimer unreachableServerTimer;
  unreachableServerTimer.start();
  QString loggedWarning = loadSeriesAndLogWarning(browser, QStringList() << missingSeriesInstanceUID);
  CHECK_BOOL(unreachableServerTimer.elapsed() < 30000, true);
  CHECK_INT(seriesRetrievedSpy.count(), 0);
  CHECK_BOOL(warningButton->isVisible(), true);
  CHECK_QSTRING(warningButton->text(),
    "Nothing has been loaded: 1 series could not be retrieved.\n"
    "Verify that the servers are reachable, then load them again.");
  // the log has the details: which series, and why
  CHECK_BOOL(loggedWarning.startsWith(warningButton->text()), true);
  CHECK_BOOL(loggedWarning.contains("Series not retrieved: " + missingSeriesInstanceUID), true);
  CHECK_BOOL(loggedWarning.contains("without an allowed server"), false);
  browser.removeServer("Unreachable");
  processEvents();

  // A series in the database is loaded right away
  QMetaObject::invokeMethod(&browser, "onLoadSeries", Qt::DirectConnection,
                            Q_ARG(QStringList, QStringList() << loadableSeriesInstanceUID));
  CHECK_INT(seriesRetrievedSpy.count(), 1);
  CHECK_QSTRING(seriesRetrievedSpy.at(0).at(0).toStringList().join(","), loadableSeriesInstanceUID);

  // A click on Load while the application loads the series it was handed (and
  // processes the events, e.g. to show its own progress) does not load them again
  bool loadRequestedDuringApplicationLoad = false;
  QMetaObject::Connection applicationLoad = QObject::connect(
    &browser, &ctkDICOMVisualBrowserWidget::seriesRetrieved,
    [&browser, &loadRequestedDuringApplicationLoad, loadableSeriesInstanceUID]()
    {
      if (loadRequestedDuringApplicationLoad)
      {
        return;
      }
      loadRequestedDuringApplicationLoad = true;
      QMetaObject::invokeMethod(&browser, "onLoadSeries", Qt::DirectConnection,
                                Q_ARG(QStringList, QStringList() << loadableSeriesInstanceUID));
    });
  QMetaObject::invokeMethod(&browser, "onLoadSeries", Qt::DirectConnection,
                            Q_ARG(QStringList, QStringList() << loadableSeriesInstanceUID));
  processEvents();
  QObject::disconnect(applicationLoad);
  CHECK_BOOL(loadRequestedDuringApplicationLoad, true);
  CHECK_INT(seriesRetrievedSpy.count(), 2);

  // No server can retrieve the missing series anymore
  warningButton->hide();
  loggedWarning = loadSeriesAndLogWarning(browser, QStringList() << missingSeriesInstanceUID);
  CHECK_INT(seriesRetrievedSpy.count(), 2);
  CHECK_BOOL(warningButton->isVisible(), true);
  CHECK_QSTRING(warningButton->text(),
    "Nothing has been loaded: 1 series could not be retrieved.\n"
    "No server allowed for the patient can retrieve them: verify the allowed servers of the patient.");
  CHECK_BOOL(loggedWarning.contains("Series not retrieved: " + missingSeriesInstanceUID), true);
  CHECK_BOOL(loggedWarning.contains("Series without an allowed server: " + missingSeriesInstanceUID), true);

  // A complete series and a missing one: the complete one is loaded
  warningButton->hide();
  loggedWarning = loadSeriesAndLogWarning(browser,
    QStringList() << loadableSeriesInstanceUID << missingSeriesInstanceUID);
  CHECK_INT(seriesRetrievedSpy.count(), 3);
  CHECK_QSTRING(seriesRetrievedSpy.at(2).at(0).toStringList().join(","), loadableSeriesInstanceUID);
  CHECK_QSTRING(warningButton->text(),
    "1 of the 2 selected series have not been loaded: 1 series could not be retrieved.\n"
    "No server allowed for the patient can retrieve them: verify the allowed servers of the patient.");
  CHECK_BOOL(loggedWarning.contains(loadableSeriesInstanceUID), false);

  // A series with an image that could not be retrieved is not loaded: it would be
  // loaded with a hole, or as the single image of its thumbnail
  const int numberOfFiles = database->filesForSeries(loadableSeriesInstanceUID).count();
  CHECK_BOOL(insertInstanceWithoutFile(database, loadableSeriesInstanceUID, "1.2.826.0.1.3680043.10.4.1"), true);
  CHECK_INT(database->instancesForSeries(loadableSeriesInstanceUID).count(), numberOfFiles + 1);

  warningButton->hide();
  loggedWarning = loadSeriesAndLogWarning(browser, QStringList() << loadableSeriesInstanceUID);
  CHECK_INT(seriesRetrievedSpy.count(), 3);
  CHECK_BOOL(warningButton->isVisible(), true);
  CHECK_QSTRING(warningButton->text(),
    QString("Nothing has been loaded: 1 series could only be partially retrieved (%1 of %2 images).\n"
            "No server allowed for the patient can retrieve them: verify the allowed servers of the patient.")
      .arg(numberOfFiles).arg(numberOfFiles + 1));
  CHECK_BOOL(loggedWarning.contains(QString("Series partially retrieved: %1 (%2 of %3 images)")
    .arg(loadableSeriesInstanceUID).arg(numberOfFiles).arg(numberOfFiles + 1)), true);

  // Both at once
  warningButton->hide();
  loggedWarning = loadSeriesAndLogWarning(browser,
    QStringList() << loadableSeriesInstanceUID << missingSeriesInstanceUID);
  CHECK_INT(seriesRetrievedSpy.count(), 3);
  CHECK_BOOL(warningButton->text().startsWith(
    QString("Nothing has been loaded: 1 series could not be retrieved, and 1 series could only be "
            "partially retrieved (%1 of %2 images).").arg(numberOfFiles).arg(numberOfFiles + 1)), true);
  CHECK_BOOL(loggedWarning.contains("Series not retrieved: " + missingSeriesInstanceUID), true);
  CHECK_BOOL(loggedWarning.contains("Series partially retrieved: " + loadableSeriesInstanceUID), true);

  qInstallMessageHandler(PreviousMessageHandler);

  //
  // Patient tabs that do not fit the width of the view: they were not shown at all,
  // and the patients could not be reached. All the patients have a tab, and the tabs
  // that do not fit are scrolled to with arrows at the ends of the tab row.
  //
  for (int patient = 5; patient <= 40; ++patient)
  {
    insertStudies(database, patient, QString("Patient^%1").arg(patient), 1);
  }
  browser.refreshBrowser();
  processEvents(500);
  CHECK_INT(patientModel->rowCount(), database->patients().count());
  QWidget* leftArrow = patientView->findChild<QWidget*>("ScrollTabsLeftButton");
  QWidget* rightArrow = patientView->findChild<QWidget*>("ScrollTabsRightButton");
  CHECK_NOT_NULL(leftArrow);
  CHECK_NOT_NULL(rightArrow);
  patientView->setCurrentIndex(patientModel->index(0, 0));
  processEvents(300);
  CHECK_BOOL(leftArrow->isVisible(), false);
  CHECK_BOOL(rightArrow->isVisible(), true);

  // Only the tabs scroll: the studies of the current patient stay in place
  const QRect studiesGeometry = studyListView->geometry();
  const QRect firstTabRect = tabRect(patientView, patientModel->index(0, 0));
  clickMouse(rightArrow, rightArrow->rect().center());
  CHECK_BOOL(tabRect(patientView, patientModel->index(0, 0)).left() < firstTabRect.left(), true);
  CHECK_BOOL(leftArrow->isVisible(), true);
  CHECK_BOOL(studyListView->geometry() == studiesGeometry, true);
  CHECK_BOOL(patientView->currentIndex() == patientModel->index(0, 0), true);

  // The tab of a patient selected beyond the view is scrolled to, beside the arrows
  const QModelIndex lastIndex = patientModel->index(patientModel->rowCount() - 1, 0);
  patientView->setCurrentIndex(lastIndex);
  processEvents(300);
  CHECK_BOOL(rightArrow->isVisible(), false);
  CHECK_BOOL(tabRect(patientView, lastIndex).right() < patientViewport->width(), true);
  CHECK_BOOL(tabRect(patientView, lastIndex).left() > leftArrow->geometry().right(), true);
  // The left arrow tells how many tabs are hidden on its side: the tabs before the
  // first one shown entirely beside it
  QRegularExpressionMatch hiddenMatch = QRegularExpression("^(\\d+) more patient\\(s\\)$").match(leftArrow->toolTip());
  CHECK_BOOL(hiddenMatch.hasMatch(), true);
  const int hiddenOnTheLeft = hiddenMatch.captured(1).toInt();
  CHECK_BOOL(hiddenOnTheLeft > 0 && hiddenOnTheLeft < patientModel->rowCount() - 1, true);
  CHECK_BOOL(tabRect(patientView, patientModel->index(hiddenOnTheLeft, 0)).left() > leftArrow->geometry().right(), true);
  CHECK_BOOL(tabRect(patientView, patientModel->index(hiddenOnTheLeft - 1, 0)).left() <
             leftArrow->geometry().right() + leftArrow->width(), true);

  // The wheel over the tabs scrolls them back
  QWheelEvent wheel(QPointF(patientViewport->width() / 2, 10),
                    patientViewport->mapToGlobal(QPoint(patientViewport->width() / 2, 10)),
                    QPoint(), QPoint(0, 120 * 1000), Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
  QApplication::sendEvent(patientViewport, &wheel);
  processEvents(100);
  CHECK_BOOL(leftArrow->isVisible(), false);
  CHECK_BOOL(rightArrow->isVisible(), true);

  if (!interactive)
  {
    QTimer::singleShot(200, &app, SLOT(quit()));
  }

  return app.exec();
}
