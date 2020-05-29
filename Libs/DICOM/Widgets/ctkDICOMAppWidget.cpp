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

#include <dcmtk/dcmimgle/dcmimage.h>

// Qt includes
#include <QAction>
#include <QCoreApplication>
#include <QCheckBox>
#include <QDebug>
#include <QMessageBox>
#include <QMetaType>
#include <QModelIndex>
#include <QPersistentModelIndex>
#include <QProgressDialog>
#include <QSettings>
#include <QSlider>
#include <QTabBar>
#include <QTimer>
#include <QTreeView>

// ctkWidgets includes
#include "ctkDirectoryButton.h"
#include "ctkFileDialog.h"

// ctkDICOMCore includes
#include "ctkDICOMBrowser.h"
#include "ctkDICOMDatabase.h"
#include "ctkDICOMFilterProxyModel.h"
#include "ctkDICOMIndexer.h"
#include "ctkDICOMModel.h"

// ctkDICOMWidgets includes
#include "ctkDICOMAppWidget.h"
#include "ctkDICOMThumbnailGenerator.h"
#include "ctkThumbnailLabel.h"
#include "ctkDICOMQueryResultsTabWidget.h"
#include "ctkDICOMQueryRetrieveWidget.h"
#include "ctkDICOMQueryWidget.h"

#include "ui_ctkDICOMAppWidget.h"

//logger
#include <ctkLogger.h>
static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMAppWidget");

Q_DECLARE_METATYPE(QPersistentModelIndex);

//----------------------------------------------------------------------------
class ctkDICOMAppWidgetPrivate: public Ui_ctkDICOMAppWidget
{
public:
  ctkDICOMAppWidget* const q_ptr;
  Q_DECLARE_PUBLIC(ctkDICOMAppWidget);

  ctkDICOMAppWidgetPrivate(ctkDICOMAppWidget* );
  ~ctkDICOMAppWidgetPrivate();

  ctkFileDialog* ImportDialog;
  ctkDICOMQueryRetrieveWidget* QueryRetrieveWidget;

  QSharedPointer<ctkDICOMDatabase> DICOMDatabase;
  QSharedPointer<ctkDICOMThumbnailGenerator> ThumbnailGenerator;
  ctkDICOMModel DICOMModel;
  ctkDICOMFilterProxyModel DICOMProxyModel;
  QSharedPointer<ctkDICOMIndexer> DICOMIndexer;
  QProgressDialog *IndexerProgress;
  QProgressDialog *UpdateSchemaProgress;

  void showIndexerDialog();
  void showUpdateSchemaDialog();

  // used when suspending the ctkDICOMModel
  QSqlDatabase EmptyDatabase;

  QTimer* AutoPlayTimer;

  bool IsSearchWidgetPopUpMode;

  // local count variables to keep track of the number of items
  // added to the database during an import operation
  bool DisplayImportSummary;
  int PatientsAddedDuringImport;
  int StudiesAddedDuringImport;
  int SeriesAddedDuringImport;
  int InstancesAddedDuringImport;
};

//----------------------------------------------------------------------------
// ctkDICOMAppWidgetPrivate methods

ctkDICOMAppWidgetPrivate::ctkDICOMAppWidgetPrivate(ctkDICOMAppWidget* parent): q_ptr(parent){
  DICOMDatabase = QSharedPointer<ctkDICOMDatabase> (new ctkDICOMDatabase);
  ThumbnailGenerator = QSharedPointer <ctkDICOMThumbnailGenerator> (new ctkDICOMThumbnailGenerator);
  DICOMDatabase->setThumbnailGenerator(ThumbnailGenerator.data());
  DICOMIndexer = QSharedPointer<ctkDICOMIndexer> (new ctkDICOMIndexer);
  DICOMIndexer->setDatabase(DICOMDatabase.data());
  IndexerProgress = 0;
  UpdateSchemaProgress = 0;
  DisplayImportSummary = true;
  PatientsAddedDuringImport = 0;
  StudiesAddedDuringImport = 0;
  SeriesAddedDuringImport = 0;
  InstancesAddedDuringImport = 0;
}

ctkDICOMAppWidgetPrivate::~ctkDICOMAppWidgetPrivate()
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

