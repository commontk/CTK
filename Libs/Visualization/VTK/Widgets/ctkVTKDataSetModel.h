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
class vtkAbstractArray;

/// \ingroup Visualization_VTK_Widgets
namespace ctkVTK
{
 enum ItemDataRole {
   PointerRole = Qt::UserRole + 1,
   LocationRole,
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

  /// This property holds the type of attribute that should be listed in the model.
  /// By default all attributes are considered.
  /// \sa ctkVTKDataSetModel::AllAttribute
  Q_PROPERTY(AttributeTypes attributeTypes READ attributeTypes WRITE setAttributeTypes)

  /// This property allows adding a 'Null' item to the model, which is useful when
  /// it is necessary to offer the user an option to not select any of the items
  /// (for example, in a combo box there is always a selected item and it may be
  /// necessary to allow the user to not select any of the attributes).
  /// By default no 'Null' item is included.
  Q_PROPERTY(bool includeNullItem READ includeNullItem WRITE setIncludeNullItem)

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

  bool includeNullItem()const;
  void setIncludeNullItem(bool includeNullItem);
  int nullItemLocation()const;

  /// Return the vtkAbstractArray associated to the index.
  /// 0 if the index doesn't contain a vtkAbstractArray
  inline vtkAbstractArray* arrayFromIndex(const QModelIndex& arrayIndex)const;

  /// Return the location from a given item. Fails and returns -1 if either
  /// the given index points to a null item or an invisible item.
  /// \sa locationFromItem()
  inline int locationFromIndex(const QModelIndex& arrayIndex)const;

  vtkAbstractArray* arrayFromItem(QStandardItem* nodeItem)const;

  /// Return the location from a given item. Fails and returns -1 if either
  /// the given item is null or should be invisible).
  /// \sa locationFromIndex(), invisibleRootItem()
  int locationFromItem(QStandardItem* nodeItem)const;

  inline QModelIndex indexFromArray(vtkAbstractArray* array, int column = 0)const;
  QStandardItem* itemFromArray(vtkAbstractArray* array, int column = 0)const;
  QModelIndexList indexes(vtkAbstractArray* array)const;

protected Q_SLOTS:
  void onDataSetModified(vtkObject* dataSet);
  void onDataSetPointDataModified(vtkObject* dataSetPointData);
  void onDataSetCellDataModified(vtkObject* dataSetCellData);
  void onArrayModified(vtkObject* array);
  void onItemChanged(QStandardItem * item);

protected:

  ctkVTKDataSetModel(ctkVTKDataSetModelPrivate* pimpl, QObject *parent=0);

  virtual void insertArray(vtkAbstractArray* array, int location);
  virtual void insertArray(vtkAbstractArray* array, int location, int row);
  virtual void updateItemFromArray(QStandardItem* item, vtkAbstractArray* array, int location, int column);
  virtual void updateArrayFromItem(vtkAbstractArray* array, QStandardItem* item);
  virtual void updateDataSet();
  virtual void populateDataSet();
  virtual void insertNullItem();
  virtual void removeNullItem();

protected:
  QScopedPointer<ctkVTKDataSetModelPrivate> d_ptr;
  int NullItemLocation;

private:
  Q_DECLARE_PRIVATE(ctkVTKDataSetModel);
  Q_DISABLE_COPY(ctkVTKDataSetModel);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(ctkVTKDataSetModel::AttributeTypes);

// -----------------------------------------------------------------------------
vtkAbstractArray* ctkVTKDataSetModel::arrayFromIndex(const QModelIndex &nodeIndex)const
{
  return this->arrayFromItem(this->itemFromIndex(nodeIndex));
}

// -----------------------------------------------------------------------------
int ctkVTKDataSetModel::locationFromIndex(const QModelIndex &nodeIndex)const
{
  return this->locationFromItem(this->itemFromIndex(nodeIndex));
}

// -----------------------------------------------------------------------------
QModelIndex ctkVTKDataSetModel::indexFromArray(vtkAbstractArray* array, int column)const
{
  QStandardItem* item = this->itemFromArray(array, column);
  return item ? item->index() : QModelIndex();
}

#endif
