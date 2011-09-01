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
  ctkDICOMDatabase*       database;
  QSqlTableModel*         directoryListModel;
};

//----------------------------------------------------------------------------
// ctkDICOMDirectoryListWidgetPrivate methods


//---------------------------
// ctkDICOMDirectoryListWidget methods

//----------------------------------------------------------------------------
ctkDICOMDirectoryListWidget::ctkDICOMDirectoryListWidget(QWidget* parentWidget)
 : Superclass(parentWidget)
 , d_ptr(new ctkDICOMDirectoryListWidgetPrivate)
{
  Q_D(ctkDICOMDirectoryListWidget);

  d->setupUi(this);

  connect(d->addButton, SIGNAL(clicked()), this, SLOT(addDirectory()));
  connect(d->removeButton, SIGNAL(clicked()), this, SLOT(removeDirectory()));

  d->removeButton->setDisabled(true);
}

//----------------------------------------------------------------------------
ctkDICOMDirectoryListWidget::~ctkDICOMDirectoryListWidget()
{
}

//----------------------------------------------------------------------------
void ctkDICOMDirectoryListWidget::addDirectory()
{
  QString newDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"));

  if ( !newDir.isEmpty() )
    {
    this->addDirectory(newDir);
    }
}

//----------------------------------------------------------------------------
void ctkDICOMDirectoryListWidget::addDirectory(const QString& newDir)
{
  Q_D(ctkDICOMDirectoryListWidget);
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

  //d->directoryListView->setModel(NULL);
  // d->tableView->setModel(NULL);
}

//----------------------------------------------------------------------------
void ctkDICOMDirectoryListWidget::removeDirectory()
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
void ctkDICOMDirectoryListWidget::setDICOMDatabase(ctkDICOMDatabase* dicomDatabase)
{
  Q_D(ctkDICOMDirectoryListWidget);
  d->database = dicomDatabase;
  d->directoryListModel = new QSqlTableModel(
    this, d->database ? d->database->database() : QSqlDatabase());
  d->directoryListModel->setTable("Directories");
  d->directoryListModel->setEditStrategy(QSqlTableModel::OnFieldChange);
  d->directoryListModel->select();
  d->directoryListView->setModel(d->directoryListModel);

  connect ( d->directoryListView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,
            SLOT(directorySelectionChanged(const QItemSelection & ,
                                           const QItemSelection &  )));
}

//----------------------------------------------------------------------------
void ctkDICOMDirectoryListWidget
::directorySelectionChanged( const QItemSelection  & selected,
                             const QItemSelection  & deselected )
{
  Q_UNUSED(deselected);
  Q_D(ctkDICOMDirectoryListWidget);
  d->removeButton->setEnabled( ! selected.empty() );
}