void ctkDICOMAppWidgetPrivate::showUpdateSchemaDialog()
{
  Q_Q(ctkDICOMAppWidget);
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

    //q->connect(UpdateSchemaProgress, SIGNAL(canceled()),
     //       DICOMIndexer.data(), SLOT(cancel()));

    q->connect(DICOMDatabase.data(), SIGNAL(schemaUpdateStarted(int)),
            UpdateSchemaProgress, SLOT(setMaximum(int)));
    q->connect(DICOMDatabase.data(), SIGNAL(schemaUpdateProgress(int)),
            UpdateSchemaProgress, SLOT(setValue(int)));
    q->connect(DICOMDatabase.data(), SIGNAL(schemaUpdateProgress(QString)),
            progressLabel, SLOT(setText(QString)));

    // close the dialog
    q->connect(DICOMDatabase.data(), SIGNAL(schemaUpdated()),
            UpdateSchemaProgress, SLOT(close()));
    // reset the database to show new data
    q->connect(DICOMDatabase.data(), SIGNAL(schemaUpdated()),
            &DICOMModel, SLOT(reset()));
    // reset the database if canceled
    q->connect(UpdateSchemaProgress, SIGNAL(canceled()),
            &DICOMModel, SLOT(reset()));
    }
  UpdateSchemaProgress->show();
}

void ctkDICOMAppWidgetPrivate::showIndexerDialog()
{
  Q_Q(ctkDICOMAppWidget);
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

    // QProgressDialog requires a rate limiter when used from another thread,
    // otherwise it crashes with stack overflow
    // (see https://bugreports.qt.io/browse/QTBUG-83265).
    // Since this feature is obsolete anyway (since ctkDICOMBrowser can now
    // has built-in progress reporting of background data import), we just disable it.
    // q->connect(DICOMIndexer.data(), SIGNAL(progress(int)),
    //        IndexerProgress, SLOT(setValue(int)));

    q->connect(DICOMIndexer.data(), SIGNAL(progressDetail(QString)),
            progressLabel, SLOT(setText(QString)));
    q->connect(DICOMIndexer.data(), SIGNAL(progressDetail(QString)),
            q, SLOT(onFileIndexed(QString)));

    // close the dialog
    q->connect(DICOMIndexer.data(), SIGNAL(indexingComplete(int,int,int,int)),
            q, SLOT(setIndexingResult(int,int,int,int)));
    // reset the database to show new data
    q->connect(DICOMIndexer.data(), SIGNAL(indexingComplete(int, int, int, int)),
            &DICOMModel, SLOT(reset()));
    // stop indexing and reset the database if canceled
    q->connect(IndexerProgress, SIGNAL(canceled()),
            DICOMIndexer.data(), SLOT(cancel()));
    q->connect(IndexerProgress, SIGNAL(canceled()),
            &DICOMModel, SLOT(reset()));

    // allow users of this widget to know that the process has finished
    q->connect(IndexerProgress, SIGNAL(canceled()),
            q, SIGNAL(directoryImported()));
    q->connect(DICOMIndexer.data(), SIGNAL(indexingComplete(int, int, int, int)),
            q, SIGNAL(directoryImported()));
    }
  IndexerProgress->show();
}

//----------------------------------------------------------------------------
// ctkDICOMAppWidget methods

