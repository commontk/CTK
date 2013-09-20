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
#include <QDebug>
#include <QList>
#include <QMap>
#include <QSettings>
#include <QTableWidgetItem>
#include <QVariant>

/// CTK includes
#include <ctkCheckableHeaderView.h>
#include <ctkCheckableModelHelper.h>

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
  d->NodeTable->model()->setHeaderData(
    NameColumn, Qt::Horizontal, static_cast<int>(Qt::Unchecked), Qt::CheckStateRole);
  QHeaderView* previousHeaderView = d->NodeTable->horizontalHeader();
  ctkCheckableHeaderView* headerView = new ctkCheckableHeaderView(Qt::Horizontal, d->NodeTable);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
  headerView->setSectionsClickable(previousHeaderView->sectionsClickable());
  headerView->setSectionsMovable(previousHeaderView->sectionsMovable());
#else
  headerView->setClickable(previousHeaderView->isClickable());
  headerView->setMovable(previousHeaderView->isMovable());
#endif
  headerView->setHighlightSections(previousHeaderView->highlightSections());
  headerView->checkableModelHelper()->setPropagateDepth(-1);
  d->NodeTable->setHorizontalHeader(headerView);

  d->RemoveButton->setEnabled(false);

  this->readSettings();

  connect(d->CallingAETitle, SIGNAL(textChanged(QString)),
    this, SLOT(saveSettings()));
  connect(d->StorageAETitle, SIGNAL(textChanged(QString)),
    this, SLOT(saveSettings()));
  connect(d->StoragePort, SIGNAL(textChanged(QString)),
    this, SLOT(saveSettings()));

  connect(d->AddButton, SIGNAL(clicked()),
    this, SLOT(addServerNode()));
  connect(d->RemoveButton, SIGNAL(clicked()),
    this, SLOT(removeCurrentServerNode()));

  connect(d->NodeTable, SIGNAL(cellChanged(int,int)),
    this, SLOT(saveSettings()));
  connect(d->NodeTable, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
    this, SLOT(updateRemoveButtonEnableState()));
}

//----------------------------------------------------------------------------
ctkDICOMServerNodeWidget::~ctkDICOMServerNodeWidget()
{
}

//----------------------------------------------------------------------------
int ctkDICOMServerNodeWidget::addServerNode()
{
  Q_D(ctkDICOMServerNodeWidget);
  const int rowCount = d->NodeTable->rowCount();
  d->NodeTable->setRowCount( rowCount + 1 );

  QTableWidgetItem* newItem = new QTableWidgetItem;
  newItem->setCheckState( Qt::Unchecked );
  d->NodeTable->setItem(rowCount, NameColumn, newItem);

  newItem = new QTableWidgetItem;
  newItem->setCheckState( Qt::Checked );
  d->NodeTable->setItem(rowCount, CGETColumn, newItem);

  d->NodeTable->setCurrentCell(rowCount, NameColumn);
  // The old rowCount becomes the added row index
  return rowCount;
}

