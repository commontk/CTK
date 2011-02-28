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
  
  //Set toolbar button style
  d->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

  //Initialize Q/R widget
  d->QueryRetrieveWidget = new ctkDICOMQueryRetrieveWidget();
  d->QueryRetrieveWidget->setWindowModality ( Qt::ApplicationModal );
  connect(d->directoryButton, SIGNAL(directoryChanged(const QString&)), this, SLOT(setDatabaseDirectory(const QString&)));

  //Initialize import widget
  d->ImportDialog = new ctkFileDialog();
  QCheckBox* importCheckbox = new QCheckBox("Copy on import", d->ImportDialog);
  d->ImportDialog->setBottomWidget(importCheckbox);
  d->ImportDialog->setFileMode(QFileDialog::Directory);
  d->ImportDialog->setLabelText(QFileDialog::Accept,"Import");
  d->ImportDialog->setWindowTitle("Import DICOM files from directory ...");
  d->ImportDialog->setWindowModality(Qt::ApplicationModal);


  //Set thumbnails width in thumbnail widget
  //d->thumbnailsWidget->setThumbnailWidth(128);
  //Test add thumbnails
  //d->thumbnailsWidget->addTestThumbnail();

  //connect signal and slots
  connect(d->treeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onDICOMModelSelected(const QModelIndex &)));
  connect(d->thumbnailsWidget, SIGNAL(selected(const ctkDICOMThumbnailWidget&)), this, SLOT(onThumbnailSelected(const ctkDICOMThumbnailWidget&)));
  connect(d->ImportDialog, SIGNAL(fileSelected(QString)),this,SLOT(onImportDirectory(QString)));

  connect(d->DICOMDatabase.data(), SIGNAL( databaseChanged() ), &(d->DICOMModel), SLOT( reset() ) );

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
void ctkDICOMAppWidget::onDICOMModelSelected(const QModelIndex& index)
{
  Q_D(ctkDICOMAppWidget);

  if ( d->DICOMModel.data(index,ctkDICOMModel::TypeRole) == ctkDICOMModel::SeriesType )
  {
    qDebug() << "Clicked on series";
    QStringList thumbnails;
    QString thumbnailPath = d->DICOMDatabase->databaseDirectory() +
                            "/thumbs/" + d->DICOMModel.data(index.parent() ,ctkDICOMModel::UIDRole).toString() + "/" +
                            d->DICOMModel.data(index ,ctkDICOMModel::UIDRole).toString() + "/";

    QModelIndex studyIndex = index.parent();
    QModelIndex seriesIndex = index;
    int imageCount = d->DICOMModel.rowCount(index);
    logger.debug(QString("Thumbs: %1").arg(imageCount));
    for (int i = 0 ; i < imageCount ; i++ )
    {
      QModelIndex imageIndex = index.child(i,0);
      QString thumbnail = thumbnailPath + d->DICOMModel.data(imageIndex, ctkDICOMModel::UIDRole).toString() + ".png";
      qDebug() << "Thumb: " << thumbnail;
      if (QFile(thumbnail).exists())
      {
        thumbnails << thumbnail;
      }
      else
      {
      logger.error("No thumbnail file " + thumbnail);
      }
    }
    d->thumbnailsWidget->setThumbnailFiles(thumbnails);

    //  thumbnailPath.append("/thumbs/").append(d->DICOMModel.data( studyIndex,ctkDICOMModel::UIDRole).toString() );
    //  thumbnailPath.append(d->DICOMModel.data( seriesIndex,ctkDICOMModel::UIDRole).toString() );
  }


  // TODO: this could check the type of the model entries
  QString thumbnailPath = d->DICOMDatabase->databaseDirectory();
  thumbnailPath.append("/dicom/").append(d->DICOMModel.data(index.parent().parent() ,ctkDICOMModel::UIDRole).toString());
  thumbnailPath.append("/").append(d->DICOMModel.data(index.parent() ,ctkDICOMModel::UIDRole).toString());
  thumbnailPath.append("/").append(d->DICOMModel.data(index ,ctkDICOMModel::UIDRole).toString());
  //thumbnailPath.append(".png");
  if (QFile(thumbnailPath).exists())
  {
    DicomImage dcmImage( thumbnailPath.toStdString().c_str() );
    ctkDICOMImage ctkImage( & dcmImage );
    d->imagePreview->clearImages();
    d->imagePreview->addImage( ctkImage );
  }
  else
  {
    d->imagePreview->clearImages();
  }


}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onThumbnailSelected(const ctkDICOMThumbnailWidget& widget)
{
  //TODO: update previewer
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
