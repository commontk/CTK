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

// Regression tests of the load of series that are not, or not yet entirely, in the
// database: the browser retrieves them from a DICOM server (dcmqrscp) and hands them
// over to the application only once all their images have been inserted.

// Qt includes
#include <QApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QProgressDialog>
#include <QPushButton>
#include <QSettings>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>
#include <QTimer>

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMCore includes
#include "ctkAbstractJob.h"
#include "ctkDICOMDatabase.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMServer.h"
#include "ctkDICOMTester.h"

// ctkDICOMWidgets includes
#include "ctkDICOMVisualBrowserWidget.h"

// STD includes
#include <iostream>

namespace
{

const QString ServerName("Test");
const QString PatientID("Facial Expression");

//------------------------------------------------------------------------------
int numberOfFiles(ctkDICOMDatabase* database, const QString& seriesInstanceUID)
{
  QStringList files = database->filesForSeries(seriesInstanceUID);
  files.removeAll(QString());
  return files.count();
}

//------------------------------------------------------------------------------
int numberOfActiveJobs(ctkDICOMScheduler* scheduler, const QString& seriesInstanceUID)
{
  return scheduler->getJobsByDICOMUIDs({}, {}, {seriesInstanceUID}, {},
    {ctkAbstractJob::JobStatus::Initialized, ctkAbstractJob::JobStatus::Queued, ctkAbstractJob::JobStatus::Running}).count();
}

//------------------------------------------------------------------------------
/// Wait until the browser is done with the jobs it started on its own (e.g. when it
/// is refreshed), so that the state of the series before the load is the one the test
/// set up, and not a race with the browser.
void settle(ctkDICOMVisualBrowserWidget& browser)
{
  ctkDICOMScheduler* scheduler = browser.scheduler();
  QElapsedTimer timer;
  timer.start();
  int quietRounds = 0;
  while (quietRounds < 3 && timer.elapsed() < 30000)
  {
    QTest::qWait(200);
    scheduler->waitForFinish(false, true);
    quietRounds = scheduler->numberOfRunningJobs() == 0 ? quietRounds + 1 : 0;
  }
}

//------------------------------------------------------------------------------
/// The state of the series when the user clicks Load
void printSeriesState(ctkDICOMVisualBrowserWidget& browser, const QString& seriesInstanceUID)
{
  std::cout << "  before the load: " << numberOfFiles(browser.dicomDatabase(), seriesInstanceUID)
            << " images of " << browser.dicomDatabase()->instancesForSeries(seriesInstanceUID).count()
            << " instances, " << numberOfActiveJobs(browser.scheduler(), seriesInstanceUID)
            << " active jobs, retrieving: " << browser.scheduler()->isRetrievingSeries(QStringList() << seriesInstanceUID)
            << std::endl;
}

//------------------------------------------------------------------------------
/// What the application is handed when the user clicks Load
struct LoadResult
{
  /// Number of times the series were handed over
  int NumberOfHandovers = 0;
  /// Files of the series in the database when they were handed over
  int NumberOfFilesAtHandover = -1;
  /// The load was still waiting after the timeout, and was canceled
  bool TimedOut = false;
  qint64 Duration = 0;
};

//------------------------------------------------------------------------------
/// Load the series like a click on Load does. A load that waits for longer than
/// \a timeoutMsec is canceled, as the user would do, so that the test fails instead
/// of waiting forever.
LoadResult loadSeries(ctkDICOMVisualBrowserWidget& browser, const QString& seriesInstanceUID,
                      int timeoutMsec = 60000)
{
  LoadResult result;
  printSeriesState(browser, seriesInstanceUID);
  QMetaObject::Connection handover = QObject::connect(
    &browser, &ctkDICOMVisualBrowserWidget::seriesRetrieved,
    [&browser, &result, seriesInstanceUID](const QStringList& seriesInstanceUIDs)
    {
      ++result.NumberOfHandovers;
      if (seriesInstanceUIDs.contains(seriesInstanceUID))
      {
        result.NumberOfFilesAtHandover = numberOfFiles(browser.dicomDatabase(), seriesInstanceUID);
      }
    });

  QElapsedTimer timer;
  timer.start();
  QTimer watchdog;
  QObject::connect(&watchdog, &QTimer::timeout, [&browser, &result, &timer, timeoutMsec]()
    {
      if (timer.elapsed() < timeoutMsec)
      {
        return;
      }
      foreach (QProgressDialog* dialog, browser.findChildren<QProgressDialog*>())
      {
        result.TimedOut = true;
        dialog->cancel();
      }
    });
  watchdog.start(100);

  QMetaObject::invokeMethod(&browser, "onLoadSeries", Qt::DirectConnection,
                            Q_ARG(QStringList, QStringList() << seriesInstanceUID));

  result.Duration = timer.elapsed();
  watchdog.stop();
  QObject::disconnect(handover);
  return result;
}

//------------------------------------------------------------------------------
bool checkLoad(const LoadResult& result, int expectedNumberOfFiles, const QString& what, int line)
{
  std::cout << qPrintable(what) << ": handed over " << result.NumberOfHandovers
            << " time(s) with " << result.NumberOfFilesAtHandover << " of "
            << expectedNumberOfFiles << " images, after " << result.Duration << " ms"
            << (result.TimedOut ? " (timed out)" : "") << std::endl;
  if (result.TimedOut || result.NumberOfHandovers != 1 ||
      result.NumberOfFilesAtHandover != expectedNumberOfFiles)
  {
    std::cerr << "Line " << line << " - " << qPrintable(what) << ": the series was not loaded"
              << " with all its images" << std::endl;
    return false;
  }
  return true;
}

} // end of anonymous namespace

