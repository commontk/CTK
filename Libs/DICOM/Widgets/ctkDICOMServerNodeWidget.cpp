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

// Qt includes
#include <QDebug>
#include <QList>
#include <QMap>
#include <QSettings>
#include <QTableWidgetItem>
#include <QVariant>

/// CTK includes
#include <ctkCheckableHeaderView.h>

// ctkDICOMWidgets includes
#include "ctkDICOMServerNodeWidget.h"
#include "ui_ctkDICOMServerNodeWidget.h"

// STD includes
#include <iostream>
//----------------------------------------------------------------------------
class ctkDICOMServerNodeWidgetPrivate: public Ui_ctkDICOMServerNodeWidget
{
public:
  ctkDICOMServerNodeWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// ctkDICOMServerNodeWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMServerNodeWidget methods

//----------------------------------------------------------------------------
ctkDICOMServerNodeWidget::ctkDICOMServerNodeWidget(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkDICOMServerNodeWidgetPrivate)
{
  Q_D(ctkDICOMServerNodeWidget);
 
  d->setupUi(this);

  // checkable headers.
  d->NodeTable->model()->setHeaderData(0, Qt::Horizontal, Qt::Unchecked, Qt::CheckStateRole);
  QHeaderView* previousHeaderView = d->NodeTable->horizontalHeader();
  ctkCheckableHeaderView* headerView = new ctkCheckableHeaderView(Qt::Horizontal, d->NodeTable);
  headerView->setClickable(previousHeaderView->isClickable());
  headerView->setMovable(previousHeaderView->isMovable());
  headerView->setHighlightSections(previousHeaderView->highlightSections());
  headerView->setPropagateToItems(true);
  d->NodeTable->setHorizontalHeader(headerView);

  d->RemoveButton->setEnabled(false);

  this->readSettings();

  connect(d->CallingAETitle, SIGNAL(textChanged(const QString&)),
    this, SLOT(saveSettings()));
  connect(d->StorageAETitle, SIGNAL(textChanged(const QString&)),
    this, SLOT(saveSettings()));
  connect(d->StoragePort, SIGNAL(textChanged(const QString&)),
    this, SLOT(saveSettings()));
  connect(d->AddButton, SIGNAL(clicked()),
    this, SLOT(addNode()));
  connect(d->RemoveButton, SIGNAL(clicked()),
    this, SLOT(removeNode()));
  connect(d->NodeTable, SIGNAL(cellChanged(int,int)),
    this, SLOT(onCellChanged(int,int)));
  connect(d->NodeTable, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)),
    this, SLOT(onCurrentItemChanged(QTableWidgetItem*, QTableWidgetItem*)));
}

//----------------------------------------------------------------------------
ctkDICOMServerNodeWidget::~ctkDICOMServerNodeWidget()
{
}