//----------------------------------------------------------------------------
ctkDICOMAppWidget::ctkDICOMAppWidget(QWidget* _parent):Superclass(_parent),
    d_ptr(new ctkDICOMAppWidgetPrivate(this))
{
  Q_D(ctkDICOMAppWidget);

  d->setupUi(this);

  this->setSearchWidgetPopUpMode(false);

  //Hide image previewer buttons
  d->NextImageButton->hide();
  d->PrevImageButton->hide();
  d->NextSeriesButton->hide();
  d->PrevSeriesButton->hide();
  d->NextStudyButton->hide();
  d->PrevStudyButton->hide();

  //Enable sorting in tree view
  d->TreeView->setSortingEnabled(true);
  d->TreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
  d->DICOMProxyModel.setSourceModel(&d->DICOMModel);
  d->TreeView->setModel(&d->DICOMModel);

  d->ThumbnailsWidget->setThumbnailSize(
    QSize(d->ThumbnailWidthSlider->value(), d->ThumbnailWidthSlider->value()));

  // Treeview signals
  connect(d->TreeView, SIGNAL(collapsed(QModelIndex)), this, SLOT(onTreeCollapsed(QModelIndex)));
  connect(d->TreeView, SIGNAL(expanded(QModelIndex)), this, SLOT(onTreeExpanded(QModelIndex)));

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
  connect(d->TreeView, SIGNAL(clicked(QModelIndex)), d->ThumbnailsWidget, SLOT(addThumbnails(QModelIndex)));
  connect(d->TreeView, SIGNAL(clicked(QModelIndex)), d->ImagePreview, SLOT(onModelSelected(QModelIndex)));
  connect(d->TreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(onModelSelected(QModelIndex)));

  connect(d->ThumbnailsWidget, SIGNAL(selected(ctkThumbnailLabel)), this, SLOT(onThumbnailSelected(ctkThumbnailLabel)));
  connect(d->ThumbnailsWidget, SIGNAL(doubleClicked(ctkThumbnailLabel)), this, SLOT(onThumbnailDoubleClicked(ctkThumbnailLabel)));
  connect(d->ImportDialog, SIGNAL(fileSelected(QString)),this,SLOT(onImportDirectory(QString)));

  connect(d->QueryRetrieveWidget, SIGNAL(canceled()), d->QueryRetrieveWidget, SLOT(hide()) );
  connect(d->QueryRetrieveWidget, SIGNAL(canceled()), this, SLOT(onQueryRetrieveFinished()) );

  connect(d->ImagePreview, SIGNAL(requestNextImage()), this, SLOT(onNextImage()));
  connect(d->ImagePreview, SIGNAL(requestPreviousImage()), this, SLOT(onPreviousImage()));
  connect(d->ImagePreview, SIGNAL(imageDisplayed(int,int)), this, SLOT(onImagePreviewDisplayed(int,int)));

  connect(d->SearchOption, SIGNAL(parameterChanged()), this, SLOT(onSearchParameterChanged()));

  connect(d->PlaySlider, SIGNAL(valueChanged(int)), d->ImagePreview, SLOT(displayImage(int)));
}

//----------------------------------------------------------------------------
ctkDICOMAppWidget::~ctkDICOMAppWidget()
{
  Q_D(ctkDICOMAppWidget);

  d->QueryRetrieveWidget->deleteLater();
  d->ImportDialog->deleteLater();
}

