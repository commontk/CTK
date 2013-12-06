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

// std includes
#include <iostream>

// Qt includes
#include <QAction>
#include <QCoreApplication>
#include <QCheckBox>
#include <QDebug>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSettings>

// ctkWidgets includes
#include "ctkDirectoryButton.h"
#include "ctkFileDialog.h"

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMIndexer.h"

// ctkDICOMWidgets includes
#include "ctkDICOMBrowser.h"
#include "ctkDICOMQueryResultsTabWidget.h"
#include "ctkDICOMQueryRetrieveWidget.h"
#include "ctkDICOMQueryWidget.h"

#include "ui_ctkDICOMBrowser.h"

//logger
#include <ctkLogger.h>
static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMBrowser");

//----------------------------------------------------------------------------
class ctkDICOMBrowserPrivate: public Ui_ctkDICOMBrowser
{
public:
  ctkDICOMBrowser* const q_ptr;
  Q_DECLARE_PUBLIC(ctkDICOMBrowser);

  ctkDICOMBrowserPrivate(ctkDICOMBrowser* );
  ~ctkDICOMBrowserPrivate();

  ctkFileDialog* ImportDialog;
  ctkDICOMQueryRetrieveWidget* QueryRetrieveWidget;

  QSharedPointer<ctkDICOMDatabase> DICOMDatabase;
  QSharedPointer<ctkDICOMIndexer> DICOMIndexer;
  QProgressDialog *IndexerProgress;
  QProgressDialog *UpdateSchemaProgress;

  void showIndexerDialog();
  void showUpdateSchemaDialog();

  // used when suspending the ctkDICOMModel
  QSqlDatabase EmptyDatabase;

  // local count variables to keep track of the number of items
  // added to the database during an import operation
  bool DisplayImportSummary;
  int PatientsAddedDuringImport;
  int StudiesAddedDuringImport;
  int SeriesAddedDuringImport;
  int InstancesAddedDuringImport;
};

//----------------------------------------------------------------------------
// ctkDICOMBrowserPrivate methods

ctkDICOMBrowserPrivate::ctkDICOMBrowserPrivate(ctkDICOMBrowser* parent): q_ptr(parent){
  DICOMDatabase = QSharedPointer<ctkDICOMDatabase> (new ctkDICOMDatabase);
  DICOMIndexer = QSharedPointer<ctkDICOMIndexer> (new ctkDICOMIndexer);
  IndexerProgress = 0;
  UpdateSchemaProgress = 0;
  DisplayImportSummary = true;
  PatientsAddedDuringImport = 0;
  StudiesAddedDuringImport = 0;
  SeriesAddedDuringImport = 0;
  InstancesAddedDuringImport = 0;
}

ctkDICOMBrowserPrivate::~ctkDICOMBrowserPrivate()
{
  if ( IndexerProgress )
    {
    delete IndexerProgress;
    }
  if ( UpdateSchemaProgress )
    {
    delete UpdateSchemaProgress;
    }
}

void ctkDICOMBrowserPrivate::showUpdateSchemaDialog()
{
  Q_Q(ctkDICOMBrowser);
  if (UpdateSchemaProgress == 0)
    {
    //
    // Set up the Update Schema Progress Dialog
    //
    UpdateSchemaProgress = new QProgressDialog(
        q->tr("DICOM Schema Update"), "Cancel", 0, 100, q,
         Qt::WindowTitleHint | Qt::WindowSystemMenuHint);

    // We don't want the progress dialog to resize itself, so we bypass the label
    // by creating our own
    QLabel* progressLabel = new QLabel(q->tr("Initialization..."));
    UpdateSchemaProgress->setLabel(progressLabel);
    UpdateSchemaProgress->setWindowModality(Qt::ApplicationModal);
    UpdateSchemaProgress->setMinimumDuration(0);
    UpdateSchemaProgress->setValue(0);

    q->connect(DICOMDatabase.data(), SIGNAL(schemaUpdateStarted(int)),
            UpdateSchemaProgress, SLOT(setMaximum(int)));
    q->connect(DICOMDatabase.data(), SIGNAL(schemaUpdateProgress(int)),
            UpdateSchemaProgress, SLOT(setValue(int)));
    q->connect(DICOMDatabase.data(), SIGNAL(schemaUpdateProgress(QString)),
            progressLabel, SLOT(setText(QString)));

    // close the dialog
    q->connect(DICOMDatabase.data(), SIGNAL(schemaUpdated()),
            UpdateSchemaProgress, SLOT(close()));
    }
  UpdateSchemaProgress->show();
}

