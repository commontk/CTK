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

// Regression tests of the queries of a DICOM server (dcmqrscp) with each filter of the
// browser and their combinations: once the query is over, the browser shows what was
// found, and the query button, the filter fields and the warnings are back to their
// idle state (the query button stayed as an hourglass after a query by series
// description, with the series filters still marked as matching nothing).

// Qt includes
#include <QApplication>
#include <QDate>
#include <QDateEdit>
#include <QElapsedTimer>
#include <QIcon>
#include <QImage>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSettings>
#include <QTemporaryDir>
#include <QTest>
#include <QTimer>

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMPatientModel.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMServer.h"
#include "ctkDICOMTester.h"

// ctkDICOMWidgets includes
#include "ctkDICOMVisualBrowserWidget.h"

// DCMTK includes
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcmetinf.h>
#include <dcmtk/dcmdata/dcuid.h>

// STD includes
#include <iostream>

namespace
{

const QString ServerName("Test");

/// Text of the message boxes shown by the browser, which the test closes as a user does
QStringList ShownMessages;

//------------------------------------------------------------------------------
void closeMessageBoxes()
{
  QMessageBox* messageBox = qobject_cast<QMessageBox*>(QApplication::activeModalWidget());
  if (messageBox)
  {
    ShownMessages.append(messageBox->text());
    messageBox->done(QMessageBox::Ok);
  }
}

//------------------------------------------------------------------------------
/// A series stored in the server
struct SeriesDescription
{
  QString PatientName;
  QString PatientID;
  QString StudyDescription;
  QDate StudyDate;
  QString SeriesDescription;
  QString Modality;
};

//------------------------------------------------------------------------------
/// Write a copy of \a templateFile with the patient, study and series of \a series,
/// under new UIDs. The series of the same study share \a studyInstanceUID.
QString writeSeries(const QString& templateFile, const SeriesDescription& series,
                    const QString& studyInstanceUID, const QString& directory)
{
  DcmFileFormat fileFormat;
  if (fileFormat.loadFile(templateFile.toLatin1().constData()).bad())
  {
    return QString();
  }

  char seriesInstanceUID[100];
  char sopInstanceUID[100];
  dcmGenerateUniqueIdentifier(seriesInstanceUID, SITE_SERIES_UID_ROOT);
  dcmGenerateUniqueIdentifier(sopInstanceUID, SITE_INSTANCE_UID_ROOT);

  DcmDataset* dataset = fileFormat.getDataset();
  dataset->putAndInsertString(DCM_PatientName, series.PatientName.toLatin1().constData());
  dataset->putAndInsertString(DCM_PatientID, series.PatientID.toLatin1().constData());
  dataset->putAndInsertString(DCM_StudyInstanceUID, studyInstanceUID.toLatin1().constData());
  dataset->putAndInsertString(DCM_StudyDescription, series.StudyDescription.toLatin1().constData());
  dataset->putAndInsertString(DCM_StudyDate, series.StudyDate.toString("yyyyMMdd").toLatin1().constData());
  dataset->putAndInsertString(DCM_SeriesInstanceUID, seriesInstanceUID);
  dataset->putAndInsertString(DCM_SeriesDescription, series.SeriesDescription.toLatin1().constData());
  dataset->putAndInsertString(DCM_Modality, series.Modality.toLatin1().constData());
  dataset->putAndInsertString(DCM_SOPInstanceUID, sopInstanceUID);
  fileFormat.getMetaInfo()->putAndInsertString(DCM_MediaStorageSOPInstanceUID, sopInstanceUID);

  const QString fileName = directory + "/" + QString(sopInstanceUID) + ".dcm";
  if (fileFormat.saveFile(fileName.toLatin1().constData()).bad())
  {
    return QString();
  }
  return fileName;
}

//------------------------------------------------------------------------------
/// The filters of a query, and what it finds
struct Scenario
{
  QString Name;
  QString PatientID;
  QString PatientName;
  QString StudyDescription;
  QString SeriesDescription;
  QStringList Modalities{"Any"};
  ctkDICOMVisualBrowserWidget::DateType Date = ctkDICOMVisualBrowserWidget::Any;
  /// Custom date range, when Date is CustomRange
  QDate StartDate;
  QDate EndDate;

