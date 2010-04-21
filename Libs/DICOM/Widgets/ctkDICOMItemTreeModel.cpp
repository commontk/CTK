/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

// ctkDICOMWidgets includes
#include "ctkDICOMItemTreeModel.h"

//----------------------------------------------------------------------------
class ctkDICOMItemTreeModelPrivate: public ctkPrivate<ctkDICOMItemTreeModel>
{
public:
  ctkDICOMItemTreeModelPrivate(){}
};

//----------------------------------------------------------------------------
// ctkDICOMItemTreeModelPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMItemTreeModel methods

//----------------------------------------------------------------------------
ctkDICOMItemTreeModel::ctkDICOMItemTreeModel(QObject* _parent):Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkDICOMItemTreeModel);
  //CTK_D(ctkDICOMItemTreeModel);

}

//----------------------------------------------------------------------------
ctkDICOMItemTreeModel::~ctkDICOMItemTreeModel()
{
}

//----------------------------------------------------------------------------
QModelIndex ctkDICOMItemTreeModel::index(int row, int column,
                            const QModelIndex &parent) const
{
  return QModelIndex();
}

//----------------------------------------------------------------------------
QModelIndex ctkDICOMItemTreeModel::parent(const QModelIndex &child) const
{
  return QModelIndex();
}

//----------------------------------------------------------------------------
Qt::ItemFlags ctkDICOMItemTreeModel::flags(const QModelIndex &index) const
{
  return 0;
}

//----------------------------------------------------------------------------
QVariant ctkDICOMItemTreeModel::data(const QModelIndex &index, int role) const
{
  return QVariant();
}

//----------------------------------------------------------------------------
QVariant ctkDICOMItemTreeModel::headerData(int section, Qt::Orientation orientation,
                                                 int role) const
{
  return QVariant();
}

//----------------------------------------------------------------------------
int ctkDICOMItemTreeModel::rowCount(const QModelIndex &parent) const
{
  return -1;
}

//----------------------------------------------------------------------------
int ctkDICOMItemTreeModel::columnCount(const QModelIndex &parent) const
{
  return -1;
}
