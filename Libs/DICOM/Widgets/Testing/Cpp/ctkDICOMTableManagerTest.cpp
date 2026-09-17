/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QDir>
#include <QSignalSpy>
#include <QTest>

// CTK includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMItem.h"
#include "ctkDICOMTableManager.h"
#include "ctkDICOMTableView.h"
#include "ctkTest.h"

// DCMTK includes
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcitem.h>

//-----------------------------------------------------------------------------
class ctkDICOMTableManagerTester: public QObject
{
  Q_OBJECT
public:
  explicit ctkDICOMTableManagerTester(QObject* parent = nullptr) : QObject(parent) {}
private slots:

  void init();
  void cleanup();

  void testRemoveUnselectedKeepsSelection();
  void testRemoveSelectedSeriesClearsSelection();
  void testRemoveSelectedPatientClearsSelection();
  void testRemoveWithPatientFilterKeepsTableContent();

private:
  void addSeries(const QString& patientID, const QString& studyUID, const QString& seriesUID);
  void selectSeries(const QString& patientID, const QString& studyUID, const QString& seriesUID);
  QString patientUID(const QString& patientID);

  ctkDICOMDatabase* Database{nullptr};
  ctkDICOMTableManager* TableManager{nullptr};
};

// ----------------------------------------------------------------------------
void ctkDICOMTableManagerTester::init()
{
  this->Database = new ctkDICOMDatabase();
  this->Database->openDatabase(":memory:");
  QVERIFY(this->Database->isOpen());

  this->addSeries("Patient1", "1.2.3.1", "1.2.3.1.1");
  this->addSeries("Patient1", "1.2.3.1", "1.2.3.1.2");
  this->addSeries("Patient2", "1.2.3.2", "1.2.3.2.1");

  this->TableManager = new ctkDICOMTableManager(this->Database);
  this->TableManager->updateTableViews();
  this->TableManager->show();

  this->selectSeries("Patient1", "1.2.3.1", "1.2.3.1.1");
}

// ----------------------------------------------------------------------------
void ctkDICOMTableManagerTester::cleanup()
{
  delete this->TableManager;
  this->TableManager = nullptr;
  delete this->Database;
  this->Database = nullptr;
}

// ----------------------------------------------------------------------------
void ctkDICOMTableManagerTester::addSeries(const QString& patientID, const QString& studyUID, const QString& seriesUID)
{
  QSharedPointer<ctkDICOMItem> dataset(new ctkDICOMItem());
  dataset->InitializeFromItem(new DcmItem(), /* takeOwnership= */ true);
  dataset->SetElementAsString(DCM_PatientName, patientID);
  dataset->SetElementAsString(DCM_PatientID, patientID);
  dataset->SetElementAsString(DCM_StudyInstanceUID, studyUID);
  dataset->SetElementAsString(DCM_SeriesInstanceUID, seriesUID);
  dataset->SetElementAsString(DCM_SOPInstanceUID, seriesUID + ".1");

  // Series without images are removed by database cleanup, therefore an image file must be referenced.
  // The file is not accessed, so it does not have to exist.
  ctkDICOMDatabase::IndexingResult indexingResult;
  indexingResult.filePath = QDir::temp().filePath(seriesUID + ".1.dcm");
  indexingResult.dataset = dataset;
  indexingResult.copyFile = false;
  indexingResult.overwriteExistingDataset = false;
  this->Database->insert(QList<ctkDICOMDatabase::IndexingResult>() << indexingResult);
}

// ----------------------------------------------------------------------------
QString ctkDICOMTableManagerTester::patientUID(const QString& patientID)
{
  foreach (const QString& uid, this->Database->patients())
  {
    if (this->Database->fieldForPatient("PatientID", uid) == patientID)
    {
      return uid;
    }
  }
  return QString();
}

// ----------------------------------------------------------------------------
void ctkDICOMTableManagerTester::selectSeries(const QString& patientID, const QString& studyUID, const QString& seriesUID)
{
  this->TableManager->setCurrentPatientsSelection(QStringList() << this->patientUID(patientID));
  this->TableManager->setCurrentStudiesSelection(QStringList() << studyUID);
  this->TableManager->setCurrentSeriesSelection(QStringList() << seriesUID);
  QCOMPARE(this->TableManager->currentPatientsSelection(), QStringList() << this->patientUID(patientID));
  QCOMPARE(this->TableManager->currentStudiesSelection(), QStringList() << studyUID);
  QCOMPARE(this->TableManager->currentSeriesSelection(), QStringList() << seriesUID);
}