//----------------------------------------------------------------------------
int ctkDICOMServerNodeWidget::addServerNode(const QMap<QString, QVariant>& node)
{
  Q_D(ctkDICOMServerNodeWidget);
  const int row = this->addServerNode();
  
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
  newItem = new QTableWidgetItem( QString("") );
  newItem->setCheckState( Qt::CheckState(node["CGET"].toInt()) );
  d->NodeTable->setItem(row, CGETColumn, newItem);
  return row;
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget::removeCurrentServerNode()
{
  Q_D(ctkDICOMServerNodeWidget);

  d->NodeTable->removeRow( d->NodeTable->currentRow() );

  this->saveSettings();
  this->updateRemoveButtonEnableState();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget::updateRemoveButtonEnableState()
{
  Q_D(ctkDICOMServerNodeWidget);
  d->RemoveButton->setEnabled(d->NodeTable->rowCount() > 0);
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget::saveSettings()
{
  Q_D(ctkDICOMServerNodeWidget);

  QSettings settings;
  const int rowCount = d->NodeTable->rowCount();
  
  settings.setValue("ServerNodeCount", rowCount);
  for (int row = 0; row < rowCount; ++row)
    {
    QMap<QString, QVariant> node = this->serverNodeParameters(row);
    settings.setValue(QString("ServerNodes/%1").arg(row), QVariant(node));
    }
  settings.setValue("CallingAETitle", this->callingAETitle());
  settings.setValue("StorageAETitle", this->storageAETitle());
  settings.setValue("StoragePort", this->storagePort());
  settings.sync();
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget::readSettings()
{
  Q_D(ctkDICOMServerNodeWidget);

  d->NodeTable->setRowCount(0);

  QSettings settings;

  QMap<QString, QVariant> node;
  if (settings.status() == QSettings::AccessError ||
      settings.value("ServerNodeCount").toInt() == 0)
    {
    d->StorageAETitle->setText("CTKSTORE");
    d->StoragePort->setText("11112");
    d->CallingAETitle->setText("CTKSTORE");

    // a dummy example
    QMap<QString, QVariant> defaultServerNode;
    defaultServerNode["Name"] = QString("ExampleHost");
    defaultServerNode["CheckState"] = static_cast<int>(Qt::Unchecked);
    defaultServerNode["AETitle"] = QString("AETITLE");
    defaultServerNode["Address"] = QString("dicom.example.com");
    defaultServerNode["Port"] = QString("11112");
    defaultServerNode["CGET"] = static_cast<int>(Qt::Unchecked);
    this->addServerNode(defaultServerNode);

    // the uk example - see http://www.dicomserver.co.uk/ 
    // and http://www.medicalconnections.co.uk/
    defaultServerNode["Name"] = QString("MedicalConnections");
    defaultServerNode["CheckState"] = static_cast<int>(Qt::Unchecked);
    defaultServerNode["AETitle"] = QString("ANYAE");
    defaultServerNode["Address"] = QString("dicomserver.co.uk");
    defaultServerNode["Port"] = QString("11112");
    defaultServerNode["CGET"] = static_cast<int>(Qt::Checked);
    this->addServerNode(defaultServerNode);

    return;
    }

  d->StorageAETitle->setText(settings.value("StorageAETitle").toString());
  d->StoragePort->setText(settings.value("StoragePort").toString());
  d->CallingAETitle->setText(settings.value("CallingAETitle").toString());

  const int count = settings.value("ServerNodeCount").toInt();
  for (int row = 0; row < count; ++row)
    {
    node = settings.value(QString("ServerNodes/%1").arg(row)).toMap();
    this->addServerNode(node);
    }
}

//----------------------------------------------------------------------------
QString ctkDICOMServerNodeWidget::callingAETitle()const
{
  Q_D(const ctkDICOMServerNodeWidget);
  return d->CallingAETitle->text();
}

//----------------------------------------------------------------------------
QString ctkDICOMServerNodeWidget::storageAETitle()const
{
  Q_D(const ctkDICOMServerNodeWidget);
  return d->StorageAETitle->text();
}

//----------------------------------------------------------------------------
int ctkDICOMServerNodeWidget::storagePort()const
{
  Q_D(const ctkDICOMServerNodeWidget);
  bool ok = false;
  int port = d->StoragePort->text().toInt(&ok);
  Q_ASSERT(ok);
  return port;
}

//----------------------------------------------------------------------------
QMap<QString,QVariant> ctkDICOMServerNodeWidget::parameters()const
{
  QMap<QString, QVariant> parameters;

  parameters["CallingAETitle"] = this->callingAETitle();
  parameters["StorageAETitle"] = this->storageAETitle();
  parameters["StoragePort"] = this->storagePort();

  return parameters;
}

//----------------------------------------------------------------------------
QStringList ctkDICOMServerNodeWidget::serverNodes()const
{
  Q_D(const ctkDICOMServerNodeWidget);

  QStringList nodes;
  const int count = d->NodeTable->rowCount();
  for (int row = 0; row < count; ++row)
    {
    QTableWidgetItem* item = d->NodeTable->item(row,NameColumn);
    nodes << (item ? item->text() : QString(""));
    }
  // If there are duplicates, serverNodeParameters(QString) will behave
  // strangely
  Q_ASSERT(nodes.removeDuplicates() == 0);
  return nodes;
}

//----------------------------------------------------------------------------
QStringList ctkDICOMServerNodeWidget::selectedServerNodes()const
{
  Q_D(const ctkDICOMServerNodeWidget);

  QStringList nodes;
  const int count = d->NodeTable->rowCount();
  for (int row = 0; row < count; ++row)
    {
    QTableWidgetItem* item = d->NodeTable->item(row, NameColumn);
    if (item && item->checkState() == Qt::Checked)
      {
      nodes << item->text();
      }
    }
  // If there are duplicates, serverNodeParameters(QString) will behave
  // strangely
  Q_ASSERT(nodes.removeDuplicates() == 0);
  return nodes;
}

//----------------------------------------------------------------------------
QMap<QString, QVariant> ctkDICOMServerNodeWidget::serverNodeParameters(const QString &node)const
{
  Q_D(const ctkDICOMServerNodeWidget);

  QMap<QString, QVariant> parameters;

  const int count = d->NodeTable->rowCount();
  for (int row = 0; row < count; row++)
    {
    if ( d->NodeTable->item(row,0)->text() == node )
      {
      // TBD: not sure what it means to merge parameters
      parameters.unite(this->serverNodeParameters(row));
      }
    }

  return parameters;
}

//----------------------------------------------------------------------------
QMap<QString, QVariant> ctkDICOMServerNodeWidget::serverNodeParameters(int row)const
{
  Q_D(const ctkDICOMServerNodeWidget);

  QMap<QString, QVariant> node;
  if (row < 0 || row >= d->NodeTable->rowCount())
    {
    return node;
    }
  const int columnCount = d->NodeTable->columnCount();
  for (int column = 0; column < columnCount; ++column)
    {
    if (!d->NodeTable->item(row, column))
      {
      continue;
      }
    QString label = d->NodeTable->horizontalHeaderItem(column)->text();
    node[label] = d->NodeTable->item(row, column)->data(Qt::DisplayRole);
    }
  node["CheckState"] = d->NodeTable->item(row, NameColumn) ?
    d->NodeTable->item(row,NameColumn)->checkState() :
    static_cast<int>(Qt::Unchecked);
  node["CGET"] = d->NodeTable->item(row, CGETColumn) ?
    d->NodeTable->item(row,CGETColumn)->checkState() :
    static_cast<int>(Qt::Unchecked);
  return node;
}
