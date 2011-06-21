/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// std includes
#include <iostream>

#include <dcmimage.h>

// Qt includes
#include <QDebug>
#include <QTreeView>
#include <QTabBar>
#include <QSettings>
#include <QAction>
#include <QModelIndex>
#include <QCheckBox>

// ctkWidgets includes
#include "ctkDirectoryButton.h"
#include "ctkFileDialog.h"

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMIndexer.h"
#include "ctkDICOMModel.h"
#include "ctkDICOMFilterProxyModel.h"

// ctkDICOMWidgets includes
#include "ctkDICOMAppWidget.h"
#include "ctkDICOMQueryResultsTabWidget.h"
#include "ctkDICOMQueryRetrieveWidget.h"
#include "ctkDICOMImportWidget.h"
#include "ctkDICOMThumbnailWidget.h"
#include "ctkDICOMThumbnailGenerator.h"

#include "ui_ctkDICOMAppWidget.h"

//logger
#include <ctkLogger.h>
static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMAppWidget");

//----------------------------------------------------------------------------
class ctkDICOMAppWidgetPrivate: public Ui_ctkDICOMAppWidget
{
public:
  ctkDICOMAppWidgetPrivate();

  ctkFileDialog* importDIalog;
  ctkDICOMQueryRetrieveWidget* QueryRetrieveWidget;

  QSharedPointer<ctkDICOMDatabase> dicomDatabase;
  QSharedPointer<ctkDICOMThumbnailGenerator> thumbnailGenerator;
  ctkDICOMModel dicomModel;
  ctkDICOMFilterProxyModel dicomProxyModel;
  QSharedPointer<ctkDICOMIndexer> dicomIndexer;

};

//----------------------------------------------------------------------------
// ctkDICOMAppWidgetPrivate methods

ctkDICOMAppWidgetPrivate::ctkDICOMAppWidgetPrivate(){
  dicomDatabase = QSharedPointer<ctkDICOMDatabase> (new ctkDICOMDatabase);
  thumbnailGenerator = QSharedPointer <ctkDICOMThumbnailGenerator> (new ctkDICOMThumbnailGenerator);
  dicomDatabase->setThumbnailGenerator(thumbnailGenerator.data());
  dicomIndexer = QSharedPointer<ctkDICOMIndexer> (new ctkDICOMIndexer);
  dicomIndexer->setThumbnailGenerator(thumbnailGenerator.data());
}

//----------------------------------------------------------------------------
// ctkDICOMAppWidget methods

//----------------------------------------------------------------------------
ctkDICOMAppWidget::ctkDICOMAppWidget(QWidget* _parent):Superclass(_parent), 
  d_ptr(new ctkDICOMAppWidgetPrivate)
{
  Q_D(ctkDICOMAppWidget);  

  d->setupUi(this);

  //Hide image previewer buttons
  d->nextImageButton->hide();
  d->prevImageButton->hide();
  d->nextSeriesButton->hide();
  d->prevSeriesButton->hide();
  d->nextStudyButton->hide();
  d->prevStudyButton->hide();

  //Enable sorting in tree view
  d->treeView->setSortingEnabled(true);
  d->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
  d->dicomProxyModel.setSourceModel(&d->dicomModel);
  d->treeView->setModel(&d->dicomProxyModel);

  connect(d->treeView, SIGNAL(collapsed(QModelIndex)), this, SLOT(onTreeCollapsed(QModelIndex)));
  connect(d->treeView, SIGNAL(expanded(QModelIndex)), this, SLOT(onTreeExpanded(QModelIndex)));

  //Set toolbar button style
  d->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

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
  d->directoryButton->setDirectory(databaseDirectory);

  connect(d->directoryButton, SIGNAL(directoryChanged(const QString&)), this, SLOT(setDatabaseDirectory(const QString&)));

  //Initialize import widget
  d->importDIalog = new ctkFileDialog();
  QCheckBox* importCheckbox = new QCheckBox("Copy on import", d->importDIalog);
  d->importDIalog->setBottomWidget(importCheckbox);
  d->importDIalog->setFileMode(QFileDialog::Directory);
  d->importDIalog->setLabelText(QFileDialog::Accept,"Import");
  d->importDIalog->setWindowTitle("Import DICOM files from directory ...");
  d->importDIalog->setWindowModality(Qt::ApplicationModal);

  //connect signal and slots
  connect(d->treeView, SIGNAL(clicked(const QModelIndex&)), d->thumbnailsWidget, SLOT(onModelSelected(const QModelIndex &)));
  connect(d->treeView, SIGNAL(clicked(const QModelIndex&)), d->imagePreview, SLOT(onModelSelected(const QModelIndex &)));
  connect(d->treeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onModelSelected(const QModelIndex &)));

  connect(d->thumbnailsWidget, SIGNAL(selected(const ctkDICOMThumbnailWidget&)), this, SLOT(onThumbnailSelected(const ctkDICOMThumbnailWidget&)));
  connect(d->thumbnailsWidget, SIGNAL(doubleClicked(const ctkDICOMThumbnailWidget&)), this, SLOT(onThumbnailDoubleClicked(const ctkDICOMThumbnailWidget&)));
  connect(d->importDIalog, SIGNAL(fileSelected(QString)),this,SLOT(onImportDirectory(QString)));

  connect(d->dicomDatabase.data(), SIGNAL( databaseChanged() ), &(d->dicomModel), SLOT( reset() ) );
  connect(d->QueryRetrieveWidget, SIGNAL( canceled() ), d->QueryRetrieveWidget, SLOT( hide() ) );

  connect(d->imagePreview, SIGNAL(requestNextImage()), this, SLOT(onNextImage()));
  connect(d->imagePreview, SIGNAL(requestPreviousImage()), this, SLOT(onPreviousImage()));

}