//----------------------------------------------------------------------------
bool ctkDICOMAppWidget::displayImportSummary()
{
  Q_D(ctkDICOMAppWidget);

  return d->DisplayImportSummary;
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::setDisplayImportSummary(bool onOff)
{
  Q_D(ctkDICOMAppWidget);

  d->DisplayImportSummary = onOff;
}

//----------------------------------------------------------------------------
int ctkDICOMAppWidget::patientsAddedDuringImport()
{
  Q_D(ctkDICOMAppWidget);

  return d->PatientsAddedDuringImport;
}

//----------------------------------------------------------------------------
int ctkDICOMAppWidget::studiesAddedDuringImport()
{
  Q_D(ctkDICOMAppWidget);

  return d->StudiesAddedDuringImport;
}

//----------------------------------------------------------------------------
int ctkDICOMAppWidget::seriesAddedDuringImport()
{
  Q_D(ctkDICOMAppWidget);

  return d->SeriesAddedDuringImport;
}

//----------------------------------------------------------------------------
int ctkDICOMAppWidget::instancesAddedDuringImport()
{
  Q_D(ctkDICOMAppWidget);

  return d->InstancesAddedDuringImport;
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::updateDatabaseSchemaIfNeeded()
{

  Q_D(ctkDICOMAppWidget);

  d->showUpdateSchemaDialog();
  d->DICOMDatabase->updateSchemaIfNeeded();
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::setDatabaseDirectory(const QString& directory)
{
  Q_D(ctkDICOMAppWidget);

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
  catch (std::exception &)
    {
    std::cerr << "Database error: " << qPrintable(d->DICOMDatabase->lastError()) << "\n";
    d->DICOMDatabase->closeDatabase();
    return;
    }

  // update the database schema if needed and provide progress
  this->updateDatabaseSchemaIfNeeded();

  d->DICOMModel.setDatabase(d->DICOMDatabase->database());
  d->DICOMModel.setEndLevel(ctkDICOMModel::SeriesType);
  d->TreeView->resizeColumnToContents(0);

  //pass DICOM database instance to Import widget
  // d->ImportDialog->setDICOMDatabase(d->DICOMDatabase);
  d->QueryRetrieveWidget->setRetrieveDatabase(d->DICOMDatabase);

  // update the button and let any connected slots know about the change
  d->DirectoryButton->setDirectory(directory);
  d->ThumbnailsWidget->setDatabaseDirectory(directory);
  d->ImagePreview->setDatabaseDirectory(directory);
  emit databaseDirectoryChanged(directory);
}

//----------------------------------------------------------------------------
QString ctkDICOMAppWidget::databaseDirectory() const
{
  QSettings settings;
  return settings.value("DatabaseDirectory").toString();
}

//----------------------------------------------------------------------------
bool ctkDICOMAppWidget::searchWidgetPopUpMode(){
  Q_D(ctkDICOMAppWidget);
  return d->IsSearchWidgetPopUpMode;
}

//------------------------------------------------------------------------------
void ctkDICOMAppWidget::setTagsToPrecache( const QStringList tags)
{
  Q_D(ctkDICOMAppWidget);
  d->DICOMDatabase->setTagsToPrecache(tags);
}

//------------------------------------------------------------------------------
const QStringList ctkDICOMAppWidget::tagsToPrecache()
{
  Q_D(ctkDICOMAppWidget);
  return d->DICOMDatabase->tagsToPrecache();
}



//----------------------------------------------------------------------------
ctkDICOMDatabase* ctkDICOMAppWidget::database(){
  Q_D(ctkDICOMAppWidget);
  return d->DICOMDatabase.data();
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::setSearchWidgetPopUpMode(bool flag){
  Q_D(ctkDICOMAppWidget);

  if(flag)
    {
    d->SearchDockWidget->setTitleBarWidget(0);
    d->SearchPopUpButton->show();
    d->SearchDockWidget->hide();
    d->SearchDockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    connect(d->SearchDockWidget, SIGNAL(topLevelChanged(bool)), this, SLOT(onSearchWidgetTopLevelChanged(bool)));
    connect(d->SearchPopUpButton, SIGNAL(clicked()), this, SLOT(onSearchPopUpButtonClicked()));
    }
  else
    {
    d->SearchDockWidget->setTitleBarWidget(new QWidget());
    d->SearchPopUpButton->hide();
    d->SearchDockWidget->show();
    d->SearchDockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    disconnect(d->SearchDockWidget, SIGNAL(topLevelChanged(bool)), this, SLOT(onSearchWidgetTopLevelChanged(bool)));
    disconnect(d->SearchPopUpButton, SIGNAL(clicked()), this, SLOT(onSearchPopUpButtonClicked()));
    }

  d->IsSearchWidgetPopUpMode = flag;
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onFileIndexed(const QString& filePath)
{
  // Update the progress dialog when the file name changes
  // - also allows for cancel button

  // QProgressDialog requires a rate limiter when used from another thread,
  // otherwise it crashes with stack overflow
  // (see https://bugreports.qt.io/browse/QTBUG-83265).
  // Since this feature is obsolete anyway (since ctkDICOMBrowser can now
  // has built-in progress reporting of background data import), we just disable it.
  //QCoreApplication::instance()->processEvents();

  qDebug() << "Indexing \n\n\n\n" << filePath <<"\n\n\n";
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::openImportDialog()
{
  Q_D(ctkDICOMAppWidget);

  d->ImportDialog->show();
  d->ImportDialog->raise();
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::openExportDialog()
{

}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::openQueryDialog()
{
  Q_D(ctkDICOMAppWidget);

  d->QueryRetrieveWidget->show();
  d->QueryRetrieveWidget->raise();

}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onQueryRetrieveFinished()
{
  Q_D(ctkDICOMAppWidget);
  d->DICOMModel.reset();
  emit this->queryRetrieveFinished();
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onRemoveAction()
{
  Q_D(ctkDICOMAppWidget);

  //d->QueryRetrieveWidget->show();
  // d->QueryRetrieveWidget->raise();
  std::cout << "on remove" << std::endl;
  QModelIndexList selection = d->TreeView->selectionModel()->selectedIndexes();
  std::cout << selection.size() << std::endl;
  QModelIndex index;
  foreach(index,selection)
  {
    QModelIndex index0 = index.sibling(index.row(), 0);
    if ( d->DICOMModel.data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::SeriesType))
    {
      QString seriesUID = d->DICOMModel.data(index0,ctkDICOMModel::UIDRole).toString();
      d->DICOMDatabase->removeSeries(seriesUID);
    }
    else if ( d->DICOMModel.data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::StudyType))
    {
      QString studyUID = d->DICOMModel.data(index0,ctkDICOMModel::UIDRole).toString();
      d->DICOMDatabase->removeStudy(studyUID);
    }
    else if ( d->DICOMModel.data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::PatientType))
    {
      QString patientUID = d->DICOMModel.data(index0,ctkDICOMModel::UIDRole).toString();
      d->DICOMDatabase->removePatient(patientUID);
    }
  }
  d->DICOMModel.reset();
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::suspendModel()
{
  Q_D(ctkDICOMAppWidget);

  d->DICOMModel.setDatabase(d->EmptyDatabase);
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::resumeModel()
{
  Q_D(ctkDICOMAppWidget);

  d->DICOMModel.setDatabase(d->DICOMDatabase->database());
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::resetModel()
{
  Q_D(ctkDICOMAppWidget);

  d->DICOMModel.reset();
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onThumbnailSelected(const ctkThumbnailLabel& widget)
{
    Q_D(ctkDICOMAppWidget);

  QModelIndex index = widget.property("sourceIndex").value<QPersistentModelIndex>();
  if(index.isValid())
    {
    d->ImagePreview->onModelSelected(index);
    }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onThumbnailDoubleClicked(const ctkThumbnailLabel& widget)
{
    Q_D(ctkDICOMAppWidget);

    QModelIndex index = widget.property("sourceIndex").value<QPersistentModelIndex>();

    if(!index.isValid())
      {
      return;
      }

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));
    QModelIndex index0 = index.sibling(index.row(), 0);

    if(model && (model->data(index0,ctkDICOMModel::TypeRole) != static_cast<int>(ctkDICOMModel::ImageType)))
      {
        this->onModelSelected(index0);
        d->TreeView->setCurrentIndex(index0);
        d->ThumbnailsWidget->addThumbnails(index0);
        d->ImagePreview->onModelSelected(index0);
      }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::setIndexingResult(int patientsAdded, int studiesAdded, int seriesAdded, int imagesAdded)
{
  Q_D(ctkDICOMAppWidget);
  d->PatientsAddedDuringImport = patientsAdded;
  d->StudiesAddedDuringImport = studiesAdded;
  d->SeriesAddedDuringImport = seriesAdded;
  d->InstancesAddedDuringImport = imagesAdded;
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onImportDirectory(QString directory)
{
  Q_D(ctkDICOMAppWidget);
  if (QDir(directory).exists())
    {
    QCheckBox* copyOnImport = qobject_cast<QCheckBox*>(d->ImportDialog->bottomWidget());
    QString targetDirectory;
    bool copyFiles = (copyOnImport->checkState() == Qt::Checked);

    // reset counts
    d->PatientsAddedDuringImport = 0;
    d->StudiesAddedDuringImport = 0;
    d->SeriesAddedDuringImport = 0;
    d->InstancesAddedDuringImport = 0;

    // show progress dialog and perform indexing
    d->showIndexerDialog();

    d->DICOMIndexer->addDirectory(directory, copyFiles);

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
void ctkDICOMAppWidget::onModelSelected(const QModelIndex &index){
Q_D(ctkDICOMAppWidget);

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

    if(model)
      {
        QModelIndex index0 = index.sibling(index.row(), 0);

        if ( model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::PatientType) )
          {
          d->NextImageButton->show();
          d->PrevImageButton->show();
          d->NextSeriesButton->show();
          d->PrevSeriesButton->show();
          d->NextStudyButton->show();
          d->PrevStudyButton->show();
          }
        else if ( model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::StudyType) )
          {
          d->NextImageButton->show();
          d->PrevImageButton->show();
          d->NextSeriesButton->show();
          d->PrevSeriesButton->show();
          d->NextStudyButton->hide();
          d->PrevStudyButton->hide();
          }
        else if ( model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::SeriesType) )
          {
          d->NextImageButton->show();
          d->PrevImageButton->show();
          d->NextSeriesButton->hide();
          d->PrevSeriesButton->hide();
          d->NextStudyButton->hide();
          d->PrevStudyButton->hide();
          }
        else
          {
          d->NextImageButton->hide();
          d->PrevImageButton->hide();
          d->NextSeriesButton->hide();
          d->PrevSeriesButton->hide();
          d->NextStudyButton->hide();
          d->PrevStudyButton->hide();
          }
        d->ActionRemove->setEnabled(
            model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::SeriesType) ||
            model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::StudyType) ||
            model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::PatientType) );
        }

      else
        {
        d->NextImageButton->hide();
        d->PrevImageButton->hide();
        d->NextSeriesButton->hide();
        d->PrevSeriesButton->hide();
        d->NextStudyButton->hide();
        d->PrevStudyButton->hide();
        d->ActionRemove->setEnabled(false);
        }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onNextImage(){
    Q_D(ctkDICOMAppWidget);

    QModelIndex currentIndex = d->ImagePreview->currentImageIndex();

    if(currentIndex.isValid())
      {
      ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(currentIndex.model()));

      if(model)
        {
        QModelIndex seriesIndex = currentIndex.parent();

        int imageCount = model->rowCount(seriesIndex);
        int imageID = currentIndex.row();

        imageID = (imageID+1)%imageCount;

        int max = d->PlaySlider->maximum();
        if(imageID > 0 && imageID < max)
          {
            d->PlaySlider->setValue(imageID);
          }

        QModelIndex nextIndex = currentIndex.sibling(imageID, 0);

        d->ImagePreview->onModelSelected(nextIndex);
        d->ThumbnailsWidget->selectThumbnailFromIndex(nextIndex);
        }
      }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onPreviousImage(){
    Q_D(ctkDICOMAppWidget);

    QModelIndex currentIndex = d->ImagePreview->currentImageIndex();

    if(currentIndex.isValid())
      {
      ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(currentIndex.model()));

      if(model)
        {
        QModelIndex seriesIndex = currentIndex.parent();

        int imageCount = model->rowCount(seriesIndex);
        int imageID = currentIndex.row();

        imageID--;
        if(imageID < 0) imageID += imageCount;

        int max = d->PlaySlider->maximum();
        if(imageID > 0 && imageID < max)
          {
            d->PlaySlider->setValue(imageID);
          }

        QModelIndex prevIndex = currentIndex.sibling(imageID, 0);

        d->ImagePreview->onModelSelected(prevIndex);
        d->ThumbnailsWidget->selectThumbnailFromIndex(prevIndex);
        }
      }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onNextSeries(){
    Q_D(ctkDICOMAppWidget);

    QModelIndex currentIndex = d->ImagePreview->currentImageIndex();

    if(currentIndex.isValid())
      {
      ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(currentIndex.model()));

      if(model)
        {
        QModelIndex seriesIndex = currentIndex.parent();
        QModelIndex studyIndex = seriesIndex.parent();

        int seriesCount = model->rowCount(studyIndex);
        int seriesID = seriesIndex.row();

        seriesID = (seriesID + 1)%seriesCount;

        QModelIndex nextIndex = seriesIndex.sibling(seriesID, 0);

        d->ImagePreview->onModelSelected(nextIndex);
        d->ThumbnailsWidget->selectThumbnailFromIndex(nextIndex);
        }
      }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onPreviousSeries(){
    Q_D(ctkDICOMAppWidget);

    QModelIndex currentIndex = d->ImagePreview->currentImageIndex();

    if(currentIndex.isValid())
      {
      ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(currentIndex.model()));

      if(model)
        {
        QModelIndex seriesIndex = currentIndex.parent();
        QModelIndex studyIndex = seriesIndex.parent();

        int seriesCount = model->rowCount(studyIndex);
        int seriesID = seriesIndex.row();

        seriesID--;
        if(seriesID < 0) seriesID += seriesCount;

        QModelIndex prevIndex = seriesIndex.sibling(seriesID, 0);

        d->ImagePreview->onModelSelected(prevIndex);
        d->ThumbnailsWidget->selectThumbnailFromIndex(prevIndex);
        }
      }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onNextStudy(){
    Q_D(ctkDICOMAppWidget);

    QModelIndex currentIndex = d->ImagePreview->currentImageIndex();

    if(currentIndex.isValid())
      {
      ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(currentIndex.model()));

      if(model)
        {
        QModelIndex seriesIndex = currentIndex.parent();
        QModelIndex studyIndex = seriesIndex.parent();
        QModelIndex patientIndex = studyIndex.parent();

        int studyCount = model->rowCount(patientIndex);
        int studyID = studyIndex.row();

        studyID = (studyID + 1)%studyCount;

        QModelIndex nextIndex = studyIndex.sibling(studyID, 0);

        d->ImagePreview->onModelSelected(nextIndex);
        d->ThumbnailsWidget->selectThumbnailFromIndex(nextIndex);
        }
      }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onPreviousStudy(){
    Q_D(ctkDICOMAppWidget);

    QModelIndex currentIndex = d->ImagePreview->currentImageIndex();

    if(currentIndex.isValid())
      {
      ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(currentIndex.model()));

      if(model)
        {
        QModelIndex seriesIndex = currentIndex.parent();
        QModelIndex studyIndex = seriesIndex.parent();
        QModelIndex patientIndex = studyIndex.parent();

        int studyCount = model->rowCount(patientIndex);
        int studyID = studyIndex.row();

        studyID--;
        if(studyID < 0) studyID += studyCount;

        QModelIndex prevIndex = studyIndex.sibling(studyID, 0);

        d->ImagePreview->onModelSelected(prevIndex);
        d->ThumbnailsWidget->selectThumbnailFromIndex(prevIndex);
        }
      }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onTreeCollapsed(const QModelIndex &index){
    Q_UNUSED(index);
    Q_D(ctkDICOMAppWidget);
    d->TreeView->resizeColumnToContents(0);
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onTreeExpanded(const QModelIndex &index){
    Q_UNUSED(index);
    Q_D(ctkDICOMAppWidget);
    d->TreeView->resizeColumnToContents(0);
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onAutoPlayCheckboxStateChanged(int state){
    Q_D(ctkDICOMAppWidget);

    if(state == 0) //OFF
      {
      disconnect(d->AutoPlayTimer, SIGNAL(timeout()), this, SLOT(onAutoPlayTimer()));
      d->AutoPlayTimer->deleteLater();
      }
    else if(state == 2) //ON
      {
      d->AutoPlayTimer = new QTimer(this);
      connect(d->AutoPlayTimer, SIGNAL(timeout()), this, SLOT(onAutoPlayTimer()));
      d->AutoPlayTimer->start(50);
      }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onAutoPlayTimer(){
    this->onNextImage();
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onThumbnailWidthSliderValueChanged(int val){
  Q_D(ctkDICOMAppWidget);
  d->ThumbnailsWidget->setThumbnailSize(QSize(val, val));
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onSearchParameterChanged(){
  Q_D(ctkDICOMAppWidget);
  d->DICOMModel.setDatabase(d->DICOMDatabase->database(), d->SearchOption->parameters());

  this->onModelSelected(d->DICOMModel.index(0,0));
  d->ThumbnailsWidget->clearThumbnails();
  d->ThumbnailsWidget->addThumbnails(d->DICOMModel.index(0,0));
  d->ImagePreview->clearImages();
  d->ImagePreview->onModelSelected(d->DICOMModel.index(0,0));
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onImagePreviewDisplayed(int imageID, int count){
  Q_D(ctkDICOMAppWidget);

  d->PlaySlider->setMinimum(0);
  d->PlaySlider->setMaximum(count-1);
  d->PlaySlider->setValue(imageID);
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onSearchPopUpButtonClicked(){
  Q_D(ctkDICOMAppWidget);

  if(d->SearchDockWidget->isFloating())
    {
    d->SearchDockWidget->hide();
    d->SearchDockWidget->setFloating(false);
    }
  else
    {
    d->SearchDockWidget->setFloating(true);
    d->SearchDockWidget->adjustSize();
    d->SearchDockWidget->show();
    }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onSearchWidgetTopLevelChanged(bool topLevel){
  Q_D(ctkDICOMAppWidget);

  if(topLevel)
    {
    d->SearchDockWidget->show();
    }
  else
    {
    d->SearchDockWidget->hide();
    }
}
