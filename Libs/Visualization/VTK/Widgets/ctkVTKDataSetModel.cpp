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
  bool ListenAbstractArrayModifiedEvent;
  ctkVTKDataSetModel::AttributeTypes AttributeType;
};


//------------------------------------------------------------------------------
ctkVTKDataSetModelPrivate::ctkVTKDataSetModelPrivate(ctkVTKDataSetModel& object)
  : q_ptr(&object)
{
  this->ListenAbstractArrayModifiedEvent = false;
  this->AttributeType = ctkVTKDataSetModel::AllAttribute;
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
  this->updateDataSet();
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
  Q_ASSERT(array);
  return array;
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
  this->setRowCount(0);

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

  QList<vtkAbstractArray*> attributeArrays;
  attributeArrays << ctkVTKDataSetModelPrivate::attributeArrayToInsert(d->AttributeType, d->DataSet->GetPointData());
  attributeArrays << ctkVTKDataSetModelPrivate::attributeArrayToInsert(d->AttributeType, d->DataSet->GetCellData());
  foreach(vtkAbstractArray* attributeArray, attributeArrays)
    {
    this->insertArray(attributeArray);
    }
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModel::insertArray(vtkAbstractArray* array)
{
  this->insertArray(array, this->rowCount());
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModel::insertArray(vtkAbstractArray* array, int row)
{
  Q_D(ctkVTKDataSetModel);
  Q_ASSERT(vtkAbstractArray::SafeDownCast(array));

  QList<QStandardItem*> items;
  for (int i= 0; i < this->columnCount(); ++i)
    {
    QStandardItem* newArrayItem = new QStandardItem();
    this->updateItemFromArray(newArrayItem, array, i);
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
void ctkVTKDataSetModel::updateItemFromArray(QStandardItem* item, vtkAbstractArray* array, int column)
{
  item->setData(QVariant::fromValue(reinterpret_cast<long long>(array)), ctkVTK::PointerRole);
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
    array->SetName(item->text().toLatin1());
    }
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModel::onDataSetModified(vtkObject* dataSet)
{
  Q_UNUSED(dataSet);
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
    this->updateItemFromArray(item, array, item->column());
    }
}

//------------------------------------------------------------------------------
void ctkVTKDataSetModel::onItemChanged(QStandardItem * item)
{
  vtkAbstractArray* array = this->arrayFromItem(item);
  Q_ASSERT(array);
  this->updateArrayFromItem(array, item);
}