//----------------------------------------------------------------------------
ctkDICOMAppWidget::~ctkDICOMAppWidget()
{
  Q_D(ctkDICOMAppWidget);  

  d->QueryRetrieveWidget->deleteLater();
  d->importDIalog->deleteLater();
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::setDatabaseDirectory(const QString& directory)
{
  Q_D(ctkDICOMAppWidget);  

  QSettings settings;
  settings.setValue("DatabaseDirectory", directory);
  settings.sync();

  //close the active DICOM database
  d->dicomDatabase->closeDatabase();
  
  //open DICOM database on the directory
  QString databaseFileName = directory + QString("/ctkDICOM.sql");
  try { d->dicomDatabase->openDatabase( databaseFileName ); }
  catch (std::exception e)
  {
    std::cerr << "Database error: " << qPrintable(d->dicomDatabase->lastError()) << "\n";
    d->dicomDatabase->closeDatabase();
    return;
  }
  
  d->dicomModel.setDatabase(d->dicomDatabase->database());
  d->dicomModel.setDisplayLevel(ctkDICOMModel::SeriesType);
  d->treeView->resizeColumnToContents(0);

  //pass DICOM database instance to Import widget
  // d->importDIalog->setdicomDatabase(d->dicomDatabase);
  d->QueryRetrieveWidget->setRetrieveDatabase(d->dicomDatabase);

  // update the button and let any connected slots know about the change
  d->directoryButton->setDirectory(directory);
  d->thumbnailsWidget->setDatabaseDirectory(directory);
  d->imagePreview->setDatabaseDirectory(directory);
  emit databaseDirectoryChanged(directory);
}

//----------------------------------------------------------------------------
QString ctkDICOMAppWidget::databaseDirectory() const
{
  QSettings settings;
  return settings.value("DatabaseDirectory").toString();
}

void ctkDICOMAppWidget::onAddToDatabase()
{
  //Q_D(ctkDICOMAppWidget);

  //d->
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::openImportDialog()
{
  Q_D(ctkDICOMAppWidget);
  
  d->importDIalog->show();
  d->importDIalog->raise();
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
void ctkDICOMAppWidget::onThumbnailSelected(const ctkDICOMThumbnailWidget& widget)
{
    Q_D(ctkDICOMAppWidget);

    d->imagePreview->onModelSelected(widget.sourceIndex());
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onThumbnailDoubleClicked(const ctkDICOMThumbnailWidget& widget)
{
    Q_D(ctkDICOMAppWidget);

    logger.debug("double clicked");
    QModelIndex index = widget.sourceIndex();

    ctkDICOMFilterProxyModel* model = const_cast<ctkDICOMFilterProxyModel*>(qobject_cast<const ctkDICOMFilterProxyModel*>(index.model()));
    QModelIndex index0 = index.sibling(index.row(), 0);

    if(model && (model->data(index0,ctkDICOMModel::TypeRole) != ctkDICOMModel::ImageType)){
        this->onModelSelected(index0);
        d->treeView->setCurrentIndex(index0);
        d->thumbnailsWidget->onModelSelected(index0);
        d->imagePreview->onModelSelected(index0);
    }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onImportDirectory(QString directory)
{
  Q_D(ctkDICOMAppWidget);
  if (QDir(directory).exists())
  {
    QCheckBox* copyOnImport = qobject_cast<QCheckBox*>(d->importDIalog->bottomWidget());
    QString targetDirectory;
    if (copyOnImport->isEnabled())
    {
       targetDirectory = d->dicomDatabase->databaseDirectory();
    }
    d->dicomIndexer->addDirectory(*d->dicomDatabase,directory,targetDirectory);
    d->dicomModel.reset();
  }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onModelSelected(const QModelIndex &index){
    Q_D(ctkDICOMAppWidget);

    ctkDICOMFilterProxyModel* model = const_cast<ctkDICOMFilterProxyModel*>(qobject_cast<const ctkDICOMFilterProxyModel*>(index.model()));

    if(model){
        QModelIndex index0 = index.sibling(index.row(), 0);

        if ( model->data(index0,ctkDICOMModel::TypeRole) == ctkDICOMModel::PatientType ){
            d->nextImageButton->show();
            d->prevImageButton->show();
            d->nextSeriesButton->show();
            d->prevSeriesButton->show();
            d->nextStudyButton->show();
            d->prevStudyButton->show();
        }else if ( model->data(index0,ctkDICOMModel::TypeRole) == ctkDICOMModel::StudyType ){
            d->nextImageButton->show();
            d->prevImageButton->show();
            d->nextSeriesButton->show();
            d->prevSeriesButton->show();
            d->nextStudyButton->hide();
            d->prevStudyButton->hide();
        }else if ( model->data(index0,ctkDICOMModel::TypeRole) == ctkDICOMModel::SeriesType ){
            d->nextImageButton->show();
            d->prevImageButton->show();
            d->nextSeriesButton->hide();
            d->prevSeriesButton->hide();
            d->nextStudyButton->hide();
            d->prevStudyButton->hide();
        }else{
            d->nextImageButton->hide();
            d->prevImageButton->hide();
            d->nextSeriesButton->hide();
            d->prevSeriesButton->hide();
            d->nextStudyButton->hide();
            d->prevStudyButton->hide();
        }
    }else{
        d->nextImageButton->hide();
        d->prevImageButton->hide();
        d->nextSeriesButton->hide();
        d->prevSeriesButton->hide();
        d->nextStudyButton->hide();
        d->prevStudyButton->hide();
    }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onNextImage(){
    Q_D(ctkDICOMAppWidget);

    QModelIndex currentIndex = d->imagePreview->currentImageIndex();

    if(currentIndex.isValid()){
        ctkDICOMFilterProxyModel* model = const_cast<ctkDICOMFilterProxyModel*>(qobject_cast<const ctkDICOMFilterProxyModel*>(currentIndex.model()));

        if(model){
            QModelIndex seriesIndex = currentIndex.parent();

            int imageCount = model->rowCount(seriesIndex);
            int imageID = currentIndex.row();

            imageID = (imageID+1)%imageCount;

            QModelIndex nextIndex = currentIndex.sibling(imageID, 0);

            d->imagePreview->onModelSelected(nextIndex);
            d->thumbnailsWidget->selectThumbnail(nextIndex);
        }
    }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onPreviousImage(){
    Q_D(ctkDICOMAppWidget);

    QModelIndex currentIndex = d->imagePreview->currentImageIndex();

    if(currentIndex.isValid()){
        ctkDICOMFilterProxyModel* model = const_cast<ctkDICOMFilterProxyModel*>(qobject_cast<const ctkDICOMFilterProxyModel*>(currentIndex.model()));

        if(model){
            QModelIndex seriesIndex = currentIndex.parent();

            int imageCount = model->rowCount(seriesIndex);
            int imageID = currentIndex.row();

            imageID--;
            if(imageID < 0) imageID += imageCount;

            QModelIndex prevIndex = currentIndex.sibling(imageID, 0);

            d->imagePreview->onModelSelected(prevIndex);
            d->thumbnailsWidget->selectThumbnail(prevIndex);
        }
    }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onNextSeries(){
    Q_D(ctkDICOMAppWidget);

    QModelIndex currentIndex = d->imagePreview->currentImageIndex();

    if(currentIndex.isValid()){
        ctkDICOMFilterProxyModel* model = const_cast<ctkDICOMFilterProxyModel*>(qobject_cast<const ctkDICOMFilterProxyModel*>(currentIndex.model()));

        if(model){
            QModelIndex seriesIndex = currentIndex.parent();
            QModelIndex studyIndex = seriesIndex.parent();

            int seriesCount = model->rowCount(studyIndex);
            int seriesID = seriesIndex.row();

            seriesID = (seriesID + 1)%seriesCount;

            QModelIndex nextIndex = seriesIndex.sibling(seriesID, 0);

            d->imagePreview->onModelSelected(nextIndex);
            d->thumbnailsWidget->selectThumbnail(nextIndex);
        }
    }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onPreviousSeries(){
    Q_D(ctkDICOMAppWidget);

    QModelIndex currentIndex = d->imagePreview->currentImageIndex();

    if(currentIndex.isValid()){
        ctkDICOMFilterProxyModel* model = const_cast<ctkDICOMFilterProxyModel*>(qobject_cast<const ctkDICOMFilterProxyModel*>(currentIndex.model()));

        if(model){
            QModelIndex seriesIndex = currentIndex.parent();
            QModelIndex studyIndex = seriesIndex.parent();

            int seriesCount = model->rowCount(studyIndex);
            int seriesID = seriesIndex.row();

            seriesID--;
            if(seriesID < 0) seriesID += seriesCount;

            QModelIndex prevIndex = seriesIndex.sibling(seriesID, 0);

            d->imagePreview->onModelSelected(prevIndex);
            d->thumbnailsWidget->selectThumbnail(prevIndex);
        }
    }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onNextStudy(){
    Q_D(ctkDICOMAppWidget);

    QModelIndex currentIndex = d->imagePreview->currentImageIndex();

    if(currentIndex.isValid()){
        ctkDICOMFilterProxyModel* model = const_cast<ctkDICOMFilterProxyModel*>(qobject_cast<const ctkDICOMFilterProxyModel*>(currentIndex.model()));

        if(model){
            QModelIndex seriesIndex = currentIndex.parent();
            QModelIndex studyIndex = seriesIndex.parent();
            QModelIndex patientIndex = studyIndex.parent();

            int studyCount = model->rowCount(patientIndex);
            int studyID = studyIndex.row();

            studyID = (studyID + 1)%studyCount;

            QModelIndex nextIndex = studyIndex.sibling(studyID, 0);

            d->imagePreview->onModelSelected(nextIndex);
            d->thumbnailsWidget->selectThumbnail(nextIndex);
        }
    }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onPreviousStudy(){
    Q_D(ctkDICOMAppWidget);

    QModelIndex currentIndex = d->imagePreview->currentImageIndex();

    if(currentIndex.isValid()){
        ctkDICOMFilterProxyModel* model = const_cast<ctkDICOMFilterProxyModel*>(qobject_cast<const ctkDICOMFilterProxyModel*>(currentIndex.model()));

        if(model){
            QModelIndex seriesIndex = currentIndex.parent();
            QModelIndex studyIndex = seriesIndex.parent();
            QModelIndex patientIndex = studyIndex.parent();

            int studyCount = model->rowCount(patientIndex);
            int studyID = studyIndex.row();

            studyID--;
            if(studyID < 0) studyID += studyCount;

            QModelIndex prevIndex = studyIndex.sibling(studyID, 0);

            d->imagePreview->onModelSelected(prevIndex);
            d->thumbnailsWidget->selectThumbnail(prevIndex);
        }
    }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onTreeCollapsed(const QModelIndex &index){
    Q_D(ctkDICOMAppWidget);
    d->treeView->resizeColumnToContents(0);
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onTreeExpanded(const QModelIndex &index){
    Q_D(ctkDICOMAppWidget);
    d->treeView->resizeColumnToContents(0);
}