void ctkDICOMBrowserPrivate::showIndexerDialog()
{
  Q_Q(ctkDICOMBrowser);
  if (IndexerProgress == 0)
    {
    //
    // Set up the Indexer Progress Dialog
    //
    IndexerProgress = new QProgressDialog( q->tr("DICOM Import"), "Cancel", 0, 100, q,
         Qt::WindowTitleHint | Qt::WindowSystemMenuHint);

    // We don't want the progress dialog to resize itself, so we bypass the label
    // by creating our own
    QLabel* progressLabel = new QLabel(q->tr("Initialization..."));
    IndexerProgress->setLabel(progressLabel);
    IndexerProgress->setWindowModality(Qt::ApplicationModal);
    IndexerProgress->setMinimumDuration(0);
    IndexerProgress->setValue(0);

    q->connect(IndexerProgress, SIGNAL(canceled()), 
                 DICOMIndexer.data(), SLOT(cancel()));

    q->connect(DICOMIndexer.data(), SIGNAL(progress(int)),
            IndexerProgress, SLOT(setValue(int)));
    q->connect(DICOMIndexer.data(), SIGNAL(indexingFilePath(QString)),
            progressLabel, SLOT(setText(QString)));
    q->connect(DICOMIndexer.data(), SIGNAL(indexingFilePath(QString)),
            q, SLOT(onFileIndexed(QString)));

    // close the dialog
    q->connect(DICOMIndexer.data(), SIGNAL(indexingComplete()),
            IndexerProgress, SLOT(close()));
    // stop indexing and reset the database if canceled
    q->connect(IndexerProgress, SIGNAL(canceled()), 
            DICOMIndexer.data(), SLOT(cancel()));

    // allow users of this widget to know that the process has finished
    q->connect(IndexerProgress, SIGNAL(canceled()), 
            q, SIGNAL(directoryImported()));
    q->connect(DICOMIndexer.data(), SIGNAL(indexingComplete()),
            q, SIGNAL(directoryImported()));
    }
  IndexerProgress->show();
}

//----------------------------------------------------------------------------
// ctkDICOMBrowser methods

//----------------------------------------------------------------------------
ctkDICOMBrowser::ctkDICOMBrowser(QWidget* _parent):Superclass(_parent),
    d_ptr(new ctkDICOMBrowserPrivate(this))
{
  Q_D(ctkDICOMBrowser);

  d->setupUi(this);

  // signals related to tracking inserts
  connect(d->DICOMDatabase.data(), SIGNAL(patientAdded(int,QString,QString,QString)), this,
                              SLOT(onPatientAdded(int,QString,QString,QString)));
  connect(d->DICOMDatabase.data(), SIGNAL(studyAdded(QString)), this, SLOT(onStudyAdded(QString)));
  connect(d->DICOMDatabase.data(), SIGNAL(seriesAdded(QString)), this, SLOT(onSeriesAdded(QString)));
  connect(d->DICOMDatabase.data(), SIGNAL(instanceAdded(QString)), this, SLOT(onInstanceAdded(QString)));


  //Set ToolBar button style
  d->ToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

  //Initialize Q/R widget
  d->QueryRetrieveWidget = new ctkDICOMQueryRetrieveWidget();
  d->QueryRetrieveWidget->setWindowModality ( Qt::ApplicationModal );

  //initialize directory from settings, then listen for changes
  QSettings settings;
  if ( settings.value("DatabaseDirectory", "") == "" )
    {
    QString directory = QString("./ctkDICOM-Database");
    settings.setValue("DatabaseDirectory", directory);
    settings.sync();
    }
  QString databaseDirectory = settings.value("DatabaseDirectory").toString();
  this->setDatabaseDirectory(databaseDirectory);
  d->DirectoryButton->setDirectory(databaseDirectory);

  d->dicomTableManager->setDICOMDatabase(d->DICOMDatabase.data());

  // TableView signals
  connect(d->dicomTableManager, SIGNAL(patientsSelectionChanged(const QItemSelection&, const QItemSelection&)),
          this, SLOT(onModelSelected(const QItemSelection&,const QItemSelection&)));
  connect(d->dicomTableManager, SIGNAL(studiesSelectionChanged(const QItemSelection&, const QItemSelection&)),
          this, SLOT(onModelSelected(const QItemSelection&,const QItemSelection&)));
  connect(d->dicomTableManager, SIGNAL(seriesSelectionChanged(const QItemSelection&, const QItemSelection&)),
          this, SLOT(onModelSelected(const QItemSelection&,const QItemSelection&)));

  connect(d->DirectoryButton, SIGNAL(directoryChanged(QString)), this, SLOT(setDatabaseDirectory(QString)));

  //Initialize import widget
  d->ImportDialog = new ctkFileDialog();
  QCheckBox* importCheckbox = new QCheckBox("Copy on import", d->ImportDialog);
  d->ImportDialog->setBottomWidget(importCheckbox);
  d->ImportDialog->setFileMode(QFileDialog::Directory);
  d->ImportDialog->setLabelText(QFileDialog::Accept,"Import");
  d->ImportDialog->setWindowTitle("Import DICOM files from directory ...");
  d->ImportDialog->setWindowModality(Qt::ApplicationModal);

  //connect signal and slots
  connect(d->ImportDialog, SIGNAL(fileSelected(QString)),this,SLOT(onImportDirectory(QString)));

  connect(d->QueryRetrieveWidget, SIGNAL(canceled()), d->QueryRetrieveWidget, SLOT(hide()) );
  connect(d->QueryRetrieveWidget, SIGNAL(canceled()), this, SLOT(onQueryRetrieveFinished()) );
}

