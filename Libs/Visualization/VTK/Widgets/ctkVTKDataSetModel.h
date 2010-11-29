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

#ifndef __ctkVTKDataSetModel_h
#define __ctkVTKDataSetModel_h

// Qt includes
#include <QStandardItemModel>

// CTK includes
#include <ctkVTKObject.h>

// CTK includes
#include "ctkVisualizationVTKWidgetsExport.h"

class vtkDataSet;
class vtkDataArray;

namespace ctkVTK
{
 enum ItemDataRole {
   PointerRole = Qt::UserRole + 1
 };
};

class ctkVTKDataSetModelPrivate;

//------------------------------------------------------------------------------
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKDataSetModel
  : public QStandardItemModel
{
  Q_OBJECT
  QVTK_OBJECT

public:
  typedef QStandardItemModel Superclass;
  ctkVTKDataSetModel(QObject *parent=0);
  virtual ~ctkVTKDataSetModel();

  virtual void setDataSet(vtkDataSet* dataSet);
  vtkDataSet* dataSet()const;

  /// Return the vtkDataArray associated to the index.
  /// 0 if the index doesn't contain a vtkDataArray
  inline vtkDataArray* arrayFromIndex(const QModelIndex& arrayIndex)const;
  vtkDataArray* arrayFromItem(QStandardItem* nodeItem)const;
  inline QModelIndex indexFromArray(vtkDataArray* dataArray, int column = 0)const;
  QStandardItem* itemFromArray(vtkDataArray* dataArray, int column = 0)const;
  QModelIndexList indexes(vtkDataArray* dataArray)const;

protected slots:
  void onDataSetModified(vtkObject* dataSet);
  void onArrayModified(vtkObject* dataArray);
  void onItemChanged(QStandardItem * item);

protected:

  ctkVTKDataSetModel(ctkVTKDataSetModelPrivate* pimpl, QObject *parent=0);

  virtual void insertArray(vtkDataArray* dataArray);
  virtual void insertArray(vtkDataArray* dataArray, int row);
  virtual void updateItemFromArray(QStandardItem* item, vtkDataArray* dataArray, int column);
  virtual void updateArrayFromItem(vtkDataArray* dataArray, QStandardItem* item);
  virtual void updateDataSet();
  virtual void populateDataSet();

protected:
  QScopedPointer<ctkVTKDataSetModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKDataSetModel);
  Q_DISABLE_COPY(ctkVTKDataSetModel);
};

// -----------------------------------------------------------------------------
vtkDataArray* ctkVTKDataSetModel::arrayFromIndex(const QModelIndex &nodeIndex)const
{
  return this->arrayFromItem(this->itemFromIndex(nodeIndex));
}

// -----------------------------------------------------------------------------
QModelIndex ctkVTKDataSetModel::indexFromArray(vtkDataArray* dataArray, int column)const
{
  QStandardItem* item = this->itemFromArray(dataArray, column);
  return item ? item->index() : QModelIndex();
}

#endif