//------------------------------------------------------------------------------
int ctkDICOMVisualBrowserWidgetTest3(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();
  bool interactive = arguments.removeOne("-I");
  if (arguments.count() < 2)
  {
    std::cerr << "Usage: " << qPrintable(testName)
              << " [-I] <path-to-image> <path-to-image> [...]" << std::endl;
    return EXIT_FAILURE;
  }
  const int numberOfImages = arguments.count();

  // Keep the test out of the settings of the user running it
  QCoreApplication::setOrganizationName("CTK");
  QCoreApplication::setApplicationName("ctkDICOMVisualBrowserWidgetTest3");
  QSettings().clear();

  ctkDICOMTester tester;
  tester.startDCMQRSCP();
  CHECK_BOOL(tester.storeData(arguments), true);

  QTemporaryDir databaseDirectory;
  CHECK_BOOL(databaseDirectory.isValid(), true);

  ctkDICOMVisualBrowserWidget browser;
  browser.setDatabaseDirectory(databaseDirectory.path());
  // The frames of a series are retrieved only when it is loaded, and no study is
  // opened on its own: the test sets up what is in the database
  browser.setAutoRetrieveFullSeries(false);
  browser.setNumberOfOpenedStudiesPerPatient(0);
  browser.resize(1000, 700);
  browser.show();
  CHECK_BOOL(QTest::qWaitForWindowExposed(&browser), true);

  ctkDICOMServer* server = new ctkDICOMServer(&browser);
  server->setConnectionName(ServerName);
  server->setCallingAETitle("CTK_AE");
  server->setCalledAETitle("CTK_AE");
  server->setHost("localhost");
  server->setPort(tester.dcmqrscpPort());
  server->setRetrieveProtocol(ctkDICOMServer::RetrieveProtocol::CGET);
  server->setQueryRetrieveEnabled(true);
  server->setTrustedEnabled(true);
  server->setMaximumRetryWait(5);
  browser.addServer(server);

  ctkDICOMScheduler* scheduler = browser.scheduler();
  ctkDICOMDatabase* database = browser.dicomDatabase();
  CHECK_NOT_NULL(scheduler);
  CHECK_NOT_NULL(database);

  // What a query finds: the patient, its study and series, and the instances of the
  // series, all without files
  scheduler->queryStudies(PatientID, QThread::HighPriority, QStringList(ServerName));
  scheduler->waitForFinish(false, true);
  CHECK_INT(database->patients().count(), 1);
  const QString patientUID = database->patients().first();
  CHECK_INT(database->studiesForPatient(patientUID).count(), 1);
  const QString studyInstanceUID = database->studiesForPatient(patientUID).first();
  scheduler->querySeries(PatientID, studyInstanceUID, QThread::HighPriority, QStringList(ServerName));
  scheduler->waitForFinish(false, true);
  CHECK_INT(database->seriesForStudy(studyInstanceUID).count(), 1);
  const QString seriesInstanceUID = database->seriesForStudy(studyInstanceUID).first();

  // Start again from a series that the browser knows, without any image
  auto resetSeries = [&]()
  {
    database->removeSeries(seriesInstanceUID, false, false);
    scheduler->querySeries(PatientID, studyInstanceUID, QThread::HighPriority, QStringList(ServerName));
    scheduler->waitForFinish(false, true);
    browser.refreshBrowser();
    settle(browser);
  };
  auto queryInstances = [&]()
  {
    scheduler->queryInstances(PatientID, studyInstanceUID, seriesInstanceUID, QThread::HighPriority,
                              QStringList(ServerName));
    scheduler->waitForFinish(false, true);
    settle(browser);
  };

  //
  // The series of a study that was never opened. The load found nothing to wait for,
  // and handed the series over without any image (or nothing at all), so that the
  // application loaded nothing and the click on Load looked ignored.
  //
  browser.refreshBrowser();
  settle(browser);
  CHECK_INT(numberOfFiles(database, seriesInstanceUID), 0);
  CHECK_INT(numberOfActiveJobs(scheduler, seriesInstanceUID), 0);
  if (!checkLoad(loadSeries(browser, seriesInstanceUID), numberOfImages,
                 "Series of a study never opened", __LINE__))
  {
    return EXIT_FAILURE;
  }

  //
  // The instances of the series have been queried, none retrieved: the load handed
  // over the instances without their images, or waited for a retrieve that nothing
  // requested.
  //
  resetSeries();
  queryInstances();
  CHECK_INT(numberOfFiles(database, seriesInstanceUID), 0);
  CHECK_INT(database->instancesForSeries(seriesInstanceUID).count(), numberOfImages);
  CHECK_INT(numberOfActiveJobs(scheduler, seriesInstanceUID), 0);
  if (!checkLoad(loadSeries(browser, seriesInstanceUID), numberOfImages,
                 "Series queried but not retrieved", __LINE__))
  {
    return EXIT_FAILURE;
  }

  //
  // Only the image of the thumbnail has been retrieved, and the frames are not
  // prefetched: the load handed over the single image of the thumbnail, or waited
  // forever for a retrieve that nothing requested.
  //
  resetSeries();
  queryInstances();
  const QStringList instances = database->instancesForSeries(seriesInstanceUID);
  CHECK_INT(instances.count(), numberOfImages);
  scheduler->retrieveSOPInstance(PatientID, studyInstanceUID, seriesInstanceUID,
                                 instances.at(instances.count() / 2), QThread::HighPriority,
                                 QStringList(ServerName));
  scheduler->waitForFinish(false, true);
  settle(browser);
  // (the browser may also have retrieved the image of its own thumbnail)
  const int numberOfThumbnailFiles = numberOfFiles(database, seriesInstanceUID);
  CHECK_BOOL(numberOfThumbnailFiles > 0 && numberOfThumbnailFiles < numberOfImages, true);
  CHECK_INT(numberOfActiveJobs(scheduler, seriesInstanceUID), 0);
  if (!checkLoad(loadSeries(browser, seriesInstanceUID), numberOfImages,
                 "Series with only the image of its thumbnail", __LINE__))
  {
    return EXIT_FAILURE;
  }

  //
  // The series is being retrieved when the user clicks Load: the load waits until all
  // its images are inserted, instead of handing over the images inserted so far.
  //
  resetSeries();
  // (the browser may have retrieved the image of its own thumbnail)
  CHECK_BOOL(numberOfFiles(database, seriesInstanceUID) < numberOfImages, true);
  scheduler->retrieveSeries(PatientID, studyInstanceUID, seriesInstanceUID, QThread::HighPriority,
                            QStringList(ServerName));
  CHECK_BOOL(scheduler->isRetrievingSeries(QStringList() << seriesInstanceUID), true);
  if (!checkLoad(loadSeries(browser, seriesInstanceUID), numberOfImages,
                 "Series being retrieved", __LINE__))
  {
    return EXIT_FAILURE;
  }

  //
  // The server stops answering: the load gives up once the retrieve has failed, and
  // says so, instead of waiting for the series forever.
  //
  resetSeries();
  queryInstances();
  const int numberOfFilesBeforeStop = numberOfFiles(database, seriesInstanceUID);
  CHECK_BOOL(numberOfFilesBeforeStop < numberOfImages, true);
  CHECK_BOOL(tester.stopDCMQRSCP(), true);
  LoadResult unreachable = loadSeries(browser, seriesInstanceUID);
  std::cout << "Server stopped: handed over " << unreachable.NumberOfHandovers << " time(s), after "
            << unreachable.Duration << " ms" << (unreachable.TimedOut ? " (timed out)" : "") << std::endl;
  CHECK_BOOL(unreachable.TimedOut, false);
  CHECK_INT(unreachable.NumberOfHandovers, 0);
  QPushButton* warningButton = browser.findChild<QPushButton*>("WarningPushButton");
  CHECK_NOT_NULL(warningButton);
  CHECK_BOOL(warningButton->isVisible(), true);
  if (numberOfFilesBeforeStop == 0)
  {
    CHECK_QSTRING(warningButton->text(),
      "Nothing has been loaded: 1 series could not be retrieved.\n"
      "Verify that the servers are reachable, then load them again.");
  }
  else
  {
    CHECK_QSTRING(warningButton->text(),
      QString("Nothing has been loaded: 1 series could only be partially retrieved (%1 of %2 images).\n"
              "Verify that the servers are reachable and have all the images, then load them again.")
        .arg(numberOfFilesBeforeStop).arg(numberOfImages));
  }

  if (!interactive)
  {
    QTimer::singleShot(200, &app, SLOT(quit()));
  }

  return app.exec();
}
