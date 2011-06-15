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

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMIndexer.h"

// ctkDICOMWidgets includes
#include "ctkDICOMImage.h"
#include "ctkDICOMModel.h"
#include "ctkDICOMAppWidget.h"
#include "ctkDICOMQueryResultsTabWidget.h"
#include "ui_ctkDICOMAppWidget.h"
#include "ctkDirectoryButton.h"
#include "ctkFileDialog.h"

#include "ctkDICOMQueryRetrieveWidget.h"
#include "ctkDICOMImportWidget.h"
#include "ctkDICOMThumbnailWidget.h"

//logger
#include <ctkLogger.h>
static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMAppWidget");

//----------------------------------------------------------------------------
class ctkDICOMAppWidgetPrivate: public Ui_ctkDICOMAppWidget
{
public:
  ctkDICOMAppWidgetPrivate();

  ctkFileDialog* ImportDialog;
  ctkDICOMQueryRetrieveWidget* QueryRetrieveWidget;

  QSharedPointer<ctkDICOMDatabase> DICOMDatabase;
  ctkDICOMModel DICOMModel;
  QSharedPointer<ctkDICOMIndexer> DICOMIndexer;

};

//----------------------------------------------------------------------------
// ctkDICOMAppWidgetPrivate methods

ctkDICOMAppWidgetPrivate::ctkDICOMAppWidgetPrivate(){
  
  DICOMDatabase = QSharedPointer<ctkDICOMDatabase> (new ctkDICOMDatabase);
  DICOMIndexer = QSharedPointer<ctkDICOMIndexer> (new ctkDICOMIndexer);
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
  d->ImportDialog = new ctkFileDialog();
  QCheckBox* importCheckbox = new QCheckBox("Copy on import", d->ImportDialog);
  d->ImportDialog->setBottomWidget(importCheckbox);
  d->ImportDialog->setFileMode(QFileDialog::Directory);
  d->ImportDialog->setLabelText(QFileDialog::Accept,"Import");
  d->ImportDialog->setWindowTitle("Import DICOM files from directory ...");
  d->ImportDialog->setWindowModality(Qt::ApplicationModal);

  //connect signal and slots
  connect(d->treeView, SIGNAL(clicked(const QModelIndex&)), d->thumbnailsWidget, SLOT(onModelSelected(const QModelIndex &)));
  connect(d->treeView, SIGNAL(clicked(const QModelIndex&)), d->imagePreview, SLOT(onModelSelected(const QModelIndex &)));
  connect(d->treeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onModelSelected(const QModelIndex &)));

  connect(d->thumbnailsWidget, SIGNAL(selected(const ctkDICOMThumbnailWidget&)), this, SLOT(onThumbnailSelected(const ctkDICOMThumbnailWidget&)));
  connect(d->ImportDialog, SIGNAL(fileSelected(QString)),this,SLOT(onImportDirectory(QString)));

  connect(d->DICOMDatabase.data(), SIGNAL( databaseChanged() ), &(d->DICOMModel), SLOT( reset() ) );
  connect(d->QueryRetrieveWidget, SIGNAL( canceled() ), d->QueryRetrieveWidget, SLOT( hide() ) );

}

