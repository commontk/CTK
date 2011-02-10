//ctkDICOMCore includes
#include "ctkDICOMDatabase.h"

// ctkDICOMWidgets includes
#include "ctkDICOMImportWidget.h"
#include "ui_ctkDICOMImportWidget.h"

// STD includes
#include <iostream>

//Qt includes
#include <QFileSystemModel>

//----------------------------------------------------------------------------
class ctkDICOMImportWidgetPrivate: public Ui_ctkDICOMImportWidget
{
public:
  ctkDICOMImportWidgetPrivate(){}

  QFileSystemModel* FileSystemModel;

  ctkDICOMDatabase* DICOMDatabase;

};

//----------------------------------------------------------------------------
// ctkDICOMImportWidgetPrivate methods

//----------------------------------------------------------------------------
// ctkDICOMImportWidget methods

//----------------------------------------------------------------------------
ctkDICOMImportWidget::ctkDICOMImportWidget(QWidget* _parent):Superclass(_parent), 
  d_ptr(new ctkDICOMImportWidgetPrivate)
{
  Q_D(ctkDICOMImportWidget);
  
  d->setupUi(this);
  
  d->DirectoryWidget->setDirectory(QDir::homePath());

  d->FileSystemModel = new QFileSystemModel(this);
  d->FileSystemModel->setRootPath(QDir::homePath());
  d->directoryList->setModel(d->FileSystemModel);
  d->directoryList->setRootIndex(d->FileSystemModel->index(QDir::homePath()));

  //connect signals and slots
  connect(d->DirectoryWidget, SIGNAL(directoryChanged(const QString&)), this, SLOT(onTopDirectoryChanged(const QString&)));
}

//----------------------------------------------------------------------------
ctkDICOMImportWidget::~ctkDICOMImportWidget()
{
  Q_D(ctkDICOMImportWidget);

  d->FileSystemModel->deleteLater();
}

void ctkDICOMImportWidget::onOK(){
  
  this->close();
}

void ctkDICOMImportWidget::onCancel(){
  this->close();
}

void ctkDICOMImportWidget::onTopDirectoryChanged(const QString& path){
  Q_D(ctkDICOMImportWidget);
  
  d->directoryList->setRootIndex(d->FileSystemModel->index(path));
}

void ctkDICOMImportWidget::setDICOMDatabase(ctkDICOMDatabase* database){
  Q_D(ctkDICOMImportWidget);

  d->DICOMDatabase = database;
}
