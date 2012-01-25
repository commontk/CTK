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

/// \ingroup Visualization_VTK_Widgets
namespace ctkVTK
{
 enum ItemDataRole {
   PointerRole = Qt::UserRole + 1
 };
};

class ctkVTKDataSetModelPrivate;

//------------------------------------------------------------------------------
/// \ingroup Visualization_VTK_Widgets
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKDataSetModel
  : public QStandardItemModel
{
  Q_OBJECT
  QVTK_OBJECT
  Q_FLAGS(AttributeType AttributeTypes)

  /// This property holds the type of attribute that should be listed in the model.s
  /// By default all attributes are considered.
  /// \sa ctkVTKDataSetModel::AllAttribute
  Q_PROPERTY(AttributeTypes attributeTypes READ attributeTypes WRITE setAttributeTypes)

public:
  typedef ctkVTKDataSetModel Self;
  typedef QStandardItemModel Superclass;
  ctkVTKDataSetModel(QObject *parent=0);
  virtual ~ctkVTKDataSetModel();

  enum AttributeType
    {
    NoAttribute = 0x1,
    ScalarsAttribute = 0x2,
    VectorsAttribute = 0x4,
    NormalsAttribute = 0x8,
    TCoordsAttribute = 0x10,
    TensorsAttribute = 0x20,
    GlobalIDsAttribute = 0x40,
    PedigreeIDsAttribute = 0x80,
    EdgeFlagAttribute = 0x100,
    AllAttribute = NoAttribute | ScalarsAttribute | VectorsAttribute | NormalsAttribute | TCoordsAttribute | TensorsAttribute | GlobalIDsAttribute | PedigreeIDsAttribute | EdgeFlagAttribute
    };
  Q_DECLARE_FLAGS(AttributeTypes, AttributeType)

  virtual void setDataSet(vtkDataSet* dataSet);
  vtkDataSet* dataSet()const;

  AttributeTypes attributeTypes()const;
  void setAttributeTypes(const AttributeTypes& attributeTypes);

  /// Return the vtkDataArray associated to the index.
  /// 0 if the index doesn't contain a vtkDataArray
  inline vtkDataArray* arrayFromIndex(const QModelIndex& arrayIndex)const;
  vtkDataArray* arrayFromItem(QStandardItem* nodeItem)const;
  inline QModelIndex indexFromArray(vtkDataArray* dataArray, int column = 0)const;
  QStandardItem* itemFromArray(vtkDataArray* dataArray, int column = 0)const;
  QModelIndexList indexes(vtkDataArray* dataArray)const;

protected Q_SLOTS:
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
Q_DECLARE_OPERATORS_FOR_FLAGS(ctkVTKDataSetModel::AttributeTypes);

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