// ----------------------------------------------------------------------------
void ctkDICOMTableManagerTester::testRemoveUnselectedKeepsSelection()
{
  QString patient1UID = this->patientUID("Patient1");
  QSignalSpy seriesSelectionSpy(this->TableManager, SIGNAL(seriesSelectionChanged(QStringList)));

  // Removing an unselected series of the selected study
  this->Database->removeSeries("1.2.3.1.2");
  this->TableManager->updateTableViews();
  QCOMPARE(this->TableManager->currentPatientsSelection(), QStringList() << patient1UID);
  QCOMPARE(this->TableManager->currentStudiesSelection(), QStringList() << "1.2.3.1");
  QCOMPARE(this->TableManager->currentSeriesSelection(), QStringList() << "1.2.3.1.1");

  // Removing an unrelated patient
  this->Database->removePatient(this->patientUID("Patient2"));
  this->TableManager->updateTableViews();
  QCOMPARE(this->TableManager->currentPatientsSelection(), QStringList() << patient1UID);
  QCOMPARE(this->TableManager->currentStudiesSelection(), QStringList() << "1.2.3.1");
  QCOMPARE(this->TableManager->currentSeriesSelection(), QStringList() << "1.2.3.1.1");

  // Refresh due to database content change
  QMetaObject::invokeMethod(this->Database, "databaseChanged");
  QCOMPARE(this->TableManager->currentPatientsSelection(), QStringList() << patient1UID);
  QCOMPARE(this->TableManager->currentStudiesSelection(), QStringList() << "1.2.3.1");
  QCOMPARE(this->TableManager->currentSeriesSelection(), QStringList() << "1.2.3.1.1");

  // Series selection did not change, so no notification is expected
  QCOMPARE(seriesSelectionSpy.count(), 0);
}

// ----------------------------------------------------------------------------
void ctkDICOMTableManagerTester::testRemoveSelectedSeriesClearsSelection()
{
  QSignalSpy seriesSelectionSpy(this->TableManager, SIGNAL(seriesSelectionChanged(QStringList)));

  this->Database->removeSeries("1.2.3.1.1");
  this->TableManager->updateTableViews();

  QCOMPARE(this->TableManager->currentSeriesSelection(), QStringList());
  QVERIFY(seriesSelectionSpy.count() > 0);
  QCOMPARE(seriesSelectionSpy.last().at(0).toStringList(), QStringList());
}

// ----------------------------------------------------------------------------
void ctkDICOMTableManagerTester::testRemoveSelectedPatientClearsSelection()
{
  QSignalSpy patientsSelectionSpy(this->TableManager, SIGNAL(patientsSelectionChanged(QStringList)));
  QSignalSpy seriesSelectionSpy(this->TableManager, SIGNAL(seriesSelectionChanged(QStringList)));

  this->Database->removePatient(this->patientUID("Patient1"));
  this->TableManager->updateTableViews();

  QCOMPARE(this->TableManager->currentPatientsSelection(), QStringList());
  QCOMPARE(this->TableManager->currentSeriesSelection(), QStringList());
  QVERIFY(patientsSelectionSpy.count() > 0);
  QCOMPARE(patientsSelectionSpy.last().at(0).toStringList(), QStringList());
  QVERIFY(seriesSelectionSpy.count() > 0);
  QCOMPARE(seriesSelectionSpy.last().at(0).toStringList(), QStringList());
}

// ----------------------------------------------------------------------------
void ctkDICOMTableManagerTester::testRemoveWithPatientFilterKeepsTableContent()
{
  // Filtering patients clears the patient selection and shows the studies and series of all matching patients
  this->TableManager->patientsTable()->setFilterText("Patient1");
  QCOMPARE(this->TableManager->currentPatientsSelection(), QStringList());
  QCOMPARE(this->TableManager->studiesTable()->uidsForAllRows(), QStringList() << "1.2.3.1");
  QStringList seriesUIDs = this->TableManager->seriesTable()->uidsForAllRows();
  seriesUIDs.sort();
  QCOMPARE(seriesUIDs, QStringList() << "1.2.3.1.1" << "1.2.3.1.2");

  // Removing an unrelated patient does not change table content
  this->Database->removePatient(this->patientUID("Patient2"));
  this->TableManager->updateTableViews();
  QCOMPARE(this->TableManager->studiesTable()->uidsForAllRows(), QStringList() << "1.2.3.1");
  seriesUIDs = this->TableManager->seriesTable()->uidsForAllRows();
  seriesUIDs.sort();
  QCOMPARE(seriesUIDs, QStringList() << "1.2.3.1.1" << "1.2.3.1.2");

  // Removing a displayed series only removes that series from the table
  this->Database->removeSeries("1.2.3.1.2");
  this->TableManager->updateTableViews();
  QCOMPARE(this->TableManager->studiesTable()->uidsForAllRows(), QStringList() << "1.2.3.1");
  QCOMPARE(this->TableManager->seriesTable()->uidsForAllRows(), QStringList() << "1.2.3.1.1");

  // Refresh due to database content change
  QMetaObject::invokeMethod(this->Database, "databaseChanged");
  QCOMPARE(this->TableManager->studiesTable()->uidsForAllRows(), QStringList() << "1.2.3.1");
  QCOMPARE(this->TableManager->seriesTable()->uidsForAllRows(), QStringList() << "1.2.3.1.1");
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkDICOMTableManagerTest)
#include "ctkDICOMTableManagerTest.moc"