  /// Number of series shown for each patient ID of the test, nothing when the query
  /// finds nothing
  QMap<QString, int> ExpectedSeries;
};

/// Token of this run in the patients, descriptions and filters: the server keeps the
/// images stored by the previous runs, and by other tests.
QString RunToken;

//------------------------------------------------------------------------------
/// Wait until the query and what it started (queries of the studies and series of the
/// patients found) are over. A query that never ends is reported by the checks, after
/// the timeout.
void settle(ctkDICOMVisualBrowserWidget& browser, int timeoutMsec = 30000)
{
  ctkDICOMScheduler* scheduler = browser.scheduler();
  QElapsedTimer timer;
  timer.start();
  int quietRounds = 0;
  while (quietRounds < 5 && timer.elapsed() < timeoutMsec)
  {
    QTest::qWait(100);
    quietRounds = scheduler->numberOfRunningJobs() == 0 && !browser.patientModel()->queryInProgress() ?
      quietRounds + 1 : 0;
  }
}

//------------------------------------------------------------------------------
bool sameIcon(const QIcon& icon, const QString& iconPath)
{
  const QSize size(32, 32);
  return icon.pixmap(size).toImage() == QIcon(iconPath).pixmap(size).toImage();
}

//------------------------------------------------------------------------------
QString iconName(const QIcon& icon)
{
  foreach (const QString& name, QStringList() << "query" << "query_success" << "query_failed" << "wait")
  {
    if (sameIcon(icon, ":/Icons/" + name + ".svg"))
    {
      return name;
    }
  }
  return "unknown";
}

//------------------------------------------------------------------------------
/// The filter fields shown as matching nothing
QStringList warnedFilterFields(ctkDICOMVisualBrowserWidget& browser)
{
  const QColor warningColor(Qt::darkYellow);
  QStringList warnedFields;
  foreach (const QString& name, QStringList()
             << "FilteringPatientIDSearchBox" << "FilteringPatientNameSearchBox"
             << "FilteringStudyDescriptionSearchBox" << "FilteringSeriesDescriptionSearchBox")
  {
    QWidget* widget = browser.findChild<QWidget*>(name);
    if (widget && widget->palette().color(widget->backgroundRole()) == warningColor)
    {
      warnedFields.append(name);
    }
  }
  foreach (const QString& name, QStringList()
             << "FilteringModalityCheckableComboBox" << "FilteringDateComboBox")
  {
    QWidget* widget = browser.findChild<QWidget*>(name);
    if (widget && widget->palette().color(QPalette::Button) == warningColor)
    {
      warnedFields.append(name);
    }
  }
  return warnedFields;
}

//------------------------------------------------------------------------------
/// Number of series shown for each patient ID
QMap<QString, int> shownSeries(ctkDICOMVisualBrowserWidget& browser)
{
  QMap<QString, int> series;
  ctkDICOMPatientModel* patientModel = browser.patientModel();
  foreach (const QString& patientUID, patientModel->filteredPatientUIDs())
  {
    const QModelIndex index = patientModel->indexFromPatientUID(patientUID);
    const int numberOfSeries = patientModel->filteredSeriesCountForPatient(patientUID);
    if (numberOfSeries > 0)
    {
      series[patientModel->patientID(index)] = numberOfSeries;
    }
  }
  return series;
}

//------------------------------------------------------------------------------
QString toString(const QMap<QString, int>& series)
{
  QStringList items;
  for (auto it = series.constBegin(); it != series.constEnd(); ++it)
  {
    items.append(QString("%1: %2").arg(it.key()).arg(it.value()));
  }
  return "{" + items.join(", ") + "}";
}

//------------------------------------------------------------------------------
/// Set the filters like a user does, in the filter fields
void setFilters(ctkDICOMVisualBrowserWidget& browser, const Scenario& scenario)
{
  browser.findChild<QLineEdit*>("FilteringPatientIDSearchBox")->setText(scenario.PatientID);
  browser.findChild<QLineEdit*>("FilteringPatientNameSearchBox")->setText(scenario.PatientName);
  browser.findChild<QLineEdit*>("FilteringStudyDescriptionSearchBox")->setText(scenario.StudyDescription);
  browser.findChild<QLineEdit*>("FilteringSeriesDescriptionSearchBox")->setText(scenario.SeriesDescription);
  browser.setFilteringModalities(scenario.Modalities);
  browser.setFilteringDate(scenario.Date);
  if (scenario.Date == ctkDICOMVisualBrowserWidget::CustomRange)
  {
    QList<QDateEdit*> dateEdits = browser.findChildren<QDateEdit*>();
    if (dateEdits.count() == 2)
    {
      dateEdits[0]->setDate(scenario.StartDate);
      dateEdits[1]->setDate(scenario.EndDate);
    }
  }
}

//------------------------------------------------------------------------------
/// Query the server with the filters of \a scenario, like a click on the query button,
/// and check the state of the browser once the query is over.
bool runScenario(ctkDICOMVisualBrowserWidget& browser, const Scenario& scenario)
{
  setFilters(browser, scenario);
  QTest::qWait(50);
  ShownMessages.clear();

  QPushButton* searchButton = browser.findChild<QPushButton*>("SearchPushButton");
  QPushButton* warningButton = browser.findChild<QPushButton*>("WarningPushButton");
  QTest::mouseClick(searchButton, Qt::LeftButton);
  settle(browser);

  const QString icon = iconName(searchButton->icon());
  const bool queryInProgress = browser.patientModel()->queryInProgress();
  const QStringList warnedFields = warnedFilterFields(browser);
  const bool expectResults = !scenario.ExpectedSeries.isEmpty();
  const QMap<QString, int> allSeries = shownSeries(browser);
  QMap<QString, int> series = allSeries;
  if (expectResults)
  {
    // (filters without the token of the run, e.g. the modality, also find the patients
    // of the previous runs)
    foreach (const QString& patientID, series.keys())
    {
      if (!patientID.contains(RunToken))
      {
        series.remove(patientID);
      }
    }
  }

  std::cout << qPrintable(scenario.Name) << ": icon " << qPrintable(icon)
            << ", query in progress " << queryInProgress
            << ", shown " << qPrintable(toString(series))
            << ", warned fields [" << qPrintable(warnedFields.join(", ")) << "]"
            << ", warning " << (warningButton->isVisible() ? qPrintable(warningButton->text()) : "<none>")
            << ", message boxes [" << qPrintable(ShownMessages.join(" | ")) << "]"
            << std::endl;

  QStringList errors;
  if (icon == "wait" || icon == "unknown")
  {
    errors << QString("the query button still shows '%1'").arg(icon);
  }
  if (queryInProgress)
  {
    errors << "the patient model still has a query in progress";
  }
  // A filter that is empty, or that accepts everything, cannot be what matches nothing
  QMap<QString, bool> setFilters;
  setFilters["FilteringPatientIDSearchBox"] = !scenario.PatientID.isEmpty();
  setFilters["FilteringPatientNameSearchBox"] = !scenario.PatientName.isEmpty();
  setFilters["FilteringStudyDescriptionSearchBox"] = !scenario.StudyDescription.isEmpty();
  setFilters["FilteringSeriesDescriptionSearchBox"] = !scenario.SeriesDescription.isEmpty();
  setFilters["FilteringModalityCheckableComboBox"] = !scenario.Modalities.contains("Any");
  setFilters["FilteringDateComboBox"] = scenario.Date != ctkDICOMVisualBrowserWidget::Any;
  foreach (const QString& field, warnedFields)
  {
    if (!setFilters.value(field))
    {
      errors << QString("the filter %1 is not set, and is marked as matching nothing").arg(field);
    }
  }

  // The filter fields tell whether anything matches them
  if (allSeries.isEmpty() && warnedFields.isEmpty())
  {
    errors << "nothing is shown, and no filter field is marked as matching nothing";
  }
  if (!allSeries.isEmpty() && !warnedFields.isEmpty())
  {
    errors << QString("series are shown, and filter fields are marked as matching nothing: %1")
                .arg(warnedFields.join(", "));
  }

  if (!scenario.SeriesDescription.isEmpty())
  {
    // dcmqrscp has no series description key: it ignores the filter, and returns
    // the series without their description, which the filter of the browser then hides.
    // Nothing else is known about the results than what is checked above.
  }
  else if (expectResults)
  {
    if (icon != "query_success")
    {
      errors << QString("the query button shows '%1' instead of a success").arg(icon);
    }
    if (series != scenario.ExpectedSeries)
    {
      errors << QString("shown series %1 instead of %2").arg(toString(series), toString(scenario.ExpectedSeries));
    }
    if (!warnedFields.isEmpty())
    {
      errors << QString("filter fields still marked as matching nothing: %1").arg(warnedFields.join(", "));
    }
    if (warningButton->isVisible())
    {
      errors << QString("unexpected warning: %1").arg(warningButton->text());
    }
  }
  else
  {
    if (icon != "query_failed")
    {
      errors << QString("the query button shows '%1' instead of a failure").arg(icon);
    }
    if (!series.isEmpty())
    {
      errors << QString("shown series %1 instead of none").arg(toString(series));
    }
    if (warnedFields.isEmpty())
    {
      errors << "no filter field is marked as matching nothing";
    }
    if (!warningButton->isVisible())
    {
      errors << "no warning tells that the query found nothing";
    }
  }

  foreach (const QString& error, errors)
  {
    std::cerr << "  " << qPrintable(scenario.Name) << ": " << qPrintable(error) << std::endl;
  }
  return errors.isEmpty();
}

//------------------------------------------------------------------------------
ctkDICOMServer* createServer(QObject* parent, int port)
{
  ctkDICOMServer* server = new ctkDICOMServer(parent);
  server->setConnectionName(ServerName);
  server->setCallingAETitle("CTK_AE");
  server->setCalledAETitle("CTK_AE");
  server->setHost("localhost");
  server->setPort(port);
  server->setRetrieveProtocol(ctkDICOMServer::RetrieveProtocol::CGET);
  server->setQueryRetrieveEnabled(true);
  server->setTrustedEnabled(true);
  server->setMaximumRetryWait(5);
  return server;
}

} // end of anonymous namespace