//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget::addNode()
{
  Q_D(ctkDICOMServerNodeWidget);

  d->NodeTable->setRowCount( d->NodeTable->rowCount() + 1 );
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget::removeNode()
{
  Q_D(ctkDICOMServerNodeWidget);

  d->NodeTable->removeRow( d->NodeTable->currentRow() );
  d->RemoveButton->setEnabled(false);
  this->saveSettings();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget::onCellChanged(int row, int column)
{
  Q_UNUSED(row);
  Q_UNUSED(column);

  this->saveSettings();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget::onCurrentItemChanged(QTableWidgetItem* current, QTableWidgetItem *previous)
{
  Q_UNUSED(current);
  Q_UNUSED(previous);

  Q_D(ctkDICOMServerNodeWidget);
  if (d->NodeTable->rowCount() > 1)
  {
    d->RemoveButton->setEnabled(true);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget::saveSettings()
{
  Q_D(ctkDICOMServerNodeWidget);

  QSettings settings;
  const int rowCount = d->NodeTable->rowCount();
  const int columnCount = d->NodeTable->columnCount();
  
  settings.setValue("ServerNodeCount", rowCount);
  for (int row = 0; row < rowCount; ++row)
    {
    QMap<QString, QVariant> node;
    for (int k = 0; k < columnCount; ++k)
      {
      if (!d->NodeTable->item(row,k))
        {
        continue;
        }
      QString label = d->NodeTable->horizontalHeaderItem(k)->text();
      node[label] = d->NodeTable->item(row, k)->data(Qt::DisplayRole);
      }
    node["CheckState"] = d->NodeTable->item(row,0) ?
      d->NodeTable->item(row,0)->checkState() : Qt::Unchecked;
    settings.setValue(QString("ServerNodes/%1").arg(row), QVariant(node));
    }
  settings.setValue("CallingAETitle", d->CallingAETitle->text());
  settings.setValue("StorageAETitle", d->StorageAETitle->text());
  settings.setValue("StoragePort", d->StoragePort->text());
  settings.sync();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget::readSettings()
{
  Q_D(ctkDICOMServerNodeWidget);

  QSettings settings;

  QMap<QString, QVariant> node;
  if (settings.status() == QSettings::AccessError ||
      settings.value("ServerNodeCount").toInt() == 0)
    {
    d->StorageAETitle->setText("CTKSTORE");
    d->StoragePort->setText("11112");
    d->CallingAETitle->setText("FINDSCU");
    d->NodeTable->setRowCount(1);
    d->NodeTable->setItem(0, NameColumn, new QTableWidgetItem("ExampleHost"));
    d->NodeTable->item(0, NameColumn)->setCheckState( Qt::Checked );
    d->NodeTable->setItem(0, AETitleColumn, new QTableWidgetItem("ANY-SCP"));
    d->NodeTable->setItem(0, AddressColumn, new QTableWidgetItem("localhost"));
    d->NodeTable->setItem(0, PortColumn, new QTableWidgetItem("11112"));
    return;
    }

  d->StorageAETitle->setText(settings.value("StorageAETitle").toString());
  d->StoragePort->setText(settings.value("StoragePort").toString());
  d->CallingAETitle->setText(settings.value("CallingAETitle").toString());
  const int count = settings.value("ServerNodeCount").toInt();
  d->NodeTable->setRowCount(count);
  for (int row = 0; row < count; row++)
    {
    node = settings.value(QString("ServerNodes/%1").arg(row)).toMap();
    QTableWidgetItem *newItem;
    newItem = new QTableWidgetItem( node["Name"].toString() );
    newItem->setCheckState( Qt::CheckState(node["CheckState"].toInt()) );
    d->NodeTable->setItem(row, NameColumn, newItem);
    newItem = new QTableWidgetItem( node["AETitle"].toString() );
    d->NodeTable->setItem(row, AETitleColumn, newItem);
    newItem = new QTableWidgetItem( node["Address"].toString() );
    d->NodeTable->setItem(row, AddressColumn, newItem);
    newItem = new QTableWidgetItem( node["Port"].toString() );
    d->NodeTable->setItem(row, PortColumn, newItem);
    }
}

//----------------------------------------------------------------------------
QString ctkDICOMServerNodeWidget::callingAETitle()
{
  Q_D(ctkDICOMServerNodeWidget);

  return d->CallingAETitle->text();
}

//----------------------------------------------------------------------------
QMap<QString,QVariant> ctkDICOMServerNodeWidget::parameters()
{
  Q_D(ctkDICOMServerNodeWidget);

  QMap<QString, QVariant> parameters;

  parameters["CallingAETitle"] = d->CallingAETitle->text();
  parameters["StorageAETitle"] = d->StorageAETitle->text();
  parameters["StoragePort"] = d->StoragePort->text();

  return parameters;
}

//----------------------------------------------------------------------------
QStringList ctkDICOMServerNodeWidget::nodes()const
{
  Q_D(const ctkDICOMServerNodeWidget);

  const int count = d->NodeTable->rowCount();
  QStringList nodes;
  for (int row = 0; row < count; row++)
    {
    nodes << d->NodeTable->item(row,NameColumn)->text();
    }
  return nodes;
}

//----------------------------------------------------------------------------
QStringList ctkDICOMServerNodeWidget::checkedNodes()const
{
  Q_D(const ctkDICOMServerNodeWidget);

  const int count = d->NodeTable->rowCount();
  QStringList nodes;
  for (int row = 0; row < count; row++)
    {
    QTableWidgetItem* item = d->NodeTable->item(row,NameColumn);
    if (item && item->checkState() == Qt::Checked)
      {
      nodes << item->text();
      }
    }
  return nodes;
}

//----------------------------------------------------------------------------
QMap<QString, QVariant> ctkDICOMServerNodeWidget::nodeParameters(const QString &node)const
{
  Q_D(const ctkDICOMServerNodeWidget);

  QMap<QString, QVariant> parameters;

  int count = d->NodeTable->rowCount();
  QStringList keys;
  keys << "Name" << "AETitle" << "Address" << "Port";
  for (int row = 0; row < count; row++)
  {
    if ( d->NodeTable->item(row,0)->text() == node )
    {
      for (int k = 0; k < keys.size(); ++k)
      {
        if ( d->NodeTable->item(row,k) )
        {
          parameters[keys.at(k)] = d->NodeTable->item(row,k)->text();
        }
        parameters["CheckState"] = d->NodeTable->item(row,0)->checkState();
      }
    }
  }

  return parameters;
}
