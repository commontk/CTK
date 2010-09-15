
// Qt includes
#include <QFileDialog>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlField>
#include <QDebug>

// ctkDICOMWidgets includes
#include "ctkDICOMDirectoryListWidget.h"
#include "ui_ctkDICOMDirectoryListWidget.h"

// STD includes
#include <iostream>

//----------------------------------------------------------------------------
class ctkDICOMDirectoryListWidgetPrivate: public Ui_ctkDICOMDirectoryListWidget
{
public:
  ctkDICOMDirectoryListWidgetPrivate(){}
  ctkDICOM*       dicom;
  QSqlTableModel* directoryListModel;
};

//----------------------------------------------------------------------------
// ctkDICOMDirectoryListWidgetPrivate methods


//---------------------------
// ctkDICOMDirectoryListWidget methods

//----------------------------------------------------------------------------
ctkDICOMDirectoryListWidget::ctkDICOMDirectoryListWidget(QWidget* _parent):Superclass(_parent), 
  d_ptr(new ctkDICOMDirectoryListWidgetPrivate)
{
  Q_D(ctkDICOMDirectoryListWidget);

  d->setupUi(this);

  connect(d->addButton, SIGNAL(clicked()), this, SLOT(addDirectoryClicked()));
  connect(d->removeButton, SIGNAL(clicked()), this, SLOT(removeDirectoryClicked()));

  d->removeButton->setDisabled(true);
}

//----------------------------------------------------------------------------
ctkDICOMDirectoryListWidget::~ctkDICOMDirectoryListWidget()
{
}

//----------------------------------------------------------------------------
void ctkDICOMDirectoryListWidget::addDirectoryClicked()
{
  Q_D(ctkDICOMDirectoryListWidget);
  QString newDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"));

  if ( !newDir.isEmpty() )
  {
    QSqlRecord newDirRecord;
    newDirRecord.append(QSqlField("Dirname",QVariant::String));
    newDirRecord.setValue("Dirname",newDir);
    /*bool success = */d->directoryListModel->insertRecord(-1,newDirRecord);
    bool success2 = d->directoryListModel->submitAll();
    if ( !success2 )
    {
      qDebug() << d->directoryListModel->lastError();
    }
    //addDirectoryQuery.prepare("insert into Directories VALUES ( :dirname )");
    //addDirectoryQuery.bindValue(":dirname",newDir);
    //addDirectoryQuery.exec();

//    d->directoryListModel;
  }

//d->directoryListView->setModel(NULL);
// d->tableView->setModel(NULL);
}

//----------------------------------------------------------------------------
void ctkDICOMDirectoryListWidget::removeDirectoryClicked()
{
  Q_D(ctkDICOMDirectoryListWidget);
  while ( ! d->directoryListView->selectionModel()->selectedIndexes().empty() )
  {
    d->directoryListModel->removeRow(
        d->directoryListView->selectionModel()->selectedIndexes().first().row()
    );
  }
}

//----------------------------------------------------------------------------
void ctkDICOMDirectoryListWidget::setDICOM(ctkDICOM* dicom)
{
  Q_D(ctkDICOMDirectoryListWidget);
  d->dicom = dicom;
  d->directoryListModel =  new QSqlTableModel(this,d->dicom->database());
  d->directoryListModel->setTable("Directories");
  d->directoryListModel->setEditStrategy(QSqlTableModel::OnFieldChange);
  d->directoryListModel->select();
  d->directoryListView->setModel(d->directoryListModel);

  connect ( d->directoryListView->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
            this,
            SLOT(directorySelectionChanged(const QItemSelection & , const QItemSelection &  )));
}

//----------------------------------------------------------------------------
void ctkDICOMDirectoryListWidget::directorySelectionChanged( const QItemSelection  & selected, const QItemSelection  & deselected )
{
  Q_UNUSED(deselected);
  Q_D(ctkDICOMDirectoryListWidget);
  d->removeButton->setEnabled( ! selected.empty() );
}
