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

// CTK includes
#include "ctkVTKDataSetModel.h"

// VTK includes
#include <vtkAbstractArray.h>
#include <vtkAssignAttribute.h>
#include <vtkCellData.h>
#include <vtkDataSet.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>

class ctkVTKDataSetModelPrivate
{
  Q_DECLARE_PUBLIC(ctkVTKDataSetModel);
protected:
  ctkVTKDataSetModel* const q_ptr;
public:
  ctkVTKDataSetModelPrivate(ctkVTKDataSetModel& object);
  virtual ~ctkVTKDataSetModelPrivate();
  void init();
  //void listenAbstractArrayModifiedEvent();

  static QList<vtkAbstractArray*> attributeArrayToInsert(const ctkVTKDataSetModel::AttributeTypes& attributeType,
                                                     vtkDataSetAttributes * dataSetAttributes);

  vtkSmartPointer<vtkDataSet> DataSet;
  vtkSmartPointer<vtkPointData> DataSetPointData;
  vtkSmartPointer<vtkCellData> DataSetCellData;

  bool ListenAbstractArrayModifiedEvent;
  ctkVTKDataSetModel::AttributeTypes AttributeType;
  bool IncludeNullItem;
};


//------------------------------------------------------------------------------
ctkVTKDataSetModelPrivate::ctkVTKDataSetModelPrivate(ctkVTKDataSetModel& object)
  : q_ptr(&object)
{
  this->ListenAbstractArrayModifiedEvent = false;
  this->AttributeType = ctkVTKDataSetModel::AllAttribute;
  this->IncludeNullItem = false;
}

