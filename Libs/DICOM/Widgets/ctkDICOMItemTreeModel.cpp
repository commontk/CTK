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

// ctkDICOMWidgets includes
#include "ctkDICOMItemTreeModel.h"

//----------------------------------------------------------------------------
class ctkDICOMItemTreeModelPrivate
{
public:
  ctkDICOMItemTreeModelPrivate(){}
};

//----------------------------------------------------------------------------
// ctkDICOMItemTreeModelPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMItemTreeModel methods

//----------------------------------------------------------------------------
ctkDICOMItemTreeModel::ctkDICOMItemTreeModel(QObject* _parent):Superclass(_parent), 
  d_ptr(new ctkDICOMItemTreeModelPrivate)
{
  //Q_D(ctkDICOMItemTreeModel);

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
