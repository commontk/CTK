// std includes
#include <iostream>

// Qt includes
#include <QDebug>
#include <QTreeView>
#include <QTabBar>
#include <QSettings>
#include <QAction>
#include <QModelIndex>

// ctkDICOMWidgets includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMModel.h"
#include "ctkDICOMAppWidget.h"
#include "ctkDICOMQueryResultsTabWidget.h"
#include "ui_ctkDICOMAppWidget.h"
#include "ctkDirectoryButton.h"
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

  ctkDICOMImportWidget* ImportWidget;
  ctkDICOMQueryRetrieveWidget* QueryRetrieveWidget;

  ctkDICOMDatabase DICOMDatabase;
  ctkDICOMModel DICOMModel;
};

//----------------------------------------------------------------------------
// ctkDICOMAppWidgetPrivate methods

ctkDICOMAppWidgetPrivate::ctkDICOMAppWidgetPrivate(){

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
  connect(d->directoryButton, SIGNAL(directoryChanged(const QString&)), this, SLOT(setDatabaseDirectory(const QString&)));

  //Initialize import widget
  d->ImportWidget = new ctkDICOMImportWidget();

  //Set thumbnails width in thumbnail widget
  //d->thumbnailsWidget->setThumbnailWidth(128);
  //Test add thumbnails
  //d->thumbnailsWidget->addTestThumbnail();

  //connect signal and slots
  connect(d->treeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onDICOMModelSelected(const QModelIndex &)));
  connect(d->thumbnailsWidget, SIGNAL(selected(const ctkDICOMThumbnailWidget&)), this, SLOT(onThumbnailSelected(const ctkDICOMThumbnailWidget&)));
}

//----------------------------------------------------------------------------
ctkDICOMAppWidget::~ctkDICOMAppWidget()
{
  Q_D(ctkDICOMAppWidget);  

  d->QueryRetrieveWidget->deleteLater();
  d->ImportWidget->deleteLater();
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::setDatabaseDirectory(const QString& directory)
{
  Q_D(ctkDICOMAppWidget);  

  QSettings settings;
  settings.setValue("DatabaseDirectory", directory);
  settings.sync();

  //close the active DICOM database
  d->DICOMDatabase.closeDatabase();
  
  //open DICOM database on the directory
  QString databaseFileName = directory + QString("/ctkDICOM.sql");
  try { d->DICOMDatabase.openDatabase( databaseFileName ); }
  catch (std::exception e)
  {
    std::cerr << "Database error: " << qPrintable(d->DICOMDatabase.GetLastError()) << "\n";
    d->DICOMDatabase.closeDatabase();
    return;
  }
  
  d->DICOMModel.setDatabase(d->DICOMDatabase.database());
  d->treeView->setModel(&d->DICOMModel);

  //pass DICOM database instance to Import widget
  d->ImportWidget->setDICOMDatabase(&d->DICOMDatabase);
}

void ctkDICOMAppWidget::onAddToDatabase()
{
  //Q_D(ctkDICOMAppWidget);

  //d->
}

//----------------------------------------------------------------------------
void ctkDICOMAppWidget::onImport(){
  Q_D(ctkDICOMAppWidget);
  
  d->ImportWidget->show();
  d->ImportWidget->raise();
}

void ctkDICOMAppWidget::onExport(){

}

void ctkDICOMAppWidget::onQuery(){
  Q_D(ctkDICOMAppWidget);

  d->QueryRetrieveWidget->show();
  d->QueryRetrieveWidget->raise();
}

void ctkDICOMAppWidget::onDICOMModelSelected(const QModelIndex& index){
  Q_D(ctkDICOMAppWidget);

  //TODO: update thumbnails and previewer
  d->thumbnailsWidget->setModelIndex(index);
}

void ctkDICOMAppWidget::onThumbnailSelected(const ctkDICOMThumbnailWidget& widget){
  //TODO: update previewer
}