//------------------------------------------------------------------------------
ctkVTKDataSetModelPrivate::~ctkVTKDataSetModelPrivate()
{
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModelPrivate::init()
{
  Q_Q(ctkVTKDataSetModel);
  q->setColumnCount(1);

  QObject::connect(q, SIGNAL(itemChanged(QStandardItem*)),
                   q, SLOT(onItemChanged(QStandardItem*)));
}
/*
//------------------------------------------------------------------------------
void ctkVTKDataSetModelPrivate::listenAbstractArrayModifiedEvent()
{
  Q_Q(ctkVTKDataSetModel);
  q->qvtkDisconnect(0, vtkCommand::ModifiedEvent, q, SLOT(onArrayModified(vtkObject*)));
  if (!this->ListenAbstractArrayModifiedEvent)
    {
    return;
    }
  const int count = q->rowCount();
  for (int i = 0; i < count; ++i)
    {
    q->qvtkConnect(q->arrayFromIndex(q->index(i,0)),vtkCommand::ModifiedEvent,
                   q, SLOT(onMRMLNodeModified(vtkObject*)));
    }
}
*/

//------------------------------------------------------------------------------
QList<vtkAbstractArray*> ctkVTKDataSetModelPrivate::attributeArrayToInsert(
    const ctkVTKDataSetModel::AttributeTypes& attributeType,
    vtkDataSetAttributes * dataSetAttributes)
{
  QList<vtkAbstractArray*> attributeArraysToInsert;
  for (int p = 0; p < dataSetAttributes->GetNumberOfArrays(); ++p)
    {
    vtkAbstractArray * array = dataSetAttributes->GetAbstractArray(p);

    bool isAttributeArray = false;
    vtkAbstractArray* attributeArrays[vtkDataSetAttributes::NUM_ATTRIBUTES];
    for(int attributeId = 0; attributeId < vtkDataSetAttributes::NUM_ATTRIBUTES; ++attributeId)
      {
      attributeArrays[attributeId] = dataSetAttributes->GetAbstractAttribute(attributeId);
      if (!isAttributeArray && attributeArrays[attributeId] == array)
        {
        isAttributeArray = true;
        }
      }

    if ((attributeType & ctkVTKDataSetModel::ScalarsAttribute && (array == attributeArrays[vtkDataSetAttributes::SCALARS]))
        || (attributeType & ctkVTKDataSetModel::VectorsAttribute && (array == attributeArrays[vtkDataSetAttributes::VECTORS]))
        || (attributeType & ctkVTKDataSetModel::NormalsAttribute && (array == attributeArrays[vtkDataSetAttributes::NORMALS]))
        || (attributeType & ctkVTKDataSetModel::TCoordsAttribute && (array == attributeArrays[vtkDataSetAttributes::TCOORDS]))
        || (attributeType & ctkVTKDataSetModel::TensorsAttribute && (array == attributeArrays[vtkDataSetAttributes::TENSORS]))
        || (attributeType & ctkVTKDataSetModel::GlobalIDsAttribute && (array == attributeArrays[vtkDataSetAttributes::GLOBALIDS]))
        || (attributeType & ctkVTKDataSetModel::PedigreeIDsAttribute && (array == attributeArrays[vtkDataSetAttributes::PEDIGREEIDS]))
        || (attributeType & ctkVTKDataSetModel::EdgeFlagAttribute && (array == attributeArrays[vtkDataSetAttributes::EDGEFLAG]))
        || (attributeType & ctkVTKDataSetModel::NoAttribute && !isAttributeArray)
        )
      {
      attributeArraysToInsert << dataSetAttributes->GetAbstractArray(p);
      }
    }
  return attributeArraysToInsert;
}

//------------------------------------------------------------------------------
// ctkVTKDataSetModel

//------------------------------------------------------------------------------
ctkVTKDataSetModel::ctkVTKDataSetModel(QObject *_parent)
  : QStandardItemModel(_parent)
  , d_ptr(new ctkVTKDataSetModelPrivate(*this))
  , NullItemLocation(-2) // -1 is already used
{
  Q_D(ctkVTKDataSetModel);
  d->init();
}

//------------------------------------------------------------------------------
ctkVTKDataSetModel::ctkVTKDataSetModel(ctkVTKDataSetModelPrivate* pimpl, QObject *parentObject)
  : QStandardItemModel(parentObject)
  , d_ptr(pimpl)
{
  Q_D(ctkVTKDataSetModel);
  d->init();
}

//------------------------------------------------------------------------------
ctkVTKDataSetModel::~ctkVTKDataSetModel()
{
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModel::setDataSet(vtkDataSet* dataSet)
{
  Q_D(ctkVTKDataSetModel);
  if (dataSet == d->DataSet.GetPointer())
    {
    return;
    }
  this->qvtkReconnect(d->DataSet, dataSet, vtkCommand::ModifiedEvent,
                      this, SLOT(onDataSetModified(vtkObject*)) );
  d->DataSet = dataSet;
  this->onDataSetModified(dataSet);
}

//------------------------------------------------------------------------------
vtkDataSet* ctkVTKDataSetModel::dataSet()const
{
  Q_D(const ctkVTKDataSetModel);
  return d->DataSet;
}

//------------------------------------------------------------------------------
ctkVTKDataSetModel::AttributeTypes ctkVTKDataSetModel::attributeTypes()const
{
  Q_D(const ctkVTKDataSetModel);
  return d->AttributeType;
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModel::setAttributeTypes(const AttributeTypes& attributeTypes)
{
  Q_D(ctkVTKDataSetModel);
  if (d->AttributeType == attributeTypes)
    {
    return;
    }
  d->AttributeType = attributeTypes;
  this->updateDataSet();
}

// ----------------------------------------------------------------------------
bool ctkVTKDataSetModel::includeNullItem()const
{
  Q_D(const ctkVTKDataSetModel);
  return d->IncludeNullItem;
}

// ----------------------------------------------------------------------------
void ctkVTKDataSetModel::setIncludeNullItem(bool includeNullItem)
{
  Q_D(ctkVTKDataSetModel);
  if (d->IncludeNullItem == includeNullItem)
    {
    // no change
    return;
    }
  if (includeNullItem)
    {
    this->insertNullItem();
    }
  else
    {
    this->removeNullItem();
    }
  d->IncludeNullItem = includeNullItem;
}

//------------------------------------------------------------------------------
vtkAbstractArray* ctkVTKDataSetModel::arrayFromItem(QStandardItem* arrayItem)const
{
  if (arrayItem == 0 || arrayItem == this->invisibleRootItem())
    {
    return 0;
    }
  QVariant arrayPointer = arrayItem->data(ctkVTK::PointerRole);
  Q_ASSERT(arrayPointer.isValid());
  vtkAbstractArray* array = static_cast<vtkAbstractArray*>(
    reinterpret_cast<void *>(arrayPointer.toLongLong()));
  if (arrayItem->data(ctkVTK::LocationRole).toInt() == this->NullItemLocation)
    {
    // null item
    Q_ASSERT(array==0);
    return 0;
    }

  Q_ASSERT(array);
  return array;
}

//------------------------------------------------------------------------------
int ctkVTKDataSetModel::locationFromItem(QStandardItem* arrayItem)const
{
  if (arrayItem == 0 || arrayItem == this->invisibleRootItem())
    {
    return -1;
    }
  return arrayItem->data(ctkVTK::LocationRole).toInt();
}

//------------------------------------------------------------------------------
QStandardItem* ctkVTKDataSetModel::itemFromArray(vtkAbstractArray* array, int column)const
{
  if (array == 0)
    {
    return 0;
    }
  QModelIndexList indexes = this->match(this->index(-1,-1), ctkVTK::PointerRole,
                                      reinterpret_cast<long long>(array), 1,
                                      Qt::MatchExactly | Qt::MatchRecursive);
  while (indexes.size())
    {
    if (indexes[0].column() == column)
      {
      return this->itemFromIndex(indexes[0]);
      }
    indexes = this->match(indexes[0], ctkVTK::PointerRole,
                          reinterpret_cast<long long>(array), 1,
                          Qt::MatchExactly | Qt::MatchRecursive);
    }
  return 0;
}

//------------------------------------------------------------------------------
QModelIndexList ctkVTKDataSetModel::indexes(vtkAbstractArray* array)const
{
  return this->match(this->index(-1,-1), ctkVTK::PointerRole,
                     QVariant::fromValue(reinterpret_cast<long long>(array)),
                     -1, Qt::MatchExactly | Qt::MatchRecursive);
}

/*
//------------------------------------------------------------------------------
void ctkVTKDataSetModel::setListenArrayModifiedEvent(bool listen)
{
  Q_D(ctkVTKDataSetModel);
  if (d->ListenArrayModifiedEvent == listen)
    {
    return;
    }
  d->ListenArrayModifiedEvent = listen;
  d->listenArrayModifiedEvent();
}

//------------------------------------------------------------------------------
bool ctkVTKDataSetModel::listenNodeModifiedEvent()const
{
  Q_D(const ctkVTKDataSetModel);
  return d->ListenNodeModifiedEvent;
}
*/
//------------------------------------------------------------------------------
void ctkVTKDataSetModel::updateDataSet()
{
  Q_D(ctkVTKDataSetModel);

  // Remove all items (except the first one, if there is a NULL item)
  if (d->IncludeNullItem)
    {
    if (this->rowCount()<1)
      {
      this->insertNullItem();
      }
    else
      {
      this->setRowCount(1);
      }
    }
  else
    {
    this->setRowCount(0);
    }

  if (d->DataSet.GetPointer() == 0)
    {
    return;
    }

  // Populate scene with nodes
  this->populateDataSet();
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModel::populateDataSet()
{
  Q_D(ctkVTKDataSetModel);
  Q_ASSERT(d->DataSet);

  foreach(vtkAbstractArray* attributeArray,
    ctkVTKDataSetModelPrivate::attributeArrayToInsert(d->AttributeType, d->DataSet->GetPointData()))
    {
    this->insertArray(attributeArray, vtkAssignAttribute::POINT_DATA);
    }

  foreach(vtkAbstractArray* attributeArray,
    ctkVTKDataSetModelPrivate::attributeArrayToInsert(d->AttributeType, d->DataSet->GetCellData()))
    {
    this->insertArray(attributeArray, vtkAssignAttribute::CELL_DATA);
    }
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModel::insertArray(vtkAbstractArray* array, int location)
{
  this->insertArray(array, location, this->rowCount());
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModel
::insertArray(vtkAbstractArray* array, int location, int row)
{
  Q_D(ctkVTKDataSetModel);
  if (vtkAbstractArray::SafeDownCast(array)==0)
    {
    // it is normal, it happens when arrays are pre-allocated for a data set
    return;
    }

  QList<QStandardItem*> items;
  for (int i= 0; i < this->columnCount(); ++i)
    {
    QStandardItem* newArrayItem = new QStandardItem();
    this->updateItemFromArray(newArrayItem, array, location, i);
    items.append(newArrayItem);
    }
  this->insertRow(row,items);
  // TODO: don't listen to nodes that are hidden from editors ?
  if (d->ListenAbstractArrayModifiedEvent)
    {
    qvtkConnect(array, vtkCommand::ModifiedEvent,
                this, SLOT(onArrayModified(vtkObject*)));
    }
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModel::updateItemFromArray(QStandardItem* item,
                                             vtkAbstractArray* array,
                                             int location,
                                             int column)
{
  item->setData(QVariant::fromValue(reinterpret_cast<long long>(array)), ctkVTK::PointerRole);
  item->setData(location, ctkVTK::LocationRole);
  switch (column)
    {
    case 0:
      item->setText(QString(array->GetName()));
      break;
    default:
      Q_ASSERT(column == 0);
      break;
    }
}

//------------------------------------------------------------------------------
/*
void ctkVTKDataSetModel::updateItemFromPointsArray(QStandardItem* item, vtkAbstractArray* array, int column)
{
  this->updateItemFromArray(item, array, column);
  switch (column)
    {
    case 0:
      item->setIcon();
      break;
    default:
      Q_ASSERT(column == 0)
      break;
    }
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModel::updateItemFromCellsArray(QStandardItem* item, vtkAbstractArray* array, int column)
{
  this->updateItemFromArray(item, array, column);
  switch (column)
    {
    case 0:
      item->setIcon();
      break;
    default:
      Q_ASSERT(column == 0)
      break;
    }
}

*/

//------------------------------------------------------------------------------
void ctkVTKDataSetModel::updateArrayFromItem(vtkAbstractArray* array, QStandardItem* item)
{
  if (item->column() == 0)
    {
    array->SetName(item->text().toUtf8());
    }
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModel::onDataSetModified(vtkObject* dataSet)
{
  Q_UNUSED(dataSet);
  Q_D(ctkVTKDataSetModel);

  // If a point or cell data array is added or removed then DataSet's Modified is not invoked.
  // Therefore, we need to add observers to the point and cell data objects to make sure
  // the list of arrays is kept up-to-date.

  vtkPointData* dataSetPointData = d->DataSet ? d->DataSet->GetPointData() : 0;
  this->qvtkReconnect(d->DataSetPointData, dataSetPointData, vtkCommand::ModifiedEvent,
                      this, SLOT(onDataSetPointDataModified(vtkObject*)) );
  d->DataSetPointData = dataSetPointData;

  vtkCellData* dataSetCellData = d->DataSet ? d->DataSet->GetCellData() : 0;
  this->qvtkReconnect(d->DataSetCellData, dataSetCellData, vtkCommand::ModifiedEvent,
                      this, SLOT(onDataSetCellDataModified(vtkObject*)) );
  d->DataSetCellData = dataSetCellData;

  this->updateDataSet();
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModel::onDataSetPointDataModified(vtkObject* dataSetPointData)
{
  Q_UNUSED(dataSetPointData);
  this->updateDataSet();
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModel::onDataSetCellDataModified(vtkObject* dataSetCellData)
{
  Q_UNUSED(dataSetCellData);
  this->updateDataSet();
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModel::onArrayModified(vtkObject* modifiedArray)
{
  vtkAbstractArray* array = vtkAbstractArray::SafeDownCast(modifiedArray);
  Q_ASSERT(array);
  QModelIndexList arrayIndexes = this->indexes(array);

  foreach (QModelIndex index, arrayIndexes)
    {
    QStandardItem* item = this->itemFromIndex(index);
    this->updateItemFromArray(
      item, array, item->data(ctkVTK::LocationRole).toInt(), item->column());
    }
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModel::onItemChanged(QStandardItem * item)
{
  vtkAbstractArray* array = this->arrayFromItem(item);
  Q_ASSERT(array);
  this->updateArrayFromItem(array, item);
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModel::insertNullItem()
{
  QStandardItem* nullItem = new QStandardItem();
  nullItem->setData(QVariant::fromValue(qlonglong(0)), ctkVTK::PointerRole);
  nullItem->setData(this->NullItemLocation, ctkVTK::LocationRole);
  nullItem->setText(QString());
  this->insertRow(0,nullItem);
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModel::removeNullItem()
{
  if (this->rowCount() <= 0)
    {
    return;
    }
  // NULL item must be the first one
  QStandardItem* nullItem = this->item(0);
  Q_ASSERT(nullItem);
  if (nullItem == 0)
    {
    return;
    }
  // NULL item has a special location value
  int nullItemLocation = nullItem->data(ctkVTK::LocationRole).toInt();
  Q_ASSERT(nullItemLocation == this->NullItemLocation);
  if (nullItemLocation != this->NullItemLocation)
    {
    return;
    }
  // the first item is indeed the NULL item, so we remove it now
  this->removeRow(0);
}

//------------------------------------------------------------------------------
int ctkVTKDataSetModel::nullItemLocation()const
{
  return this->NullItemLocation;
}