//----------------------------------------------------------------------------
ctkDICOMAppWidget::~ctkDICOMAppWidget()
{
  Q_D(ctkDICOMAppWidget);  

  d->QueryRetrieveWidget->deleteLater();
  d->ImportDialog->deleteLater();
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
  try { d->DICOMDatabase->openDatabase( databaseFileName ); }
  catch (std::exception e)
  {
    std::cerr << "Database error: " << qPrintable(d->DICOMDatabase->lastError()) << "\n";
    d->DICOMDatabase->closeDatabase();
    return;
  }
  
  d->DICOMModel.setDatabase(d->DICOMDatabase->database());
  d->treeView->setModel(&d->DICOMModel);

  //pass DICOM database instance to Import widget
  // d->ImportDialog->setDICOMDatabase(d->DICOMDatabase);
  d->QueryRetrieveWidget->setRetrieveDatabase(d->DICOMDatabase);

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
void ctkDICOMAppWidget::onThumbnailSelected(const ctkDICOMThumbnailWidget& widget)
{
    Q_D(ctkDICOMAppWidget);

    d->imagePreview->onModelSelected(widget.sourceIndex());
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onImportDirectory(QString directory)
{
  Q_D(ctkDICOMAppWidget);
  if (QDir(directory).exists())
  {
    QCheckBox* copyOnImport = qobject_cast<QCheckBox*>(d->ImportDialog->bottomWidget());
    QString targetDirectory;
    if (copyOnImport->isEnabled())
    {
       targetDirectory = d->DICOMDatabase->databaseDirectory();
    }
    d->DICOMIndexer->addDirectory(*d->DICOMDatabase,directory,targetDirectory);
    d->DICOMModel.reset();
  }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onModelSelected(const QModelIndex &index){
     Q_D(ctkDICOMAppWidget);

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

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
        ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(currentIndex.model()));

        if(model){
            QModelIndex seriesIndex = currentIndex.parent();

            int imageCount = model->rowCount(seriesIndex);
            int imageID = currentIndex.row();

            imageID = (imageID+1)%imageCount;

            QModelIndex nextIndex = currentIndex.sibling(imageID, 0);

            d->imagePreview->onModelSelected(nextIndex);
        }
    }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onPreviousImage(){
    Q_D(ctkDICOMAppWidget);

    QModelIndex currentIndex = d->imagePreview->currentImageIndex();

    if(currentIndex.isValid()){
        ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(currentIndex.model()));

        if(model){
            QModelIndex seriesIndex = currentIndex.parent();

            int imageCount = model->rowCount(seriesIndex);
            int imageID = currentIndex.row();

            imageID--;
            if(imageID < 0) imageID += imageCount;

            QModelIndex prevIndex = currentIndex.sibling(imageID, 0);

            d->imagePreview->onModelSelected(prevIndex);
        }
    }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onNextSeries(){
    Q_D(ctkDICOMAppWidget);

    QModelIndex currentIndex = d->imagePreview->currentImageIndex();

    if(currentIndex.isValid()){
        ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(currentIndex.model()));

        if(model){
            QModelIndex seriesIndex = currentIndex.parent();
            QModelIndex studyIndex = seriesIndex.parent();

            int seriesCount = model->rowCount(studyIndex);
            int seriesID = seriesIndex.row();

            seriesID = (seriesID + 1)%seriesCount;

            QModelIndex nextIndex = seriesIndex.sibling(seriesID, 0);

            d->imagePreview->onModelSelected(nextIndex);
        }
    }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onPreviousSeries(){
    Q_D(ctkDICOMAppWidget);

    QModelIndex currentIndex = d->imagePreview->currentImageIndex();

    if(currentIndex.isValid()){
        ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(currentIndex.model()));

        if(model){
            QModelIndex seriesIndex = currentIndex.parent();
            QModelIndex studyIndex = seriesIndex.parent();

            int seriesCount = model->rowCount(studyIndex);
            int seriesID = seriesIndex.row();

            seriesID--;
            if(seriesID < 0) seriesID += seriesCount;

            QModelIndex nextIndex = seriesIndex.sibling(seriesID, 0);

            d->imagePreview->onModelSelected(nextIndex);
        }
    }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onNextStudy(){
    Q_D(ctkDICOMAppWidget);

    QModelIndex currentIndex = d->imagePreview->currentImageIndex();

    if(currentIndex.isValid()){
        ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(currentIndex.model()));

        if(model){
            QModelIndex seriesIndex = currentIndex.parent();
            QModelIndex studyIndex = seriesIndex.parent();
            QModelIndex patientIndex = studyIndex.parent();

            int studyCount = model->rowCount(patientIndex);
            int studyID = studyIndex.row();

            studyID = (studyID + 1)%studyCount;

            QModelIndex nextIndex = studyIndex.sibling(studyID, 0);

            d->imagePreview->onModelSelected(nextIndex);
        }
    }
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onPreviousStudy(){
    Q_D(ctkDICOMAppWidget);

    QModelIndex currentIndex = d->imagePreview->currentImageIndex();

    if(currentIndex.isValid()){
        ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(currentIndex.model()));

        if(model){
            QModelIndex seriesIndex = currentIndex.parent();
            QModelIndex studyIndex = seriesIndex.parent();
            QModelIndex patientIndex = studyIndex.parent();

            int studyCount = model->rowCount(patientIndex);
            int studyID = studyIndex.row();

            studyID--;
            if(studyID < 0) studyID += studyCount;

            QModelIndex nextIndex = studyIndex.sibling(studyID, 0);

            d->imagePreview->onModelSelected(nextIndex);
        }
    }
}