//----------------------------------------------------------------------------
ctkDICOMBrowser::~ctkDICOMBrowser()
{
  Q_D(ctkDICOMBrowser);

  d->QueryRetrieveWidget->deleteLater();
  d->ImportDialog->deleteLater();
}

//----------------------------------------------------------------------------
bool ctkDICOMBrowser::displayImportSummary()
{
  Q_D(ctkDICOMBrowser);

  return d->DisplayImportSummary;
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::setDisplayImportSummary(bool onOff)
{
  Q_D(ctkDICOMBrowser);

  d->DisplayImportSummary = onOff;
}

//----------------------------------------------------------------------------
int ctkDICOMBrowser::patientsAddedDuringImport()
{
  Q_D(ctkDICOMBrowser);

  return d->PatientsAddedDuringImport;
}

//----------------------------------------------------------------------------
int ctkDICOMBrowser::studiesAddedDuringImport()
{
  Q_D(ctkDICOMBrowser);

  return d->StudiesAddedDuringImport;
}

//----------------------------------------------------------------------------
int ctkDICOMBrowser::seriesAddedDuringImport()
{
  Q_D(ctkDICOMBrowser);

  return d->SeriesAddedDuringImport;
}

//----------------------------------------------------------------------------
int ctkDICOMBrowser::instancesAddedDuringImport()
{
  Q_D(ctkDICOMBrowser);

  return d->InstancesAddedDuringImport;
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::updateDatabaseSchemaIfNeeded()
{

  Q_D(ctkDICOMBrowser);

  d->showUpdateSchemaDialog();
  d->DICOMDatabase->updateSchemaIfNeeded();
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::setDatabaseDirectory(const QString& directory)
{
  Q_D(ctkDICOMBrowser);

  QSettings settings;
  settings.setValue("DatabaseDirectory", directory);
  settings.sync();

  //close the active DICOM database
  d->DICOMDatabase->closeDatabase();

  //open DICOM database on the directory
  QString databaseFileName = directory + QString("/ctkDICOM.sql");
  try
    {
    d->DICOMDatabase->openDatabase( databaseFileName );
    }
  catch (std::exception e)
    {
    std::cerr << "Database error: " << qPrintable(d->DICOMDatabase->lastError()) << "\n";
    d->DICOMDatabase->closeDatabase();
    return;
    }

  // update the database schema if needed and provide progress
  this->updateDatabaseSchemaIfNeeded();

  //pass DICOM database instance to Import widget
  d->QueryRetrieveWidget->setRetrieveDatabase(d->DICOMDatabase);

  // update the button and let any connected slots know about the change
  d->DirectoryButton->setDirectory(directory);
  emit databaseDirectoryChanged(directory);
}

//----------------------------------------------------------------------------
QString ctkDICOMBrowser::databaseDirectory() const
{
  QSettings settings;
  return settings.value("DatabaseDirectory").toString();
}

//------------------------------------------------------------------------------
void ctkDICOMBrowser::setTagsToPrecache( const QStringList tags)
{
  Q_D(ctkDICOMBrowser);
  d->DICOMDatabase->setTagsToPrecache(tags);
}

//------------------------------------------------------------------------------
const QStringList ctkDICOMBrowser::tagsToPrecache()
{
  Q_D(ctkDICOMBrowser);
  return d->DICOMDatabase->tagsToPrecache();
}



//----------------------------------------------------------------------------
ctkDICOMDatabase* ctkDICOMBrowser::database(){
  Q_D(ctkDICOMBrowser);
  return d->DICOMDatabase.data();
}

//----------------------------------------------------------------------------
ctkDICOMTableManager* ctkDICOMBrowser::dicomTableManager()
{
  Q_D(ctkDICOMBrowser);
  return d->dicomTableManager;
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onFileIndexed(const QString& filePath)
{
  // Update the progress dialog when the file name changes
  // - also allows for cancel button
  QCoreApplication::instance()->processEvents();
  qDebug() << "Indexing \n\n\n\n" << filePath <<"\n\n\n";
  
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::openImportDialog()
{
  Q_D(ctkDICOMBrowser);

  d->ImportDialog->show();
  d->ImportDialog->raise();
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::openExportDialog()
{

}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::openQueryDialog()
{
  Q_D(ctkDICOMBrowser);

  d->QueryRetrieveWidget->show();
  d->QueryRetrieveWidget->raise();

}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onQueryRetrieveFinished()
{
  Q_D(ctkDICOMBrowser);
  emit this->queryRetrieveFinished();
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onRemoveAction()
{
  Q_D(ctkDICOMBrowser);
  QStringList selectedSeriesUIDs = d->dicomTableManager->currentSeriesSelection();

  foreach (const QString& uid, selectedSeriesUIDs)
    {
      d->DICOMDatabase->removeSeries(uid);
    }
  QStringList selectedStudiesUIDs = d->dicomTableManager->currentStudiesSelection();
  foreach (const QString& uid, selectedStudiesUIDs)
    {
      d->DICOMDatabase->removeStudy(uid);
    }
  QStringList selectedPatientUIDs = d->dicomTableManager->currentPatientsSelection();
  foreach (const QString& uid, selectedPatientUIDs)
    {
      d->DICOMDatabase->removePatient(uid);
    }
  // Update the table views
  d->dicomTableManager->updateTableViews();
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onPatientAdded(int databaseID, QString patientID, QString patientName, QString patientBirthDate )
{
  Q_D(ctkDICOMBrowser);
  Q_UNUSED(databaseID);
  Q_UNUSED(patientID);
  Q_UNUSED(patientName);
  Q_UNUSED(patientBirthDate);
  ++d->PatientsAddedDuringImport;
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onStudyAdded(QString studyUID)
{
  Q_D(ctkDICOMBrowser);
  Q_UNUSED(studyUID);
  ++d->StudiesAddedDuringImport;
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onSeriesAdded(QString seriesUID)
{
  Q_D(ctkDICOMBrowser);
  Q_UNUSED(seriesUID);
  ++d->SeriesAddedDuringImport;
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onInstanceAdded(QString instanceUID)
{
  Q_D(ctkDICOMBrowser);
  Q_UNUSED(instanceUID);
  ++d->InstancesAddedDuringImport;
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onImportDirectory(QString directory)
{
  Q_D(ctkDICOMBrowser);
  if (QDir(directory).exists())
    {
    QCheckBox* copyOnImport = qobject_cast<QCheckBox*>(d->ImportDialog->bottomWidget());
    QString targetDirectory;
    if (copyOnImport->checkState() == Qt::Checked)
      {
      targetDirectory = d->DICOMDatabase->databaseDirectory();
      }

    // reset counts
    d->PatientsAddedDuringImport = 0;
    d->StudiesAddedDuringImport = 0;
    d->SeriesAddedDuringImport = 0;
    d->InstancesAddedDuringImport = 0;

    // show progress dialog and perform indexing
    d->showIndexerDialog();
    d->DICOMIndexer->addDirectory(*d->DICOMDatabase,directory,targetDirectory);

    // display summary result
    if (d->DisplayImportSummary)
      {
      QString message = "Directory import completed.\n\n";
      message += QString("%1 New Patients\n").arg(QString::number(d->PatientsAddedDuringImport));
      message += QString("%1 New Studies\n").arg(QString::number(d->StudiesAddedDuringImport));
      message += QString("%1 New Series\n").arg(QString::number(d->SeriesAddedDuringImport));
      message += QString("%1 New Instances\n").arg(QString::number(d->InstancesAddedDuringImport));
      QMessageBox::information(this,"DICOM Directory Import", message);
      }
  }
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onModelSelected(const QItemSelection &item1, const QItemSelection &item2)
{
  Q_UNUSED(item1);
  Q_UNUSED(item2);
  Q_D(ctkDICOMBrowser);
  d->ActionRemove->setEnabled(true);
}