//------------------------------------------------------------------------------
int ctkDICOMVisualBrowserWidgetTest4(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();
  bool interactive = arguments.removeOne("-I");
  if (arguments.count() != 1)
  {
    std::cerr << "Usage: " << qPrintable(testName) << " [-I] <path-to-image>" << std::endl;
    return EXIT_FAILURE;
  }
  const QString templateFile = arguments.first();

  // Keep the test out of the settings of the user running it
  QCoreApplication::setOrganizationName("CTK");
  QCoreApplication::setApplicationName("ctkDICOMVisualBrowserWidgetTest4");
  QSettings().clear();

  // Two patients: one with a recent study of two series, one with an old study
  RunToken = QString::number(QRandomGenerator::global()->bounded(100000, 1000000));
  const QString janeID = "P" + RunToken;
  const QString facialID = "FE" + RunToken;
  const QDate today = QDate::currentDate();
  const QDate oldDate(2005, 5, 11);
  QList<QPair<SeriesDescription, int> > seriesToStore;
  seriesToStore
    << qMakePair(SeriesDescription{"Doe" + RunToken + "^Jane", janeID, "Thyroid" + RunToken + " US", today,
                                   "Piur Thyroid ROI" + RunToken, "SR"}, 0)
    << qMakePair(SeriesDescription{"Doe" + RunToken + "^Jane", janeID, "Thyroid" + RunToken + " US", today,
                                   "Thyroid Axial" + RunToken, "CT"}, 0)
    << qMakePair(SeriesDescription{"Facial" + RunToken + "^Expression", facialID, "Head" + RunToken + " MRI",
                                   oldDate, "3D Cor T1" + RunToken, "MR"}, 1);

  QTemporaryDir dataDirectory;
  CHECK_BOOL(dataDirectory.isValid(), true);
  QStringList studyInstanceUIDs;
  for (int study = 0; study < 2; ++study)
  {
    char studyInstanceUID[100];
    dcmGenerateUniqueIdentifier(studyInstanceUID, SITE_STUDY_UID_ROOT);
    studyInstanceUIDs << studyInstanceUID;
  }
  QStringList files;
  foreach (const auto& series, seriesToStore)
  {
    const QString fileName = writeSeries(templateFile, series.first,
                                         studyInstanceUIDs.at(series.second), dataDirectory.path());
    CHECK_BOOL(fileName.isEmpty(), false);
    files << fileName;
  }

  QTimer messageBoxCloser;
  QObject::connect(&messageBoxCloser, &QTimer::timeout, closeMessageBoxes);
  messageBoxCloser.start(100);

  ctkDICOMTester tester;
  tester.startDCMQRSCP();
  CHECK_BOOL(tester.storeData(files), true);

  const QMap<QString, int> nothing;
  const QString nobody = "Nobody" + RunToken;
  QList<Scenario> scenarios;
  {
    Scenario s; s.Name = "Patient ID"; s.PatientID = janeID;
    s.ExpectedSeries = {{janeID, 2}}; scenarios << s;
  }
  {
    Scenario s; s.Name = "Patient name"; s.PatientName = "Doe" + RunToken;
    s.ExpectedSeries = {{janeID, 2}}; scenarios << s;
  }
  {
    Scenario s; s.Name = "Study description"; s.StudyDescription = "Head" + RunToken;
    s.ExpectedSeries = {{facialID, 1}}; scenarios << s;
  }
  {
    Scenario s; s.Name = "Series description"; s.SeriesDescription = "ROI" + RunToken;
    s.ExpectedSeries = {{janeID, 1}}; scenarios << s;
  }
  {
    Scenario s; s.Name = "Modality"; s.Modalities = QStringList() << "CT";
    s.ExpectedSeries = {{janeID, 1}}; scenarios << s;
  }
  {
    Scenario s; s.Name = "Date (last week)"; s.Date = ctkDICOMVisualBrowserWidget::LastWeek;
    s.ExpectedSeries = {{janeID, 2}}; scenarios << s;
  }
  {
    Scenario s; s.Name = "Date (custom range)"; s.Date = ctkDICOMVisualBrowserWidget::CustomRange;
    s.StartDate = oldDate.addDays(-1); s.EndDate = oldDate.addDays(1);
    s.ExpectedSeries = {{facialID, 1}}; scenarios << s;
  }
  {
    Scenario s; s.Name = "Patient name and series description"; s.PatientName = "Doe" + RunToken;
    s.SeriesDescription = "Axial"; s.ExpectedSeries = {{janeID, 1}}; scenarios << s;
  }
  {
    Scenario s; s.Name = "Patient ID and study description"; s.PatientID = facialID;
    s.StudyDescription = "Head"; s.ExpectedSeries = {{facialID, 1}}; scenarios << s;
  }
  {
    Scenario s; s.Name = "Patient ID and modality"; s.PatientID = janeID;
    s.Modalities = QStringList() << "SR"; s.ExpectedSeries = {{janeID, 1}}; scenarios << s;
  }
  {
    Scenario s; s.Name = "Study description and modality"; s.StudyDescription = "Thyroid" + RunToken;
    s.Modalities = QStringList() << "SR"; s.ExpectedSeries = {{janeID, 1}}; scenarios << s;
  }
  {
    Scenario s; s.Name = "Study and series descriptions"; s.StudyDescription = "Thyroid" + RunToken;
    s.SeriesDescription = "ROI"; s.ExpectedSeries = {{janeID, 1}}; scenarios << s;
  }
  {
    Scenario s; s.Name = "Study description and date"; s.StudyDescription = "Head" + RunToken;
    s.Date = ctkDICOMVisualBrowserWidget::CustomRange;
    s.StartDate = oldDate; s.EndDate = oldDate;
    s.ExpectedSeries = {{facialID, 1}}; scenarios << s;
  }
  {
    Scenario s; s.Name = "Series description and modality"; s.SeriesDescription = "T1" + RunToken;
    s.Modalities = QStringList() << "MR"; s.ExpectedSeries = {{facialID, 1}}; scenarios << s;
  }
  {
    Scenario s; s.Name = "Series description and date"; s.SeriesDescription = "Axial" + RunToken;
    s.Date = ctkDICOMVisualBrowserWidget::LastWeek; s.ExpectedSeries = {{janeID, 1}}; scenarios << s;
  }
  {
    Scenario s; s.Name = "All filters"; s.PatientID = janeID; s.PatientName = "Doe" + RunToken;
    s.StudyDescription = "Thyroid"; s.SeriesDescription = "ROI"; s.Modalities = QStringList() << "SR";
    s.Date = ctkDICOMVisualBrowserWidget::LastWeek; s.ExpectedSeries = {{janeID, 1}}; scenarios << s;
  }
  {
    Scenario s; s.Name = "Patient ID matching nothing"; s.PatientID = nobody;
    s.ExpectedSeries = nothing; scenarios << s;
  }
  {
    Scenario s; s.Name = "Patient name matching nothing"; s.PatientName = nobody;
    s.ExpectedSeries = nothing; scenarios << s;
  }
  {
    Scenario s; s.Name = "Study description matching nothing"; s.StudyDescription = nobody;
    s.ExpectedSeries = nothing; scenarios << s;
  }
  {
    Scenario s; s.Name = "Series description matching nothing"; s.SeriesDescription = nobody;
    s.ExpectedSeries = nothing; scenarios << s;
  }
  {
    Scenario s; s.Name = "Study and series descriptions of different patients";
    s.StudyDescription = "Head" + RunToken; s.SeriesDescription = "ROI" + RunToken;
    s.ExpectedSeries = nothing; scenarios << s;
  }
  {
    Scenario s; s.Name = "Series description and modality of different series";
    s.SeriesDescription = "ROI" + RunToken; s.Modalities = QStringList() << "CT";
    s.ExpectedSeries = nothing; scenarios << s;
  }
  {
    Scenario s; s.Name = "Patient name and date of different patients"; s.PatientName = "Doe" + RunToken;
    s.Date = ctkDICOMVisualBrowserWidget::CustomRange; s.StartDate = oldDate; s.EndDate = oldDate;
    s.ExpectedSeries = nothing; scenarios << s;
  }

  int numberOfFailures = 0;

  // Each query in an empty database, so that all that is shown comes from the query
  std::cout << "Queries in an empty database" << std::endl;
  foreach (const Scenario& scenario, scenarios)
  {
    QTemporaryDir databaseDirectory;
    CHECK_BOOL(databaseDirectory.isValid(), true);
    ctkDICOMVisualBrowserWidget browser;
    browser.setDatabaseDirectory(databaseDirectory.path());
    browser.setAutoRetrieveFullSeries(false);
    browser.addServer(createServer(&browser, tester.dcmqrscpPort()));
    browser.resize(1000, 700);
    browser.show();
    CHECK_BOOL(QTest::qWaitForWindowExposed(&browser), true);
    if (!runScenario(browser, scenario))
    {
      ++numberOfFailures;
    }
  }

  // The queries one after the other in the same browser, so that a query starts
  // from the database and the state left by the previous ones
  std::cout << "Queries one after the other" << std::endl;
  QTemporaryDir databaseDirectory;
  CHECK_BOOL(databaseDirectory.isValid(), true);
  ctkDICOMVisualBrowserWidget browser;
  browser.setDatabaseDirectory(databaseDirectory.path());
  browser.setAutoRetrieveFullSeries(false);
  browser.addServer(createServer(&browser, tester.dcmqrscpPort()));
  browser.resize(1000, 700);
  browser.show();
  CHECK_BOOL(QTest::qWaitForWindowExposed(&browser), true);
  foreach (const Scenario& scenario, scenarios)
  {
    if (!runScenario(browser, scenario))
    {
      ++numberOfFailures;
    }
  }

  if (numberOfFailures > 0)
  {
    std::cerr << numberOfFailures << " queries left the browser in a wrong state" << std::endl;
    return EXIT_FAILURE;
  }

  if (!interactive)
  {
    QTimer::singleShot(200, &app, SLOT(quit()));
  }

  return app.exec();
}
